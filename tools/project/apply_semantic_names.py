#!/usr/bin/env python3
from __future__ import annotations

import argparse
import csv
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from workspace import WorkspaceError, require_workspace_root, resolve_within


MAP_FIELDS = (
    "kind",
    "address",
    "name",
    "confidence",
    "evidence",
    "notes",
)
FUNCTION_FIELDS = (
    "address",
    "size",
    "name",
    "status",
    "module",
    "notes",
)
IDENTIFIER = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
SYMBOL_LINE = re.compile(
    r"^(?P<name>[A-Za-z_][A-Za-z0-9_]*)"
    r"\s*=\s*(?P<address>0x[0-9A-Fa-f]+);(?P<suffix>.*)$"
)


class SemanticNameError(RuntimeError):
    pass


@dataclass(frozen=True)
class Mapping:
    kind: str
    address: int
    name: str
    confidence: str
    evidence: str
    notes: str


@dataclass(frozen=True)
class Move:
    source: Path
    destination: Path


def parse_address(value: str) -> int:
    try:
        return int(value, 0)
    except ValueError as error:
        raise SemanticNameError(f"invalid address: {value}") from error


def load_map(path: Path) -> list[Mapping]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if tuple(reader.fieldnames or ()) != MAP_FIELDS:
            raise SemanticNameError(
                f"{path}: expected fields {','.join(MAP_FIELDS)}"
            )
        rows = list(reader)
    result: list[Mapping] = []
    seen_addresses: set[tuple[str, int]] = set()
    seen_names: set[str] = set()
    for row in rows:
        kind = row["kind"]
        if kind not in {"function", "global"}:
            raise SemanticNameError(f"unsupported mapping kind: {kind}")
        address = parse_address(row["address"])
        name = row["name"]
        if not IDENTIFIER.fullmatch(name):
            raise SemanticNameError(f"invalid C identifier: {name}")
        key = (kind, address)
        if key in seen_addresses:
            raise SemanticNameError(
                f"duplicate {kind} mapping at {address:#010x}"
            )
        if name in seen_names:
            raise SemanticNameError(f"duplicate semantic name: {name}")
        seen_addresses.add(key)
        seen_names.add(name)
        result.append(
            Mapping(
                kind=kind,
                address=address,
                name=name,
                confidence=row["confidence"],
                evidence=row["evidence"],
                notes=row["notes"],
            )
        )
    return result


