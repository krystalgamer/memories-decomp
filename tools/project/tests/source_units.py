"""Helpers for host tests that compile part of a grouped translation unit."""

from __future__ import annotations

from pathlib import Path
import re


DEFINITION = re.compile(r"^[A-Za-z_][\w \*]*?\b(\w+)\(")


def keep_definitions(text: str, names: set[str]) -> str:
    """Drop every top-level non-static function definition not in names.

    Declarations, static helpers, macros and data are kept, so the remaining
    definitions see the same preamble as in the grouped unit.
    """
    lines = text.split("\n")
    kept = []
    index = 0
    while index < len(lines):
        match = DEFINITION.match(lines[index])
        if match and not lines[index].startswith("static"):
            end = index
            while "{" not in lines[end] and not lines[end].rstrip().endswith(";"):
                end += 1
            if "{" in lines[end] and match.group(1) not in names:
                while lines[end] != "}":
                    end += 1
                index = end + 1
                continue
        kept.append(lines[index])
        index += 1
    return "\n".join(kept)


def absolute_includes(text: str, directory: Path) -> str:
    """Resolve quoted includes so the text can be compiled from elsewhere."""
    return re.sub(
        r'^#include "([^"]+)"',
        lambda match: '#include "' + str((directory / match.group(1)).resolve()) + '"',
        text,
        flags=re.MULTILINE,
    )
