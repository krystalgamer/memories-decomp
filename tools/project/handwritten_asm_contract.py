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
    )
)
ALLOWED_CONSTRAINTS = frozenset({"r", "=r"})
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


def _check_template(template: str) -> None:
    instructions = 0
    for instruction in template.split(";"):
        normalized = re.sub(r"\s+", " ", instruction).strip()
        normalized = re.sub(r"\( ", "(", re.sub(r" \)", ")", normalized))
        if not normalized:
            continue
        instructions += 1
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
    if not instructions:
        raise HandwrittenAsmError("asm templates without an instruction are not allowed")


def _check_operands(text: str, code: str, start: int, end: int) -> None:
    for match in _STRING.finditer(text, start, end):
        value = match.group(1)
        after = code[match.end():end].lstrip()
        if after.startswith("("):
            if value not in ALLOWED_CONSTRAINTS:
                raise HandwrittenAsmError(f"unsupported asm constraint: {value}")
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