def load_functions(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if tuple(reader.fieldnames or ()) != FUNCTION_FIELDS:
            raise SemanticNameError(f"{path}: unexpected function fields")
        return list(reader)


def load_matching(path: Path) -> dict[str, Any]:
    value = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(value, dict) or value.get("schema") != 1:
        raise SemanticNameError(f"{path}: unsupported matching schema")
    functions = value.get("functions")
    if not isinstance(functions, list):
        raise SemanticNameError(f"{path}: missing function list")
    return value


def snake_case(value: str) -> str:
    value = value.replace("__", "_")
    value = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1_\2", value)
    value = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", value)
    return re.sub(r"_+", "_", value).strip("_").lower()


def source_files(root: Path) -> list[Path]:
    return sorted(
        path
        for path in (root / "src").rglob("*")
        if path.is_file() and path.suffix in {".c", ".h", ".s"}
    )


def replace_tokens(text: str, replacements: dict[str, str]) -> str:
    if not replacements:
        return text
    pattern = re.compile(
        r"\b(?:"
        + "|".join(
            re.escape(name)
            for name in sorted(replacements, key=len, reverse=True)
        )
        + r")\b"
    )
    return pattern.sub(lambda match: replacements[match.group(0)], text)


def symbol_state(
    text: str,
) -> tuple[list[str], dict[int, tuple[int, str, str]], dict[str, int]]:
    lines = text.splitlines()
    by_address: dict[int, tuple[int, str, str]] = {}
    by_name: dict[str, int] = {}
    for index, line in enumerate(lines):
        match = SYMBOL_LINE.match(line.strip())
        if match is None:
            continue
        address = parse_address(match.group("address"))
        name = match.group("name")
        if address in by_address:
            raise SemanticNameError(
                f"symbols.txt contains duplicate address {address:#010x}"
            )
        if name in by_name:
            raise SemanticNameError(
                f"symbols.txt contains duplicate name {name}"
            )
        by_address[address] = (index, name, match.group("suffix"))
        by_name[name] = address
    return lines, by_address, by_name


def update_symbols(text: str, mappings: list[Mapping]) -> str:
    lines, by_address, by_name = symbol_state(text)
    additions: list[str] = []
    for mapping in mappings:
        existing_address = by_name.get(mapping.name)
        if existing_address is not None and existing_address != mapping.address:
            raise SemanticNameError(
                f"{mapping.name} already names {existing_address:#010x}"
            )
        existing = by_address.get(mapping.address)
        if existing is not None:
            index, old_name, suffix = existing
            if mapping.kind == "function" and "type:func" not in suffix:
                suffix = f"{suffix.rstrip()} // type:func"
            lines[index] = (
                f"{mapping.name} = 0x{mapping.address:08X};{suffix}"
            )
            by_name.pop(old_name, None)
            by_name[mapping.name] = mapping.address
        else:
            suffix = " // type:func" if mapping.kind == "function" else ""
            additions.append(
                f"{mapping.name} = 0x{mapping.address:08X};{suffix}"
            )
            by_name[mapping.name] = mapping.address
    if additions:
        if lines and lines[-1]:
            lines.append("")
        lines.append("// Semantic names")
        lines.extend(additions)
    return "\n".join(lines) + "\n"


def write_csv(path: Path, rows: list[dict[str, str]]) -> None:
    temporary = path.with_suffix(path.suffix + ".tmp")
    with temporary.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(
            handle,
            fieldnames=FUNCTION_FIELDS,
            lineterminator="\n",
        )
        writer.writeheader()
        writer.writerows(rows)
    temporary.replace(path)


def write_json(path: Path, value: dict[str, Any]) -> None:
    temporary = path.with_suffix(path.suffix + ".tmp")
    temporary.write_text(
        json.dumps(value, indent=2) + "\n",
        encoding="utf-8",
    )
    temporary.replace(path)


def plan(
    root: Path,
    mappings: list[Mapping],
) -> tuple[
    dict[Path, str],
    list[dict[str, str]],
    dict[str, Any],
    str,
    list[Move],
    dict[str, str],
]:
    functions_path = root / "config/slus_01411/functions.csv"
    matching_path = root / "config/slus_01411/matching_c.json"
    symbols_path = root / "config/slus_01411/symbols.txt"
    function_rows = load_functions(functions_path)
    matching = load_matching(matching_path)
    inventory = {
        parse_address(row["address"]): row
        for row in function_rows
    }
    matching_rows = {
        parse_address(row["address"]): row
        for row in matching["functions"]
        if isinstance(row, dict) and isinstance(row.get("address"), str)
    }
    matching_source_counts: dict[str, int] = {}
    for row in matching_rows.values():
        source_value = row.get("source")
        if isinstance(source_value, str):
            matching_source_counts[source_value] = (
                matching_source_counts.get(source_value, 0) + 1
            )
    replacements: dict[str, str] = {}
    moves: list[Move] = []

    for mapping in mappings:
        if mapping.kind == "function":
            row = inventory.get(mapping.address)
            if row is None:
                raise SemanticNameError(
                    f"{mapping.address:#010x}: absent from function inventory"
                )
            module = row["module"]
            if module != "game" and (
                not module.startswith("psyq/")
                or mapping.confidence != "confirmed"
            ):
                raise SemanticNameError(
                    f"{mapping.address:#010x}: non-game semantic names require "
                    "confirmed Psy-Q evidence"
                )
            old_name = row["name"]
            if old_name != mapping.name:
                if old_name in replacements:
                    raise SemanticNameError(
                        f"duplicate replacement source: {old_name}"
                    )
                replacements[old_name] = mapping.name
                row["name"] = mapping.name
            matching_row = matching_rows.get(mapping.address)
            if matching_row is not None:
                source = root / str(matching_row["source"])
                address_stem = f"func_{mapping.address:08X}"
                if (
                    matching_source_counts.get(str(matching_row["source"])) == 1
                    and source.stem in {old_name, address_stem}
                ):
                    destination = source.with_name(
                        f"{snake_case(mapping.name)}.c"
                    )
                    if destination != source:
                        moves.append(Move(source, destination))
                        matching_row["source"] = str(
                            destination.relative_to(root)
                        )
        else:
            old_name = f"D_{mapping.address:08X}"
            replacements[old_name] = mapping.name

    destinations = [move.destination for move in moves]
    if len(destinations) != len(set(destinations)):
        raise SemanticNameError("multiple semantic names select one source path")
    move_sources = {move.source for move in moves}
    for move in moves:
        if not move.source.is_file():
            raise SemanticNameError(f"missing source: {move.source}")
        if move.destination.exists() and move.destination not in move_sources:
            raise SemanticNameError(
                f"destination already exists: {move.destination}"
            )

    updated_sources: dict[Path, str] = {}
    for source in source_files(root):
        updated = replace_tokens(
            source.read_text(encoding="utf-8"),
            replacements,
        )
        if updated != source.read_text(encoding="utf-8"):
            updated_sources[source] = updated

    symbols_text = symbols_path.read_text(encoding="utf-8")
    updated_symbols = update_symbols(symbols_text, mappings)
    return (
        updated_sources,
        function_rows,
        matching,
        updated_symbols,
        moves,
        replacements,
    )


def apply(root: Path, mappings: list[Mapping], *, check: bool) -> int:
    (
        updated_sources,
        function_rows,
        matching,
        updated_symbols,
        moves,
        replacements,
    ) = plan(root, mappings)
    functions_path = root / "config/slus_01411/functions.csv"
    matching_path = root / "config/slus_01411/matching_c.json"
    symbols_path = root / "config/slus_01411/symbols.txt"

    current_functions = load_functions(functions_path)
    current_matching = load_matching(matching_path)
    current_symbols = symbols_path.read_text(encoding="utf-8")
    pending = (
        bool(updated_sources)
        or current_functions != function_rows
        or current_matching != matching
        or current_symbols != updated_symbols
        or bool(moves)
    )
    if check:
        if pending:
            print(
                f"semantic names require changes: "
                f"{len(replacements)} symbols, {len(updated_sources)} sources, "
                f"{len(moves)} moves"
            )
            return 1
        print(f"semantic names: OK ({len(mappings)} mappings)")
        return 0

    for path, text in updated_sources.items():
        path.write_text(text, encoding="utf-8")
    write_csv(functions_path, function_rows)
    write_json(matching_path, matching)
    symbols_path.write_text(updated_symbols, encoding="utf-8")
    for move in moves:
        move.destination.parent.mkdir(parents=True, exist_ok=True)
        move.source.rename(move.destination)
    print(
        f"applied semantic names: {len(mappings)} mappings, "
        f"{len(updated_sources)} source edits, {len(moves)} source moves"
    )
    return 0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Apply an evidence-backed semantic symbol map."
    )
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--apply", action="store_true")
    mode.add_argument("--check", action="store_true")
    parser.add_argument(
        "--map",
        default="notes/semantic-symbol-map.csv",
        help="semantic map path relative to the repository root",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        map_path = resolve_within(root, args.map, must_exist=True)
        mappings = load_map(map_path)
        return apply(root, mappings, check=args.check)
    except (
        SemanticNameError,
        WorkspaceError,
        OSError,
        UnicodeError,
        ValueError,
        KeyError,
        TypeError,
        json.JSONDecodeError,
        csv.Error,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
