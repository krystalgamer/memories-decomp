#!/usr/bin/env python3
"""Verify that repository-local paths linked from notes exist."""

from __future__ import annotations

import re
import sys
from dataclasses import dataclass
from pathlib import Path
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[2]
NOTES = ROOT / "notes"

LINK = re.compile(
    r"\[[^\]]*\]\("
    r"(?P<target><[^>]+>|[^)\s]+)"
    r"(?:\s+(?:\"[^\"]*\"|'[^']*'|\([^)]*\)))?"
    r"\)"
)


@dataclass(frozen=True)
class Problem:
    note: Path
    line: int
    target: str
    reason: str


def local_target(raw: str) -> str | None:
    target = raw[1:-1] if raw.startswith("<") and raw.endswith(">") else raw
    parsed = urlsplit(target)
    if parsed.scheme or parsed.netloc or not parsed.path:
        return None
    return unquote(parsed.path)


def check_note_links(root: Path = ROOT) -> tuple[int, int, list[Problem]]:
    note_paths = sorted((root / "notes").rglob("*.md"))
    links = 0
    problems: list[Problem] = []

    for note in note_paths:
        text = note.read_text(encoding="utf-8")
        for line_number, line in enumerate(text.splitlines(), 1):
            for match in LINK.finditer(line):
                target = local_target(match.group("target"))
                if target is None:
                    continue
                links += 1
                resolved = (note.parent / target).resolve()
                try:
                    resolved.relative_to(root)
                except ValueError:
                    problems.append(
                        Problem(note, line_number, target, "escapes repository")
                    )
                    continue
                if not resolved.exists():
                    problems.append(Problem(note, line_number, target, "does not exist"))

    return len(note_paths), links, problems


def main() -> int:
    if Path.cwd().resolve() != ROOT:
        print("error: run this command from the repository root", file=sys.stderr)
        return 1

    note_count, link_count, problems = check_note_links()
    for problem in problems:
        note = problem.note.relative_to(ROOT)
        print(
            f"error: {note}:{problem.line}: {problem.target}: {problem.reason}",
            file=sys.stderr,
        )
    if problems:
        return 1
    print(f"note links: OK ({link_count} local links in {note_count} notes)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
