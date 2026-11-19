"""Dependence checker for asm_massage residuals.

The rules preserve the C's meaning under their stated preconditions; the
residual diff is free-form. This module proves that a residual only
reorders instructions in ways that keep every data dependence, so a large
residual is mechanically reviewable rather than trusted on the byte match.

Model (on the program after renaming, before and after the residual):

* Delay slots are folded: a slot instruction is treated as executing just
  before its branch (it runs on both paths). A slot that writes a register
  its branch reads cannot be folded and is reported.
* Moves are residual lines removed in one place and added unchanged in
  another; re-registered lines keep their operation and constants but use
  other registers. Everything else is an insertion or a deletion: dead
  insertions, rebuilt constants and frameless save-area accesses are
  accepted; deleting a pure instruction is accepted when the allocation
  check shows its readers get the value elsewhere.
* Value identity: webs joined by a register copy, by a stack spill slot or
  by holding the same constant are interchangeable.
* Conflicts: RAW/WAR/WAW on GPRs; GTE data and control registers (a GTE
  command reads and writes every GTE register); memory, where two accesses
  conflict if one stores and they may overlap. Accesses through the same
  unchanged base with disjoint byte ranges do not overlap; distinct global
  symbols do not; and, by assumption A2, nor do the regions records,
  packets, vertex caches, colour caches, vertices and ordering table
  (inferred from the GsARGUNIT field each base pointer was loaded from).
* Every conflicting pair keeps its order (web-level dependences, so two
  values that merely share a register are independent; the literal $at of
  rule-built sequences is split per definition).
* Allocation: in the final program every register read must be reached
  only by definitions of the web it expects, on every path. This proves a
  pure instruction may move between blocks; a load additionally must not
  pass a store that may write its word (including those of a loop it
  leaves). A side effect (store, GTE write or command, branch) may cross a
  branch only if it is dead on that path: a GTE register rewritten before
  any GTE command, or (A6) a store into the packet being built. Moves
  across labels and control-flow changes are reported.
* GTE waits: a GTE command must not get closer to the GTE register write
  before it than it was (or than two instructions, the documented wait).
"""

from __future__ import annotations

from dataclasses import dataclass, field
import re

from asm_massage import BRANCHES, LOADS, STORES, Instr, label_index, live_after

GTE_WRITES = {"mtc2": "g", "ctc2": "c", "lwc2": "g"}
GTE_READS = {"mfc2": "g", "cfc2": "c", "swc2": "g"}
SIZES = {"lb": 1, "lbu": 1, "sb": 1, "lh": 2, "lhu": 2, "sh": 2}
# GsARGUNIT fields (normal and shared) that hold base pointers, by offset.
# Globals that hold a pointer into one of the regions (A2): the direct
# drivers' lit-colour cache is the same buffer the shared drivers reach
# through nortop2.
GLOBAL_REGIONS = {"gModel_dwLitColorCacheBase": "colours"}

ARG_REGIONS = {
    0x0: "header", 0x4: "ot-header", 0x10: "packet", 0x14: "records",
    0x18: "vertices", 0x1C: "cache", 0x20: "normals", 0x24: "colours",
}


@dataclass
class Report:
    moves: int = 0
    rewrites: int = 0
    inserts: int = 0
    dead_inserts: int = 0
    saves: int = 0
    constants: int = 0
    copies: int = 0
    deletes: int = 0
    merged: list[str] = field(default_factory=list)
    rotations: int = 0
    refused: list[str] = field(default_factory=list)
    flags: list[str] = field(default_factory=list)

    def summary(self) -> str:
        head = (f"dependence check: {self.moves} moves, {self.rewrites} re-registered, "
                f"{self.inserts} insertions ({self.dead_inserts} dead, {self.constants} constants, "
                f"{self.copies} copies, {self.saves} saves), "
                f"{self.deletes} deletions, {len(self.merged)} merged duplicates, "
                f"{self.rotations} rotated loads; ")
        if not self.flags:
            return head + "all dependences preserved"
        return head + f"{len(self.flags)} flagged"


def is_gte_command(instr: Instr) -> bool:
    return instr.op == ".word"


def side_effect(instr: Instr) -> bool:
    return (instr.op in STORES or instr.op in GTE_WRITES or instr.op in ("mtc2", "ctc2")
            or is_gte_command(instr) or instr.op in BRANCHES)


def gte_regs(instr: Instr) -> tuple[set, set]:
    reads, writes = set(), set()
    if instr.op in GTE_WRITES:
        writes.add(f"{GTE_WRITES[instr.op]}{instr.args[0 if instr.op == 'lwc2' else 1][1]}")
    if instr.op in GTE_READS:
        reads.add(f"{GTE_READS[instr.op]}{instr.args[0 if instr.op == 'swc2' else 1][1]}")
    return reads, writes


def mem_access(instr: Instr):
    """(is_store, base, offset or symbol, size) for a memory instruction."""
    if instr.op not in LOADS | STORES or instr.op in ("mfc2", "cfc2"):
        return None
    arg = instr.args[1]
    if arg[0] != "m":
        return None
    return instr.op in STORES, arg[2], arg[1], SIZES.get(instr.op, 4)


