#!/usr/bin/env python3
"""Reject foreign function prototypes declared directly in built C sources."""

from __future__ import annotations

import csv
from dataclasses import dataclass
import json
from pathlib import Path
import re
import sys

import candidate_builds


ROOT = Path(__file__).resolve().parents[2]
CONFIG = Path("config/slus_01411")
FUNCTION_NAME = re.compile(r"\b(?P<name>[A-Za-z_]\w*)\s*\(")
FUNCTION_POINTER_OBJECT = re.compile(
    r"\(\s*\*\s*[A-Za-z_]\w*(?:\s*\[[^]]*\])*\s*\)\s*\([^)]*\)"
)
CONDITIONAL_DIRECTIVE = re.compile(
    r"^\s*#\s*(?P<directive>if|ifdef|ifndef|elif|else|endif)\b"
)
IGNORED_NAMES = frozenset(
    {
        "asm",
        "__asm",
        "__asm__",
        "__attribute__",
        "section",
        "void",
        "char",
        "short",
        "int",
        "long",
        "signed",
        "unsigned",
        "float",
        "double",
        "s8",
        "u8",
        "s16",
        "u16",
        "s32",
        "u32",
        "s64",
        "u64",
    }
)


class HeaderContractError(RuntimeError):
    pass


@dataclass(frozen=True)
class SourceUnit:
    path: Path
    module: str


def read_json(path: Path) -> object:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise HeaderContractError(f"{path}: {error}") from error


def inventory_statuses(root: Path) -> dict[tuple[str, str], str]:
    paths = [(root / CONFIG / "functions.csv", "resident")]
    paths.extend(
        (path, f"overlay/{path.stem.removesuffix('_functions')}")
        for path in sorted((root / CONFIG / "overlays").glob("*_functions.csv"))
    )
    statuses: dict[tuple[str, str], str] = {}
    for path, module in paths:
        try:
            with path.open(encoding="utf-8", newline="") as handle:
                for row in csv.DictReader(handle):
                    name = row.get("name")
                    status = row.get("status")
                    if name and status:
                        statuses[(module, name)] = status
        except (OSError, UnicodeError, csv.Error) as error:
            raise HeaderContractError(f"{path}: {error}") from error
    return statuses


def matching_sources(root: Path) -> list[SourceUnit]:
    paths = [(root / CONFIG / "matching_c.json", "resident")]
    paths.extend(
        (path, f"overlay/{path.stem.removesuffix('_matching_c')}")
        for path in sorted((root / CONFIG / "overlays").glob("*_matching_c.json"))
    )
    sources: set[SourceUnit] = set()
    for path, module in paths:
        value = read_json(path)
        entries = value.get("functions") if isinstance(value, dict) else None
        if not isinstance(entries, list):
            raise HeaderContractError(f"{path}: missing functions list")
        for entry in entries:
            source = entry.get("source") if isinstance(entry, dict) else None
            if not isinstance(source, str):
                raise HeaderContractError(f"{path}: function source must be a string")
            sources.add(SourceUnit(root / source, module))
    return sorted(sources, key=lambda unit: (unit.path, unit.module))


def matching_owners(root: Path) -> dict[tuple[str, str], Path]:
    inventories = [(root / CONFIG / "functions.csv", "resident")]
    inventories.extend(
        (path, f"overlay/{path.stem.removesuffix('_functions')}")
        for path in sorted((root / CONFIG / "overlays").glob("*_functions.csv"))
    )
    names_by_address: dict[tuple[str, int], str] = {}
    for path, module in inventories:
        try:
            with path.open(encoding="utf-8", newline="") as handle:
                for row in csv.DictReader(handle):
                    address = row.get("address")
                    name = row.get("name")
                    if address and name:
                        names_by_address[(module, int(address, 0))] = name
        except (OSError, UnicodeError, ValueError, csv.Error) as error:
            raise HeaderContractError(f"{path}: {error}") from error

    manifests = [(root / CONFIG / "matching_c.json", "resident")]
    manifests.extend(
        (path, f"overlay/{path.stem.removesuffix('_matching_c')}")
        for path in sorted((root / CONFIG / "overlays").glob("*_matching_c.json"))
    )
    owners: dict[tuple[str, str], Path] = {}
    for path, module in manifests:
        value = read_json(path)
        entries = value.get("functions") if isinstance(value, dict) else None
        if not isinstance(entries, list):
            raise HeaderContractError(f"{path}: missing functions list")
        for entry in entries:
            address = entry.get("address") if isinstance(entry, dict) else None
            source = entry.get("source") if isinstance(entry, dict) else None
            if not isinstance(address, str) or not isinstance(source, str):
                raise HeaderContractError(
                    f"{path}: function address and source must be strings"
                )
            name = names_by_address.get((module, int(address, 0)))
            if name is not None:
                owners[(module, name)] = root / source
    return owners


