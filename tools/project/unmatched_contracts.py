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
DATA_EXCEPTIONS = Path(
    "config/slus_01411/unmatched_data_contract_exceptions.json"
)
LINKER_SYMBOLS = Path("config/slus_01411/c_symbols.ld")
IDENTIFIER = re.compile(r"\b[A-Za-z_]\w*\b")
FUNCTION_DECLARATION = re.compile(r"\b(?P<name>[A-Za-z_]\w*)\s*\(")
DECLARATION_KEYWORDS = {"__attribute__", "asm"}
LINKER_ASSIGNMENT = re.compile(
    r"^(?P<name>[A-Za-z_]\w*)\s*=\s*0x[0-9A-Fa-f]+\s*;",
    re.MULTILINE,
)


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


def linker_symbols(root: Path) -> set[str]:
    return {
        match.group("name")
        for match in LINKER_ASSIGNMENT.finditer(read_text(root / LINKER_SYMBOLS))
    }


def split_declarators(text: str) -> list[str]:
    parts: list[str] = []
    start = 0
    parentheses = 0
    brackets = 0
    for index, char in enumerate(text):
        if char == "(":
            parentheses += 1
        elif char == ")" and parentheses:
            parentheses -= 1
        elif char == "[":
            brackets += 1
        elif char == "]" and brackets:
            brackets -= 1
        elif char == "," and parentheses == 0 and brackets == 0:
            parts.append(text[start:index].strip())
            start = index + 1
    parts.append(text[start:].strip())
    return [part for part in parts if part]


def remove_parenthesized_annotation(text: str, name: str) -> str:
    pattern = re.compile(rf"\b{re.escape(name)}\s*\(")
    while True:
        match = pattern.search(text)
        if match is None:
            return text
        opening = text.find("(", match.start())
        depth = 0
        end = None
        quote: str | None = None
        escaped = False
        for index in range(opening, len(text)):
            char = text[index]
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
                depth += 1
            elif char == ")":
                depth -= 1
                if depth == 0:
                    end = index + 1
                    break
        if end is None:
            return text
        text = text[:match.start()] + text[end:]


def extern_object_symbols(statement: str) -> list[str]:
    if not statement.startswith("extern "):
        return []
    alias = candidate_builds.ASM_ALIAS.search(statement)
    if alias is not None:
        return [alias.group("name")]
    declaration = remove_parenthesized_annotation(
        statement.rstrip(";"),
        "__attribute__",
    )
    result: list[str] = []
    for declarator in split_declarators(declaration):
        pointer = re.search(
            r"\(\s*\*\s*(?P<name>[A-Za-z_]\w*)\s*\)",
            declarator,
        )
        if pointer is not None:
            result.append(pointer.group("name"))
            continue
        ordinary_function = re.search(
            r"\b(?P<name>[A-Za-z_]\w*)\s*\(",
            declarator,
        )
        if ordinary_function is not None:
            continue
        declarator = re.sub(r"(?:\[[^\]]*\]\s*)+$", "", declarator)
        name = re.search(r"(?P<name>[A-Za-z_]\w*)\s*$", declarator)
        if name is not None:
            result.append(name.group("name"))
    return result


def extern_object_declarations(
    text: str,
    known_symbols: set[str] | None = None,
) -> list[tuple[str, str]]:
    result: list[tuple[str, str]] = []
    for statement in candidate_builds.top_level_statements(text):
        symbols = extern_object_symbols(statement)
        if known_symbols is not None:
            symbols = [
                symbol for symbol in symbols if symbol in known_symbols
            ]
        result.extend((symbol, statement) for symbol in sorted(symbols))
    return result


def declaration_name(statement: str) -> str | None:
    if re.search(r"\(\s*\*\s*[A-Za-z_]\w*\s*\)", statement):
        return None
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