class Program:
    """A folded (delay-slot-free) instruction order with region facts."""

    def __init__(self, instrs: list[Instr]):
        self.raw = instrs
        self.order: list[Instr] = []
        self.problems: list[str] = []
        n = 0
        while n < len(instrs):
            instr = instrs[n]
            if instr.op in BRANCHES and n + 1 < len(instrs):
                slot = instrs[n + 1]
                if slot.op != "nop":
                    if set(slot.defs()) & set(instr.uses()):
                        self.problems.append(f"delay slot writes the branch operand: {instr.text()} / {slot.text()}")
                    self.order.append(slot)
                self.order.append(instr)
                n += 2
                continue
            if instr.op != "nop":
                self.order.append(instr)
            n += 1
        self.position = {id(i): k for k, i in enumerate(self.order)}
        # Labels that directly precede another label name the same place;
        # anchors use one canonical name per place, numbered from the end
        # like canonical_labels, so equal control flow compares equal.
        alias: dict[str, str] = {}
        run: list[str] = []
        for instr in instrs + [Instr("nop")]:
            if instr.is_label:
                run.append(instr.args[0][1])
                continue
            for name in run:
                alias[name] = run[-1]
            run = []
        places = [n for n in dict.fromkeys(alias[i.args[0][1]] for i in instrs if i.is_label)]
        canonical = {name: f".L{len(places) - k}" for k, name in enumerate(places)}
        self.label = {name: canonical[alias[name]] for name in alias}
        # Block index of each instruction: blocks end at branches and
        # start at places.
        self.block: dict[int, int] = {}
        self.anchors: list[str] = []
        block = 0
        for instr in self.order:
            if instr.is_label and alias[instr.args[0][1]] == instr.args[0][1]:
                block += 1
                self.anchors.append(self.label[instr.args[0][1]] + ":")
            self.block[id(instr)] = block
            if instr.op in BRANCHES:
                block += 1
                text = instr.text()
                if instr.args and instr.args[-1][0] == "l" and instr.args[-1][1] in self.label:
                    text = text.rsplit(" ", 1)[0] + " " + self.label[instr.args[-1][1]]
                self.anchors.append(text)

    def region(self, reg: str, at: int, depth: int = 0) -> str | None:
        """Memory region a base register points into at position AT."""
        if depth > 6:
            return None
        for k in range(at - 1, -1, -1):
            instr = self.order[k]
            if reg not in instr.defs():
                continue
            op, a = instr.op, instr.args
            if op == "lw" and a[1][0] == "m" and isinstance(a[1][1], str):
                return global_region(a[1][1])
            if op == "lw" and a[1][0] == "m" and isinstance(a[1][1], int):
                base, off = a[1][2], a[1][1]
                if base in ("$a0", "$a0.0"):
                    return ARG_REGIONS.get(off)
                if self.region(base, k, depth + 1) == "ot-header" and off == 4:
                    return "ot"
                return None
            if op == "addiu" and a[1][1] == reg:
                continue  # pointer increment: keep looking for its origin
            if op in ("addu", "addiu"):
                for src in instr.uses():
                    found = self.region(src, k, depth + 1)
                    if found:
                        return found
                return None
            return None
        # Not defined above in program order (loop-carried): use any def.
        regions = set()
        for k, instr in enumerate(self.order):
            if reg in instr.defs() and not (instr.op == "addiu" and instr.args[1][1] == reg):
                regions.add(self.region(reg, k + 1, depth + 1))
        return regions.pop() if len(regions) == 1 else None


def global_region(offset: str) -> str | None:
    match = re.fullmatch(r"%lo\((\w+)\)", offset)
    return GLOBAL_REGIONS.get(match.group(1)) if match else None


def rotations(program: list[Instr]) -> tuple[int, list[str]]:
    """Prove the loop rotations rotate-loop-loads made (in the rules-stage,
    web-named program). Each rotated GTE input register R has exactly two
    definitions, both "lw R, off(p)": one in the preheader and one in the
    loop body after the body's only change of p, a constant increment, on
    the straight path from the loop head; its one reader is the mtc2 that
    replaced the original lwc2, before that increment. So the mtc2 sees the
    word at the current p, as the lwc2 did, provided memory is unchanged in
    between: no store in the loop or between the preheader load and the loop
    may alias it (A2). R is read nowhere else, so the extra load on the last
    iteration is dead; it reads past the array (A7)."""
    from asm_massage import label_index, live_after
    from asm_massage_rules import loops

    labels = label_index(program)
    spans = loops(program)
    prog = Program(program)
    count, problems = 0, []
    for m, move in enumerate(program):
        if move.op != "mtc2" or "rotate-loop-loads" not in move.origin:
            continue
        reg = move.args[0][1]
        defs = [n for n, i in enumerate(program) if reg in i.defs()]
        readers = [n for n, i in enumerate(program) if reg in i.uses()]
        loop = [(a, b) for a, b in spans if a < m <= b + 1]
        why = None
        if readers != [m] or len(defs) != 2 or not loop:
            why = "not a single rotated value"
        else:
            start, end = min(loop, key=lambda ab: ab[1] - ab[0])
            pre, body = defs
            loads = [program[pre], program[body]]
            name = program[start].args[0][1]
            base = loads[0].args[1][2] if loads[0].op == "lw" else None
            incs = [n for n in range(start + 1, end + 2) if base in program[n].defs()]
            if (any(i.op != "lw" or i.args[1] != loads[0].args[1] for i in loads)
                    or not pre < start < m < body <= end + 1):
                why = "loads do not bracket the loop"
            elif [n for n, i in enumerate(program) if i.op in BRANCHES and i.args
                  and i.args[-1] == ("l", name)] != [end]:
                why = "loop head reached other than by its back edge"
            elif any(base in program[n].defs() for n in range(pre + 1, start)):
                why = "pointer changes before the loop"
            elif (len(incs) != 1 or program[incs[0]].op != "addiu"
                  or program[incs[0]].args[1] != ("r", base) or not m < incs[0] < body
                  or any(program[n].is_label or program[n].op in BRANCHES
                         for n in range(start + 1, body))):
                why = "pointer does not advance once, straight, between the move and the load"
            elif any(program[n].op in STORES and may_alias(prog, program[n], loads[1])
                     for n in list(range(pre + 1, start)) + list(range(start + 1, end + 2))):
                why = "a store in the loop may write the rotated word"
            else:
                exits = {end + 2} | {labels[i.args[-1][1]] for i in program[start + 1:end + 1]
                                     if i.op in BRANCHES and i.args and i.args[-1][0] == "l"
                                     and not start <= labels.get(i.args[-1][1], -1) <= end + 1}
                if any(e < len(program) and (reg in program[e].uses() or (
                        reg not in program[e].defs() and live_after(program, e, reg)))
                        for e in exits):
                    why = "rotated value live after the loop"
        if why:
            problems.append(f"rotation of {move.text()}: {why}")
        else:
            count += 1
    return count, problems


