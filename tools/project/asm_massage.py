#!/usr/bin/env python3
"""Massage GCC 2.8.1 output into a hand-written renderer's exact instructions.

Profiles with "asm_massage": true build through massage_unit (see
tools/project/build_baseline.py); see notes/asm-massage.md.

The C for a hand-written renderer is plain, portable C that uses only the
stock Psy-Q inline_c.h GTE macros and libgpu.h primitive macros, compiled
with the project's fixed -O2 flags. GCC's code is correct but is not the code
the original programmer wrote by hand. This tool rewrites GCC's raw -S
output (no MASPSX) in four steps:

0. A pre-pass expands the GCC/assembler pseudo-instructions (see
   parse_instruction) and gives every branch in a ".set reorder" region the
   delay-slot nop the assembler would have added, so the list is exactly
   machine instructions.

1. Shared rules (asm_massage_rules.RULES): named, documented rewrites for the
   hand optimisations that recur across the renderer family, for example the
   frameless prologue, native COP2 command words, direct GTE register reads,
   word loads of halfword indices and the single-word OT tag build.
2. A per-function register map, keyed by def-use webs ("$2.3" is the third
   live range GCC put in $2), giving the original's register for each web.
3. A per-function residual: a strict unified diff over the canonical text for
   what the rules cannot express (the original's scheduling).

Load-delay nops are then re-derived from the final order. The result is a
fully explicit noreorder/nomacro/noat file that GNU as encodes one-to-one,
so neither MASPSX nor the assembler inserts, removes or expands anything.

Commands (run from the repository root):

  asm_massage.py check  [EDIT...]
                                compile, massage, assemble, link and compare
                                each function with the retail executable and
                                prove its residual (a directory stands for
                                every edit file in it; none means every
                                massaged unit in the matching manifest)
  asm_massage.py apply  EDIT IN OUT
                                massage GCC's -S output IN into OUT; EDIT is
                                one edit file or a unit's .massage directory
  asm_massage.py derive EDIT    rewrite EDIT's [registers] and [residual]
                                from the retail bytes (authoring aid)
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass, field
import difflib
from pathlib import Path
import re
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))

PROFILES = "config/slus_01411/compiler_profiles.json"
# The named profile massaged units build with (compiler and flags).
PROFILE = "gcc_2_8_1_o2_massaged"
BINUTILS = "tools/toolchains/binutils-2.42/bin/mipsel-none-elf-"
EXECUTABLE = "game/SLUS_014.11"
SYMBOLS = "config/slus_01411/symbols.txt"

ABI = [
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra",
]
ABI_NUMBER = {name: number for number, name in enumerate(ABI)}
ABI_NUMBER["$s8"] = 30
# Registers with a fixed role; they are never split into webs.
PRECOLORED = {"$zero", "$sp", "$ra", "$gp", "$k0", "$k1"}
CALLEE_SAVED = [f"$s{n}" for n in range(8)] + ["$fp"]
RETURN_READS = ["$v0"] + CALLEE_SAVED


class MassageError(Exception):
    pass


# Operand kinds: d = GPR written, u = GPR read, i = immediate, m = memory
# (base register read), c = COP2 data register, k = COP2 control register,
# l = label.
SCHEMA = {
    **{op: "duu" for op in (
        "addu", "subu", "and", "or", "xor", "nor", "slt", "sltu",
        "sllv", "srlv", "srav", "add", "sub")},
    **{op: "dui" for op in (
        "sll", "srl", "sra", "addiu", "addi", "andi", "ori", "xori",
        "slti", "sltiu")},
    "lui": "di",
    **{op: "dm" for op in ("lb", "lbu", "lh", "lhu", "lw", "lwl", "lwr")},
    **{op: "um" for op in ("sb", "sh", "sw", "swl", "swr")},
    "lwc2": "cm", "swc2": "cm",
    "mfc2": "dc", "cfc2": "dk", "mtc2": "uc", "ctc2": "uk",
    "beq": "uul", "bne": "uul",
    "bltz": "ul", "bgez": "ul", "blez": "ul", "bgtz": "ul",
    "j": "l", "jr": "u", "jal": "l", "jalr": "du",
    "mult": "uu", "multu": "uu", "div": "uu", "divu": "uu",
    "mflo": "d", "mfhi": "d", "mtlo": "u", "mthi": "u",
    "nop": "", ".word": "i",
}
BRANCHES = {"beq", "bne", "bltz", "bgez", "blez", "bgtz", "j", "jr", "jal", "jalr"}
LOADS = {"lb", "lbu", "lh", "lhu", "lw", "lwl", "lwr", "mfc2", "cfc2"}
STORES = {"sb", "sh", "sw", "swl", "swr", "swc2"}


@dataclass
class Instr:
    op: str
    args: list = field(default_factory=list)
    origin: set = field(default_factory=lambda: {"gcc"})
    # Webs a return reads implicitly (set by webify).
    implicit: list | None = None
    # After renaming: the same instruction with web names (for checks).
    web: "Instr | None" = None
    # A rule-made instruction that stands for an instruction of the rule's
    # input (rotate-loop-loads: the mtc2 for the lwc2 it replaced).
    replaced: "Instr | None" = None

    @property
    def is_label(self) -> bool:
        return self.op == ":"

    def regs(self, kinds: str) -> list[str]:
        out = []
        for kind, arg in zip(SCHEMA.get(self.op, ""), self.args):
            if kind in kinds and kind in "du":
                out.append(arg[1])
            elif kind == "m" and "m" in kinds:
                out.append(arg[2])
        if self.op == "jal" and "d" in kinds:
            out.append("$ra")
        if self.op == "jr" and self.args == [("r", "$ra")] and "u" in kinds:
            # A return reads the result and every callee-saved register.
            out += RETURN_READS if self.implicit is None else self.implicit
        return out

    def defs(self) -> list[str]:
        return self.regs("d")

    def uses(self) -> list[str]:
        return self.regs("um")

    def replace_reg(self, old: str, new: str, *, defs: bool = True, uses: bool = True) -> None:
        schema = SCHEMA.get(self.op, "")
        for index, (kind, arg) in enumerate(zip(schema, self.args)):
            if kind == "d" and defs and arg[1] == old:
                self.args[index] = ("r", new)
            elif kind == "u" and uses and arg[1] == old:
                self.args[index] = ("r", new)
            elif kind == "m" and uses and arg[2] == old:
                self.args[index] = ("m", arg[1], new)

    def text(self) -> str:
        if self.is_label:
            return f"{self.args[0][1]}:"
        if not self.args:
            return self.op
        return f"{self.op} " + ", ".join(format_arg(self.op, a) for a in self.args)


def format_int(value: int) -> str:
    return f"-0x{-value:X}" if value < 0 else f"0x{value:X}"


def format_arg(op: str, arg: tuple) -> str:
    kind = arg[0]
    if kind == "r":
        return arg[1]
    if kind == "c":
        return f"${arg[1]}"
    if kind == "l":
        return arg[1]
    if kind == "m":
        off = arg[1] if isinstance(arg[1], str) else format_int(arg[1])
        return f"{off}({arg[2]})"
    if kind == "i":
        if isinstance(arg[1], str):
            return arg[1]
        if op == ".word":
            return f"0x{arg[1] & 0xFFFFFFFF:08X}"
        if op in ("sll", "srl", "sra"):
            return str(arg[1])
        return format_int(arg[1])
    raise MassageError(f"bad operand {arg}")


def canonical_reg(token: str) -> str:
    token = token.strip()
    if re.fullmatch(r"\$\d+", token):
        return ABI[int(token[1:])]
    if token in ABI_NUMBER:
        return ABI[ABI_NUMBER[token]]
    if re.fullmatch(r"\$[\w.]+|%\w+", token):
        return token  # virtual register
    raise MassageError(f"not a register: {token!r}")


def parse_int(token: str) -> int:
    return int(token.strip(), 0)


def split_operands(text: str) -> list[str]:
    parts, depth, current = [], 0, ""
    for char in text:
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
        if char == "," and depth == 0:
            parts.append(current.strip())
            current = ""
        else:
            current += char
    if current.strip():
        parts.append(current.strip())
    return parts


MEM = re.compile(r"^(.*?)\(\s*(\$[\w.]+)\s*\)$")


# Pseudo-instructions the pre-pass expands (what MASPSX/ASPSX expanded):
#   li d,imm           addiu d,$zero,imm | lui d,hi | ori d,$zero,imm | lui+ori
#   la d,sym           lui d,%hi(sym); addiu d,d,%lo(sym)
#   move d,s           addu d,s,$zero
#   negu d,s / not d,s subu d,$zero,s / nor d,s,$zero
#   lX d,sym           lui d,%hi(sym); lX d,%lo(sym)(d)    (load: own base)
#   sX r,sym           lui $at,%hi(sym); sX r,%lo(sym)($at)
#   addu/subu d,s,imm  addiu d,s,+-imm   (includes the $sp frame adjust)
#   and/or/xor/slt/sltu d,s,imm   andi/ori/xori/slti/sltiu
#   sll/srl/sra d,s,$r sllv/srlv/srav
#   beqz/bnez r,L, b L beq/bne r,$zero,L, beq $zero,$zero,L
#   j $r               jr $r
# Anything else that is not a machine instruction (div/mul/rem macros,
# branch-on-immediate) is rejected.
PSEUDO_OPS = {"li", "la", "move", "negu", "not", "beqz", "bnez", "b"}


def parse_instruction(line: str) -> list[Instr]:
    """Parse one assembly statement, expanding pseudo-instructions into the
    machine instructions listed above."""
    match = re.match(r"^(\.?\w+)\s*(.*)$", line.strip())
    if not match:
        raise MassageError(f"cannot parse {line!r}")
    op, rest = match.group(1), match.group(2)
    ops = split_operands(rest)
    reg = canonical_reg

    def imm(token: str):
        token = token.strip()
        if token.startswith("%"):
            return token
        return parse_int(token)

    if op == "nop":
        return [Instr("nop")]
    if op == ".word":
        return [Instr(".word", [("i", parse_int(ops[0]))])]
    if op == "move":
        return [Instr("addu", [("r", reg(ops[0])), ("r", reg(ops[1])), ("r", "$zero")])]
    if op == "negu":
        return [Instr("subu", [("r", reg(ops[0])), ("r", "$zero"), ("r", reg(ops[1]))])]
    if op == "not":
        return [Instr("nor", [("r", reg(ops[0])), ("r", reg(ops[1])), ("r", "$zero")])]
    if op == "la":
        symbol = ops[1].replace(" ", "")
        dest = ("r", reg(ops[0]))
        return [Instr("lui", [dest, ("i", f"%hi({symbol})")]),
                Instr("addiu", [dest, dest, ("i", f"%lo({symbol})")])]
    if op in ("div", "divu", "rem", "remu", "mul") and len(ops) == 3:
        raise MassageError(f"unsupported assembler macro {line!r}")
    if op == "li":
        value = parse_int(ops[1]) & 0xFFFFFFFF
        signed = value - (1 << 32) if value & 0x80000000 else value
        dest = ("r", reg(ops[0]))
        if -0x8000 <= signed < 0x8000:
            return [Instr("addiu", [dest, ("r", "$zero"), ("i", signed)])]
        if value & 0xFFFF == 0:
            return [Instr("lui", [dest, ("i", value >> 16)])]
        if value < 0x10000:
            return [Instr("ori", [dest, ("r", "$zero"), ("i", value)])]
        return [Instr("lui", [dest, ("i", value >> 16)]),
                Instr("ori", [dest, dest, ("i", value & 0xFFFF)])]
    if op in ("beqz", "bnez"):
        return [Instr("beq" if op == "beqz" else "bne",
                      [("r", reg(ops[0])), ("r", "$zero"), ("l", ops[1])])]
    if op == "b":
        return [Instr("beq", [("r", "$zero"), ("r", "$zero"), ("l", ops[0])])]
    if op == "j" and (ops[0] in ABI_NUMBER or re.fullmatch(r"\$\d+", ops[0])):
        return [Instr("jr", [("r", reg(ops[0]))])]
    if op in ("addu", "subu") and len(ops) == 3 and not ops[2].startswith("$"):
        value = parse_int(ops[2])
        return [Instr("addiu", [("r", reg(ops[0])), ("r", reg(ops[1])),
                                ("i", value if op == "addu" else -value)])]
    if op in ("sll", "srl", "sra") and len(ops) == 3 and ops[2].startswith("$"):
        op += "v"
    if op in ("and", "or", "xor", "slt", "sltu") and len(ops) == 3 and not ops[2].startswith("$"):
        return [Instr({"sltu": "sltiu"}.get(op, op + "i"), [("r", reg(ops[0])), ("r", reg(ops[1])), ("i", parse_int(ops[2]))])]
    schema = SCHEMA.get(op)
    if schema is None:
        raise MassageError(f"unsupported instruction {line!r}")
    if schema in ("dm", "um") and not MEM.match(ops[1]):
        # Absolute symbol access: lw $r,sym / sw $r,sym(+off)
        symbol = ops[1].replace(" ", "")
        if schema == "dm":
            base = reg(ops[0])
            return [Instr("lui", [("r", base), ("i", f"%hi({symbol})")]),
                    Instr(op, [("r", base), ("m", f"%lo({symbol})", base)])]
        return [Instr("lui", [("r", "$at"), ("i", f"%hi({symbol})")]),
                Instr(op, [("r", reg(ops[0])), ("m", f"%lo({symbol})", "$at")])]
    args = []
    if len(ops) != len(schema):
        raise MassageError(f"operand count mismatch in {line!r}")
    for kind, token in zip(schema, ops):
        if kind in "du":
            args.append(("r", reg(token)))
        elif kind in "ck":
            args.append(("c", int(token.strip().lstrip("$"))))
        elif kind == "i":
            args.append(("i", imm(token)))
        elif kind == "l":
            args.append(("l", token.strip()))
        elif kind == "m":
            mem = MEM.match(token.strip())
            if not mem:
                raise MassageError(f"bad memory operand in {line!r}")
            off = mem.group(1).strip() or "0"
            args.append(("m", imm(off), reg(mem.group(2))))
    return [Instr(op, args)]


SKIP_DIRECTIVES = (
    ".file", ".text", ".align", ".globl", ".ent", ".end", ".frame", ".mask",
    ".fmask", ".set", ".section", ".rdata", ".data", ".type", ".size",
)


def parse_function(source: str, function: str) -> list[Instr]:
    """Return the machine instruction list of FUNCTION from GCC -S output.

    GCC writes most code in the assembler's default ".set reorder" mode and
    only its own filled delay slots inside ".set noreorder" blocks. In
    reorder mode the assembler owns the delay slot; the pre-pass gives each
    such branch an explicit nop slot, as ASPSX did (load-delay nops are not
    added here: insert_hazard_nops derives them from the final order)."""
    instrs: list[Instr] = []
    inside = False
    reorder = True
    for raw in source.splitlines():
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        directive = re.match(r"^\.set\s+(no)?reorder\b", line)
        if directive:
            reorder = directive.group(1) is None
            continue
        if line.startswith(".ent"):
            inside = line.split()[1] == function
            continue
        if line.startswith(".end"):
            if inside:
                break
            continue
        if not inside or line.startswith(SKIP_DIRECTIVES):
            continue
        for statement in line.split(";"):
            statement = statement.strip()
            if not statement:
                continue
            label = re.match(r"^([\w.$]+):\s*(.*)$", statement)
            if label:
                if label.group(1) != function:
                    instrs.append(Instr(":", [("l", label.group(1))]))
                if label.group(2):
                    statement = label.group(2)
                else:
                    continue
            parsed = parse_instruction(statement)
            if reorder and any(i.op in BRANCHES for i in parsed):
                parsed.append(Instr("nop", [], {"delay"}))
            instrs.extend(parsed)
    if not instrs:
        raise MassageError(f"function {function} not found")
    return instrs


# --------------------------------------------------------------------------
# Control flow and def-use webs


def label_index(instrs: list[Instr]) -> dict[str, int]:
    return {i.args[0][1]: n for n, i in enumerate(instrs) if i.is_label}


def is_delay_slot(instrs: list[Instr], index: int) -> bool:
    return index > 0 and instrs[index - 1].op in BRANCHES


def successors(instrs: list[Instr], index: int, labels: dict[str, int]) -> list[int]:
    if instrs[index].op in BRANCHES:
        return [index + 1]
    if is_delay_slot(instrs, index):
        branch = instrs[index - 1]
        out = []
        if branch.op not in ("j", "jr"):
            out.append(index + 1)
        if branch.op in ("jr",):
            return []
        target = branch.args[-1][1]
        out.append(labels[target])
        return [n for n in out if n < len(instrs)]
    return [index + 1] if index + 1 < len(instrs) else []


def reaching(instrs: list[Instr]) -> list[set]:
    """Reaching definitions per instruction entry. A definition is
    (index, register); index -1 is the function entry (live-in)."""
    labels = label_index(instrs)
    preds = [[] for _ in instrs]
    for n in range(len(instrs)):
        for s in successors(instrs, n, labels):
            preds[s].append(n)
    regs = {r for i in instrs for r in i.defs() + i.uses()}
    entry = {(-1, r) for r in regs}
    into = [set() for _ in instrs]
    out = [set() for _ in instrs]
    changed = True
    while changed:
        changed = False
        for n, instr in enumerate(instrs):
            new_in = set(entry) if n == 0 else set()
            for p in preds[n]:
                new_in |= out[p]
            defs = set(instr.defs())
            new_out = {d for d in new_in if d[1] not in defs} | {(n, r) for r in defs}
            if new_in != into[n] or new_out != out[n]:
                into[n], out[n] = new_in, new_out
                changed = True
    return into


def dominators(instrs: list[Instr]) -> list[set]:
    """Instruction-level dominator sets (index 0 is the entry)."""
    labels = label_index(instrs)
    preds = [[] for _ in instrs]
    for n in range(len(instrs)):
        for s in successors(instrs, n, labels):
            preds[s].append(n)
    everything = set(range(len(instrs)))
    dom = [everything.copy() for _ in instrs]
    dom[0] = {0}
    changed = True
    while changed:
        changed = False
        for n in range(1, len(instrs)):
            new = set(everything)
            for p in preds[n]:
                new &= dom[p]
            new = (new if preds[n] else set()) | {n}
            if new != dom[n]:
                dom[n] = new
                changed = True
    return dom


def webify(instrs: list[Instr]) -> list[Instr]:
    """Rename every non-precoloured GPR to its def-use web ("$2.3" is the
    third web of GCC's $2 by first definition; "$a0.0" is a live-in)."""
    into = reaching(instrs)
    parent: dict = {}

    def find(x):
        parent.setdefault(x, x)
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    use_web: dict = {}
    for n, instr in enumerate(instrs):
        for r in set(instr.uses()):
            if r in PRECOLORED:
                continue
            defs = sorted(d for d in into[n] if d[1] == r)
            if not defs:
                raise MassageError(f"use of undefined {r} at {instr.text()}")
            find(defs[0])
            for d in defs[1:]:
                parent[find(d)] = find(defs[0])
            use_web[(n, r)] = defs[0]
        for r in instr.defs():
            find((n, r))
    groups: dict = {}
    for key in list(parent):
        groups.setdefault(find(key), []).append(key)
    names: dict = {}
    per_reg: dict = {}
    for root in sorted(groups, key=lambda k: min(d[0] for d in groups[k])):
        members = groups[root]
        reg = members[0][1]
        first = min(d[0] for d in members)
        if first == -1:
            name = f"{reg}.0"
        else:
            per_reg[reg] = per_reg.get(reg, 0) + 1
            name = f"{reg}.{per_reg[reg]}"
        for m in members:
            names[m] = name
    result = []
    for n, instr in enumerate(instrs):
        new = Instr(instr.op, list(instr.args), set(instr.origin))
        schema = SCHEMA.get(new.op, "")
        for index, (kind, arg) in enumerate(zip(schema, new.args)):
            if kind == "d" and arg[1] not in PRECOLORED:
                new.args[index] = ("r", names[find((n, arg[1]))])
            elif kind == "u" and arg[1] not in PRECOLORED:
                new.args[index] = ("r", names[find(use_web[(n, arg[1])])])
            elif kind == "m" and arg[2] not in PRECOLORED:
                new.args[index] = ("m", arg[1], names[find(use_web[(n, arg[2])])])
        if new.op == "jr" and new.args == [("r", "$ra")]:
            new.implicit = [names[find(use_web[(n, r)])] for r in RETURN_READS]
        result.append(new)
    return result


def web_home(name: str) -> str:
    """Physical register GCC used for a web."""
    return name.rsplit(".", 1)[0] if re.fullmatch(r"\$\w+\.\d+", name) else name


def live_after(instrs: list[Instr], index: int, reg: str) -> bool:
    """Whether REG may be read after instruction INDEX (webs are SSA-like,
    so any later read of the same web name, or a loop back edge, counts)."""
    labels = label_index(instrs)
    seen, stack = set(), list(successors(instrs, index, labels))
    while stack:
        n = stack.pop()
        if n in seen:
            continue
        seen.add(n)
        if reg in instrs[n].uses():
            return True
        if reg in instrs[n].defs():
            continue
        stack.extend(successors(instrs, n, labels))
    return False


HEADER = ["\t.set\tnoreorder", "\t.set\tnomacro", "\t.set\tnoat", "\t.text"]


def emit_function(instrs: list[Instr], function: str, label_prefix: str = ".L") -> list[str]:
    """One function's lines. Canonical labels are ".L<n>"; a unit of several
    functions gives each its own prefix so the labels stay distinct."""

    def relabel(arg: tuple) -> tuple:
        if arg[0] == "l" and arg[1].startswith(".L"):
            return ("l", label_prefix + arg[1][2:])
        return arg

    lines = ["\t.align\t2", f"\t.globl\t{function}", f"\t.ent\t{function}", f"{function}:"]
    for instr in instrs:
        instr = Instr(instr.op, [relabel(a) for a in instr.args], instr.origin)
        lines.append(instr.text() if instr.is_label else "\t" + instr.text())
    lines.append(f"\t.end\t{function}")
    return lines


def emit(instrs: list[Instr], function: str) -> str:
    return "\n".join(HEADER + emit_function(instrs, function)) + "\n"


def canonical_labels(instrs: list[Instr]) -> list[Instr]:
    """Drop unreferenced labels and number the rest from the end of the
    function (.L1 is the last label), so an extra or missing label in a
    function's setup does not renumber the loop and exit labels."""
    used = {i.args[-1][1] for i in instrs if i.op in BRANCHES and i.args and i.args[-1][0] == "l"}
    names = [i.args[0][1] for i in instrs if i.is_label and i.args[0][1] in used]
    mapping, out = {name: f".L{len(names) - k}" for k, name in enumerate(names)}, []
    for instr in instrs:
        if instr.is_label and instr.args[0][1] not in used:
            continue
        new = Instr(instr.op, list(instr.args), set(instr.origin), instr.implicit, instr.web,
                    instr.replaced)
        new.args = [("l", mapping[a[1]]) if a[0] == "l" else a for a in new.args]
        out.append(new)
    return out


# --------------------------------------------------------------------------
# Edit files


@dataclass
class EditFile:
    path: Path
    settings: dict
    registers: dict
    residual: list[str]

    @property
    def function(self) -> str:
        return self.settings["function"]


def read_edit(path: Path) -> EditFile:
    settings, registers, residual = {}, {}, []
    section = None
    for raw in path.read_text().splitlines():
        if section == "residual":
            residual.append(raw)
            continue
        line = raw.split("#", 1)[0].strip() if not raw.startswith("#") else ""
        if not line:
            continue
        if line.startswith("[") and line.endswith("]"):
            section = line[1:-1]
            continue
        key, _, value = line.partition("=")
        key, value = key.strip(), value.strip()
        if section is None:
            settings[key] = value
        elif section == "registers":
            registers[key] = canonical_reg(value)
        else:
            raise MassageError(f"unknown section [{section}] in {path}")
    for key in ("function", "source", "address", "size", "rules"):
        if key not in settings:
            raise MassageError(f"{path}: missing {key}")
    while residual and not residual[-1].strip():
        residual.pop()
    return EditFile(path, settings, registers, residual)


def write_edit(edit: EditFile, registers: dict, residual: list[str],
               notes: dict | None = None) -> None:
    head = []
    for raw in edit.path.read_text().splitlines():
        if raw.strip() in ("[registers]", "[residual]"):
            break
        head.append(raw)
    while head and not head[-1].strip():
        head.pop()
    lines = head + ["", "[registers]"]

    def order(name):
        home, _, n = name.partition(".")
        return (ABI_NUMBER.get(home, 99), int(n) if n.isdigit() else 0, name)

    notes = notes or {}
    lines += [f"{k} = {registers[k]}".ljust(16) + (f"  # {notes[k]}" if k in notes else "")
              for k in sorted(registers, key=order)]
    lines += ["", "[residual]"] + residual
    edit.path.write_text("\n".join(lines) + "\n")


# --------------------------------------------------------------------------
# Residual diff


def make_residual(old: list[str], new: list[str]) -> list[str]:
    return [line.rstrip("\n") for line in difflib.unified_diff(
        old, new, "rules", "retail", n=1, lineterm="")]


HUNK = re.compile(r"^@@ -(\d+)(?:,(\d+))? \+(\d+)(?:,(\d+))? @@")


def apply_residual(lines: list[Instr], patch: list[str]) -> list[Instr]:
    """Apply a strict unified diff to the canonical text of LINES. Context
    must match exactly at the recorded position; added lines are parsed and
    tagged with the "residual" origin."""
    if not patch:
        return lines
    out: list[Instr] = []
    cursor = 0
    body = [p for p in patch if not p.startswith(("---", "+++"))]
    n = 0
    while n < len(body):
        match = HUNK.match(body[n])
        if not match:
            raise MassageError(f"bad residual line: {body[n]!r}")
        start = int(match.group(1)) - (0 if match.group(2) == "0" else 1)
        out.extend(lines[cursor:start])
        cursor = start
        n += 1
        while n < len(body) and not body[n].startswith("@@"):
            tag, text = body[n][:1], body[n][1:]
            if tag in (" ", "-"):
                if cursor >= len(lines) or lines[cursor].text() != text:
                    found = lines[cursor].text() if cursor < len(lines) else "<end>"
                    raise MassageError(
                        f"residual context mismatch at line {cursor + 1}: "
                        f"expected {text!r}, found {found!r}")
                if tag == " ":
                    out.append(lines[cursor])
                cursor += 1
            elif tag == "+":
                out.append(parse_text_line(text, {"residual"}))
            else:
                raise MassageError(f"bad residual line: {body[n]!r}")
            n += 1
    out.extend(lines[cursor:])
    return out


def parse_text_line(text: str, origin: set) -> Instr:
    text = text.strip()
    if text.endswith(":"):
        return Instr(":", [("l", text[:-1])], set(origin))
    parsed = parse_instruction(text)
    if len(parsed) != 1:
        raise MassageError(f"residual line expands to several instructions: {text!r}")
    parsed[0].origin = set(origin)
    return parsed[0]


# --------------------------------------------------------------------------
# Pipeline


def count(instrs: list[Instr]) -> int:
    return sum(1 for i in instrs if not i.is_label)


def apply_rule(rule, instrs: list[Instr], context) -> list[Instr]:
    before = count(instrs)
    instrs = rule.apply(instrs, context)
    context.stats[rule.name] = (before, count(instrs))
    return instrs


def run_pipeline(edit: EditFile, gcc_text: str, *, registers=None, capture: dict | None = None,
                 residual=None, stage: str = "final", context=None):
    """Return the massaged instruction list. STAGE "rules" stops before the
    register map, "renamed" before the residual."""
    import asm_massage_rules as rules_module

    registers = edit.registers if registers is None else registers
    residual = edit.residual if residual is None else residual
    instrs = parse_function(gcc_text, edit.function)
    instrs = webify(instrs)
    names = edit.settings["rules"].split()
    context = rules_module.Context(edit.settings) if context is None else context
    for name in names:
        if name not in rules_module.RULES:
            raise MassageError(f"unknown rule {name}")
        rule = rules_module.RULES[name]
        if rule.stage == "web":
            if capture is not None:
                # Each web rule's input, for checks against the program
                # before the rule (rotations).
                capture.setdefault("inputs", {})[name] = instrs
            instrs = apply_rule(rule, instrs, context)
    if stage == "rules":
        return canonical_labels(instrs)
    instrs = rename(instrs, registers)
    for name in names:
        rule = rules_module.RULES[name]
        if rule.stage == "physical":
            instrs = apply_rule(rule, instrs, context)
    instrs = canonical_labels(instrs)
    if capture is not None:
        capture["dropped"] = context.dropped
    if stage == "renamed":
        return instrs
    instrs = apply_residual(instrs, residual)
    return rules_module.insert_hazard_nops(instrs)


# Directives that would put anything but code in a massaged unit. GCC emits
# them for string literals, jump tables, initialised or tentative data; the
# massage only rewrites .text, so a unit with any of them is rejected rather
# than silently losing the data. Inside a function, .word is an inline_c.h
# GTE command word and is kept as an instruction.
SECTION_DIRECTIVES = re.compile(r"^\.(data|rdata|sdata|bss|sbss|section|comm|lcomm)\b")
DATA_DIRECTIVES = re.compile(r"^\.(word|half|hword|byte|ascii|asciiz|space|float|double)\b")


def unit_functions(gcc_text: str) -> list[str]:
    """The functions of GCC's -S output, in output (source) order."""
    names = []
    inside = False
    for raw in gcc_text.splitlines():
        line = raw.split("#", 1)[0].strip()
        if SECTION_DIRECTIVES.match(line) or (not inside and DATA_DIRECTIVES.match(line)):
            raise MassageError(f"massaged units may hold only code: {line}")
        if line.startswith(".ent"):
            names.append(line.split()[1])
            inside = True
        elif line.startswith(".end"):
            inside = False
    return names


def edit_directory(source: Path) -> Path:
    """Where a massaged source keeps its edit files:
    src/game/foo.c -> src/game/foo.massage/<function>.massage."""
    return source.with_suffix(".massage")


def read_edits(directory: Path) -> dict[str, EditFile]:
    edits = {}
    for path in sorted(directory.glob("*.massage")):
        edit = read_edit(path)
        if edit.function != path.stem:
            raise MassageError(f"{path}: function {edit.function} does not match the file name")
        edits[edit.function] = edit
    if not edits:
        raise MassageError(f"{directory}: no edit files")
    return edits


def massage_unit(gcc_text: str, edits: dict[str, EditFile]) -> str:
    """Massage every function of a translation unit, in GCC's order, each
    with its own edit file and label prefix (.L<function>_<n>)."""
    names = unit_functions(gcc_text)
    missing = sorted(set(names) - set(edits))
    if missing or len(set(names)) != len(names):
        # Edit files without a function are allowed: a source built as
        # several objects (split by defines) shares one edit directory, and
        # matching_source_contracts checks the directory against the manifest.
        raise MassageError("no edit for " + ", ".join(missing) if missing else "duplicate function")
    lines = list(HEADER)
    for name in names:
        lines += emit_function(run_pipeline(edits[name], gcc_text), name, f".L{name}_")
    return "\n".join(lines) + "\n"


def web_chains(instrs: list[Instr]) -> dict[str, str]:
    """Map each web to the first web of its chain. A web whose only reader
    defines another web in the same GCC register (lui/lw pairs, srl/sll/addu
    index chains, two-address updates) continues that chain, so one map
    entry names the register of the whole chain."""
    parent: dict[str, str] = {}

    def find(x: str) -> str:
        parent.setdefault(x, x)
        while parent[x] != x:
            x = parent[x]
        return x

    readers: dict[str, list[int]] = {}
    for n, instr in enumerate(instrs):
        for r in set(instr.uses()):
            readers.setdefault(r, []).append(n)
    for n, instr in enumerate(instrs):
        for d in instr.defs():
            for u in set(instr.uses()):
                if (u != d and readers.get(u) == [n] and web_home(u) == web_home(d)
                        and web_home(d) != d):
                    a, b = find(u), find(d)
                    if a != b:
                        parent[max(a, b, key=web_order)] = min(a, b, key=web_order)
    return {w: find(w) for w in parent}


def web_order(name: str):
    home, _, n = name.partition(".")
    return (home, int(n) if n.isdigit() else 0)


def rename(instrs: list[Instr], registers: dict) -> list[Instr]:
    chains = web_chains(instrs)
    registers = dict(registers)
    for web, head in chains.items():
        if web not in registers and head in registers:
            registers[web] = registers[head]
    out = []
    for instr in instrs:
        new = Instr(instr.op, list(instr.args), set(instr.origin))
        schema = SCHEMA.get(new.op, "")
        for index, (kind, arg) in enumerate(zip(schema, new.args)):
            if kind in "du":
                new.args[index] = ("r", physical(arg[1], registers))
            elif kind == "m":
                new.args[index] = ("m", arg[1], physical(arg[2], registers))
        if instr.implicit is not None:
            new.implicit = [physical(r, registers) for r in instr.implicit]
        new.web = instr
        out.append(new)
    return out


def physical(name: str, registers: dict) -> str:
    if name in registers:
        return registers[name]
    if name in ABI_NUMBER:
        return name
    home = web_home(name)
    if home in ABI_NUMBER:
        return home
    raise MassageError(f"register map has no entry for {name}")


# --------------------------------------------------------------------------
# Toolchain and retail comparison


def run(command: list[str], **kwargs) -> subprocess.CompletedProcess:
    return subprocess.run(command, cwd=ROOT, check=True, **kwargs)


def compile_c(source: Path, work: Path) -> str:
    """gcc -S under the massaged profile; the massage consumes this directly."""
    import json

    profile = json.loads((ROOT / PROFILES).read_text())["profiles"][PROFILE]
    raw = work / (source.stem + ".gcc.s")
    run([str(ROOT / profile["compiler"]), "-S", *profile["compiler_flags"],
         "-o", str(raw), str(source)])
    return raw.read_text()


def check_source_contract(text: str) -> list[str]:
    """The C may only reach the hardware through stock Psy-Q macros."""
    problems = []
    text = re.sub(r"/\*.*?\*/|//[^\n]*", " ", text, flags=re.S)
    if re.search(r"\b(asm|__asm__|__asm)\b", text):
        problems.append("inline assembly outside inline_c.h")
    if re.search(r"\bregister\b", text):
        problems.append("register variables")
    return problems


def argument_layout(edit: EditFile) -> str | None:
    """The type of the function's pointer argument in its C source (which
    GsARGUNIT layout names the regions of its fields), or None."""
    text = (ROOT / edit.settings["source"]).read_text()
    match = re.search(rf"\b{re.escape(edit.function)}\s*\(\s*(?:const\s+)?(\w+)\s*\*", text)
    return match.group(1) if match else None


def symbol_table() -> dict[str, int]:
    table = {}
    for line in (ROOT / SYMBOLS).read_text().splitlines():
        match = re.match(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);", line)
        if match:
            table[match.group(1)] = int(match.group(2), 16)
    return table


def retail_words(address: int, size: int) -> list[int]:
    data = (ROOT / EXECUTABLE).read_bytes()
    text_address = struct.unpack_from("<I", data, 0x18)[0]
    offset = address - text_address + 0x800
    return [struct.unpack_from("<I", data, offset + i)[0] for i in range(0, size, 4)]


def assemble_and_link(asm_text: str, edit: EditFile, work: Path) -> tuple[bytes, list[str]]:
    stem = edit.path.stem
    asm = work / f"{stem}.massaged.s"
    obj = work / f"{stem}.o"
    elf = work / f"{stem}.elf"
    binary = work / f"{stem}.bin"
    asm.write_text(asm_text)
    # Assemble and link exactly as the build does: under
    # USE_SYSTEM_MIPS_BINUTILS=1 (CI) the packaged assembler pads and
    # 16-byte-aligns .text, which would shift a function that does not start
    # on a 16-byte boundary unless the build's compatibility flags and
    # alignment normalization are applied here too.
    import build_baseline

    run([str(ROOT / (BINUTILS + "as")), "-EL", "-mips1", "-G0",
         *build_baseline.assembler_compatibility_flags(), "-o", str(obj), str(asm)])
    build_baseline.normalize_text_alignment(ROOT, obj)
    relocs = run([str(ROOT / (BINUTILS + "readelf")), "-rW", str(obj)],
                 capture_output=True).stdout.decode()
    referenced = sorted(set(re.findall(r"R_MIPS_(?:HI16|LO16)\s+\S+\s+(\w+)", relocs)))
    table = symbol_table()
    defsyms = []
    for name in referenced:
        if name not in table:
            raise MassageError(f"no address for {name} in {SYMBOLS}")
        defsyms += ["--defsym", f"{name}=0x{table[name]:08X}"]
    run([str(ROOT / (BINUTILS + "ld")), "-EL", *build_baseline.linker_compatibility_flags(),
         "-Ttext", edit.settings["address"],
         "-e", edit.function, *defsyms, "-o", str(elf), str(obj)])
    run([str(ROOT / (BINUTILS + "objcopy")), "-O", "binary", "-j", ".text", str(elf), str(binary)])
    text_relocs = relocs.split("'.rel.text'", 1)[1].split("Relocation section", 1)[0] if "'.rel.text'" in relocs else ""
    reloc_lines = [line.split()[2] + " " + line.split()[4]
                   for line in text_relocs.splitlines() if "R_MIPS_" in line]
    return binary.read_bytes(), reloc_lines


def retail_text(edit: EditFile) -> list[Instr]:
    """Retail instructions in the canonical syntax, with %hi/%lo pairs
    resolved to symbols and branch targets turned into labels."""
    import rabbitizer

    rabbitizer.config.regNames_gprAbiNames = rabbitizer.Abi.O32
    rabbitizer.config.pseudos_enablePseudos = False
    address = int(edit.settings["address"], 16)
    words = retail_words(address, int(edit.settings["size"], 16))
    by_address = {v: k for k, v in symbol_table().items()}
    instrs, targets = [], {}
    for n, word in enumerate(words):
        decoded = rabbitizer.Instruction(word, vram=address + 4 * n)
        text = decoded.disassemble()
        if decoded.isBranch() or decoded.getOpcodeName() in ("j", "jal"):
            target = decoded.getBranchVramGeneric() if decoded.isBranch() else decoded.getInstrIndexAsVram()
            targets.setdefault(target, f"$R{target:08X}")
            op, _, operands = text.partition(" ")
            registers = split_operands(operands)[:-1]
            text = f"{op} " + ", ".join(registers + [targets[target]])
        elif text.startswith(".word") or "INVALID" in text:
            text = f".word 0x{word:08X}"
        instrs.append((address + 4 * n, parse_instruction(re.sub(r"\s+", " ", text))[0]))
    # Pair lui with the next %lo user of the same register.
    for n, (_, instr) in enumerate(instrs):
        if instr.op != "lui":
            continue
        base = instr.args[0][1]
        for _, later in instrs[n + 1:]:
            mem = [a for a in later.args if a[0] == "m" and a[2] == base]
            if mem and isinstance(mem[0][1], int):
                value = ((instr.args[1][1] << 16) + mem[0][1]) & 0xFFFFFFFF
                if value in by_address:
                    name = by_address[value]
                    instr.args[1] = ("i", f"%hi({name})")
                    later.args = [("m", f"%lo({name})", a[2]) if a is mem[0] else a for a in later.args]
                break
            if base in later.defs():
                break
    out = []
    for vram, instr in instrs:
        if vram in targets:
            out.append(Instr(":", [("l", targets[vram])]))
        out.append(instr)
    return canonical_labels(out)


def strip_hazard_nops(instrs: list[Instr]) -> list[Instr]:
    """Remove the nops insert_hazard_nops would add back."""
    import asm_massage_rules as rules_module

    out = []
    for n, instr in enumerate(instrs):
        if (instr.op == "nop" and not is_delay_slot(instrs, n) and out
                and rules_module.needs_hazard_nop(out[-1], instrs[n + 1] if n + 1 < len(instrs) else None)):
            continue
        out.append(instr)
    return out


def origin_counts(instrs: list[Instr]) -> dict[str, int]:
    counts: dict[str, int] = {}
    for instr in instrs:
        if instr.is_label:
            continue
        key = "+".join(sorted(instr.origin))
        counts[key] = counts.get(key, 0) + 1
    return counts


def command_check(edit: EditFile, work: Path, verbose: bool) -> int:
    source = ROOT / edit.settings["source"]
    problems = check_source_contract(source.read_text())
    if problems:
        print(f"{source}: " + "; ".join(problems))
        return 1
    gcc_text = compile_c(source, work)
    gcc_instrs = [i for i in parse_function(gcc_text, edit.function) if not i.is_label]
    import asm_massage_rules as rules_module

    context = rules_module.Context(edit.settings)
    final = run_pipeline(edit, gcc_text, context=context)
    binary, relocs = assemble_and_link(emit(final, edit.function), edit, work)
    address = int(edit.settings["address"], 16)
    size = int(edit.settings["size"], 16)
    expected = retail_words(address, size)
    mine = [struct.unpack_from("<I", binary, i)[0] for i in range(0, len(binary) - len(binary) % 4, 4)]
    residual_lines = [l for l in edit.residual if l[:1] in "+-" and not l.startswith(("+++", "---"))]
    print(f"{edit.function}: gcc -O2 {sum(1 for i in gcc_instrs if i.op != 'nop')} instructions "
          f"(+{sum(1 for i in gcc_instrs if i.op == 'nop')} nops), massaged {len(mine)}, retail {len(expected)}")
    print(f"  register map entries: {len(edit.registers)}; residual: "
          f"{sum(1 for l in residual_lines if l[0] == '-')} removed, "
          f"{sum(1 for l in residual_lines if l[0] == '+')} added lines")
    print("  instruction count per rule (before->after): " + ", ".join(
        f"{k} {a}->{b}" for k, (a, b) in context.stats.items() if a != b))
    print("  final instructions by origin: " + ", ".join(
        f"{k}={v}" for k, v in sorted(origin_counts(final).items())))
    print("  relocations: " + ", ".join(relocs))
    import asm_massage_deps

    capture: dict = {}
    before = run_pipeline(edit, gcc_text, stage="renamed", capture=capture)
    frameless = edit.settings.get("frameless", "").split()
    ruled: dict = {}
    webs = run_pipeline(edit, gcc_text, stage="rules", capture=ruled)
    deps = asm_massage_deps.check(before, apply_residual(before, edit.residual),
                                  (frameless[0], int(frameless[1], 0)) if frameless else None,
                                  webs, dropped=capture["dropped"],
                                  unrotated=ruled.get("inputs", {}).get("rotate-loop-loads"),
                                  layout=argument_layout(edit))
    print("  " + deps.summary())
    for flag in deps.flags[: None if verbose else 8]:
        print("    " + flag)
    if len(deps.flags) > 8 and not verbose:
        print(f"    ... {len(deps.flags) - 8} more (-v)")
    # Every retail %hi/%lo pair that addresses a known symbol must be a
    # relocation against that symbol, in the same order.
    wanted = []
    for instr in retail_text(edit):
        for arg in instr.args:
            for value in arg[1:2]:
                match = re.fullmatch(r"%(hi|lo)\((\w+)\)", str(value))
                if match:
                    wanted.append(f"R_MIPS_{match.group(1).upper()}16 {match.group(2)}")
    # In-function jumps (j to a local label) relocate against .text; the
    # linked bytes check their targets.
    if [r for r in relocs if r != "R_MIPS_26 .text"] != wanted:
        print("  RELOCATION MISMATCH, retail: " + ", ".join(wanted))
        return 1
    if mine == expected:
        print(f"  MATCH {len(mine) * 4:#x} bytes at {address:#010x}")
        if deps.flags:
            print("  RESIDUAL NOT PROVEN (see the flags above)")
            return 1
        return 0
    print("  MISMATCH")
    if verbose:
        for n, (a, b) in enumerate(zip(mine, expected)):
            if a != b:
                print(f"    +{4 * n:#05x}: {a:08X} != {b:08X}")
    return 1


def command_derive(edit: EditFile, work: Path) -> int:
    source = ROOT / edit.settings["source"]
    gcc_text = compile_c(source, work)
    ruled = run_pipeline(edit, gcc_text, stage="rules")
    target = strip_hazard_nops(retail_text(edit))

    def shape(instr: Instr) -> str:
        t = instr.text()
        return re.sub(r"\$[\w.]+|%\w+", lambda m: m.group(0) if m.group(0) in PRECOLORED else "R", t)

    ruled_shapes = [shape(i) for i in ruled]
    target_shapes = [shape(i) for i in target]
    votes: dict = {}
    matcher = difflib.SequenceMatcher(None, ruled_shapes, target_shapes, autojunk=False)
    for block in matcher.get_matching_blocks():
        weight = 1
        for k in range(block.size):
            a, b = ruled[block.a + k], target[block.b + k]
            for kinds in ("d", "um"):
                for ra, rb in zip(a.regs(kinds), b.regs(kinds)):
                    if ra not in PRECOLORED:
                        votes.setdefault(ra, {}).setdefault(rb, 0)
                        votes[ra][rb] += weight
    chains = web_chains(ruled)
    totals: dict = {}
    for web, counter in votes.items():
        head = chains.get(web, web)
        for r, count in counter.items():
            totals.setdefault(head, {}).setdefault(r, 0)
            totals[head][r] += count
    registers = {}
    for head, counter in totals.items():
        best = max(counter, key=lambda r: (counter[r], r))
        if best != web_home(head):
            registers[head] = best
    # Registers no aligned instruction votes for (rule-made ones such as a
    # promoted stack slot) take the register of a copy partner.
    changed = True
    while changed:
        changed = False
        for instr in ruled:
            if instr.op == "addu" and instr.args[2] == ("r", "$zero"):
                a, b = (chains.get(x, x) for x in (instr.args[0][1], instr.args[1][1]))
                for x, y in ((a, b), (b, a)):
                    if x not in registers and x not in PRECOLORED and y not in PRECOLORED:
                        partner = registers.get(y)
                        if partner:
                            registers[x] = partner
                            changed = True
    renamed = run_pipeline(edit, gcc_text, registers=registers, residual=[], stage="renamed")
    residual = make_residual([i.text() for i in renamed], [i.text() for i in target])
    notes = {}
    for instr in ruled:
        for d in instr.defs():
            notes.setdefault(d, instr.text())
    write_edit(edit, registers, residual, notes)
    print(f"{edit.path}: {len(registers)} register entries, "
          f"{sum(1 for l in residual if l[:1] in '+-' and not l.startswith(('+++', '---')))} residual lines")
    return 0


def massaged_sources() -> list[Path]:
    """Every resident source the matching manifest builds with a massaged
    profile."""
    import json

    profiles = json.loads((ROOT / PROFILES).read_text())["profiles"]
    manifest = json.loads((ROOT / "config/slus_01411/matching_c.json").read_text())
    return sorted({
        ROOT / entry["source"] for entry in manifest["functions"]
        if profiles[entry["profile"]].get("asm_massage") is True
    })


def edit_paths(paths: list[Path]) -> list[Path]:
    """Expand directories (a unit's .massage directory) into edit files; no
    paths means every massaged unit in the manifest."""
    if not paths:
        paths = [edit_directory(source) for source in massaged_sources()]
    out = []
    for path in paths:
        out += sorted(path.glob("*.massage")) if path.is_dir() else [path]
    return out


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = parser.add_subparsers(dest="command", required=True)
    check = sub.add_parser("check")
    check.add_argument("edit", type=Path, nargs="*")
    check.add_argument("-v", "--verbose", action="store_true")
    apply = sub.add_parser("apply")
    apply.add_argument("edit", type=Path)
    apply.add_argument("input", type=Path)
    apply.add_argument("output", type=Path)
    derive = sub.add_parser("derive")
    derive.add_argument("edit", type=Path)
    parser.add_argument("--work", type=Path, default=Path("tmp/asm_massage"))
    args = parser.parse_args(argv)
    if Path.cwd().resolve() != ROOT:
        parser.error("run from the repository root")
    work = (ROOT / args.work).resolve()
    if not work.is_relative_to(ROOT):
        parser.error("work directory must be inside the repository")
    work.mkdir(parents=True, exist_ok=True)
    try:
        if args.command == "check":
            paths = edit_paths(args.edit)
            if not paths:
                raise MassageError("no edit files")
            failed = []
            for path in paths:
                try:
                    if command_check(read_edit(path), work, args.verbose):
                        failed.append(path)
                except MassageError as error:
                    print(f"{path}: {error}")
                    failed.append(path)
            if len(paths) > 1:
                print(f"asm_massage check: {len(paths) - len(failed)}/{len(paths)} MATCH and proven")
            return 1 if failed else 0
        if args.command == "derive":
            return command_derive(read_edit(args.edit), work)
        gcc_text = args.input.read_text()
        if args.edit.is_dir():
            args.output.write_text(massage_unit(gcc_text, read_edits(args.edit)))
        else:
            edit = read_edit(args.edit)
            args.output.write_text(emit(run_pipeline(edit, gcc_text), edit.function))
        return 0
    except MassageError as error:
        print(f"asm_massage: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    # Run through the importable module so asm_massage_rules shares its classes.
    import asm_massage

    raise SystemExit(asm_massage.main())