def split_declarators(statement: str) -> list[str]:
    parts: list[str] = []
    start = 0
    parentheses = 0
    brackets = 0
    quote: str | None = None
    escaped = False
    for index, char in enumerate(statement):
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
        elif char == "(":
            parentheses += 1
        elif char == ")" and parentheses:
            parentheses -= 1
        elif char == "[":
            brackets += 1
        elif char == "]" and brackets:
            brackets -= 1
        elif char == "," and parentheses == 0 and brackets == 0:
            parts.append(statement[start:index].strip())
            start = index + 1
    parts.append(statement[start:].rstrip(";").strip())
    return [part for part in parts if part]


def mask_bracket_contents(text: str) -> str:
    output = list(text)
    depth = 0
    for index, char in enumerate(text):
        if char == "[":
            depth += 1
        elif char == "]" and depth:
            depth -= 1
        elif depth:
            output[index] = " "
    return "".join(output)


def declaration_names(statement: str) -> list[tuple[str, str | None]]:
    if statement.startswith("typedef "):
        return []
    result: list[tuple[str, str | None]] = []
    for declarator in split_declarators(statement):
        if "=" in declarator:
            continue
        declarator_without_pointer_objects = FUNCTION_POINTER_OBJECT.sub(
            " ", mask_bracket_contents(declarator)
        )
        name = next(
            (
                match.group("name")
                for match in FUNCTION_NAME.finditer(
                    declarator_without_pointer_objects
                )
                if match.group("name") not in IGNORED_NAMES
            ),
            None,
        )
        if name is None:
            continue
        alias = candidate_builds.ASM_ALIAS.search(declarator)
        result.append((name, alias.group("name") if alias is not None else None))
    return result


def mask_non_code(source: str) -> str:
    text = candidate_builds.strip_c_comments(source)
    output = list(text)
    quote: str | None = None
    escaped = False
    for index, char in enumerate(text):
        if quote is not None:
            if char != "\n":
                output[index] = " "
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
        elif char in ('"', "'"):
            quote = char
            output[index] = " "
    masked = "".join(output)
    lines = masked.splitlines(keepends=True)
    conditional_depth = 0
    index = 0
    while index < len(lines):
        end = index
        while (
            end + 1 < len(lines)
            and lines[end].rstrip("\r\n").endswith("\\")
        ):
            end += 1
        logical_line = re.sub(
            r"\\\r?\n",
            "",
            "".join(lines[index : end + 1]),
        )
        match = CONDITIONAL_DIRECTIVE.match(logical_line)
        opening = (
            match is not None
            and match.group("directive") in {"if", "ifdef", "ifndef"}
        )
        closing = match is not None and match.group("directive") == "endif"
        is_directive = logical_line.lstrip().startswith("#")
        if is_directive or conditional_depth:
            for line_index in range(index, end + 1):
                lines[line_index] = "".join(
                    "\n" if char == "\n" else " " for char in lines[line_index]
                )
        if closing and conditional_depth:
            conditional_depth -= 1
        if opening:
            conditional_depth += 1
        index = end + 1
    return "".join(lines)


def definition_names(source: str) -> set[str]:
    text = mask_non_code(source)
    definitions: set[str] = set()
    start = 0
    depth = 0
    for index, char in enumerate(text):
        if char == "{":
            if depth == 0:
                prefix = candidate_builds.normalized_statement(text[start:index])
                if "=" not in prefix and not prefix.startswith("typedef "):
                    declarations = declaration_names(prefix + ";")
                    if declarations:
                        definitions.add(declarations[-1][0])
            depth += 1
        elif char == "}" and depth:
            depth -= 1
            if depth == 0:
                start = index + 1
        elif char == ";" and depth == 0:
            start = index + 1
    return definitions


def audit(root: Path = ROOT) -> tuple[list[str], dict[str, int]]:
    statuses = inventory_statuses(root)
    owners = matching_owners(root)
    problems: list[str] = []
    source_count = 0
    declaration_count = 0
    same_unit_count = 0
    unmatched_count = 0

    for unit in matching_sources(root):
        path = unit.path
        source_count += 1
        try:
            source = path.read_text(encoding="utf-8", errors="surrogateescape")
        except OSError as error:
            problems.append(f"{path.relative_to(root)}: {error}")
            continue
        definitions = definition_names(source)
        for statement in candidate_builds.top_level_statements(source):
            for name, alias in declaration_names(statement):
                declaration_count += 1
                symbols = {name}
                if alias is not None:
                    symbols.add(alias)
                if any(
                    owners.get((unit.module, symbol)) == path
                    for symbol in symbols
                ):
                    same_unit_count += 1
                    continue
                if not any(
                    (unit.module, symbol) in owners for symbol in symbols
                ) and any(symbol in definitions for symbol in symbols):
                    same_unit_count += 1
                    continue
                if statuses.get((unit.module, name)) == "unmatched_asm":
                    if unit.module == "resident":
                        unmatched_count += 1
                        continue
                    problems.append(
                        f"{path.relative_to(root)}: overlay unmatched function "
                        f"declaration {name} has no downstream ownership check: "
                        f"{statement}"
                    )
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