def is_argument(prog: Program, name: str) -> bool:
    """Whether a web is the GsARGUNIT pointer ($a0 or a copy of it)."""
    if name in ("$a0", "$a0.0"):
        return True
    same = getattr(prog, "same", None)
    if same is not None and same(name) in (same("$a0"), same("$a0.0")):
        return True
    defs = [i.web for i in prog.order if i.web is not None and name in i.web.defs()]
    return (len(defs) == 1 and defs[0].op in ("addu", "or") and defs[0].args[2] == ("r", "$zero")
            and defs[0].args[1][1] in ("$a0", "$a0.0"))


def web_region(prog: Program, name: str, depth: int = 0) -> str | None:
    """Region of a def-use web (a GCC live range, whatever register the map
    gives it), from its definitions anywhere in the program; pointer
    increments keep the region."""
    if depth > 6:
        return None
    if is_argument(prog, name):
        return None
    regions = set()
    for instr in prog.order:
        web = instr.web
        if web is None or name not in web.defs():
            continue
        op, a = web.op, web.args
        if op == "addiu" and a[1][1] == name:
            continue
        if op == "lw" and a[1][0] == "m" and isinstance(a[1][1], str):
            regions.add(global_region(a[1][1]))
        elif op == "lw" and a[1][0] == "m" and isinstance(a[1][1], int):
            base, off = a[1][2], a[1][1]
            # A copy of $a0 that the renaming turned back into $a0 (and
            # dropped as a self-move) is still the argument pointer.
            physical_arg = (instr.op == "lw" and instr.args[1][2] == "$a0"
                            and not any("$a0" in i.defs() for i in prog.order))
            if physical_arg or is_argument(prog, base):
                regions.add(ARG_REGIONS.get(off))
            elif web_region(prog, base, depth + 1) == "ot-header" and off == 4:
                regions.add("ot")
            else:
                regions.add(None)
        elif op in ("addu", "addiu"):
            found = {web_region(prog, src, depth + 1) for src in web.uses()} - {None}
            regions.add(found.pop() if len(found) == 1 else None)
        else:
            regions.add(None)
    return regions.pop() if len(regions) == 1 else None


def may_alias(prog: Program, x: Instr, y: Instr) -> bool:
    ax, ay = mem_access(x), mem_access(y)
    sx, bx, ox, nx = ax
    sy, by, oy, ny = ay
    if isinstance(ox, str) or isinstance(oy, str):
        if isinstance(ox, str) and isinstance(oy, str):
            return ox.split("(")[1] == oy.split("(")[1]
        return False  # a global and a heap pointer (A2)
    px, py = prog.position[id(x)], prog.position[id(y)]
    lo, hi = sorted((px, py))
    if bx == by:
        # Same base: constant increments between the two (straight-line,
        # as the order is) shift one offset; any other write is unknown.
        delta, known = 0, True
        for k in range(lo, hi):
            instr = prog.order[k]
            if bx not in instr.defs():
                continue
            if (instr.op == "addiu" and instr.args[1] == ("r", bx)
                    and isinstance(instr.args[2][1], int)
                    and not any(prog.order[m].is_label for m in range(lo, hi))):
                delta += instr.args[2][1]
            else:
                known = False
        if known:
            if px > py:
                ox += delta
            else:
                oy += delta
            return not (ox + nx <= oy or oy + ny <= ox)
    rx, ry = prog.region(bx, px), prog.region(by, py)
    # The renamed program may reuse a register for unrelated webs; the
    # web-level region of an instruction GCC emitted is more precise.
    if x.web is not None and mem_access(x.web):
        rx = web_region(prog, mem_access(x.web)[1]) or rx
    if y.web is not None and mem_access(y.web):
        ry = web_region(prog, mem_access(y.web)[1]) or ry
    if rx and ry and rx != ry:
        return False  # A2: distinct regions never overlap
    return True


