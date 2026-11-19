"""Shared rule catalogue for tools/project/asm_massage.py.

Each rule rewrites the instruction list into the form the original
hand-written renderers use. Rules are generic: they match dataflow, not
function-specific text, and each refuses (leaves the code alone) unless its
safety conditions hold. "web" rules run on def-use webs before the register
map; "physical" rules run on the mapped registers.

See notes/asm-massage.md for the catalogue and its justification.
"""

from __future__ import annotations

from dataclasses import dataclass
import re
from typing import Callable

from asm_massage import (
    BRANCHES, CALLEE_SAVED, LOADS, PRECOLORED, STORES, Instr, MassageError,
    dominators, is_delay_slot, label_index, live_after, successors, symbol_table,
)


class Context:
    def __init__(self, settings: dict):
        self.settings = settings
        self.fresh: dict[str, int] = {}
        # rule name -> (instructions before, instructions after)
        self.stats: dict[str, tuple[int, int]] = {}

    def new_reg(self, prefix: str) -> str:
        self.fresh[prefix] = self.fresh.get(prefix, 0) + 1
        return f"%{prefix}{self.fresh[prefix]}"


@dataclass
class Rule:
    name: str
    stage: str  # "web" or "physical"
    summary: str
    apply: Callable[[list[Instr], Context], list[Instr]]


RULES: dict[str, Rule] = {}


def rule(name: str, stage: str, summary: str):
    def register(function):
        RULES[name] = Rule(name, stage, summary, function)
        return function
    return register


def tag(instr: Instr, name: str) -> Instr:
    instr.origin = (instr.origin - {"gcc"}) | {name}
    return instr


def delete(instrs: list[Instr], indexes: set[int]) -> list[Instr]:
    """Remove INDEXES; a removed delay-slot instruction becomes a nop."""
    out = []
    for n, instr in enumerate(instrs):
        if n in indexes:
            if is_delay_slot(instrs, n):
                out.append(Instr("nop", [], {"delay"}))
            continue
        out.append(instr)
    return out


def defs_of(instrs: list[Instr], reg: str) -> list[int]:
    return [n for n, i in enumerate(instrs) if reg in i.defs()]


def uses_of(instrs: list[Instr], reg: str) -> list[int]:
    return [n for n, i in enumerate(instrs) if reg in i.uses()]


def straight(instrs: list[Instr], start: int, end: int) -> bool:
    """No label or branch strictly between START and END."""
    return all(not instrs[n].is_label and instrs[n].op not in BRANCHES
               and not is_delay_slot(instrs, n) for n in range(start + 1, end))


# ---------------------------------------------------------------------------


@rule("strip-nops", "web",
      "Drop nops outside delay slots (inline_c.h load-delay nops) except the "
      "GTE waits in front of a command; "
      "insert_hazard_nops re-derives them from "
      "the final order (delay-slot nops are instructions and stay).")
def strip_nops(instrs, context):
    def gte_wait(n):
        # The nops of gte_nclip()/gte_rtps() and friends give the GTE its two
        # cycles after the last register write; they stay.
        while n < len(instrs) and instrs[n].op == "nop":
            n += 1
        return n < len(instrs) and instrs[n].op == ".word"

    return [i for n, i in enumerate(instrs)
            if i.op != "nop" or is_delay_slot(instrs, n) or gte_wait(n)]


DMPSX = {
    0x0000007F: 0x4A180001,  # RTPS
    0x000000BF: 0x4A280030,  # RTPT
    0x00000FFF: 0x4AE80413,  # NCDS
    0x0000107F: 0x4B08041B,  # NCCS
    0x000010BF: 0x4B18043F,  # NCCT
    0x0000117F: 0x4B400006,  # NCLIP
    0x000011BF: 0x4B58002D,  # AVSZ3
    0x000011FF: 0x4B68002E,  # AVSZ4
}


@rule("dmpsx-native", "web",
      "inline_c.h GTE command markers are DMPSX placeholders; DMPSX replaced "
      "them with the native COP2 command word.")
def dmpsx_native(instrs, context):
    for instr in instrs:
        if instr.op == ".word":
            value = instr.args[0][1]
            if value in DMPSX:
                instr.args[0] = ("i", DMPSX[value])
                tag(instr, "dmpsx-native")
            elif value & 0x3F == 0x3F and value < 0x10000:
                raise MassageError(f"unknown DMPSX marker {value:#x}")
    return instrs


def stack_slot(instrs: list[Instr], arg: tuple):
    """Stack offset addressed by a memory operand, through $sp or a web
    defined once as $sp plus a constant."""
    if arg[0] != "m" or not isinstance(arg[1], int):
        return None
    if arg[2] == "$sp":
        return arg[1]
    # GCC may duplicate the address computation into each way into a join;
    # identical copies define the same address.
    values = {(instrs[d].op, instrs[d].args[1][1], instrs[d].args[2][1])
              if instrs[d].op == "addiu" else None for d in defs_of(instrs, arg[2])}
    if len(values) == 1:
        value = values.pop()
        if value is not None and value[1] == "$sp" and isinstance(value[2], int):
            return value[2] + arg[1]
    return None


@rule("gte-direct-read", "web",
      "gte_stflg/gte_stotz store a GTE register to a stack variable that C "
      "then reads; read it straight into the GPR with cfc2/mfc2 instead.")
def gte_direct_read(instrs, context):
    changed = True
    while changed:
        changed = False
        for j, store in enumerate(instrs):
            if store.op not in ("sw", "swc2"):
                continue
            slot = stack_slot(instrs, store.args[1])
            if slot is None:
                continue
            # The reads this store feeds: the slot loads before the next
            # store to the slot (the C variable may be reused, e.g. two
            # gte_stflg(&flag)); the first must be straight-line from it.
            later = [n for n in range(j + 1, len(instrs)) if instrs[n].op in STORES | {"lw"}
                     and instrs[n].args[1][0] == "m" and stack_slot(instrs, instrs[n].args[1]) == slot]
            following = [n for n in later if instrs[n].op in STORES]
            loads = [n for n in later if instrs[n].op == "lw" and (not following or n < following[0])]
            if len(loads) != 1 or not straight(instrs, j, loads[0]):
                continue
            load = loads[0]
            dest = instrs[load].args[0][1]
            if store.op == "sw":
                value = store.args[0][1]
                producers = defs_of(instrs, value)
                if (len(producers) != 1 or instrs[producers[0]].op != "cfc2"
                        or uses_of(instrs, value) != [j]):
                    continue
                read = producers[0]
                op, cop = "cfc2", instrs[read].args[1]
            else:
                read, op, cop = j, "mfc2", store.args[0]
            if not straight(instrs, read, load) or any(
                    instrs[n].op == ".word" or dest in instrs[n].uses()
                    for n in range(read + 1, load)):
                continue
            new = tag(Instr(op, [("r", dest), cop], set(instrs[read].origin)), "gte-direct-read")
            instrs = instrs[:read] + [new] + instrs[read + 1:]
            instrs = delete(instrs, {load} | ({j} if store.op == "sw" else set()))
            changed = True
            break
    return instrs


@rule("gte-direct-write", "web",
      "gte_ldrgb and friends load a GTE register from memory; C that has the "
      "value in a variable stores it to a stack slot first. Move it straight "
      "from the GPR with mtc2 instead.")
def gte_direct_write(instrs, context):
    changed = True
    while changed:
        changed = False
        for j, store in enumerate(instrs):
            if store.op != "sw":
                continue
            slot = stack_slot(instrs, store.args[1])
            if slot is None:
                continue
            users = [n for n, i in enumerate(instrs) if n != j and any(
                stack_slot(instrs, a) == slot for a in i.args if a[0] == "m")]
            if len(users) != 1 or instrs[users[0]].op != "lwc2" or users[0] < j:
                continue
            load = users[0]
            value = store.args[0][1]
            if not straight(instrs, j, load) or any(
                    value in instrs[n].defs() for n in range(j + 1, load)):
                continue
            new = tag(Instr("mtc2", [("r", value), instrs[load].args[0]],
                            set(instrs[load].origin)), "gte-direct-write")
            instrs = instrs[:load] + [new] + instrs[load + 1:]
            instrs = delete(instrs, {j})
            changed = True
            break
    return instrs


