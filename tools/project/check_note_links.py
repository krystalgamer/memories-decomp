#!/usr/bin/env python3
"""Verify that repository-local paths referenced from notes exist."""

from __future__ import annotations

import re
import sys
from dataclasses import dataclass
from pathlib import Path
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[2]
NOTES = ROOT / "notes"

INLINE_PATH = re.compile(
    r"`(?P<target>"
    r"(?:config|notes|src|tools)/"
    r"[A-Za-z0-9_./-]+"
    r"\.(?:S|c|csv|h|json|ld|md|py|txt|yaml|yml)"
    r")(?::[0-9][^`]*)?`"
)
UNTRACKED_PATH_PREFIXES = ("src/hirata/", "tools/vendor/")
REFERENCE_DEFINITION = re.compile(
    r"^[ \t]{0,3}\[(?P<label>[^\]]+)\]:[ \t]*(?P<rest>.*)$"
)
REFERENCE_USAGE = re.compile(
    r"!?\[(?P<text>[^\]]+)\](?:\[(?P<label>[^\]]*)\])?"
)


@dataclass(frozen=True)
class Problem:
    note: Path
    line: int
    target: str
    reason: str


def local_target(raw: str) -> str | None:
    target = raw[1:-1] if raw.startswith("<") and raw.endswith(">") else raw
    target = re.sub(r"\\([\\`*{}\[\]()#+\-.!_>])", r"\1", target)
    parsed = urlsplit(target)
    if parsed.scheme or parsed.netloc or not parsed.path:
        return None
    return unquote(parsed.path)


def normalize_label(label: str) -> str:
    return " ".join(label.split()).casefold()


def parse_destination(text: str, start: int = 0) -> tuple[str, int] | None:
    while start < len(text) and text[start].isspace():
        start += 1
    if start >= len(text):
        return None
    if text[start] == "<":
        index = start + 1
        while index < len(text):
            if text[index] == "\\" and index + 1 < len(text):
                index += 2
                continue
            if text[index] == ">":
                return text[start : index + 1], index + 1
            index += 1
        return None

    index = start
    depth = 0
    while index < len(text):
        char = text[index]
        if char == "\\" and index + 1 < len(text):
            index += 2
            continue
        if char == "(":
            depth += 1
        elif char == ")":
            if depth == 0:
                break
            depth -= 1
        elif char.isspace() and depth == 0:
            break
        index += 1
    if index == start or depth != 0:
        return None
    return text[start:index], index


def inline_targets(line: str) -> list[tuple[int, int, str]]:
    targets: list[tuple[int, int, str]] = []
    search_from = 0
    while True:
        marker = line.find("](", search_from)
        if marker < 0:
            return targets
        parsed = parse_destination(line, marker + 2)
        if parsed is None:
            search_from = marker + 2
            continue
        target, end = parsed
        cursor = end
        quote = ""
        depth = 0
        while cursor < len(line):
            char = line[cursor]
            if char == "\\" and cursor + 1 < len(line):
                cursor += 2
                continue
            if quote:
                if char == quote:
                    quote = ""
            elif char in "\"'":
                quote = char
            elif char == "(":
                depth += 1
            elif char == ")":
                if depth == 0:
                    targets.append((marker, cursor + 1, target))
                    search_from = cursor + 1
                    break
                depth -= 1
            cursor += 1
        else:
            search_from = marker + 2


def reference_definitions(lines: list[str]) -> tuple[dict[str, str], set[int]]:
    definitions: dict[str, str] = {}
    definition_lines: set[int] = set()
    for index, line in enumerate(lines):
        match = REFERENCE_DEFINITION.match(line)
        if match is None:
            continue
        definition_lines.add(index + 1)
        parsed = parse_destination(match.group("rest"))
        if (
            parsed is None
            and not match.group("rest").strip()
            and index + 1 < len(lines)
        ):
            parsed = parse_destination(lines[index + 1])
            if parsed is not None:
                definition_lines.add(index + 2)
        if parsed is not None:
            definitions.setdefault(normalize_label(match.group("label")), parsed[0])
    return definitions, definition_lines


def resolve_target(
    root: Path,
    note: Path,
    line_number: int,
    target: str,
    *,
    root_relative: bool,
) -> Problem | None:
    base = root if root_relative else note.parent
    resolved = (base / target).resolve()
    try:
        relative = resolved.relative_to(root)
    except ValueError:
        return Problem(note, line_number, target, "escapes repository")
    normalized = relative.as_posix()
    if root_relative and normalized.startswith(UNTRACKED_PATH_PREFIXES):
        return None
    if not resolved.exists():
        return Problem(note, line_number, target, "does not exist")
    return None


def check_note_links(root: Path = ROOT) -> tuple[int, int, list[Problem]]:
    note_paths = sorted((root / "notes").rglob("*.md"))
    references = 0
    problems: list[Problem] = []

    for note in note_paths:
        text = note.read_text(encoding="utf-8")
        check_inline_paths = (root / "notes/research") not in note.parents
        lines = text.splitlines()
        definitions, definition_lines = reference_definitions(lines)

        for line_number, line in enumerate(lines, 1):
            inline_spans = inline_targets(line)
            for _, _, raw_target in inline_spans:
                target = local_target(raw_target)
                if target is None:
                    continue
                references += 1
                problem = resolve_target(
                    root, note, line_number, target, root_relative=False
                )
                if problem is not None:
                    problems.append(problem)

            if line_number not in definition_lines:
                for match in REFERENCE_USAGE.finditer(line):
                    if any(
                        start <= match.start() < end
                        for start, end, _ in inline_spans
                    ):
                        continue
                    explicit_label = match.group("label")
                    if explicit_label is None and match.end() < len(line):
                        if line[match.end()] == "(":
                            continue
                    label = (
                        explicit_label
                        if explicit_label not in (None, "")
                        else match.group("text")
                    )
                    raw_target = definitions.get(normalize_label(label))
                    if raw_target is None:
                        continue
                    target = local_target(raw_target)
                    if target is None:
                        continue
                    references += 1
                    problem = resolve_target(
                        root, note, line_number, target, root_relative=False
                    )
                    if problem is not None:
                        problems.append(problem)

            if not check_inline_paths:
                continue
            for match in INLINE_PATH.finditer(line):
                target = match.group("target")
                problem = resolve_target(
                    root, note, line_number, target, root_relative=True
                )
                if problem is None:
                    resolved = (root / target).resolve()
                    normalized = resolved.relative_to(root).as_posix()
                    if normalized.startswith(UNTRACKED_PATH_PREFIXES):
                        continue
                references += 1
                if problem is not None:
                    problems.append(problem)

    return len(note_paths), references, problems


def main() -> int:
    if Path.cwd().resolve() != ROOT:
        print("error: run this command from the repository root", file=sys.stderr)
        return 1

    note_count, reference_count, problems = check_note_links()
    for problem in problems:
        note = problem.note.relative_to(ROOT)
        print(
            f"error: {note}:{problem.line}: {problem.target}: {problem.reason}",
            file=sys.stderr,
        )
    if problems:
        return 1
    print(
        f"note paths: OK ({reference_count} local references in "
        f"{note_count} notes)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