def is_save_slot(instr: Instr, frameless) -> bool:
    """sw/lw of an $sN in the frameless save area of the argument block:
    the callee-saved convention, not program data."""
    arg, base = frameless
    if instr.op not in ("sw", "lw") or instr.args[1][0] != "m" or instr.args[1][2] != arg:
        return False
    reg, off = instr.args[0][1], instr.args[1][1]
    return (reg[:2] == "$s" and reg[2:].isdigit() and isinstance(off, int)
            and base <= off < base + 4 * 8)


def web(instr: Instr) -> Instr:
    """The web-level twin of a renamed instruction (data dependences are
    between webs: two webs sharing a register are not dependent)."""
    return instr.web if instr.web is not None else instr


def constant_feeds(program, into, n, values, same) -> bool:
    """An inserted constant materialisation (rebuilt where it is used, as
    the original does) whose every reader expects a web with that value."""
    local = constants(program[max(0, n - 1):n + 1])
    instr = program[n]
    if instr.op not in ("addiu", "lui", "ori") or not instr.defs():
        return False
    reg = instr.defs()[0]
    if reg not in local:
        return False
    if instr.op == "lui" and n + 1 < len(program) and program[n + 1].op == "ori" \
            and program[n + 1].args[1][1] == reg:
        return True  # the ori that completes it is checked on its own
    found = False
    for k, reader in enumerate(program):
        if (n, reg) not in into[k] or reader.web is None:
            continue
        for kind, phys, wide in zip(_kinds(reader), reader.args, reader.web.args):
            name = phys[1] if kind == "u" else phys[2] if kind == "m" else None
            want = wide[1] if kind == "u" else wide[2] if kind == "m" else None
            if name == reg:
                if values.get(want) != local[reg]:
                    return False
                found = True
    return found


def copy_feeds(program, into, n, same) -> bool:
    """An inserted register copy (the original keeps a value in two
    registers) whose source holds one web on every path and whose every
    reader expects that web (or a copy of it)."""
    instr = program[n]
    if instr.op not in ("addu", "or") or instr.args[2] != ("r", "$zero"):
        return False
    reg, src = instr.args[0][1], instr.args[1][1]
    sources = set()
    for d, r in into[n]:
        if r != src:
            continue
        definer = program[d] if d >= 0 else None
        if definer is None or definer.web is None:
            return False
        defs = [w for w, p in zip(definer.web.defs(), definer.defs()) if p == src]
        if len(defs) != 1:
            return False
        sources.add(same(defs[0]))
    if len(sources) != 1:
        return False
    value = sources.pop()
    found = False
    for k, reader in enumerate(program):
        if (n, reg) not in into[k] or reg not in reader.uses():
            continue
        if reader.web is None:
            return False
        for kind, phys, wide in zip(_kinds(reader), reader.args, reader.web.args):
            name = phys[1] if kind == "u" else phys[2] if kind == "m" else None
            want = wide[1] if kind == "u" else wide[2] if kind == "m" else None
            if name == reg:
                if same(want) != value:
                    return False
                found = True
    return found


def split_at(before: list[Instr]) -> None:
    """Rules build constants and tag words in a literal $at; give each of
    those short-lived values its own name in the web twins."""
    import copy

    current, count = "$at", 0
    for instr in before:
        if instr.is_label:
            current = "$at"
            continue
        twin = web(instr)
        if "$at" not in twin.uses() and "$at" not in twin.defs():
            continue
        twin = copy.copy(twin)
        twin.args = list(twin.args)
        for k, (kind, arg) in enumerate(zip(_kinds(twin), twin.args)):
            if kind == "u" and arg[1] == "$at":
                twin.args[k] = ("r", current)
            elif kind == "m" and arg[2] == "$at":
                twin.args[k] = ("m", arg[1], current)
        for k, (kind, arg) in enumerate(zip(_kinds(twin), twin.args)):
            if kind == "d" and arg[1] == "$at":
                count += 1
                current = f"$at#{count}"
                twin.args[k] = ("r", current)
        instr.web = twin


def sign_variant(x: Instr, y: Instr) -> bool:
    return {x.op, y.op} == {"lb", "lbu"} and x.args[1] == y.args[1]


def zero_tested(program: list[Instr], n: int) -> bool:
    """Every reader of instruction N's result compares it with zero."""
    reg = program[n].defs()[0]
    found = False
    for k in range(n + 1, len(program)):
        instr = program[k]
        if reg in instr.uses():
            if not (instr.op in ("beq", "bne") and ("r", "$zero") in instr.args[:2]):
                return False
            found = True
        if reg in instr.defs():
            break
    return found


def shape(instr: Instr) -> tuple:
    """Operation and constants; registers are wildcards except $zero."""
    return (instr.op, tuple(a if a[0] not in ("r", "m") or a == ("r", "$zero")
                            else (a[0],) + ((a[1],) if a[0] == "m" else ())
                            for a in instr.args))


