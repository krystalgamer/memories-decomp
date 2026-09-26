#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from dataclasses import replace
from pathlib import Path
from typing import Any

from build_baseline import BuildError, tool
from classify_functions import (
    ClassificationError,
    classify_function,
    load_regions,
    validate_coverage,
)
from function_inventory import (
    FUNCTION_PATTERN,
    Function,
    InventoryError,
    load_inventory,
    parse_generated_function_tree,
    validate_function_order,
    write_inventory,
)
from workspace import WorkspaceError, require_workspace_root, resolve_within


def parse_integer(value: Any, description: str) -> int:
    if isinstance(value, int) and not isinstance(value, bool):
        return value
    if isinstance(value, str):
        try:
            return int(value, 0)
        except ValueError as error:
            raise InventoryError(f"{description} is not an integer: {value}") from error
    raise InventoryError(f"{description} must be an integer or integer string")


def load_matching_ranges(path: Path) -> list[tuple[int, int]]:
    with path.open("r", encoding="utf-8") as handle:
        manifest = json.load(handle)
    entries = manifest.get("functions") if isinstance(manifest, dict) else None
    if (
        not isinstance(manifest, dict)
        or manifest.get("schema") != 1
        or not isinstance(entries, list)
    ):
        raise InventoryError(f"{path}: unsupported matching-C configuration")

    ranges: list[tuple[int, int]] = []
    for index, entry in enumerate(entries):
        if not isinstance(entry, dict):
            raise InventoryError(f"{path}: function {index} must be an object")
        address = parse_integer(entry.get("address"), f"{path}: function {index} address")
        size = parse_integer(entry.get("size"), f"{path}: function {index} size")
        if address < 0 or size <= 0:
            raise InventoryError(f"{path}: function {index} has an invalid range")
        ranges.append((address, size))

    ordered = sorted(ranges)
    for (address, size), (next_address, _) in zip(ordered, ordered[1:]):
        if address + size > next_address:
            raise InventoryError(f"{path}: overlapping matching-C functions")
    return ordered


SYMBOL_PATTERN = re.compile(
    r"^(?P<address>[0-9A-Fa-f]+)\s+(?P<size>[0-9A-Fa-f]+)\s+"
    r"[Tt]\s+(?P<name>\S+)$"
)
INSTRUCTION_WORD_PATTERN = re.compile(
    r"^/\*\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]{8}\s+"
    r"(?P<bytes>[0-9A-Fa-f]{8})\s+\*/"
)


def instruction_shapes(
    assembly_root: Path, names: set[str]
) -> dict[str, tuple[int, ...]]:
    shapes: dict[str, tuple[int, ...]] = {}
    for path in sorted(assembly_root.rglob("*.s")):
        current: str | None = None
        words: list[int] = []
        for raw_line in path.read_text(encoding="utf-8").splitlines():
            line = raw_line.strip()
            match = FUNCTION_PATTERN.fullmatch(line)
            if match is not None:
                if current is not None:
                    if current in shapes:
                        raise InventoryError(f"{assembly_root}: duplicate {current}")
                    shapes[current] = tuple(words)
                current = match.group("name") if match.group("name") in names else None
                words = []
                continue
            word = INSTRUCTION_WORD_PATTERN.match(line)
            if current is not None and word is not None:
                value = int.from_bytes(bytes.fromhex(word.group("bytes")), "little")
                opcode = value >> 26
                if opcode in (2, 3):
                    value &= 0xFC000000
                elif opcode not in (0, 16, 17, 18, 19):
                    value &= 0xFFFF0000
                words.append(value)
        if current is not None:
            if current in shapes:
                raise InventoryError(f"{assembly_root}: duplicate {current}")
            shapes[current] = tuple(words)
    missing = names - set(shapes)
    if missing:
        raise InventoryError(
            f"{assembly_root}: missing handwritten assembly "
            + ", ".join(sorted(missing)[:5])
        )
    return shapes


