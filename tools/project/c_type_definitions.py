#!/usr/bin/env python3
"""Reject type definitions in C sources; shared headers own all types."""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
TYPE_DEFINITION = re.compile(
    r"\b(?:typedef|(?:struct|union|enum)\s+(?:[A-Za-z_]\w*\s*)?\{)"
)


def mask_comments_and_literals(text: str) -> str:
    pattern = re.compile(
        r"/\*.*?\*/|//[^\n]*|\"(?:\\.|[^\"\\])*\"|'(?:\\.|[^'\\])*'",
        re.DOTALL,
    )
    return pattern.sub(lambda match: re.sub(r"[^\n]", " ", match.group(0)), text)


def type_definition_lines(text: str) -> list[int]:
    masked = mask_comments_and_literals(text)
    return list(
        dict.fromkeys(
            masked.count("\n", 0, match.start()) + 1
            for match in TYPE_DEFINITION.finditer(masked)
        )
    )


def main() -> int:
    failures: list[tuple[Path, list[int]]] = []
    for path in sorted((ROOT / "src").rglob("*.c")):
        lines = type_definition_lines(
            path.read_text(encoding="utf-8", errors="surrogateescape")
        )
        if lines:
            failures.append((path, lines))
    if failures:
        for path, lines in failures:
            relative = path.relative_to(ROOT)
            print(f"error: {relative}: C-file type definitions at lines {lines}")
        return 1
    print("C-file type definitions: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