def conflict(prog: Program, x: Instr, y: Instr) -> str | None:
    wxi, wyi = web(x), web(y)
    rx, wx = set(wxi.uses()) - {"$zero"}, set(wxi.defs()) - {"$zero"}
    ry, wy = set(wyi.uses()) - {"$zero"}, set(wyi.defs()) - {"$zero"}
    if wx & ry:
        return "RAW " + ",".join(sorted(wx & ry))
    if rx & wy:
        return "WAR " + ",".join(sorted(rx & wy))
    if wx & wy:
        return "WAW " + ",".join(sorted(wx & wy))
    gx, gy = gte_regs(x), gte_regs(y)
    if is_gte_command(x) and (gy[0] or gy[1] or is_gte_command(y)) or \
            is_gte_command(y) and (gx[0] or gx[1]):
        return "GTE command order"
    if gx[1] & (gy[0] | gy[1]) or gy[1] & gx[0]:
        return "GTE register"
    mx, my = mem_access(x), mem_access(y)
    if mx and my and (mx[0] or my[0]) and may_alias(prog, x, y):
        return "memory"
    return None


def allocation(program: list[Instr], same) -> list[str]:
    """In the final physical program every register read must see the
    definition of the web it reads (no other web's write in between).
    SAME maps a web to its copy class (webs joined by register copies)."""
    from asm_massage import reaching

    into = reaching(program)
    problems = []
    for n, instr in enumerate(program):
        if instr.web is None or instr.is_label:
            continue
        best = None
        for twin in _twins(instr):
            found = _reads(program, into, n, instr, twin, same)
            if best is None or len(found) < len(best):
                best = found
        problems += best
    return problems


def _twins(instr: Instr) -> list[Instr]:
    """The web twin, and its operand swap for a commutative operation."""
    twins = [instr.web]
    if instr.op in ("addu", "and", "or", "xor", "nor") and len(instr.web.args) == 3:
        twins.append(Instr(instr.op, [instr.web.args[0], instr.web.args[2], instr.web.args[1]]))
    return twins


def _reads(program, into, n, instr, twin, same):
    problems = []
    pairs = []
    for kind, phys, wide in zip(_kinds(instr), instr.args, twin.args):
        if kind == "u":
            pairs.append((phys[1], wide[1]))
        elif kind == "m":
            pairs.append((phys[2], wide[2]))
    for reg, name in pairs:
        if reg in ("$zero", "$sp", "$ra", "$gp"):
            continue
        for d, r in into[n]:
            if r != reg:
                continue
            if d < 0:
                if not (name.endswith(".0") or same(name) == same(reg + ".0") or name == reg):
                    problems.append(f"{instr.text()} reads {reg} for {name}, but it is the entry value")
                continue
            definer = program[d]
            if definer.web is None:
                continue  # rule- or residual-made (saves, copies)
            if not any(same(name) == same(w) for w in definer.web.defs()):
                problems.append(f"{instr.text()} reads {reg} for {name}, "
                                f"but {definer.text()} may have overwritten it")
    return problems


def _kinds(instr: Instr) -> str:
    from asm_massage import SCHEMA

    return SCHEMA.get(instr.op, "")


def constants(instrs: list[Instr]) -> dict:
    """Value of every web (or physical register, for instructions without a
    twin) that is a constant: addiu r,$zero,k / lui / lui+ori."""
    values: dict = {}
    for instr in instrs:
        if instr.is_label:
            continue
        twin = web(instr)
        a = twin.args
        if twin.op == "addiu" and len(a) == 3 and a[1] == ("r", "$zero") and isinstance(a[2][1], int):
            values[a[0][1]] = a[2][1] & 0xFFFFFFFF
        elif twin.op == "lui" and isinstance(a[1][1], int):
            values[a[0][1]] = (a[1][1] << 16) & 0xFFFFFFFF
        elif twin.op == "ori" and a[1][1] in values and isinstance(a[2][1], int):
            values[a[0][1]] = values[a[1][1]] | a[2][1]
    return values


def copy_classes(webs: list[Instr], values: dict | None = None):
    """Webs joined by register copies (addu d,s,$zero), by a spill slot or
    by holding the same constant share a value."""
    parent: dict = {}

    def find(x):
        parent.setdefault(x, x)
        while parent[x] != x:
            x = parent[x]
        return x

    def union(x, y):
        a, b = find(x), find(y)
        if a != b:
            parent[a] = b

    slots: dict = {}
    for instr in webs:
        if instr.op in ("addu", "or") and len(instr.args) == 3 and instr.args[2] == ("r", "$zero"):
            union(instr.args[0][1], instr.args[1][1])
        # A spill slot is a copy too: whatever is stored there is reloaded.
        if instr.op in ("sw", "lw") and instr.args[1][0] == "m" and instr.args[1][2] == "$sp":
            slots.setdefault(instr.args[1][1], []).append(instr.args[0][1])
    for regs in slots.values():
        for other in regs[1:]:
            union(regs[0], other)
    by_value: dict = {}
    for name, value in (values or {}).items():
        by_value.setdefault(value, []).append(name)
    for names in by_value.values():
        for other in names[1:]:
            union(names[0], other)
    return find


def pb_pos_of_after(pa: Program, pb: Program, instr: Instr, identity) -> int:
    """Position, in the before order, just before the instruction that
    follows INSTR's new place (approximates its new place in old terms)."""
    order = pa.order
    k = next(n for n, i in enumerate(order) if identity(i) is instr)
    for nxt in order[k + 1:]:
        old = identity(nxt)
        if id(old) in pb.position and old is not instr:
            return pb.position[id(old)]
    return len(pb.order) - 1