@rule("unspill", "web",
      "GCC spills loop-invariant values to the stack when it runs out of "
      "registers; the original keeps them in registers. A stack slot written "
      "once from a web that does not change afterwards and only reloaded "
      "reads that web; a spill that is never reloaded goes.")
def unspill(instrs, context):
    for j, store in enumerate(instrs):
        if store.op != "sw" or store.args[1][0] != "m" or store.args[1][2] != "$sp":
            continue
        slot = store.args[1][1]
        value = store.args[0][1]
        accesses = [n for n, i in enumerate(instrs) if i.op in STORES | LOADS
                    and len(i.args) > 1 and i.args[1][0] == "m" and i.args[1][2] == "$sp"
                    and i.args[1][1] == slot and n != j]
        if not accesses and not is_delay_slot(instrs, j):
            # A spill nobody reloads.
            return unspill(delete(instrs, {j}), context)
        if (not accesses or any(not reload_reads(instrs, instrs[n].op, value) or n < j
                                for n in accesses)
                or value in PRECOLORED or any(d > j for d in defs_of(instrs, value))
                or not (defs_of(instrs, value) or value.split(".")[0] in ("$a0", "$a1", "$a2", "$a3"))):
            continue
        gone = {j}
        for n in accesses:
            loaded = instrs[n].args[0][1]
            if len(defs_of(instrs, loaded)) != 1:
                # The reload feeds a merge (colour = template or poly): it
                # becomes a register copy.
                instrs[n] = tag(Instr("addu", [("r", loaded), ("r", value), ("r", "$zero")],
                                      set(instrs[n].origin)), "unspill")
                continue
            for i in instrs:
                if loaded in i.uses():
                    i.replace_reg(loaded, value, defs=False)
                    tag(i, "unspill")
            gone.add(n)
        return unspill(delete(instrs, gone), context)
    return promote_slots(instrs, context)


def promote_slots(instrs, context):
    """A stack slot GCC uses as the home of a variable that changes (a
    loop-carried value spilled for want of registers) becomes a register:
    each word store is a copy into it and each word load a copy out, which
    the register map then coalesces."""
    slots: dict = {}
    for n, i in enumerate(instrs):
        if "$sp" in i.uses() and not (i.op in ("lw", "sw") and i.args[1][0] == "m"
                                     and i.args[1][2] == "$sp" and isinstance(i.args[1][1], int)):
            if not (i.op == "addiu" and i.args[0] == ("r", "$sp")):
                for a in i.args:
                    if a[0] == "m" and a[2] == "$sp" and isinstance(a[1], int):
                        slots.setdefault(a[1] & ~3, []).append(None)
                if i.op == "addiu" and i.args[1] == ("r", "$sp"):
                    slots.setdefault(i.args[2][1] & ~3, []).append(None)
            continue
        if i.op in ("lw", "sw") and i.args[1][0] == "m" and i.args[1][2] == "$sp" \
                and i.args[0][1] not in CALLEE_SAVED + ["$ra"]:
            slots.setdefault(i.args[1][1], []).append(n)
    for slot, accesses in slots.items():
        if None in accesses or slot % 4:
            continue
        stores = [n for n in accesses if instrs[n].op == "sw"]
        loads = [n for n in accesses if instrs[n].op == "lw"]
        if len(stores) < 2 or not loads:
            continue  # single stores are unspill's
        reg = context.new_reg("slot")
        for n in accesses:
            i = instrs[n]
            if i.op == "sw":
                new = Instr("addu", [("r", reg), i.args[0], ("r", "$zero")], set(i.origin))
            else:
                new = Instr("addu", [i.args[0], ("r", reg), ("r", "$zero")], set(i.origin))
            instrs[n] = tag(new, "unspill")
        return promote_slots(instrs, context)
    return instrs


def reload_reads(instrs: list[Instr], op: str, value: str) -> bool:
    """Whether a reload with OP of a slot holding VALUE reads all of it: a
    word, or (little-endian) the low byte/halfword of a value that fits."""
    if op == "lw":
        return True
    bits = {"lbu": 8, "lhu": 16}.get(op)
    producers = defs_of(instrs, value)
    if bits is None or not producers:
        return False
    return all(instrs[p].op == "srl" and instrs[p].args[2][1] >= 32 - bits for p in producers)


def mem_only_uses(instrs: list[Instr], reg: str, allow_increment: bool = False) -> bool:
    for n in uses_of(instrs, reg):
        instr = instrs[n]
        if allow_increment and instr.op == "addiu" and instr.args[0][1] == reg and instr.args[1][1] == reg:
            continue
        schema = [a for a in instr.args if a[0] == "r" and a[1] == reg]
        mem = [a for a in instr.args if a[0] == "m" and a[2] == reg]
        if schema or not mem or any(not isinstance(a[1], int) for a in mem):
            return False
        if instr.op in STORES and instr.args[0] == ("r", reg):
            return False
    return True


def shift_mem(instr: Instr, old: str, new: str, delta: int) -> None:
    instr.args = [("m", a[1] + delta, new) if a[0] == "m" and a[2] == old else a
                  for a in instr.args]


@rule("fold-address", "web",
      "addiu t,b,K followed by memory accesses at 0(t): use K(b) directly "
      "(gte_stsxy0(&packet[3]) becomes swc2 $12,0xC(packet)).")
def fold_address(instrs, context):
    changed = True
    while changed:
        changed = False
        for n, instr in enumerate(instrs):
            if instr.op != "addiu" or not isinstance(instr.args[2][1], int):
                continue
            t, b, k = instr.args[0][1], instr.args[1][1], instr.args[2][1]
            if t in PRECOLORED or b == "$sp" or defs_of(instrs, t) != [n] or not mem_only_uses(instrs, t):
                continue
            uses = uses_of(instrs, t)
            if not uses or not all(u > n and straight(instrs, n, u)
                                   and all(b not in instrs[m].defs() for m in range(n + 1, u))
                                   for u in uses):
                continue
            for u in uses:
                shift_mem(instrs[u], t, b, k)
                tag(instrs[u], "fold-address")
            instrs = delete(instrs, {n})
            changed = True
            break
    return instrs


def induction(instrs: list[Instr], reg: str):
    """(base def index, increment indexes) when REG is defined once as
    another register plus a constant and otherwise only incremented."""
    base, steps = None, []
    for n in defs_of(instrs, reg):
        i = instrs[n]
        if i.op != "addiu" or not isinstance(i.args[2][1], int):
            return None
        if i.args[1][1] == reg:
            steps.append(n)
        elif base is None:
            base = n
        else:
            return None
    return (base, steps) if base is not None else None


@rule("undo-strength-reduction", "web",
      "GCC's loop optimiser biases pointers (record+18, packet+3) or keeps a "
      "second copy of a pointer; address everything from the real pointer.")
def undo_strength_reduction(instrs, context):
    changed = True
    while changed:
        changed = False
        for q in sorted({r for i in instrs for r in i.defs() if r not in PRECOLORED and r != "$at"}):
            shape = induction(instrs, q)
            if shape is None or not mem_only_uses(instrs, q, allow_increment=True):
                continue
            base_index, steps = shape
            p, k = instrs[base_index].args[1][1], instrs[base_index].args[2][1]
            if p in PRECOLORED:
                continue
            if not live_after(instrs, base_index, p):
                # The real pointer dies here: the biased one replaces it.
                for i in instrs:
                    if q in i.uses() or q in i.defs():
                        if any(a[0] == "m" and a[2] == q for a in i.args):
                            shift_mem(i, q, p, k)
                            tag(i, "undo-strength-reduction")
                        i.replace_reg(q, p)
                instrs = delete(instrs, {base_index})
                changed = True
                break
            delta = pointer_delta(instrs, p, q, base_index)
            if delta is None:
                continue
            for n, i in enumerate(instrs):
                if any(a[0] == "m" and a[2] == q for a in i.args):
                    shift_mem(i, q, p, delta[n])
                    tag(i, "undo-strength-reduction")
            instrs = delete(instrs, {base_index, *steps})
            changed = True
            break
    return instrs


