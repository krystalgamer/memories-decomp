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

Three narrow forms reproduce encodings GCC 2.8.1 cannot emit from C. Each is
accepted only as a whole statement with an exact operand signature:

* ``lwr %0, N(%1)`` alone, with outputs ``"=r"(x)``, inputs ``"r"(p),
  "0"(x)`` (the tied input names the same expression as the output) and no
  clobbers: it merges an unaligned halfword into a word C already loaded;
* ``.set noreorder; beq %0, %1, %2; mtc2 %0, $N; .set reorder`` with no
  outputs, inputs ``"r", "r", "i"(&&label)`` and no clobbers;
* ``.set noreorder; beqz %1, %2; lw %0, N(%3); .set reorder`` with output
  ``"=r"``, inputs ``"r", "i"(&&label), "r"`` and no clobbers.

The branch target is a C label GCC does not see as a control-flow edge, so
the surrounding code is vouched for only by the exact image match. The "0"
and "i" constraints are rejected anywhere else; no other branch, jump, label
or register name is accepted.
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
NOREORDER = ".set noreorder"
REORDER = ".set reorder"
LWR_INSTRUCTION = re.compile(rf"lwr %0, {_OFFSET}\(%1\)")
# (branch, delay slot, output constraints, input constraints, label index).
BRANCH_FORMS = (
    (
        re.compile(r"beq %0, %1, %2"),
        re.compile(rf"mtc2 %0, {_GTE_REGISTER}"),
        (),
        ("r", "r", "i"),
        2,
    ),
    (
        re.compile(r"beqz %1, %2"),
        re.compile(rf"lw %0, {_OFFSET}\(%3\)"),
        ("=r",),
        ("r", "i", "r"),
        1,
    ),
)
ALLOWED_CONSTRAINTS = frozenset({"r", "=r"})
_LABEL_OPERAND = re.compile(r"&&\s*[A-Za-z_]\w*")
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


Operand = tuple[str, str]


def _operands(text: str, code: str, start: int, end: int) -> list[Operand]:
    """Return (constraint, normalized expression) pairs of one section.

    A clobber has an empty expression.
    """
    operands: list[Operand] = []
    for match in _STRING.finditer(text, start, end):
        value = match.group(1)
        after = code[match.end():end]
        stripped = after.lstrip()
        if stripped.startswith("("):
            open_paren = match.end() + after.index("(")
            close_paren = _matching_paren(code, open_paren)
            expression = re.sub(r"\s+", "", text[open_paren + 1:close_paren])
            operands.append((value, expression))
        else:
            operands.append((value, ""))
    return operands


def _require(
    actual: list[Operand], expected: tuple[str, ...], what: str
) -> None:
    constraints = tuple(constraint for constraint, _ in actual)
    if constraints != expected or any(not expr for _, expr in actual):
        raise HandwrittenAsmError(
            f"{what} must have operands {expected}, found {constraints}"
        )


def _check_special_form(
    instructions: list[str],
    outputs: list[Operand],
    inputs: list[Operand],
    clobbers: list[Operand],
) -> bool:
    """Validate an lwr or branch form; return False for ordinary templates."""
    if any(LWR_INSTRUCTION.fullmatch(item) for item in instructions):
        if len(instructions) != 1:
            raise HandwrittenAsmError("lwr must be the only instruction of its asm")
        _require(outputs, ("=r",), "lwr outputs")
        _require(inputs, ("r", "0"), "lwr inputs")
        if inputs[1][1] != outputs[0][1]:
            raise HandwrittenAsmError(
                "lwr's tied \"0\" input must name the same expression as its output"
            )
        if clobbers:
            raise HandwrittenAsmError("lwr takes no clobbers")
        return True
    if NOREORDER not in instructions and REORDER not in instructions:
        return False
    if (
        len(instructions) != 4
        or instructions[0] != NOREORDER
        or instructions[3] != REORDER
    ):
        raise HandwrittenAsmError(
            "a .set noreorder block must hold exactly one branch and its delay slot"
        )
    branch, slot = instructions[1], instructions[2]
    for branch_pattern, slot_pattern, out_c, in_c, label in BRANCH_FORMS:
        if branch_pattern.fullmatch(branch) is None:
            continue
        if slot_pattern.fullmatch(slot) is None:
            raise HandwrittenAsmError(
                f"delay-slot instruction does not belong to {branch}: {slot}"
            )
        _require(outputs, out_c, f"{branch} outputs")
        _require(inputs, in_c, f"{branch} inputs")
        if _LABEL_OPERAND.fullmatch(inputs[label][1]) is None:
            raise HandwrittenAsmError(
                "a branch target must be the address of a C label"
            )
        if clobbers:
            raise HandwrittenAsmError("a branch block takes no clobbers")
        return True
    raise HandwrittenAsmError(f"branch is not allowlisted: {branch}")


def _check_template(template: str) -> None:
    instructions = _instructions(template)
    if not instructions:
        raise HandwrittenAsmError("asm templates without an instruction are not allowed")
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


def _check_ordinary_operands(
    outputs: list[Operand], inputs: list[Operand], clobbers: list[Operand]
) -> None:
    for constraint, expression in outputs + inputs:
        if constraint not in ALLOWED_CONSTRAINTS or not expression:
            raise HandwrittenAsmError(f"unsupported asm constraint: {constraint}")
    for clobber, expression in clobbers:
        if clobber not in ALLOWED_CLOBBERS or expression:
            raise HandwrittenAsmError(f"unsupported asm clobber: {clobber}")


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
        instructions = _instructions(
            "".join(_strings(text, code, template_start, template_end))
        )
        parsed = [_operands(text, code, start, end) for start, end in sections[1:]]
        parsed += [[] for _ in range(3 - len(parsed))]
        outputs, inputs, clobbers = parsed
        if any(expression for _, expression in clobbers):
            raise HandwrittenAsmError("clobbers cannot carry operands")
        if any(not expression for _, expression in outputs + inputs):
            raise HandwrittenAsmError("an operand constraint needs an expression")
        if not _check_special_form(instructions, outputs, inputs, clobbers):
            _check_template(";".join(instructions))
            _check_ordinary_operands(outputs, inputs, clobbers)
        spans.append((match.start(), close_paren + 1 + tail.end()))
    return spans