def load_text_symbols(root: Path, elf: Path) -> dict[tuple[int, int], list[str]]:
    nm = tool(root, "nm")
    result = subprocess.run(
        [str(nm), "-S", "--defined-only", "--numeric-sort", str(elf)],
        cwd=root,
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        raise InventoryError(f"{elf}: nm failed: {result.stderr.strip()}")

    symbols: dict[tuple[int, int], list[str]] = {}
    for line in result.stdout.splitlines():
        match = SYMBOL_PATTERN.fullmatch(line.strip())
        if match is None:
            continue
        name = match.group("name")
        if name.endswith(".NON_MATCHING"):
            continue
        key = (int(match.group("address"), 16), int(match.group("size"), 16))
        symbols.setdefault(key, []).append(name)
    return symbols


def matching_functions(
    ranges: list[tuple[int, int]], symbols: dict[tuple[int, int], list[str]]
) -> list[Function]:
    matching: list[Function] = []
    for address, size in ranges:
        names = symbols.get((address, size), [])
        if len(names) != 1:
            raise InventoryError(
                f"{address:#010x} ({size:#x} bytes): expected one exact-size "
                f"linked C symbol, found {names}"
            )
        matching.append(Function(address, size, names[0], "matching_c"))
    return matching


def refresh_inventory(
    generated: list[Function],
    matching: list[Function],
    existing: list[Function],
    *,
    regions: list[dict[str, Any]] | None = None,
    module: str = "",
    handwritten_reference: list[Function] | None = None,
    handwritten_shapes: dict[str, tuple[int, ...]] | None = None,
    generated_shapes: dict[str, tuple[int, ...]] | None = None,
) -> list[Function]:
    if (regions is None and not module) or (regions is not None and module):
        raise InventoryError("provide exactly one of regions or an overlay module")
    if handwritten_reference is not None and (
        regions is None
        or handwritten_shapes is None
        or generated_shapes is None
    ):
        raise InventoryError("handwritten reference requires verified instruction shapes")

    reference = {
        function.name: function
        for function in handwritten_reference or []
        if function.status == "handwritten_asm" and function.module == "game"
    }
    if len(reference) != sum(
        function.status == "handwritten_asm" and function.module == "game"
        for function in handwritten_reference or []
    ):
        raise InventoryError("handwritten reference has duplicate names")

    previous = {function.address: function for function in existing}
    if len(previous) != len(existing):
        raise InventoryError("existing inventory contains duplicate addresses")

    current = sorted([*generated, *matching], key=lambda function: function.address)
    validate_function_order(current, "regional function inventory")
    removed = set(previous) - {function.address for function in current}
    if removed:
        raise InventoryError(f"inventory boundaries changed; missing {sorted(removed)}")

    refreshed: list[Function] = []
    for function in current:
        old = previous.get(function.address)
        if old is not None and old.size != function.size:
            raise InventoryError(
                f"{function.address:#010x}: size changed from "
                f"{old.size:#x} to {function.size:#x}"
            )
        if regions is not None:
            classified = classify_function(function, regions)
            if function.status == "matching_c" and classified.module != "game":
                raise InventoryError(
                    f"{function.address:#010x}: matching C is not game-owned"
                )
        else:
            classified = replace(function, module=module)
        if (
            classified.module == "game"
            and function.status == "unmatched_asm"
            and function.name in reference
        ):
            counterpart = reference[function.name]
            original_shape = handwritten_shapes.get(function.name, ())
            new_shape = generated_shapes.get(function.name, ())
            if (
                counterpart.size != function.size
                or len(original_shape) != function.size // 4
                or len(new_shape) != function.size // 4
                or original_shape != new_shape
            ):
                raise InventoryError(
                    f"{function.name}: Japanese/US handwritten instruction "
                    "shapes differ"
                )
        if old is not None:
            if old.module != classified.module:
                raise InventoryError(
                    f"{function.address:#010x}: module changed from "
                    f"{old.module} to {classified.module}"
                )
            if function.status == "matching_c":
                classified = replace(
                    classified, notes=old.notes if old.status == "matching_c" else ""
                )
            elif old.status == "matching_c":
                raise InventoryError(
                    f"{function.address:#010x}: matching C reverted to assembly"
                )
            elif classified.status == "unmatched_asm":
                classified = replace(classified, status=old.status, notes=old.notes)
            else:
                classified = replace(classified, notes=old.notes)
        elif classified.status == "unmatched_asm" and function.name in reference:
            counterpart = reference[function.name]
            classified = replace(
                classified,
                status="handwritten_asm",
                notes=(
                    "Matches the name, size, and opcode/register instruction "
                    "shape of independently classified North American handwritten "
                    f"function at {counterpart.address:#010x}."
                ),
            )
        refreshed.append(classified)

    if regions is not None:
        validate_coverage(refreshed, regions)
    return refreshed


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Refresh a versioned resident or runtime-overlay function inventory."
    )
    parser.add_argument("--assembly-root", required=True)
    parser.add_argument("--manifest", required=True, help="matching_c.json path")
    parser.add_argument("--elf", required=True, help="byte-exact linked ELF")
    parser.add_argument("--output", required=True)
    owner = parser.add_mutually_exclusive_group(required=True)
    owner.add_argument("--regions", help="function_regions.json path")
    owner.add_argument("--module", help="overlay module name, e.g. overlay/password")
    parser.add_argument(
        "--handwritten-reference",
        help="previously verified handwritten inventory (resident images only)",
    )
    parser.add_argument(
        "--reference-assembly-root",
        help="generated assembly of the handwritten reference image",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_arguments()
    try:
        root = require_workspace_root()
        if bool(args.handwritten_reference) != bool(args.reference_assembly_root):
            raise InventoryError(
                "--handwritten-reference and --reference-assembly-root "
                "must be supplied together"
            )
        assembly_root = resolve_within(
            root, args.assembly_root, must_exist=bool(args.regions)
        )
        manifest = resolve_within(root, args.manifest, must_exist=True)
        elf = resolve_within(root, args.elf, must_exist=True)
        output = resolve_within(root, args.output)
        regions = load_regions(root, args.regions) if args.regions else None
        reference = (
            load_inventory(resolve_within(root, args.handwritten_reference, must_exist=True))
            if args.handwritten_reference else None
        )
        generated = (
            parse_generated_function_tree(
                assembly_root, require_functions=regions is not None
            )
            if assembly_root.exists() else []
        )
        matching = matching_functions(
            load_matching_ranges(manifest), load_text_symbols(root, elf)
        )
        existing = load_inventory(output) if output.exists() else []
        if reference is not None:
            names = {
                function.name for function in reference
                if function.status == "handwritten_asm" and function.module == "game"
            }
            handwritten_names = {
                function.name for function in generated
                if function.name in names
            }
            reference_root = resolve_within(
                root, args.reference_assembly_root, must_exist=True
            )
            reference_shapes = instruction_shapes(reference_root, handwritten_names)
            japanese_shapes = instruction_shapes(assembly_root, handwritten_names)
        else:
            reference_shapes = japanese_shapes = None
        refreshed = refresh_inventory(
            generated,
            matching,
            existing,
            regions=regions,
            module=args.module or "",
            handwritten_reference=reference,
            handwritten_shapes=reference_shapes,
            generated_shapes=japanese_shapes,
        )
        write_inventory(output, refreshed)
    except (
        BuildError,
        ClassificationError,
        InventoryError,
        WorkspaceError,
        OSError,
        UnicodeError,
        ValueError,
        TypeError,
        KeyError,
        json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print(f"inventory: {output.relative_to(root)} ({len(refreshed)} functions)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
