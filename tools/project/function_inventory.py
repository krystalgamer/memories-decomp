#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import re
import sys
from dataclasses import dataclass
from pathlib import Path

from workspace import WorkspaceError, require_workspace_root, resolve_within


class InventoryError(RuntimeError):
    pass


FUNCTION_PATTERN = re.compile(
    r"^nonmatching\s+(?P<name>\S+),\s+0x(?P<size>[0-9A-Fa-f]+)$"
)
INSTRUCTION_PATTERN = re.compile(
    r"^/\*\s+[0-9A-Fa-f]+\s+(?P<address>[0-9A-Fa-f]{8})\s+"
    r"[0-9A-Fa-f]{8}\s+\*/"
)
FIELDS = ("address", "size", "name", "status", "module", "notes")
STATUSES = {
    "unmatched_asm",
    "handwritten_asm",
    "sdk_asm",
    "matching_c",
}


@dataclass(frozen=True)
class Function:
    address: int
    size: int
    name: str
    status: str
    module: str = ""
    notes: str = ""


def parse_generated_functions(
    path: Path, *, require_functions: bool = True
) -> list[Function]:
    """Read boundaries without treating disassembler heuristics as provenance."""
    functions: list[Function] = []
    pending: tuple[str, int] | None = None

    with path.open("r", encoding="utf-8") as handle:
        for raw_line in handle:
            line = raw_line.strip()
            function_match = FUNCTION_PATTERN.fullmatch(line)
            if function_match:
                if pending is not None:
                    raise InventoryError(
                        f"{path}: function {pending[0]} has no first instruction"
                    )
                pending = (
                    function_match.group("name"),
                    int(function_match.group("size"), 16),
                )
                continue

            if pending is not None:
                instruction_match = INSTRUCTION_PATTERN.match(line)
                if instruction_match:
                    name, size = pending
                    functions.append(
                        Function(
                            address=int(instruction_match.group("address"), 16),
                            size=size,
                            name=name,
                            status="unmatched_asm",
                        )
                    )
                    pending = None
                continue

    if pending is not None:
        raise InventoryError(f"{path}: function {pending[0]} has no first instruction")
    if require_functions and not functions:
        raise InventoryError(f"{path}: no generated functions found")
    return functions


def validate_function_order(
    functions: list[Function], description: str
) -> None:
    if not functions:
        raise InventoryError(f"{description}: no generated functions found")

    seen_addresses: set[int] = set()
    previous_end = 0
    for function in functions:
        if function.address in seen_addresses:
            raise InventoryError(
                f"{description}: duplicate function address "
                f"{function.address:#010x}"
            )
        if function.address < previous_end:
            raise InventoryError(
                f"{description}: overlapping function at "
                f"{function.address:#010x}"
            )
        seen_addresses.add(function.address)
        previous_end = function.address + function.size


def parse_generated_function_tree(path: Path) -> list[Function]:
    if not path.is_dir():
        raise InventoryError(f"{path}: assembly root is not a directory")
    functions: list[Function] = []
    for assembly_path in sorted(path.rglob("*.s")):
        functions.extend(
            parse_generated_functions(
                assembly_path,
                require_functions=False,
            )
        )
    functions.sort(key=lambda function: function.address)
    validate_function_order(functions, str(path))
    return functions


def parse_hex_field(value: str, description: str) -> int:
    try:
        return int(value, 0)
    except ValueError as error:
        raise InventoryError(f"{description} is not an integer: {value}") from error


def load_inventory(path: Path) -> list[Function]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if tuple(reader.fieldnames or ()) != FIELDS:
            raise InventoryError(
                f"{path}: expected CSV fields {', '.join(FIELDS)}"
            )

        functions: list[Function] = []
        for line_number, row in enumerate(reader, start=2):
            status = row["status"]
            if status not in STATUSES:
                raise InventoryError(
                    f"{path}:{line_number}: unsupported status {status}"
                )
            functions.append(
                Function(
                    address=parse_hex_field(
                        row["address"], f"{path}:{line_number}: address"
                    ),
                    size=parse_hex_field(
                        row["size"], f"{path}:{line_number}: size"
                    ),
                    name=row["name"],
                    status=status,
                    module=row["module"],
                    notes=row["notes"],
                )
            )
    return functions


def merge_inventory(
    generated: list[Function], existing: list[Function]
) -> list[Function]:
    existing_by_address = {function.address: function for function in existing}
    if len(existing_by_address) != len(existing):
        raise InventoryError("existing inventory contains duplicate addresses")

    generated_addresses = {function.address for function in generated}
    removed = sorted(set(existing_by_address) - generated_addresses)
    invalid_removed = [
        address
        for address in removed
        if existing_by_address[address].status != "matching_c"
    ]
    if invalid_removed:
        formatted = ", ".join(
            f"{address:#010x}" for address in invalid_removed[:10]
        )
        raise InventoryError(
            f"generated split removed inventory functions: {formatted}"
        )

    merged: list[Function] = []
    for function in generated:
        previous = existing_by_address.get(function.address)
        if previous is None:
            merged.append(function)
            continue
        if previous.size != function.size:
            raise InventoryError(
                f"{function.address:#010x}: size changed from "
                f"{previous.size:#x} to {function.size:#x}"
            )
        merged.append(
            Function(
                address=function.address,
                size=function.size,
                name=function.name,
                status=previous.status,
                module=previous.module,
                notes=previous.notes,
            )
        )
    merged.extend(existing_by_address[address] for address in removed)
    merged.sort(key=lambda function: function.address)
    validate_function_order(merged, "merged function inventory")
    return merged


def write_inventory(path: Path, functions: list[Function]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(f"{path.name}.tmp")
    try:
        with temporary.open("w", encoding="utf-8", newline="") as handle:
            writer = csv.DictWriter(handle, fieldnames=FIELDS, lineterminator="\n")
            writer.writeheader()
            for function in functions:
                writer.writerow(
                    {
                        "address": f"0x{function.address:08X}",
                        "size": f"0x{function.size:X}",
                        "name": function.name,
                        "status": function.status,
                        "module": function.module,
                        "notes": function.notes,
                    }
                )
        temporary.replace(path)
    except OSError:
        temporary.unlink(missing_ok=True)
        raise


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Create or update the resident-function inventory."
    )
    parser.add_argument(
        "--assembly-root",
        default="tmp/splat/asm",
        help="generated assembly directory relative to the repository root",
    )
    parser.add_argument(
        "--output",
        default="config/slus_01411/functions.csv",
        help="inventory path relative to the repository root",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        assembly_root = resolve_within(
            root, args.assembly_root, must_exist=True
        )
        output = resolve_within(root, args.output)
        generated = parse_generated_function_tree(assembly_root)
        existing = load_inventory(output) if output.exists() else []
        merged = merge_inventory(generated, existing)
        write_inventory(output, merged)
    except (
        InventoryError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        csv.Error,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    print(f"inventory: {output.relative_to(root)}")
    print(f"functions: {len(merged)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
