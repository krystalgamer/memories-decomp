#!/usr/bin/env python3
"""Validate the inline assembly allowed for formerly hand-written routines.

The resident model renderers between 0x800612C0 and 0x8006AF74 were written
by hand: they have no stack frame, keep caller registers in scratch words of
their argument block and issue GTE commands without the Psy-Q nop padding.
Their C uses the macros in src/game/handwritten_asm.h for exactly those
operations and nothing else. Only profiles with "handwritten_asm_macros" may do so, and only
for functions in that address range.

After preprocessing, every asm statement must consist solely of allowlisted
instructions with "r"/"=r" operands and an optional "memory" clobber. Any other
asm, including asm written directly in the unexpanded source, is rejected.

Three narrow forms reproduce encodings GCC 2.8.1 cannot emit from C:

* ``lwr %0, N(%1)`` tied to its own input ("0") merges an unaligned halfword
  into a word that C already loaded;
* a ``.set noreorder`` block holding exactly one compare branch to a C label
  (an "i" operand spelled ``&&label``) and one delay-slot instruction, which
  is either a GTE register write or a word load;
* nothing else: no other branch, jump, label or register name is accepted.
"""

from __future__ import annotations

import re

HANDWRITTEN_ASM_PROFILE_KEY = "handwritten_asm_macros"
HANDWRITTEN_ASM_MANIFEST = "config/slus_01411/matching_c.json"
HANDWRITTEN_ASM_RANGE = (0x800612C0, 0x8006AF74)

GTE_COMMAND_WORDS = frozenset(
    {
        "4A180001",  # rtps
        "4A280030",  # rtpt
        "4B58002D",  # avsz3
        "4B68002E",  # avsz4
        "4B400006",  # nclip
        "4B08041B",  # nccs
        "4B18043F",  # ncct
    }
)
_OFFSET = r"(?:0x[0-9A-Fa-f]+|\d+)"
_GTE_REGISTER = r"\$(?:[0-9]|[12][0-9]|3[01])"
ALLOWED_INSTRUCTIONS = tuple(
    re.compile(pattern)
    for pattern in (
        # Caller-register save/restore in the argument block.
        rf"(?:sw|lw) \$(?:1[6-9]|2[0-3]), {_OFFSET}\(%0\)",
        rf"(?:lwc2|swc2) {_GTE_REGISTER}, {_OFFSET}\(%0\)",
        rf"(?:mtc2|mfc2) %0, {_GTE_REGISTER}",
        r"cfc2 %0, \$31",
        r"nop",
        r"\.word 0x(?P<word>[0-9A-Fa-f]{8})",
        rf"lwr %0, {_OFFSET}\(%1\)",
    )
)
NOREORDER = ".set noreorder"
REORDER = ".set reorder"
BRANCH_INSTRUCTIONS = (
    re.compile(r"beq %0, %1, %2"),
    re.compile(r"beqz %1, %2"),
)
DELAY_SLOT_INSTRUCTIONS = (
    re.compile(rf"mtc2 %0, {_GTE_REGISTER}"),
    re.compile(rf"lw %0, {_OFFSET}\(%3\)"),
)
ALLOWED_CONSTRAINTS = frozenset({"r", "=r", "0", "i"})
ALLOWED_CLOBBERS = frozenset({"memory"})
_STRING = re.compile(r'"((?:\\.|[^"\\])*)"')
_ASM_STATEMENT = re.compile(
    r"\b(?:asm|__asm|__asm__)\s*(?:\b(?:volatile|__volatile__)\b\s*)?\("
)


class HandwrittenAsmError(ValueError):
    pass


def in_handwritten_range(address: int) -> bool:
    start, end = HANDWRITTEN_ASM_RANGE
    return start <= address < end


def _matching_paren(code: str, start: int) -> int:
    depth = 0
    for index in range(start, len(code)):
        if code[index] == "(":
            depth += 1
        elif code[index] == ")":
            depth -= 1
            if depth == 0:
                return index
    raise HandwrittenAsmError("unterminated asm statement")


def _top_level_sections(code: str, start: int, end: int) -> list[tuple[int, int]]:
    sections: list[tuple[int, int]] = []
    depth = 0
    section_start = start
    for index in range(start, end):
        char = code[index]
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
        elif char == ":" and depth == 0:
            sections.append((section_start, index))
            section_start = index + 1
    sections.append((section_start, end))
    return sections


