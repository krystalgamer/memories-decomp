#!/usr/bin/env python3
"""Reject foreign function prototypes declared directly in built C sources."""

from __future__ import annotations

import csv
import json
from pathlib import Path
import re
import sys

import candidate_builds


ROOT = Path(__file__).resolve().parents[2]
CONFIG = Path("config/slus_01411")
FUNCTION_POINTER = re.compile(r"\(\s*\*[^)]*\)")
FUNCTION_NAME = re.compile(r"\b(?P<name>[A-Za-z_]\w*)\s*\(")
FUNCTION_DEFINITION = re.compile(
    r"\b(?P<name>[A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{"
)
IGNORED_NAMES = frozenset({"asm", "__asm", "__asm__", "__attribute__", "section"})


class HeaderContractError(RuntimeError):
    pass


def read_json(path: Path) -> object:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise HeaderContractError(f"{path}: {error}") from error


def inventory_statuses(root: Path) -> dict[str, str]:
    paths = [root / CONFIG / "functions.csv"]
    paths.extend(sorted((root / CONFIG / "overlays").glob("*_functions.csv")))
    statuses: dict[str, str] = {}
    for path in paths:
        try:
            with path.open(encoding="utf-8", newline="") as handle:
                for row in csv.DictReader(handle):
                    name = row.get("name")
                    status = row.get("status")
                    if name and status:
                        statuses[name] = status
        except (OSError, UnicodeError, csv.Error) as error:
            raise HeaderContractError(f"{path}: {error}") from error
    return statuses


def matching_sources(root: Path) -> list[Path]:
    paths = [root / CONFIG / "matching_c.json"]
    paths.extend(sorted((root / CONFIG / "overlays").glob("*_matching_c.json")))
    sources: set[Path] = set()
    for path in paths:
        value = read_json(path)
        entries = value.get("functions") if isinstance(value, dict) else None
        if not isinstance(entries, list):
            raise HeaderContractError(f"{path}: missing functions list")
        for entry in entries:
            source = entry.get("source") if isinstance(entry, dict) else None
            if not isinstance(source, str):
                raise HeaderContractError(f"{path}: function source must be a string")
            sources.add(root / source)
    return sorted(sources)


def declaration_name(statement: str) -> str | None:
    if FUNCTION_POINTER.search(statement) is not None:
        return None
    if "=" in statement or statement.startswith("typedef "):
        return None
    for match in FUNCTION_NAME.finditer(statement):
        name = match.group("name")
        if name not in IGNORED_NAMES:
            return name
    return None


def definition_names(source: str) -> set[str]:
    text = candidate_builds.strip_c_comments(source)
    return {match.group("name") for match in FUNCTION_DEFINITION.finditer(text)}


def audit(root: Path = ROOT) -> tuple[list[str], dict[str, int]]:
    statuses = inventory_statuses(root)
    problems: list[str] = []
    source_count = 0
    declaration_count = 0
    same_unit_count = 0
    unmatched_count = 0

    for path in matching_sources(root):
        source_count += 1
        try:
            source = path.read_text(encoding="utf-8", errors="surrogateescape")
        except OSError as error:
            problems.append(f"{path.relative_to(root)}: {error}")
            continue
        definitions = definition_names(source)
        for statement in candidate_builds.top_level_statements(source):
            name = declaration_name(statement)
            if name is None:
                continue
            declaration_count += 1
            if name in definitions:
                same_unit_count += 1
                continue
            if statuses.get(name) == "unmatched_asm":
                unmatched_count += 1
                continue
            problems.append(
                f"{path.relative_to(root)}: foreign function declaration "
                f"{name} belongs in a header: {statement}"
            )

    stats = {
        "sources": source_count,
        "declarations": declaration_count,
        "same_unit": same_unit_count,
        "unmatched": unmatched_count,
    }
    return sorted(set(problems)), stats


def main() -> int:
    if Path.cwd().resolve() != ROOT:
        print(f"error: run from repository root {ROOT}", file=sys.stderr)
        return 1
    try:
        problems, stats = audit()
    except HeaderContractError as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    if problems:
        for problem in problems:
            print(f"error: {problem}", file=sys.stderr)
        return 1
    print(
        "translation-unit headers: OK "
        f"({stats['sources']} sources, {stats['same_unit']} same-unit forwards, "
        f"{stats['unmatched']} unmatched declarations delegated)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