def gte_dead_at(program: list[Instr], target: str, reg: str) -> bool:
    """Every path from TARGET rewrites GTE register REG before any GTE
    command (commands read all GTE registers) or the return."""
    from asm_massage import successors

    labels = label_index(program)
    seen, stack = set(), [labels[target]]
    while stack:
        n = stack.pop()
        if n in seen:
            continue
        seen.add(n)
        instr = program[n]
        if is_gte_command(instr) or (instr.op in GTE_READS and gte_regs(instr)[0] == {reg}):
            return False
        if instr.op in GTE_WRITES and gte_regs(instr)[1] == {reg}:
            continue
        stack.extend(successors(program, n, labels))
    return True


def dead_side_effect(program: list[Instr], instr: Instr, target: str, prog: Program) -> bool:
    """A GTE write whose register is rewritten before use on the branch
    path, or (A6) a store into the packet being built: until tag-build
    links a packet into the OT its words are scratch, and a skip path
    reuses or abandons them."""
    if instr.op in ("mtc2", "ctc2", "lwc2"):
        reg = next(iter(gte_regs(instr)[1]))
        return gte_dead_at(program, target, reg)
    access = mem_access(instr)
    if access and access[0]:
        k = prog.position[id(instr)]
        return prog.region(access[1], k) == "packet"
    return False


def web_read_before_write(program: list[Instr], start: int, name: str) -> bool:
    """Whether some path from instruction START reads web NAME before an
    instruction defines it again."""
    from asm_massage import successors

    labels = label_index(program)
    seen, stack = set(), [start]
    while stack:
        n = stack.pop()
        if n in seen or n >= len(program):
            continue
        seen.add(n)
        twin = web(program[n])
        if name in twin.uses():
            return True
        if name in twin.defs():
            continue
        stack.extend(successors(program, n, labels))
    return False


def merge_join_duplicates(program: list[Instr], same) -> tuple[list[Instr], list[str], list[str]]:
    """Undo GCC's tail duplication: an identical pure instruction that is
    the last one executed on every way into a label is moved to just after
    the label. Returns the new program, the merged instructions and the
    reasons for any candidate that was refused.

    Proof obligations, for a label J and its incoming paths (the
    fall-through and every branch to J, whose delay slot is the last
    instruction executed on that path):
    1. every path ends in the same pure instruction (same operation,
       constants and operand values: source webs in one value class, and
       one destination class);
    2. there are at least two such copies;
    3. nothing runs between a copy and J, so its destination is not read
       there, and the copy's operands still hold their values at J;
    4. a copy in the delay slot of a conditional branch also ran on the
       not-taken path; its result must not be read there before it is
       recomputed, so dropping it changes nothing on that path;
    then the single copy after J runs exactly when one of the copies ran
    and dominates every later use (the allocation check re-verifies the
    uses)."""
    merged, refused = [], []
    changed = True
    while changed:
        changed = False
        labels = label_index(program)
        for name, j in labels.items():
            start = j
            while start > 0 and program[start - 1].is_label:
                start -= 1
            names = {program[k].args[0][1] for k in range(start, j + 1)}
            last = start - 1  # the instruction that falls into the place
            copies = []  # (index, falls into the place)
            if last >= 0:
                prev = program[last]
                into_prev = last > 0 and program[last - 1].op in BRANCHES
                if prev.is_label or prev.op in BRANCHES:
                    continue
                if into_prev:
                    branch = program[last - 1]
                    target = branch.args[-1][1] if branch.args and branch.args[-1][0] == "l" else None
                    if branch.op in ("j", "jr") and target not in names:
                        pass  # the slot leaves elsewhere: no fall-through path
                    elif target not in names:
                        continue  # a slot that reaches the place on one path only
                    else:
                        copies.append((last, True))
                else:
                    copies.append((last, True))
            for n, instr in enumerate(program):
                if (instr.op in BRANCHES and instr.args and instr.args[-1][0] == "l"
                        and instr.args[-1][1] in names and n + 1 != last):
                    copies.append((n + 1, n + 2 == start))
            if len(copies) < 2:
                continue
            first = program[copies[0][0]]
            if side_effect(first) or first.op == "nop" or first.is_label or not first.defs():
                continue
            if not all(program[k].text() == first.text() for k, _ in copies):
                continue
            twins = [web(program[k]) for k, _ in copies]
            if not all([same(r) for r in t.uses()] == [same(r) for r in twins[0].uses()]
                       and [same(r) for r in t.defs()] == [same(r) for r in twins[0].defs()]
                       for t in twins):
                refused.append(f"{first.text()} before {name}: operands differ between paths")
                continue
            # A copy in a conditional branch's slot also ran on the
            # not-taken path (unless that path is the place itself).
            if any(not falls and program[k - 1].op not in ("j", "jr")
                   and any(web_read_before_write(program, k + 1, d) for d in web(program[k]).defs())
                   for k, falls in copies):
                refused.append(f"{first.text()} before {name}: its result is read on a "
                               "not-taken path before being recomputed")
                continue
            positions = {k for k, _ in copies}
            keep = program[copies[0][0]]
            out = []
            for k, instr in enumerate(program):
                if k in positions:
                    if k > 0 and program[k - 1].op in BRANCHES:
                        out.append(Instr("nop", [], {"merged"}))
                    continue
                out.append(instr)
                if k == j:
                    out.append(keep)
            program = out
            merged.append(f"{first.text()} at {name}")
            changed = True
            break
    return program, merged, refused