def pointer_delta(instrs: list[Instr], p: str, q: str, base_index: int):
    """Constant q - p at the entry of every instruction that uses q, or None."""
    labels = label_index(instrs)
    preds = [[] for _ in instrs]
    for n in range(len(instrs)):
        for s in successors(instrs, n, labels):
            preds[s].append(n)
    UNDEF, CONFLICT = "undef", "conflict"
    into = [UNDEF] * len(instrs)
    out = [UNDEF] * len(instrs)
    for _ in range(4 * len(instrs)):
        changed = False
        for n, instr in enumerate(instrs):
            values = {out[m] for m in preds[n]} - {UNDEF}
            value = UNDEF if not values else values.pop() if len(values) == 1 else CONFLICT
            into[n] = value
            if n == base_index:
                value = instr.args[2][1]
            elif q in instr.defs():
                value = value + instr.args[2][1] if isinstance(value, int) else CONFLICT
            elif p in instr.defs():
                if instr.op == "addiu" and instr.args[1][1] == p and isinstance(value, int):
                    value -= instr.args[2][1]
                else:
                    value = CONFLICT
            if out[n] != value:
                out[n] = value
                changed = True
        if not changed:
            break
    result = {}
    for n, instr in enumerate(instrs):
        if any(a[0] == "m" and a[2] == q for a in instr.args):
            if not isinstance(into[n], int):
                return None
            result[n] = into[n]
    return result


@rule("halfword-words", "web",
      "Halfword vertex indices read from one record are loaded as the words "
      "that hold them (at the first index load, which dominates the rest) and "
      "extracted with shifts (little-endian), fusing the x8 scaling into the "
      "low-half extraction; two lone high halves share one lw+lwr register.")
def halfword_words(instrs, context):
    groups: dict = {}
    spans = loops(instrs)
    for n, instr in enumerate(instrs):
        if instr.op == "lhu" and isinstance(instr.args[1][1], int):
            # Group per pointer and per innermost loop around the load.
            around = [(a, b) for a, b in spans if a < n <= b + 1]
            loop = min(around, key=lambda ab: ab[1] - ab[0]) if around else None
            groups.setdefault((instr.args[1][2], loop), []).append(n)
    dom = None
    for (base, _), members in groups.items():
        if len(members) < 2:
            continue
        first, last = members[0], members[-1]
        # The first load dominates the rest; stores between must go through
        # another pointer: records are read-only model data, never the
        # packet buffer or OT the loop writes.
        dom = dom or dominators(instrs)
        if any(first not in dom[n] for n in members) or any(
                base in instrs[n].defs() or instrs[n].op in STORES and instrs[n].args[1][2] == base
                for n in range(first, last + 1)):
            continue
        offsets = {instrs[n].args[1][1] for n in members}
        words = sorted({off & ~3 for off in offsets})
        # lwr packing (little-endian): when a record needs more than four word
        # registers and only the high halves of words k-4 and k are used,
        # "lw x,k; lwr x,k-2" puts both in one register (half k-2 low, half
        # k+2 high), as the GT4 polygon drivers load norm0/vert0.
        packed = {}
        for w in words:
            if len(words) - len(packed) <= 4:
                break  # the drivers keep record words in t0-t3
            if (w - 4 in words and offsets & {w - 4, w - 2} == {w - 2}
                    and offsets & {w, w + 2} == {w + 2} and w - 4 not in packed.values()
                    and w not in packed and w - 4 not in packed):
                packed[w] = w - 4
        # A load past a branch whose word holds no other index, is not
        # lwr-packed and is read whole elsewhere (FT4's norm0, in the word
        # whose low half is u3/v3) stays a halfword load where C reads it.
        involved = set(packed) | set(packed.values())
        whole = {i.args[1][1] for i in instrs if i.op == "lw" and i.args[1][0] == "m"
                 and i.args[1][2] == base and isinstance(i.args[1][1], int)}
        lone = [n for n in members if n != first
                and not straight(instrs, first, n)
                and sum(1 for m in members if instrs[m].args[1][1] & ~3
                        == instrs[n].args[1][1] & ~3) == 1
                and instrs[n].args[1][1] & ~3 not in involved
                and instrs[n].args[1][1] & ~3 in whole]
        if lone:
            members = [n for n in members if n not in lone]
            if len(members) < 2:
                continue
            offsets = {instrs[n].args[1][1] for n in members}
            words = sorted({off & ~3 for off in offsets})
            packed = {w: v for w, v in packed.items() if w in words and v in words}
        words = [w for w in words if w not in packed.values()]
        regs = {w: context.new_reg("word") for w in words}
        replace: dict[int, list[Instr]] = {}
        for n in members:
            dest, off = instrs[n].args[0][1], instrs[n].args[1][1]
            if off & ~3 in packed.values():
                host = next(k for k, v in packed.items() if v == off & ~3)
                word, off = ("r", regs[host]), 0  # now the low half
            else:
                word = ("r", regs[off & ~3])
            following = [u for u in uses_of(instrs, dest)]
            scale = None
            if len(following) == 1 and instrs[following[0]].op == "sll" \
                    and instrs[following[0]].args[1][1] == dest \
                    and not any(instrs[m].is_label for m in range(n, following[0])):
                scale = following[0]
            if off & 2:
                new = [Instr("srl", [("r", dest), word, ("i", 16)])]
            elif scale is not None:
                amount = instrs[scale].args[2][1]
                new = [Instr("sll", [("r", dest), word, ("i", 16)])]
                fused = instrs[scale]
                instrs[scale] = Instr("srl", [fused.args[0], ("r", dest), ("i", 16 - amount)])
                tag(instrs[scale], "halfword-words")
            else:
                new = [Instr("andi", [("r", dest), word, ("i", 0xFFFF)])]
            replace[n] = [tag(i, "halfword-words") for i in new]
        loads = []
        for w in words:
            loads.append(tag(Instr("lw", [("r", regs[w]), ("m", w, base)]), "halfword-words"))
            if w in packed:
                loads.append(tag(Instr("lwr", [("r", regs[w]), ("m", w - 2, base)]), "halfword-words"))
        out = []
        for n, instr in enumerate(instrs):
            if n == first:
                out.extend(loads)
            if n in replace:
                out.extend(replace[n])
            else:
                out.append(instr)
        return halfword_words(out, context)
    return instrs


@rule("aligned-word-copy", "web",
      "GCC copies a 2-byte-aligned 4-byte struct (DVECTOR) with adjacent "
      "lwl/lwr and swl/swr pairs; the cache entries and packets are word "
      "aligned, so the original copies it with lw/sw. (Webs treat the pair "
      "as one load, so this rule must run before any rule that could split "
      "it.)")
def aligned_word_copy(instrs, context):
    from asm_massage import web_home

    out, n = [], 0
    while n < len(instrs):
        a = instrs[n]
        b = instrs[n + 1] if n + 1 < len(instrs) else None
        if (b is not None and (a.op, b.op) in (("lwl", "lwr"), ("swl", "swr"))
                and isinstance(a.args[1][1], int) and isinstance(b.args[1][1], int)
                and a.args[1][2] == b.args[1][2] and a.args[1][1] == b.args[1][1] + 3
                and web_home(a.args[0][1]) == web_home(b.args[0][1])
                and (a.op == "lwl" or a.args[0] == b.args[0])
                and not is_delay_slot(instrs, n + 1)):
            op = "lw" if a.op == "lwl" else "sw"
            out.append(tag(Instr(op, [b.args[0], b.args[1]], set(b.origin)), "aligned-word-copy"))
            n += 2
            continue
        out.append(a)
        n += 1
    return out


@rule("reuse-load", "web",
      "A second load of the same word through an unchanged pointer reuses the "
      "first (GCC reloads because packet stores might alias; packets never "
      "overlap the model data and vertex caches they are built from).")