def _strings(text: str, code: str, start: int, end: int) -> list[str]:
    values: list[str] = []
    for match in _STRING.finditer(text, start, end):
        values.append(match.group(1))
    remainder = _STRING.sub(lambda match: " " * len(match.group(0)), text[start:end])
    if remainder.strip():
        raise HandwrittenAsmError("asm template must consist of string literals")
    return values


def _instructions(template: str) -> list[str]:
    template = template.replace("\\n", ";").replace("\\t", " ")
    normalized_instructions: list[str] = []
    for instruction in template.split(";"):
        normalized = re.sub(r"\s+", " ", instruction).strip()
        normalized = re.sub(r"\( ", "(", re.sub(r" \)", ")", normalized))
        if normalized:
            normalized_instructions.append(normalized)
    return normalized_instructions


def _check_branch_block(instructions: list[str]) -> None:
    if (
        len(instructions) != 4
        or instructions[0] != NOREORDER
        or instructions[3] != REORDER
    ):
        raise HandwrittenAsmError(
            "a .set noreorder block must hold exactly one branch and its delay slot"
        )
    branch, slot = instructions[1], instructions[2]
    if not any(pattern.fullmatch(branch) for pattern in BRANCH_INSTRUCTIONS):
        raise HandwrittenAsmError(f"branch is not allowlisted: {branch}")
    if not any(pattern.fullmatch(slot) for pattern in DELAY_SLOT_INSTRUCTIONS):
        raise HandwrittenAsmError(f"delay-slot instruction is not allowlisted: {slot}")


def _check_template(template: str) -> None:
    instructions = _instructions(template)
    if not instructions:
        raise HandwrittenAsmError("asm templates without an instruction are not allowed")
    if NOREORDER in instructions or REORDER in instructions:
        _check_branch_block(instructions)
        return
    for normalized in instructions:
        for pattern in ALLOWED_INSTRUCTIONS:
            match = pattern.fullmatch(normalized)
            if match is None:
                continue
            word = match.groupdict().get("word")
            if word is not None and word.upper() not in GTE_COMMAND_WORDS:
                raise HandwrittenAsmError(f"unsupported GTE command word: {normalized}")
            break
        else:
            raise HandwrittenAsmError(f"instruction is not allowlisted: {normalized}")


def _check_operands(text: str, code: str, start: int, end: int) -> None:
    for match in _STRING.finditer(text, start, end):
        value = match.group(1)
        after = code[match.end():end].lstrip()
        if after.startswith("("):
            if value not in ALLOWED_CONSTRAINTS:
                raise HandwrittenAsmError(f"unsupported asm constraint: {value}")
            if value == "i":
                operand_end = _matching_paren(code, match.end() + code[match.end():end].index("("))
                operand = code[match.end():operand_end + 1]
                if re.fullmatch(r"\s*\(\s*&&\s*[A-Za-z_]\w*\s*\)", operand) is None:
                    raise HandwrittenAsmError("an \"i\" operand must be the address of a C label")
        elif value not in ALLOWED_CLOBBERS:
            raise HandwrittenAsmError(f"unsupported asm clobber: {value}")


def handwritten_asm_spans(text: str, code: str) -> list[tuple[int, int]]:
    """Return validated asm statement spans in comment-stripped text.

    ``code`` is ``text`` with literals masked. Register pins must already be
    masked out of ``code``; any asm statement that fails the allowlist raises.
    """
    spans: list[tuple[int, int]] = []
    for match in _ASM_STATEMENT.finditer(code):
        open_paren = match.end() - 1
        close_paren = _matching_paren(code, open_paren)
        tail = re.match(r"\s*;", code[close_paren + 1:])
        if tail is None:
            raise HandwrittenAsmError("asm must be a complete statement")
        sections = _top_level_sections(code, open_paren + 1, close_paren)
        if len(sections) > 4:
            raise HandwrittenAsmError("asm has too many operand sections")
        template_start, template_end = sections[0]
        _check_template("".join(_strings(text, code, template_start, template_end)))
        for start, end in sections[1:]:
            _check_operands(text, code, start, end)
        spans.append((match.start(), close_paren + 1 + tail.end()))
    return spans
