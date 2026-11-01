#!/usr/bin/env python3
"""Shared C-statement parsing helpers for the declaration contract checks."""

from __future__ import annotations

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]
ASM_ALIAS = re.compile(
    r'\basm\s*\(\s*"(?P<name>[A-Za-z_]\w*)"\s*\)'
)


def strip_c_comments(text: str) -> str:
    output: list[str] = []
    index = 0
    quote: str | None = None
    escaped = False
    while index < len(text):
        char = text[index]
        follow = text[index + 1] if index + 1 < len(text) else ""
        if quote is not None:
            output.append(char)
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
            index += 1
            continue
        if char in ('"', "'"):
            quote = char
            output.append(char)
            index += 1
            continue
        if char == "/" and follow == "*":
            output.extend((" ", " "))
            index += 2
            while index < len(text):
                char = text[index]
                follow = text[index + 1] if index + 1 < len(text) else ""
                if char == "*" and follow == "/":
                    output.extend((" ", " "))
                    index += 2
                    break
                output.append("\n" if char == "\n" else " ")
                index += 1
            continue
        if char == "/" and follow == "/":
            output.extend((" ", " "))
            index += 2
            while index < len(text) and text[index] != "\n":
                output.append(" ")
                index += 1
            continue
        output.append(char)
        index += 1
    return "".join(output)


def normalized_statement(text: str) -> str:
    lines = [
        line
        for line in text.splitlines()
        if not line.lstrip().startswith("#")
    ]
    return " ".join(" ".join(lines).split())


def top_level_statements(text: str) -> list[str]:
    text = strip_c_comments(text)
    statements: list[str] = []
    start = 0
    depth = 0
    braces: list[bool] = []
    quote: str | None = None
    escaped = False
    for index, char in enumerate(text):
        if quote is not None:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
            continue
        if char in ('"', "'"):
            quote = char
            continue
        if char == "{":
            prefix = normalized_statement(text[start:index])
            transparent = bool(
                re.search(r'\bextern\s+"C"\s*$', prefix)
            )
            braces.append(transparent)
            if transparent:
                start = index + 1
            else:
                depth += 1
            continue
        if char == "}":
            transparent = braces.pop() if braces else False
            if not transparent and depth > 0:
                depth -= 1
            if transparent or depth == 0:
                start = index + 1
            continue
        if char != ";" or depth != 0:
            continue
        statement = normalized_statement(text[start:index + 1])
        if statement:
            statements.append(statement)
        start = index + 1
    return statements


def canonical_declaration_index(
    symbols: set[str],
    header_root: Path | None = None,
) -> dict[str, list[tuple[str, str]]]:
    """Canonical declarations of symbols in resident headers, by header."""
    header_root = header_root or ROOT / "src"
    index = {symbol: [] for symbol in symbols}
    for path in sorted(header_root.rglob("*.h")):
        relative_path = path.relative_to(header_root)
        parts = relative_path.parts
        if parts and parts[0] == "overlays":
            continue
        relative = relative_path.as_posix()
        for statement in top_level_statements(
            path.read_text(encoding="utf-8", errors="surrogateescape")
        ):
            statement_symbols = symbols & set(
                re.findall(r"\b[A-Za-z_]\w*\b", statement)
            )
            for symbol in statement_symbols:
                index[symbol].append((relative, statement))
    for declarations in index.values():
        declarations.sort()
    return index
