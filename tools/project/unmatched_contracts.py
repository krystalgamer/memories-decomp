#!/usr/bin/env python3
from __future__ import annotations

from collections import Counter, defaultdict
import csv
import json
from pathlib import Path
import re
import sys

import candidate_builds


ROOT = Path(__file__).resolve().parents[2]
FUNCTIONS = Path("config/slus_01411/functions.csv")
MATCHING_C = Path("config/slus_01411/matching_c.json")
UNMATCHED_HEADER = Path("src/unmatched.h")
EXCEPTIONS = Path("config/slus_01411/unmatched_contract_exceptions.json")
IDENTIFIER = re.compile(r"\b[A-Za-z_]\w*\b")
FUNCTION_DECLARATION = re.compile(r"\b(?P<name>[A-Za-z_]\w*)\s*\(")
DECLARATION_KEYWORDS = {"__attribute__", "asm"}


class ContractError(RuntimeError):
    pass


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="surrogateescape")


def inventory(root: Path) -> dict[str, str]:
    path = root / FUNCTIONS
    with path.open(newline="", encoding="utf-8") as handle:
        rows = list(csv.DictReader(handle))
    return {
        row["name"]: row["status"]
        for row in rows
        if row["module"] == "game"
    }


def matching_sources(root: Path) -> list[Path]:
    data = json.loads(read_text(root / MATCHING_C))
    entries = data["functions"] if isinstance(data, dict) else data
    return sorted({root / entry["source"] for entry in entries})


def declaration_name(statement: str) -> str | None:
    match = FUNCTION_DECLARATION.search(statement)
    if match is None or match.group("name") in DECLARATION_KEYWORDS:
        return None
    return match.group("name")


def declarations(text: str) -> list[tuple[str, str]]:
    result = []
    for statement in candidate_builds.top_level_statements(text):
        name = declaration_name(statement)
        if name is not None:
            result.append((name, statement))
    return result


def strip_literals_and_directives(text: str) -> str:
    text = candidate_builds.strip_c_comments(text)
    output: list[str] = []
    quote: str | None = None
    escaped = False
    for char in text:
        if quote is not None:
            output.append("\n" if char == "\n" else " ")
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
            continue
        if char in ('"', "'"):
            quote = char
            output.append(" ")
            continue
        output.append(char)
    return "\n".join(
        "" if line.lstrip().startswith("#") else line
        for line in "".join(output).splitlines()
    )


def executable_references(
    text: str,
    unmatched: set[str],
    local_declarations: list[tuple[str, str]],
) -> set[str]:
    counts = Counter(
        token
        for token in IDENTIFIER.findall(strip_literals_and_directives(text))
        if token in unmatched
    )
    for name, _ in local_declarations:
        if name in unmatched:
            counts[name] -= 1
    return {name for name, count in counts.items() if count > 0}


def load_exceptions(root: Path) -> list[dict[str, str]]:
    data = json.loads(read_text(root / EXCEPTIONS))
    if data.get("schema") != 1 or not isinstance(data.get("exceptions"), list):
        raise ContractError(f"{EXCEPTIONS}: expected schema 1 exception list")
    required = {"symbol", "source", "declaration", "reason"}
    exceptions = data["exceptions"]
    for index, item in enumerate(exceptions):
        if not isinstance(item, dict) or set(item) != required:
            raise ContractError(
                f"{EXCEPTIONS}: exception {index} must contain "
                f"{sorted(required)}"
            )
        if not all(isinstance(item[key], str) and item[key] for key in required):
            raise ContractError(
                f"{EXCEPTIONS}: exception {index} values must be non-empty strings"
            )
    keys = [
        (item["source"], item["symbol"], item["declaration"])
        for item in exceptions
    ]
    if len(keys) != len(set(keys)):
        raise ContractError(f"{EXCEPTIONS}: duplicate exception")
    return exceptions


def validate(root: Path = ROOT) -> tuple[list[str], dict[str, int]]:
    statuses = inventory(root)
    unmatched = {
        name for name, status in statuses.items() if status == "unmatched_asm"
    }
    errors: list[str] = []

    central_pairs = declarations(read_text(root / UNMATCHED_HEADER))
    central = defaultdict(list)
    for name, statement in central_pairs:
        central[name].append(statement)
        status = statuses.get(name)
        if status != "unmatched_asm":
            errors.append(
                f"{UNMATCHED_HEADER}: stale declaration {name}: "
                f"functions.csv status is {status or 'absent'}"
            )
    for name, statements in central.items():
        if len(statements) > 1:
            errors.append(
                f"{UNMATCHED_HEADER}: duplicate declarations for {name}: "
                f"{statements}"
            )

    configured = load_exceptions(root)
    approved = {
        (item["source"], item["symbol"], item["declaration"])
        for item in configured
    }
    found_approved: set[tuple[str, str, str]] = set()
    local_sites: list[tuple[str, str, str]] = []
    referenced_sites: list[tuple[str, str]] = []

    for source in matching_sources(root):
        relative = source.relative_to(root).as_posix()
        text = read_text(source)
        source_declarations = [
            pair for pair in declarations(text) if pair[0] in unmatched
        ]
        source_approved: set[str] = set()
        for name, statement in source_declarations:
            key = (relative, name, statement)
            local_sites.append(key)
            if key in approved:
                found_approved.add(key)
                source_approved.add(name)
            else:
                errors.append(
                    f"{relative}: local declaration of unmatched function "
                    f"{name} is not approved: {statement}"
                )
        for name in sorted(
            executable_references(text, unmatched, source_declarations)
        ):
            referenced_sites.append((relative, name))
            if name not in central and name not in source_approved:
                errors.append(
                    f"{relative}: unmatched function {name} is referenced "
                    f"without a declaration in {UNMATCHED_HEADER}"
                )

    for source, name, statement in sorted(approved - found_approved):
        errors.append(
            f"{EXCEPTIONS}: configured exception not found exactly: "
            f"{source}: {statement}"
        )
    for item in configured:
        if item["symbol"] not in unmatched:
            errors.append(
                f"{EXCEPTIONS}: stale exception {item['symbol']}: "
                f"functions.csv status is {statuses.get(item['symbol'], 'absent')}"
            )
        if item["symbol"] in central:
            errors.append(
                f"{EXCEPTIONS}: {item['symbol']} is both central and exceptional"
            )

    stats = {
        "unmatched": len(unmatched),
        "central": len(central),
        "exception_names": len({item["symbol"] for item in configured}),
        "exception_sites": len(configured),
        "referenced_names": len({name for _, name in referenced_sites}),
        "referenced_sites": len(referenced_sites),
        "local_sites": len(local_sites),
    }
    return sorted(set(errors)), stats


def main() -> int:
    if Path.cwd().resolve() != ROOT:
        print(f"error: run from repository root {ROOT}", file=sys.stderr)
        return 1
    try:
        errors, stats = validate()
    except (ContractError, KeyError, OSError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 1
    print(
        "unmatched function contracts: OK "
        f"({stats['unmatched']} unmatched, {stats['central']} central, "
        f"{stats['exception_names']} exception names/"
        f"{stats['exception_sites']} sites, "
        f"{stats['referenced_names']} referenced names)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