def reuse_load(instrs, context):
    for n, later in enumerate(instrs):
        if later.op != "lw" or not isinstance(later.args[1][1], int):
            continue
        base, off = later.args[1][2], later.args[1][1]
        for m in range(n - 1, -1, -1):
            earlier = instrs[m]
            if earlier.is_label or base in earlier.defs():
                break
            if earlier.op in STORES and earlier.args[1][2] == base:
                break
            if earlier.op == "lw" and earlier.args[1] == ("m", off, base):
                kept, gone = earlier.args[0][1], later.args[0][1]
                if len(defs_of(instrs, kept)) != 1 or len(defs_of(instrs, gone)) != 1:
                    break
                for i in instrs:
                    if gone in i.uses():
                        i.replace_reg(gone, kept, defs=False)
                        tag(i, "reuse-load")
                return reuse_load(delete(instrs, {n}), context)
    return instrs


# --- value trees -----------------------------------------------------------


def value(instrs: list[Instr], reg: str, at: int, depth: int = 0):
    """Symbolic value of REG as read by instruction AT, from the definitions
    in AT's extended basic block."""
    if reg == "$zero":
        return ("const", 0)
    if depth > 8:
        return ("reg", reg)
    for n in range(at - 1, -1, -1):
        i = instrs[n]
        # Without a label the only way in is falling through (a delay slot
        # runs before its branch's fall-through), so walk back to a label.
        if i.is_label:
            break
        if reg not in i.defs():
            continue
        op, a = i.op, i.args
        if op == "lui" and isinstance(a[1][1], int):
            return ("const", (a[1][1] << 16) & 0xFFFFFFFF)
        if op in ("ori", "addiu", "andi") and isinstance(a[2][1], int):
            inner = value(instrs, a[1][1], n, depth + 1)
            if inner[0] == "const":
                v = {"ori": inner[1] | a[2][1], "addiu": inner[1] + a[2][1],
                     "andi": inner[1] & a[2][1]}[op]
                return ("const", v & 0xFFFFFFFF)
            if op == "andi":
                return ("and", inner, ("const", a[2][1]))
            return ("reg", reg)
        if op in ("and", "or"):
            return (op, value(instrs, a[1][1], n, depth + 1), value(instrs, a[2][1], n, depth + 1))
        if op == "sll":
            return ("sll", value(instrs, a[1][1], n, depth + 1), a[2][1])
        if op in LOADS and a[1][0] == "m":
            return ("load", op, a[1][1], a[1][2], n)
        return ("reg", reg)
    # Loop-invariant constants are defined outside the block.
    defs = defs_of(instrs, reg)
    if len(defs) == 1 and defs[0] < at:
        found = value(instrs, reg, defs[0] + 1, depth + 1)
        if found[0] == "const":
            return found
    return ("reg", reg)


def masked(tree, mask: int):
    """X for a tree (X & MASK) in either operand order."""
    if tree[0] != "and":
        return None
    for x, c in ((tree[1], tree[2]), (tree[2], tree[1])):
        if c == ("const", mask):
            # getaddr() then setaddr() masks the same value twice.
            return masked(x, mask) or x
    return None


def or_pair(tree):
    if tree[0] != "or":
        return []
    return [(tree[1], tree[2]), (tree[2], tree[1])]


@rule("tag-build", "web",
      "setlen(p,len)+addPrim(ot,p) becomes the hand-written single-word tag "
      "build: p->tag = len<<24 | ot->addr; ot = p & 0xFFFFFF (the OT entry's "
      "len byte is zero in every ordering table, see the note).")
def tag_build(instrs, context):
    for n, store in enumerate(instrs):
        if store.op != "sw" or store.args[1][1] != 0:
            continue
        p = store.args[1][2]
        found = None
        for hi, lo in or_pair(value(instrs, store.args[0][1], n)):
            old = masked(hi, 0xFF000000)
            link = masked(lo, 0x00FFFFFF)
            if (old and link and old[0] == "load" and old[1] == "lw" and old[2] == 0 and old[3] == p
                    and link[0] == "load" and link[1] == "lw" and link[2] == 0):
                found = link
        if found is None:
            continue
        slot, link_load = found[3], found[4]
        # The OT store: ot->addr = p, len byte kept by addPrim.
        ot_store = None
        # (up to the end of the block, including a closing delay slot)
        for m in range(n + 1, len(instrs)):
            i = instrs[m]
            if i.is_label or is_delay_slot(instrs, m - 1):
                break
            if i.op in BRANCHES:
                continue
            if i.op == "sw" and i.args[1] == ("m", 0, slot):
                for hi, lo in or_pair(value(instrs, i.args[0][1], m)):
                    old, link = masked(hi, 0xFF000000), masked(lo, 0x00FFFFFF)
                    if old and old[:4] == ("load", "lw", 0, slot) and link == ("reg", p):
                        ot_store = m
                break
        if ot_store is None:
            continue
        # setlen: sb LEN,3(p) before the tag store.
        length = None
        for m in range(n - 1, -1, -1):
            i = instrs[m]
            if i.is_label or i.op in BRANCHES:
                break
            if i.op == "sb" and i.args[1] == ("m", 3, p):
                v = value(instrs, i.args[0][1], m)
                if v[0] == "const":
                    length, sb_index = v[1], m
                break
        if length is None:
            continue
        if any(p in instrs[m].defs() for m in range(sb_index, n)):
            continue
        t = context.new_reg("tag")
        seq = [
            Instr("lw", [("r", t), ("m", 0, slot)]),
            Instr("sll", [("r", t), ("r", t), ("i", 8)]),
            Instr("ori", [("r", t), ("r", t), ("i", length)]),
            Instr("srl", [("r", "$at"), ("r", t), ("i", 8)]),
            Instr("sll", [("r", t), ("r", t), ("i", 24)]),
            Instr("or", [("r", t), ("r", t), ("r", "$at")]),
            Instr("sw", [("r", t), ("m", 0, p)]),
            Instr("sll", [("r", t), ("r", p), ("i", 8)]),
            Instr("srl", [("r", t), ("r", t), ("i", 8)]),
            Instr("sw", [("r", t), ("m", 0, slot)]),
        ]
        seq = [tag(i, "tag-build") for i in seq]
        if any(slot in instrs[m].defs() for m in range(link_load, ot_store)):
            continue
        # The OT entry is read where the C read it; the rest replaces the
        # packet tag store.
        out = (instrs[:link_load] + seq[:1] + instrs[link_load:n] + seq[1:]
               + instrs[n + 1:])
        removed = {sb_index + (1 if sb_index >= link_load else 0), ot_store + len(seq) - 1}
        out = delete(out, removed)
        return dce(out, context)
    return instrs


@rule("word-compose", "web",
      "C composes a word from adjacent byte/halfword globals (portable); the "
      "original read the containing word with one lw.")