def load_exceptions(
    root: Path,
    path: Path = EXCEPTIONS,
) -> list[dict[str, str]]:
    data = json.loads(read_text(root / path))
    if data.get("schema") != 1 or not isinstance(data.get("exceptions"), list):
        raise ContractError(f"{path}: expected schema 1 exception list")
    required = {"symbol", "source", "declaration", "reason"}
    exceptions = data["exceptions"]
    for index, item in enumerate(exceptions):
        if not isinstance(item, dict) or set(item) != required:
            raise ContractError(
                f"{path}: exception {index} must contain "
                f"{sorted(required)}"
            )
        if not all(isinstance(item[key], str) and item[key] for key in required):
            raise ContractError(
                f"{path}: exception {index} values must be non-empty strings"
            )
    keys = [
        (item["source"], item["symbol"], item["declaration"])
        for item in exceptions
    ]
    if len(keys) != len(set(keys)):
        raise ContractError(f"{path}: duplicate exception")
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
    linker = linker_symbols(root)
    central_data_pairs = extern_object_declarations(
        read_text(root / UNMATCHED_HEADER),
    )
    central_data = defaultdict(list)
    for name, statement in central_data_pairs:
        central_data[name].append(statement)
    data_configured = load_exceptions(root, DATA_EXCEPTIONS)
    data_approved = {
        (item["source"], item["symbol"], item["declaration"])
        for item in data_configured
    }
    data_found_approved: set[tuple[str, str, str]] = set()
    local_data = defaultdict(list)

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
        for name, statement in extern_object_declarations(text, linker):
            key = (relative, name, statement)
            local_data[name].append((relative, statement))
            if name not in central_data:
                continue
            if key in data_approved:
                data_found_approved.add(key)
            else:
                errors.append(
                    f"{relative}: local declaration of central unmatched data "
                    f"{name} is not approved: {statement}"
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

    all_data_symbols = set(local_data) | set(central_data)
    data_header_index = candidate_builds.canonical_declaration_index(
        all_data_symbols,
        root / "src",
    )
    for name, statements in central_data.items():
        duplicates = [
            statement
            for statement, count in Counter(statements).items()
            if count > 1
        ]
        if duplicates:
            errors.append(
                f"{UNMATCHED_HEADER}: duplicate data declarations for "
                f"{name}: {duplicates}"
            )
        other_headers = sorted(
            {
                path
                for path, _ in data_header_index.get(name, [])
                if path != UNMATCHED_HEADER.relative_to("src").as_posix()
            }
        )
        if other_headers:
            errors.append(
                f"{UNMATCHED_HEADER}: stale data declaration {name}: "
                f"also declared by resident headers {other_headers}"
            )
        if name not in linker:
            errors.append(
                f"{UNMATCHED_HEADER}: stale data declaration {name}: "
                f"absent from {LINKER_SYMBOLS}"
            )

    for source, name, statement in sorted(
        data_approved - data_found_approved
    ):
        errors.append(
            f"{DATA_EXCEPTIONS}: configured exception not found exactly: "
            f"{source}: {statement}"
        )
    for item in data_configured:
        if item["symbol"] not in linker:
            errors.append(
                f"{DATA_EXCEPTIONS}: stale exception {item['symbol']}: "
                f"absent from {LINKER_SYMBOLS}"
            )
        if item["symbol"] not in central_data:
            errors.append(
                f"{DATA_EXCEPTIONS}: exception {item['symbol']} has no "
                f"central declaration in {UNMATCHED_HEADER}"
            )

    headerless_data = {
        name
        for name in local_data
        if not data_header_index.get(name)
    }

    stats = {
        "unmatched": len(unmatched),
        "central": len(central),
        "exception_names": len({item["symbol"] for item in configured}),
        "exception_sites": len(configured),
        "referenced_names": len({name for _, name in referenced_sites}),
        "referenced_sites": len(referenced_sites),
        "local_sites": len(local_sites),
        "central_data": len(central_data),
        "local_data_names": len(local_data),
        "local_data_sites": sum(len(sites) for sites in local_data.values()),
        "headerless_data": len(headerless_data),
        "headerless_data_sites": sum(
            len(local_data[name]) for name in headerless_data
        ),
        "data_exception_names": len(
            {item["symbol"] for item in data_configured}
        ),
        "data_exception_sites": len(data_configured),
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
        "unmatched contracts: OK "
        f"({stats['unmatched']} unmatched, {stats['central']} central, "
        f"{stats['exception_names']} exception names/"
        f"{stats['exception_sites']} sites, "
        f"{stats['referenced_names']} referenced names; "
        f"{stats['central_data']} central data, "
        f"{stats['headerless_data']} headerless data/"
        f"{stats['headerless_data_sites']} sites)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