def check(before: list[Instr], after_raw: list[Instr], frameless=None, webs=None) -> Report:
    """BEFORE: the program the residual applies to; AFTER_RAW: the result
    (objects carried over by context lines are shared with BEFORE)."""
    import copy
    import itertools
    from asm_massage import reaching

    report = Report()
    split_at(before)
    original = before
    values = constants(original)
    same = copy_classes(webs or [], values)
    before, report.merged, report.refused = merge_join_duplicates(original, same)
    kept = {id(i) for i in original}
    in_after = {id(a) for a in after_raw}
    # Work on copies: identities (which original instruction each final one
    # is) are decided below, not taken from the diff's context lines, which
    # pair repeated texts arbitrarily.
    after = [copy.copy(i) for i in after_raw]
    ident: dict[int, Instr] = {}
    for new, old in zip(after, after_raw):
        if id(old) in kept:
            ident[id(new)] = old
    def saved(i):
        return bool(frameless) and is_save_slot(i, frameless)

    removed = [i for i in before if id(i) not in in_after and not i.is_label and not saved(i)
               and i.op != "nop"]
    report.saves += sum(1 for i in before if id(i) not in in_after and saved(i))
    fresh = [new for new, old in zip(after, after_raw)
             if id(old) not in kept and not new.is_label and not saved(new) and new.op != "nop"]
    report.saves += sum(1 for new, old in zip(after, after_raw) if id(old) not in kept and saved(new))
    # Branches compare by place, not label spelling (a merged duplicate can
    # leave a label that names the same place as its successor).
    before_labels, after_labels = Program(before).label, Program(after_raw).label

    def text(instr, labels):
        if instr.op in BRANCHES and instr.args and instr.args[-1][0] == "l":
            return instr.text().rsplit(" ", 1)[0] + " " + labels.get(instr.args[-1][1], instr.args[-1][1])
        return instr.text()

    pool: dict[str, list[Instr]] = {}
    for instr in removed:
        pool.setdefault(text(instr, before_labels), []).append(instr)
    rest = []
    for instr in fresh:
        same_text = pool.get(text(instr, after_labels))
        if same_text:
            ident[id(instr)] = same_text.pop(0)
            report.moves += 1
        else:
            rest.append(instr)
    left = [i for lst in pool.values() for i in lst]
    for instr in after:
        instr.web = web(ident[id(instr)]) if id(instr) in ident else None
    into = reaching(after)
    readers: dict[str, list[int]] = {}
    for n, instr in enumerate(after):
        for r in instr.uses():
            readers.setdefault(r, []).append(n)
    index = {id(i): n for n, i in enumerate(after)}

    def local_cost(n):
        instr = after[n]
        affected = {n} | set(k for r in instr.defs() for k in readers.get(r, []))
        total = 0
        for k in affected:
            if after[k].web is not None and not after[k].is_label:
                total += min(len(_reads(after, into, k, after[k], twin, same)) for twin in _twins(after[k]))
        return total

    # Re-registered: the residual rewrote an instruction's registers. Pair
    # it with the removed instruction of the same shape under which its
    # registers hold the expected webs; an unpairable one is an insertion.
    # A byte load that changed signedness pairs only if every reader just
    # compares it with zero.
    inserts = []
    before_pos = {id(i): k for k, i in enumerate(before)}
    pending = list(rest)
    while pending:
        best = None
        for instr in pending:
            n = index[id(instr)]
            for cand in left:
                if shape(cand) != shape(instr) and not sign_variant(cand, instr):
                    continue
                instr.web = web(cand)
                if cand.op != instr.op and not zero_tested(after, n):
                    continue
                key = (local_cost(n), abs(before_pos[id(cand)] - n))
                if best is None or key < best[0]:
                    best = (key, instr, cand)
            instr.web = None
        if best is None:
            inserts += pending
            break
        _, instr, cand = best
        instr.web = web(cand)
        left.remove(cand)
        pending.remove(instr)
        ident[id(instr)] = cand
        report.rewrites += 1
    deletes = left

    # Swap identities among same-shape instructions while that removes
    # register-allocation contradictions.
    classes: dict = {}
    for n, instr in enumerate(after):
        if id(instr) in ident and not instr.is_label and instr.op != "nop":
            classes.setdefault(shape(instr), []).append(n)
    improved = True
    while improved:
        improved = False
        for members in classes.values():
            for i, j in itertools.combinations(members, 2):
                a, b = after[i], after[j]
                before_cost = local_cost(i) + local_cost(j)
                ident[id(a)], ident[id(b)] = ident[id(b)], ident[id(a)]
                a.web, b.web = web(ident[id(a)]), web(ident[id(b)])
                if local_cost(i) + local_cost(j) < before_cost:
                    improved = True
                else:
                    ident[id(a)], ident[id(b)] = ident[id(b)], ident[id(a)]
                    a.web, b.web = web(ident[id(a)]), web(ident[id(b)])
    # A rewrite whose registers still contradict its pairing is not the
    # same computation: count it as an insertion and its partner as a
    # deletion (both must then be dead).
    for instr in [i for i in rest if id(i) in ident]:
        n = index[id(instr)]
        if min(len(_reads(after, into, n, instr, twin, same)) for twin in _twins(instr)):
            deletes.append(ident.pop(id(instr)))
            instr.web = None
            inserts.append(instr)
            report.rewrites -= 1
    report.inserts, report.deletes = len(inserts), len(deletes)

    pb, pa = Program(before), Program(after)
    pb.same = pa.same = same
    report.flags += pa.problems
    if pb.anchors != pa.anchors:
        diff = [f"{x} -> {y}" for x, y in zip(pb.anchors, pa.anchors) if x != y][:3]
        report.flags.append("control flow changed: " + "; ".join(diff or ["anchor count"]))

    def identity(instr):
        return ident.get(id(instr), instr)

    pos_b = {id(i): k for k, i in enumerate(pb.order)}
    pos_a = {id(identity(i)): k for k, i in enumerate(pa.order)}
    block_a = {id(identity(i)): pa.block[id(i)] for i in pa.order}
    shared = [i for i in pb.order if id(i) in pos_a and not i.is_label]
    moved = {id(i) for i in shared}
    labels_after, labels_before = label_index(after), label_index(before)
    anchors = [a for a in pb.order if a.is_label or a.op in BRANCHES]
    loops = [(pb.position[id(lab)], pb.position[id(br)]) for br in pb.order
             if br.op in BRANCHES and br.args and br.args[-1][0] == "l"
             for lab in pb.order if lab.is_label and lab.args[0][1] == br.args[-1][1]
             and pb.position[id(lab)] < pb.position[id(br)]]
    for instr in shared:
        bb, ba = pb.block[id(instr)], block_a[id(instr)]
        if bb == ba:
            continue
        lo, hi = sorted((pos_b[id(instr)], pb_pos_of_after(pa, pb, instr, identity)))
        if not side_effect(instr) and instr.op not in BRANCHES:
            # A pure instruction may move between blocks: the allocation
            # check proves every reader sees it on every path and that it
            # reads the right values. A load must not cross a store that
            # may write its word (including the stores of a loop it leaves).
            if mem_access(instr):
                region = set(range(lo, hi + 1))
                for start, end in loops:
                    if lo < start <= hi or lo <= end < hi:
                        region |= set(range(start, end + 1))
                if any(pb.order[k].op in STORES and may_alias(pb, instr, pb.order[k]) for k in region):
                    report.flags.append(f"load moves past a possible store: {instr.text()}")
            continue
        lo_b, hi_b = min(bb, ba), max(bb, ba)
        for anchor in anchors:
            blk = pb.block[id(anchor)]
            if anchor.is_label and lo_b < blk <= hi_b:
                report.flags.append(f"moves across label {anchor.text()}: {instr.text()}")
            elif not anchor.is_label and lo_b <= blk < hi_b and anchor is not instr:
                target = anchor.args[-1][1] if anchor.args and anchor.args[-1][0] == "l" else None
                program = after_raw if ba < bb else before
                if target and dead_side_effect(program, instr, target, pb):
                    continue
                report.flags.append(f"moves a side effect across {anchor.text()}: {instr.text()}")
    # 3. Every conflicting pair keeps its order (web-level dependences).
    for k, x in enumerate(shared):
        for y in shared[k + 1:]:
            if id(x) not in moved and id(y) not in moved:
                continue
            if pos_a[id(x)] < pos_a[id(y)]:
                continue
            why = conflict(pb, x, y)
            if why:
                report.flags.append(f"{why}: {x.text()} now after {y.text()}")
    # 4. Registers hold the webs their readers expect.
    report.flags += allocation(after, same)
    if webs:
        count, problems = rotations(webs)
        report.rotations += count
        report.flags += problems
    # 5. Insertions and deletions must be dead code, or frameless save pairs.
    for instr in inserts:
        n = next(k for k, i in enumerate(after) if i is instr)
        if side_effect(instr) or any(live_after(after, n, r) for r in instr.defs()):
            if constant_feeds(after, into, n, values, same):
                report.constants += 1
                continue
            if copy_feeds(after, into, n, same):
                report.copies += 1
                continue
            report.flags.append(f"inserted live instruction: {instr.text()}")
        else:
            report.dead_inserts += 1
    for instr in deletes:
        # A deleted pure instruction is fine once the allocation check has
        # shown every remaining reader gets the value from elsewhere.
        if side_effect(instr):
            report.flags.append(f"deleted side effect: {instr.text()}")
    # 6. GTE waits.
    def waits(prog, ident_of):
        out, last = {}, None
        for k, instr in enumerate(prog):
            if instr.op in ("mtc2", "ctc2", "lwc2"):
                last = k
            if is_gte_command(instr):
                out[id(ident_of(instr))] = k - last - 1 if last is not None else 99
            if instr.is_label:
                last = None
        return out

    from asm_massage_rules import insert_hazard_nops
    wb = waits(insert_hazard_nops(list(before)), lambda i: i)
    wa = waits(insert_hazard_nops(list(after)), identity)
    for key, gap in wa.items():
        if key in wb and gap < min(2, wb[key]):
            report.flags.append(f"GTE command now {gap} instructions after a GTE register write")
    return report