def word_compose(instrs, context):
    table = symbol_table()

    def terms(tree, shift=0):
        if tree[0] == "or":
            left, right = terms(tree[1], shift), terms(tree[2], shift)
            return None if left is None or right is None else left + right
        if tree[0] == "sll":
            return terms(tree[1], shift + tree[2])
        if tree[0] == "load" and tree[1] in ("lbu", "lhu") and isinstance(tree[2], str):
            match = re.fullmatch(r"%lo\((\w+)\)", tree[2])
            if match:
                return [(match.group(1), 1 if tree[1] == "lbu" else 2, shift, tree[4])]
        if tree[0] == "load" and tree[1] in ("lbu", "lhu") and isinstance(tree[2], int):
            # Fields of one structure: (base register, offset).
            return [((tree[3], tree[2]), 1 if tree[1] == "lbu" else 2, shift, tree[4])]
        return None

    for n, instr in enumerate(instrs):
        if instr.op != "or":
            continue
        found = terms(value(instrs, instr.args[0][1], n + 1))
        if not found or len(found) < 2:
            continue
        found.sort(key=lambda t: t[2])
        base = found[0][0]
        if found[0][2] != 0:
            continue
        if isinstance(base, tuple):
            # Adjacent fields through one pointer (GsWORKUNIT otz and p):
            # the word is aligned because the structures are word aligned.
            reg, offset = base
            if (offset % 4 or any(not isinstance(t[0], tuple) or t[0][0] != reg for t in found)
                    or [t[0][1] - offset for t in found] != [t[2] // 8 for t in found]
                    or sum(t[1] for t in found) != 4):
                continue
            dest = instr.args[0][1]
            first = min(t[3] for t in found)
            if any(reg in instrs[m].defs() for m in range(first, n)):
                continue
            seq = [tag(Instr("lw", [("r", dest), ("m", offset, reg)]), "word-compose")]
            out = instrs[:first] + seq + instrs[first:]
            out = delete(out, {n + 1})
            return word_compose(dce(out, context), context)
        if base not in table:
            continue
        covered = 0
        for name, size, shift, _ in found:
            if name not in table or table[name] != table[base] + shift // 8 or shift != covered * 8:
                break
            covered += size
        else:
            if covered != 4 or table[base] % 4:
                continue
            dest = instr.args[0][1]
            first = min(t[3] for t in found)
            address = instrs[first].args[1][2]
            lui = [m for m in range(first) if instrs[m].op == "lui" and instrs[m].args[0][1] == address]
            start = lui[-1] if lui and straight(instrs, lui[-1], first) else first
            seq = [tag(Instr("lui", [("r", dest), ("i", f"%hi({base})")]), "word-compose"),
                   tag(Instr("lw", [("r", dest), ("m", f"%lo({base})", dest)]), "word-compose")]
            out = instrs[:start] + seq + instrs[start:]
            out = delete(out, {n + len(seq)})
            return word_compose(dce(out, context), context)
    return instrs


PURE = {
    "addu", "subu", "and", "or", "xor", "nor", "slt", "sltu", "sllv", "srlv",
    "srav", "sll", "srl", "sra", "addiu", "andi", "ori", "xori", "slti",
    "sltiu", "lui", "lb", "lbu", "lh", "lhu", "lw", "mfc2", "cfc2",
}


@rule("loop-load-forwarding", "web",
      "A loop that reloads a word it stored on every back edge (the OT entry "
      "re-read when the second pass links its packet) keeps the stored value "
      "in the register instead: the load moves in front of the loop.")
def loop_load_forwarding(instrs, context):
    labels = label_index(instrs)
    for name, start in labels.items():
        back = [n for n, i in enumerate(instrs) if n > start and i.op in BRANCHES
                and i.args and i.args[-1] == ("l", name)]
        if not back:
            continue
        end = max(back) + 1
        for n in range(start + 1, end):
            load = instrs[n]
            if load.is_label or load.op in BRANCHES:
                break
            if load.op != "lw" or load.args[1][1] != 0:
                continue
            t, base = load.args[0][1], load.args[1][2]
            if any(base in instrs[m].defs() for m in range(start, end + 1)):
                break
            if any(touches(instrs[m], {t}) or instrs[m].op in STORES and instrs[m].args[1][2] == base
                   for m in range(start + 1, n)):
                break
            ok = True
            for b in back:
                stores = [m for m in range(n + 1, b + 2) if instrs[m].op in STORES
                          and instrs[m].args[1][2] == base]
                if not stores or instrs[stores[-1]].args[:2] != [("r", t), ("m", 0, base)] \
                        or any(t in instrs[m].defs() for m in range(stores[-1] + 1, b + 2)):
                    ok = False
            if not ok:
                break
            moved = tag(load, "loop-load-forwarding")
            return loop_load_forwarding(instrs[:start] + [moved] + instrs[start:n] + instrs[n + 1:],
                                        context)
    return instrs


AT_OPS = {"and", "or", "xor", "nor", "addu", "subu", "slt", "sltu"}


def slot_dead_when_taken(instrs, slot: int) -> bool:
    """Whether the delay-slot instruction's results are unread on the
    taken path of its branch."""
    from asm_massage import label_index, live_after

    branch = instrs[slot - 1]
    if not branch.args or branch.args[-1][0] != "l":
        return False
    t = label_index(instrs).get(branch.args[-1][1])
    if t is None:
        return False
    for reg in instrs[slot].defs():
        if reg in instrs[t].uses() or (reg not in instrs[t].defs() and live_after(instrs, t, reg)):
            return False
    return True


@rule("at-constants", "web",
      "A large constant (lui, lui+ori) used only as an ALU operand is built in "
      "$at right before each use, as the assembler's immediate macros did "
      "(and t1,t1,0xFF9FFFFF), instead of being hoisted into its own "
      "register.")
def at_constants(instrs, context):
    from asm_massage import web_home

    for n, instr in enumerate(instrs):
        if instr.op != "lui" or not isinstance(instr.args[1][1], int):
            continue
        reg = instr.args[0][1]
        if web_home(reg) == "$at" or reg in PRECOLORED:
            continue
        chain = [n]
        if defs_of(instrs, reg) != [n]:
            continue
        follow = instrs[n + 1] if n + 1 < len(instrs) else None
        if (follow is not None and follow.op == "ori" and follow.args[1][1] == reg
                and uses_of(instrs, reg) == [n + 1] and len(defs_of(instrs, follow.args[0][1])) == 1):
            # lui x; ori y,x,lo: the constant is y.
            chain.append(n + 1)
            reg = follow.args[0][1]
        uses = uses_of(instrs, reg)
        if not uses or any(instrs[u].op not in AT_OPS or u in chain for u in uses):
            continue
        if any(instrs[u].op == "addu" and ("r", "$zero") in instrs[u].args[1:] for u in uses):
            continue  # a register copy: the constant is loaded into its register
        if any(live_at(instrs, u, "$at") for u in uses):
            continue
        # A use GCC put in a delay slot (from the fall-through path) leaves
        # the slot with its constant, which needs the taken path not to read
        # its result.
        if any(is_delay_slot(instrs, u) and not slot_dead_when_taken(instrs, u) for u in uses):
            continue
        value = instrs[chain[0]].args[1][1] << 16
        if len(chain) == 2:
            value |= instrs[chain[1]].args[2][1]
        out = []
        for m, i in enumerate(instrs):
            if m in chain:
                if is_delay_slot(instrs, m):
                    out.append(Instr("nop", [], {"at-constants"}))
                continue
            if m in uses:
                if is_delay_slot(instrs, m):
                    out.append(Instr("nop", [], {"at-constants"}))
                out.append(tag(Instr("lui", [("r", "$at"), ("i", value >> 16)]), "at-constants"))
                if value & 0xFFFF:
                    out.append(tag(Instr("ori", [("r", "$at"), ("r", "$at"), ("i", value & 0xFFFF)]),
                                   "at-constants"))
                i.replace_reg(reg, "$at", defs=False)
                if i.args[1] == ("r", "$at") and i.op in COMMUTATIVE:
                    i.args = [i.args[0], i.args[2], i.args[1]]
                tag(i, "at-constants")
            out.append(i)
        return at_constants(out, context)
    return instrs


@rule("at-branch-constants", "web",
      "A small nonzero constant (addiu r,$zero,imm) used only as a beq/bne "
      "operand is built in $at right before each branch, as the assembler's "
      "beq r,imm macro did, instead of being hoisted into its own register.")
def at_branch_constants(instrs, context):
    from asm_massage import web_home

    for n, small in enumerate(instrs):
        if (small.op != "addiu" or small.args[1] != ("r", "$zero")
                or not isinstance(small.args[2][1], int) or small.args[2][1] == 0):
            continue
        reg = small.args[0][1]
        if reg in PRECOLORED or web_home(reg) == "$at" or defs_of(instrs, reg) != [n]:
            continue
        uses = uses_of(instrs, reg)
        if not uses or any(instrs[u].op not in ("beq", "bne") for u in uses):
            continue
        if any(live_at(instrs, u, "$at") for u in uses):
            continue
        # Only a test of a loop-invariant value (beq mode,1): a constant the
        # loop compares fresh loads with stays in its own register.
        spans = loops(instrs)
        def invariant(u):
            other = [a[1] for a in instrs[u].args[:2] if a[0] == "r" and a[1] != reg]
            around = [(a, b) for a, b in spans if a < u <= b + 1]
            return all(not any(a < d <= b + 1 for a, b in around for d in defs_of(instrs, o))
                       for o in other)
        if not all(invariant(u) for u in uses):
            continue
        out = []
        for m, i in enumerate(instrs):
            if m == n:
                if is_delay_slot(instrs, m):
                    out.append(Instr("nop", [], {"at-branch-constants"}))
                continue
            if m in uses:
                out.append(tag(Instr("addiu", [("r", "$at"), ("r", "$zero"), small.args[2]]),
                               "at-branch-constants"))
                i.replace_reg(reg, "$at", defs=False)
                if i.args[0] == ("r", "$at"):
                    i.args = [i.args[1], i.args[0], i.args[2]]
                tag(i, "at-branch-constants")
            out.append(i)
        return at_branch_constants(out, context)
    return instrs


@rule("sign-extend-load", "web",
      "A (s8)/(s16) cast of a loaded byte or halfword is a signed load (lb, "
      "lh), not lbu/lhu followed by sll/sra.")
def sign_extend_load(instrs, context):
    widths = {"lbu": ("lb", 24), "lhu": ("lh", 16)}
    for n, load in enumerate(instrs):
        if load.op not in widths:
            continue
        signed, bits = widths[load.op]
        reg = load.args[0][1]
        uses = uses_of(instrs, reg)
        if len(uses) != 1 or defs_of(instrs, reg) != [n]:
            continue
        shl = instrs[uses[0]]
        if shl.op != "sll" or shl.args[1][1] != reg or shl.args[2][1] != bits:
            continue
        mid = shl.args[0][1]
        uses2 = uses_of(instrs, mid)
        if len(uses2) != 1 or len(defs_of(instrs, mid)) != 1:
            continue
        sra = instrs[uses2[0]]
        if sra.op != "sra" or sra.args[1][1] != mid or sra.args[2][1] != bits:
            continue
        dest = sra.args[0][1]
        # The result web has no other definition, so defining it at the
        # load is the same value wherever it is read.
        if defs_of(instrs, dest) != [uses2[0]] or any(
                dest in instrs[k].uses() for k in range(n + 1, uses2[0])):
            continue
        new = tag(Instr(signed, [("r", dest), load.args[1]], set(load.origin)), "sign-extend-load")
        instrs = instrs[:n] + [new] + instrs[n + 1:]
        return sign_extend_load(delete(instrs, {uses[0], uses2[0]}), context)
    return instrs


@rule("zero-extend-shifts", "web",
      "andi d,s,0xFFFF is written sll 16 / srl 16, as everywhere in the "
      "hand-written code; in a delay slot the sll stays in the slot and the "
      "srl starts the fall-through (d must be dead at the branch target).")
def zero_extend_shifts(instrs, context):
    labels = label_index(instrs)
    for n, instr in enumerate(instrs):
        if instr.op != "andi" or instr.args[2] != ("i", 0xFFFF):
            continue
        d, src = instr.args[0], instr.args[1]
        first = tag(Instr("sll", [d, src, ("i", 16)], set(instr.origin)), "zero-extend-shifts")
        second = tag(Instr("srl", [d, d, ("i", 16)]), "zero-extend-shifts")
        if is_delay_slot(instrs, n):
            branch = instrs[n - 1]
            if branch.op in ("j", "jr") or live_at(instrs, labels[branch.args[-1][1]], d[1]):
                continue
        # In a delay slot, the srl lands at the start of the fall-through.
        return zero_extend_shifts(instrs[:n] + [first, second] + instrs[n + 1:], context)
    return instrs


@rule("loop-constant", "web",
      "A small constant (addiu r,$zero,imm) that only one loop reads is set at "
      "the top of the loop body, as the original does, rather than hoisted "
      "by GCC's loop-invariant motion (a vacated delay slot becomes a nop).")
def loop_constant(instrs, context):
    for start, end in loops(instrs):
        for n in range(0, start):
            small = instrs[n]
            if small.op != "addiu" or small.args[1] != ("r", "$zero"):
                continue
            reg = small.args[0][1]
            if (reg in PRECOLORED or defs_of(instrs, reg) != [n]
                    or any(not start < u <= end + 1 for u in uses_of(instrs, reg))):
                continue
            tag(small, "loop-constant")
            hole = [Instr("nop", [], {"loop-constant"})] if is_delay_slot(instrs, n) else []
            return loop_constant(instrs[:n] + hole + instrs[n + 1:start + 1] + [small]
                                 + instrs[start + 1:], context)
    return instrs


@rule("zero-as-addiu", "web",
      "GCC's move d,$zero is written li d,0 (addiu d,$zero,0) in the "
      "hand-written code.")
def zero_as_addiu(instrs, context):
    for instr in instrs:
        if instr.op in ("addu", "or") and instr.args[1:] == [("r", "$zero"), ("r", "$zero")]:
            instr.op = "addiu"
            instr.args = [instr.args[0], ("r", "$zero"), ("i", 0)]
            tag(instr, "zero-as-addiu")
    return instrs


@rule("increment-sink", "web",
      "A pointer increment moves to the end of its block, past the accesses "
      "through that pointer (their offsets grow by the increment), as the "
      "hand-written code advances the packet pointer last.")
def increment_sink(instrs, context):
    for n, instr in enumerate(instrs):
        if (instr.op != "addiu" or instr.args[0] != instr.args[1] or instr.args[0][1] in PRECOLORED
                or not isinstance(instr.args[2][1], int) or is_delay_slot(instrs, n)):
            continue
        p, k = instr.args[0][1], instr.args[2][1]
        m = n + 1
        while m < len(instrs):
            i = instrs[m]
            if i.is_label or i.op in BRANCHES or p in i.defs():
                break
            if i.op == "addiu" and i.args[0] == i.args[1]:
                break  # another increment: never swap two of them back and forth
            if p in i.uses():
                if any(a[0] == "r" and a[1] == p for a in i.args) or not all(
                        isinstance(a[1], int) for a in i.args if a[0] == "m" and a[2] == p):
                    break
            m += 1
        if m == n + 1:
            continue
        for i in instrs[n + 1:m]:
            if p in i.uses():
                shift_mem(i, p, p, k)
                tag(i, "increment-sink")
        moved = tag(instr, "increment-sink")
        return increment_sink(instrs[:n] + instrs[n + 1:m] + [moved] + instrs[m:], context)
    return instrs


COP2_ACCESS = {"lwc2", "swc2", "mfc2", "mtc2", "cfc2", "ctc2"}


@rule("gte-load-early", "web",
      "inline_c.h loads the SZ registers together (gte_ldsz3/4) once every "
      "vertex has passed its test; the original writes each one right after "
      "the test of its own value, in that test's delay slot when it is "
      "empty and no store is waiting for it. A write whose value no earlier test reads does not cross a "
      "test. The early writes only change GTE registers that the skip path "
      "never reads before reloading them.")
def gte_load_early(instrs, context):
    for n, instr in enumerate(instrs):
        if instr.op != "mtc2" or is_delay_slot(instrs, n):
            continue
        source, cop = instr.args[0][1], instr.args[1]
        k, before_branches, into_slot = n, None, None
        while k > 0:
            j = k - 1
            prev = instrs[j]
            if prev.is_label or prev.op == ".word" or source in prev.defs():
                break
            if prev.op in COP2_ACCESS and (prev.op != "mtc2" or prev.args[1] == cop):
                break
            if is_delay_slot(instrs, j):
                branch = instrs[j - 1]
                if source in branch.uses():
                    # The test of this value: take its slot if it is empty.
                    before_branches = None
                    # An empty slot is taken unless a store waits right
                    # before the test (store-delay-slot puts it there).
                    if prev.op == "nop" and not (j >= 2 and instrs[j - 2].op in STORES):
                        into_slot = j
                    break
                if prev.op in COP2_ACCESS:
                    break
                if before_branches is None:
                    before_branches = k
                k = j - 1
                continue
            if prev.op in BRANCHES:
                break
            k = j
        if before_branches is not None:
            # No test of the value above: never cross another test.
            k = before_branches
        moved = tag(instr, "gte-load-early")
        if into_slot is not None:
            return gte_load_early(instrs[:into_slot] + [moved] + instrs[into_slot + 1:n]
                                  + instrs[n + 1:], context)
        if k != n:
            return gte_load_early(instrs[:k] + [moved] + instrs[k:n] + instrs[n + 1:], context)
    return instrs


@rule("store-delay-slot", "web",
      "A store right before a branch moves into the branch's empty delay "
      "slot (it ran on both paths before and still does).")
def store_delay_slot(instrs, context):
    for n, instr in enumerate(instrs[:-1]):
        if (instr.op in BRANCHES and instrs[n + 1].op == "nop" and n > 0
                and instrs[n - 1].op in STORES and not is_delay_slot(instrs, n - 1)
                and not instrs[n - 1].is_label):
            store = tag(instrs[n - 1], "store-delay-slot")
            return store_delay_slot(instrs[:n - 1] + [instr, store] + instrs[n + 2:], context)
    return instrs


COMMUTATIVE = {"addu", "and", "or", "xor", "nor"}


def loops(instrs: list[Instr]) -> list[tuple[int, int]]:
    """(label index, back-branch index) of every backward branch."""
    labels = label_index(instrs)
    return [(labels[i.args[-1][1]], n) for n, i in enumerate(instrs)
            if i.op in BRANCHES and i.args and i.args[-1][0] == "l"
            and labels.get(i.args[-1][1], len(instrs)) < n]


@rule("invariant-last", "web",
      "Commutative operand order is free in GCC; inside a loop the original "
      "writes the loop-invariant operand last (addu t5,index,vertices; "
      "addu v0,index,ot).")
def invariant_last(instrs, context):
    for start, end in loops(instrs):
        body = range(start, end + 2)
        for n in body:
            instr = instrs[n]
            if instr.op not in COMMUTATIVE or len(instr.args) != 3:
                continue
            first, second = instr.args[1][1], instr.args[2][1]

            def invariant(reg):
                return not any(reg in instrs[m].defs() for m in body)

            if invariant(first) and not invariant(second):
                instr.args = [instr.args[0], instr.args[2], instr.args[1]]
                tag(instr, "invariant-last")
    return instrs


@rule("dce", "web", "Delete pure instructions whose results are never read.")
def dce(instrs, context):
    changed = True
    while changed:
        changed = False
        dead = {n for n, i in enumerate(instrs) if i.op in PURE and i.defs()
                and i.defs()[0] not in PRECOLORED
                and not live_after(instrs, n, i.defs()[0])}
        if dead:
            instrs = delete(instrs, dead)
            changed = True
    return instrs


@rule("drop-self-moves", "physical",
      "A copy whose source and destination got the same register disappears.")
def drop_self_moves(instrs, context):
    dead = {n for n, i in enumerate(instrs)
            if i.op in ("addu", "or") and i.args[2] == ("r", "$zero") and i.args[0] == i.args[1]
            or i.op == "addiu" and i.args[0] == i.args[1] and i.args[2] == ("i", 0)}
    return delete(instrs, dead)


@rule("frameless", "physical",
      "No $sp frame: callee-saved registers are parked in the caller's "
      "argument block (setting: frameless = <arg reg> <first slot offset>).")
def frameless(instrs, context):
    setting = context.settings.get("frameless")
    if not setting:
        raise MassageError("frameless rule needs 'frameless = <reg> <offset>'")
    arg_reg, base = setting.split()
    base = int(base, 0)
    frame, saves, restores = set(), [], []
    for n, i in enumerate(instrs):
        if i.op == "addiu" and i.args[0] == ("r", "$sp") and i.args[1] == ("r", "$sp"):
            frame.add(n)
        elif i.op in ("sw", "lw") and i.args[1][0] == "m" and i.args[1][2] == "$sp" \
                and i.args[0][1] in CALLEE_SAVED:
            (saves if i.op == "sw" else restores).append(n)
    others = [i.text() for n, i in enumerate(instrs)
              if n not in frame | set(saves) | set(restores) and "$sp" in i.uses() + i.defs()]
    if others:
        raise MassageError("frameless: $sp still used by " + "; ".join(others))
    used = sorted({r for n, i in enumerate(instrs) for r in i.defs()
                   if r in CALLEE_SAVED and n not in restores}, key=CALLEE_SAVED.index)
    slot = {r: base + 4 * k for k, r in enumerate(used)}
    # Saves follow the argument unpacking: the leading loads through the
    # argument register (and through pointers loaded from it) and the
    # arithmetic on those values.
    unpacked, save_at = {arg_reg, "$zero"}, 0
    for n, i in enumerate(instrs):
        if n in frame or n in saves:
            continue
        if (i.op in PURE and i.op not in ("lui",) and set(i.uses()) <= unpacked
                and not set(i.defs()) & set(CALLEE_SAVED)):
            unpacked |= set(i.defs())
            continue
        save_at = n
        break
    # Restores open the exit block (after the last label before the return).
    returns = [n for n, i in enumerate(instrs) if i.op == "jr" and i.args[0] == ("r", "$ra")]
    if not returns:
        raise MassageError("frameless: no return")
    labels = [n for n in range(returns[0]) if instrs[n].is_label]
    restore_at = labels[-1] + 1 if labels else (min(restores) if restores else returns[0])
    out = []
    for n, i in enumerate(instrs):
        if n == save_at:
            out += [tag(Instr("sw", [("r", r), ("m", slot[r], arg_reg)]), "frameless") for r in used]
        if n == restore_at:
            out += [tag(Instr("lw", [("r", r), ("m", slot[r], arg_reg)]), "frameless") for r in used]
        if n in frame or n in saves or n in restores:
            if is_delay_slot(instrs, n):
                out.append(Instr("nop", [], {"delay"}))
            continue
        out.append(i)
    return out


# --- scheduling rules (physical registers) ---------------------------------


def live_at(instrs: list[Instr], index: int, reg: str) -> bool:
    """Whether REG may be read at or after instruction INDEX."""
    if reg in instrs[index].uses():
        return True
    if reg in instrs[index].defs():
        return False
    return live_after(instrs, index, reg)


def touches(instr: Instr, regs: set[str]) -> bool:
    return bool(regs & (set(instr.uses()) | set(instr.defs())))


def is_symbol_pair(instrs: list[Instr], n: int) -> bool:
    """lui r,%hi(sym) followed by a load r,%lo(sym)(r)."""
    if n + 1 >= len(instrs):
        return False
    hi, lo = instrs[n], instrs[n + 1]
    return (hi.op == "lui" and isinstance(hi.args[1][1], str) and hi.args[1][1].startswith("%hi")
            and lo.op in LOADS and lo.args[1][0] == "m" and lo.args[1][2] == hi.args[0][1]
            and lo.args[0][1] == hi.args[0][1])


def conditional(instr: Instr) -> bool:
    return instr.op in BRANCHES and instr.op not in ("j", "jr", "jal", "jalr")


@rule("guard-chain", "physical",
      "Adjacent early-exit tests to one target (nothing between them but the "
      "second test's operands): the global word the later "
      "test reads is loaded before the first test, and the first test's "
      "delay-slot work moves to the last test's slot (the count test keeps a "
      "nop), as the hand-written renderers check count and draw mode.")
def guard_chain(instrs, context):
    labels = label_index(instrs)
    for i, first in enumerate(instrs):
        if not conditional(first):
            continue
        target = first.args[-1][1]
        j = next((m for m in range(i + 2, len(instrs))
                  if instrs[m].is_label or instrs[m].op in BRANCHES), None)
        if j is None or instrs[j].is_label or not conditional(instrs[j]) \
                or instrs[j].args[-1][1] != target:
            continue
        exit_index = labels[target]
        condition = set(instrs[j].uses())
        # Only adjacent tests: everything between computes the second test's
        # operands (its global load and its constant).
        if any(not (set(instrs[m].defs()) & condition) for m in range(i + 2, j)) \
                or not any(is_symbol_pair(instrs, m) for m in range(i + 2, j)):
            continue
        # 1. Hoist the global loads feeding the later test.
        moved = []
        n = i + 2
        while n < j:
            if is_symbol_pair(instrs, n) and instrs[n].args[0][1] in condition:
                reg = instrs[n].args[0][1]
                if (not touches(first, {reg}) and not touches(instrs[i + 1], {reg})
                        and not any(touches(instrs[m], {reg}) for m in range(i + 2, n))
                        and not live_at(instrs, exit_index, reg)):
                    moved.append(n)
                    n += 2
                    continue
            n += 1
        if not moved:
            continue
        pairs = [instrs[m] for k in moved for m in (k, k + 1)]
        for instr in pairs:
            tag(instr, "guard-chain")
        gone = {m for k in moved for m in (k, k + 1)}
        instrs = (instrs[:i] + pairs + [x for m, x in enumerate(instrs[i:j + 2], i) if m not in gone]
                  + instrs[j + 2:])
        i += len(pairs)
        labels = label_index(instrs)
        exit_index = labels[target]
        # 2. Sink the first test's slot into the last test's slot.
        slot, last_slot = instrs[i + 1], instrs[j + 1]
        if slot.op == "nop":
            return instrs
        dests = set(slot.defs())
        sources = set(slot.uses())
        if (any(touches(instrs[m], dests) or set(instrs[m].defs()) & sources for m in range(i + 2, j + 1))
                or any(live_at(instrs, exit_index, d) for d in dests)):
            return instrs
        displaced = []
        if last_slot.op != "nop":
            if (touches(last_slot, dests | sources) or touches(slot, set(last_slot.defs()))
                    or any(live_at(instrs, exit_index, d) for d in last_slot.defs())):
                return instrs
            displaced = [tag(last_slot, "guard-chain")]
        instrs = (instrs[:i + 1] + [Instr("nop", [], {"guard-chain"})] + instrs[i + 2:j + 1]
                  + [tag(slot, "guard-chain")] + displaced + instrs[j + 2:])
        return instrs
    return instrs


@rule("preheader-order", "physical",
      "In the block before a loop, constants (lui/ori) follow the global "
      "template loads, as in the hand-written setup (template words, the "
      "terminator constant, the argument fields).")
def preheader_order(instrs, context):
    for start, _ in loops(instrs):
        begin = start
        while begin > 0 and not instrs[begin - 1].is_label and not is_delay_slot(instrs, begin - 1) \
                and instrs[begin - 1].op not in BRANCHES:
            begin -= 1
        pairs = [n for n in range(begin, start) if is_symbol_pair(instrs, n)]
        after = pairs[-1] + 2 if pairs else begin
        for n in range(begin, pairs[-1] if pairs else begin):
            hi = instrs[n]
            if hi.op != "lui" or not isinstance(hi.args[1][1], int):
                continue
            reg = hi.args[0][1]
            chain = [n]
            if n + 1 < start and instrs[n + 1].op == "ori" and instrs[n + 1].args[0][1] == reg \
                    and instrs[n + 1].args[1][1] == reg:
                chain.append(n + 1)
            if any(touches(instrs[m], {reg}) for m in range(chain[-1] + 1, after)):
                continue
            moving = [tag(instrs[m], "preheader-order") for m in chain]
            rest = [x for m, x in enumerate(instrs) if m not in chain]
            insert = after - len(chain)
            return preheader_order(rest[:insert] + moving + rest[insert:], context)
    return instrs


ALU = {
    "addu", "subu", "and", "or", "xor", "nor", "slt", "sltu", "sllv", "srlv",
    "srav", "sll", "srl", "sra", "addiu", "andi", "ori", "xori", "slti",
    "sltiu", "lui",
}


@rule("fill-load-delay", "physical",
      "Where a load's result is read by the next instruction, the original "
      "fills the load delay with the nearest ALU instruction above it "
      "(skipping stores and GTE operations) when that one can move, instead "
      "of a nop.")
def fill_load_delay(instrs, context):
    for i, load in enumerate(instrs):
        following = instrs[i + 1] if i + 1 < len(instrs) else None
        if following is None or is_delay_slot(instrs, i) or not needs_hazard_nop(load, following):
            continue
        for k in range(i - 1, -1, -1):
            candidate = instrs[k]
            if candidate.is_label or candidate.op in BRANCHES or is_delay_slot(instrs, k):
                break
            if candidate.op not in ALU:
                continue
            dests, sources = set(candidate.defs()), set(candidate.uses())
            if any(touches(x, dests) or set(x.defs()) & sources for x in instrs[k + 1:i + 1]):
                break
            moved = tag(candidate, "fill-load-delay")
            return fill_load_delay(instrs[:k] + instrs[k + 1:i + 1] + [moved] + instrs[i + 1:], context)
    return instrs


@rule("fill-branch-slot", "physical",
      "An empty delay slot of a conditional branch takes the nearest ALU "
      "instruction above it (skipping loads, stores and GTE operations) when "
      "that can move without creating a load-use hazard; otherwise it stays "
      "a nop.")
def fill_branch_slot(instrs, context):
    for n, branch in enumerate(instrs[:-1]):
        if not conditional(branch) or instrs[n + 1].op != "nop" or "guard-chain" in instrs[n + 1].origin:
            continue
        for k in range(n - 1, -1, -1):
            candidate = instrs[k]
            if candidate.is_label or candidate.op in BRANCHES or is_delay_slot(instrs, k):
                break
            if candidate.op not in ALU:
                continue
            dests, sources = set(candidate.defs()), set(candidate.uses())
            if any(touches(x, dests) or set(x.defs()) & sources for x in instrs[k + 1:n + 1]):
                break
            if k > 0 and needs_hazard_nop(instrs[k - 1], instrs[k + 1]):
                break
            moved = tag(candidate, "fill-branch-slot")
            return fill_branch_slot(instrs[:k] + instrs[k + 1:n + 1] + [moved] + instrs[n + 2:],
                                    context)
    return instrs


@rule("fill-jump-slot", "physical",
      "An empty delay slot of an unconditional jump takes the nearest small "
      "constant load (addiu r,$zero,imm) above it in its block, as the "
      "original sets the next pass's flag in the jump's slot.")
def fill_jump_slot(instrs, context):
    for n, jump in enumerate(instrs[:-1]):
        if jump.op != "j" or instrs[n + 1].op != "nop":
            continue
        for k in range(n - 1, -1, -1):
            candidate = instrs[k]
            if candidate.is_label or candidate.op in BRANCHES or is_delay_slot(instrs, k):
                break
            if not (candidate.op == "addiu" and candidate.args[1] == ("r", "$zero")):
                continue
            dests = set(candidate.defs())
            if any(touches(x, dests) for x in instrs[k + 1:n]):
                break
            if k > 0 and needs_hazard_nop(instrs[k - 1], instrs[k + 1]):
                break
            moved = tag(candidate, "fill-jump-slot")
            return fill_jump_slot(instrs[:k] + instrs[k + 1:n + 1] + [moved] + instrs[n + 2:],
                                  context)
    return instrs


def needs_hazard_nop(prev: Instr, nxt: Instr | None) -> bool:
    """MIPS I load delay: the instruction after a load (or mfc2/cfc2) must
    not read its destination."""
    if nxt is None or prev.op not in LOADS:
        return False
    return prev.args[0][1] in nxt.uses()


def insert_hazard_nops(instrs: list[Instr]) -> list[Instr]:
    out: list[Instr] = []
    real = [i for i in instrs]
    for n, instr in enumerate(real):
        out.append(instr)
        if instr.is_label:
            continue
        following = next((i for i in real[n + 1:] if not i.is_label), None)
        if needs_hazard_nop(instr, following):
            out.append(Instr("nop", [], {"hazard"}))
    return out
