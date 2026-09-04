#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

from function_inventory import (
    Function,
    InventoryError,
    load_inventory,
    parse_generated_function_tree,
)
from workspace import WorkspaceError, require_workspace_root, resolve_within


class ProgressError(RuntimeError):
    pass


README_PROGRESS_START = "<!-- BEGIN GENERATED PROGRESS -->"
README_PROGRESS_END = "<!-- END GENERATED PROGRESS -->"


def parse_integer(value: Any, description: str) -> int:
    if isinstance(value, int) and not isinstance(value, bool):
        return value
    if isinstance(value, str):
        try:
            return int(value, 0)
        except ValueError as error:
            raise ProgressError(
                f"{description} is not a valid integer: {value}"
            ) from error
    raise ProgressError(f"{description} must be an integer or integer string")


def load_text_size(root: Path) -> int:
    path = resolve_within(
        root, "config/slus_01411/image_map.json", must_exist=True
    )
    with path.open("r", encoding="utf-8") as handle:
        image_map = json.load(handle)
    for region in image_map["regions"]:
        if region["name"] == "text":
            start = parse_integer(region["file_start"], "text.file_start")
            end = parse_integer(region["file_end"], "text.file_end")
            return end - start
    raise ProgressError("image map has no text region")


def validate_inventory(
    generated: list[Function], inventory: list[Function]
) -> None:
    generated_by_address = {function.address: function for function in generated}
    inventory_by_address = {function.address: function for function in inventory}
    if len(generated_by_address) != len(generated):
        raise ProgressError("generated function list contains duplicate addresses")
    if len(inventory_by_address) != len(inventory):
        raise ProgressError("function inventory contains duplicate addresses")
    expected_generated = {
        address
        for address, function in inventory_by_address.items()
        if function.status != "matching_c"
    }
    if set(generated_by_address) != expected_generated:
        raise ProgressError(
            "function inventory does not match the generated split; run make inventory"
        )
    for address, generated_function in generated_by_address.items():
        inventory_function = inventory_by_address[address]
        if (
            generated_function.size != inventory_function.size
            or generated_function.name != inventory_function.name
        ):
            raise ProgressError(
                f"function inventory differs at {address:#010x}; "
                "run make inventory"
            )


def atomic_write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(f"{path.name}.tmp")
    try:
        temporary.write_text(text, encoding="utf-8")
        temporary.replace(path)
    except OSError:
        temporary.unlink(missing_ok=True)
        raise


def atomic_write_json(path: Path, value: dict[str, Any]) -> None:
    atomic_write_text(path, json.dumps(value, indent=2, sort_keys=True) + "\n")


def format_bytes(value: int) -> str:
    return f"{value:,} (`0x{value:X}`)"


def format_percentage(value: int, total: int) -> str:
    if total <= 0:
        raise ProgressError("progress percentage has an empty denominator")
    return f"{value / total:.2%}"


def load_overlay_inventories(root: Path) -> dict[str, dict[str, int]]:
    directory = resolve_within(root, "config/slus_01411/overlays")
    overlays: dict[str, dict[str, int]] = {}
    for path in sorted(directory.glob("*_functions.csv")):
        name = path.name[: -len("_functions.csv")]
        functions = load_inventory(path)
        matching = [
            function for function in functions if function.status == "matching_c"
        ]
        overlays[name] = {
            "function_count": len(functions),
            "function_bytes": sum(function.size for function in functions),
            "matching_c_function_count": len(matching),
            "matching_c_bytes": sum(function.size for function in matching),
        }
    return overlays


def render_overlay_progress(overlays: dict[str, dict[str, int]]) -> list[str]:
    if not overlays:
        return []
    lines = [
        "Runtime overlay modules:",
        "",
        "| Module | Matching C functions | Matching C bytes |",
        "|---|---:|---:|",
    ]
    for name in sorted(overlays):
        overlay = overlays[name]
        count = overlay["matching_c_function_count"]
        total_count = overlay["function_count"]
        matched = overlay["matching_c_bytes"]
        total_bytes = overlay["function_bytes"]
        lines.append(
            f"| `{name}` | "
            f"{count:,} / {total_count:,} "
            f"({format_percentage(count, total_count)}) | "
            f"{format_bytes(matched)} / {format_bytes(total_bytes)} "
            f"({format_percentage(matched, total_bytes)}) |"
        )
    lines.append("")
    return lines


def render_readme_progress(progress: dict[str, Any]) -> str:
    game_count = progress["game_function_count"]
    game_bytes = progress["game_function_bytes"]
    matching_count = progress["matching_c_function_count"]
    matching_bytes = progress["matching_c_bytes"]
    assembly_count = progress["assembly_function_count"]
    assembly_bytes = progress["assembly_function_bytes"]
    handwritten_count = progress["handwritten_function_count"]
    handwritten_bytes = progress["handwritten_function_bytes"]
    sdk_count = progress["sdk_function_count"]
    sdk_bytes = progress["sdk_function_bytes"]

    return "\n".join(
        (
            "| Metric | Current |",
            "|---|---:|",
            (
                "| Matching C functions | "
                f"**{matching_count:,} / {game_count:,} "
                f"({format_percentage(matching_count, game_count)})** |"
            ),
            (
                "| Matching C bytes | "
                f"**{format_bytes(matching_bytes)} / {format_bytes(game_bytes)} "
                f"({format_percentage(matching_bytes, game_bytes)})** |"
            ),
            (
                "| Remaining compiler-generated game assembly | "
                f"{assembly_count:,} functions, {format_bytes(assembly_bytes)} |"
            ),
            (
                "| Intentional handwritten game assembly | "
                f"{handwritten_count:,} functions, "
                f"{format_bytes(handwritten_bytes)} |"
            ),
            (
                "| Preserved Psy-Q CRT/SDK assembly | "
                f"{sdk_count:,} functions, {format_bytes(sdk_bytes)} |"
            ),
            (
                "| Total discovered functions | "
                f"{progress['function_count']:,} |"
            ),
            (
                "| Embedded/unassigned resident text | "
                f"{format_bytes(progress['unassigned_text_bytes'])} |"
            ),
            "",
            *render_overlay_progress(progress.get("overlays", {})),
            (
                "_Generated from `config/slus_01411/functions.csv` and "
                "`config/slus_01411/overlays/*_functions.csv` by "
                "`tools/project/progress.py`._"
            ),
        )
    )


def expected_readme(current: str, generated: str) -> str:
    if current.count(README_PROGRESS_START) != 1:
        raise ProgressError(
            f"README.md must contain exactly one {README_PROGRESS_START}"
        )
    if current.count(README_PROGRESS_END) != 1:
        raise ProgressError(
            f"README.md must contain exactly one {README_PROGRESS_END}"
        )

    prefix, remainder = current.split(README_PROGRESS_START, 1)
    _, suffix = remainder.split(README_PROGRESS_END, 1)
    replacement = (
        f"{README_PROGRESS_START}\n\n{generated}\n\n{README_PROGRESS_END}"
    )
    return prefix + replacement + suffix


def sync_readme(
    root: Path, progress: dict[str, Any], *, check: bool
) -> str:
    path = resolve_within(root, "README.md", must_exist=True)
    current = path.read_text(encoding="utf-8")
    expected = expected_readme(current, render_readme_progress(progress))
    if check:
        if current != expected:
            raise ProgressError("README.md progress is stale; run make progress")
        return "current"

    if current == expected:
        return "current"
    atomic_write_text(path, expected)
    return "updated"


def calculate(root: Path) -> dict[str, Any]:
    assembly_root = resolve_within(
        root, "tmp/splat/asm", must_exist=True
    )
    generated = parse_generated_function_tree(assembly_root)
    inventory_path = resolve_within(
        root, "config/slus_01411/functions.csv", must_exist=True
    )
    functions = load_inventory(inventory_path)
    validate_inventory(generated, functions)
    text_bytes = load_text_size(root)
    function_bytes = sum(function.size for function in functions)
    handwritten = [
        function for function in functions if function.status == "handwritten_asm"
    ]
    handwritten_bytes = sum(function.size for function in handwritten)
    assembly = [
        function for function in functions if function.status == "unmatched_asm"
    ]
    assembly_bytes = sum(function.size for function in assembly)
    sdk = [function for function in functions if function.status == "sdk_asm"]
    sdk_bytes = sum(function.size for function in sdk)
    matching = [function for function in functions if function.status == "matching_c"]
    matching_bytes = sum(function.size for function in matching)
    game = [function for function in functions if function.module == "game"]
    game_bytes = sum(function.size for function in game)
    game_status_count = len(handwritten) + len(assembly) + len(matching)
    if len(game) != game_status_count:
        raise ProgressError(
            "game function count does not match matching/assembly statuses"
        )
    modules: dict[str, dict[str, int]] = {}
    for function in functions:
        module = modules.setdefault(
            function.module,
            {"function_count": 0, "function_bytes": 0},
        )
        module["function_count"] += 1
        module["function_bytes"] += function.size
    if function_bytes > text_bytes:
        raise ProgressError(
            f"function bytes {function_bytes:#x} exceed text size {text_bytes:#x}"
        )

    return {
        "target": "SLUS-01411",
        "text_bytes": text_bytes,
        "function_count": len(functions),
        "function_bytes": function_bytes,
        "game_function_count": len(game),
        "game_function_bytes": game_bytes,
        "handwritten_function_count": len(handwritten),
        "handwritten_function_bytes": handwritten_bytes,
        "assembly_function_count": len(assembly),
        "assembly_function_bytes": assembly_bytes,
        "sdk_function_count": len(sdk),
        "sdk_function_bytes": sdk_bytes,
        "matching_c_function_count": len(matching),
        "matching_c_bytes": matching_bytes,
        "modules": modules,
        "overlays": load_overlay_inventories(root),
        "unassigned_text_bytes": text_bytes - function_bytes,
    }


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate project progress metrics and README status."
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail if the generated README progress section is stale",
    )
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    try:
        root = require_workspace_root()
        progress = calculate(root)
        output = resolve_within(root, "tmp/reports/progress.json")
        atomic_write_json(output, progress)
        readme_status = sync_readme(
            root,
            progress,
            check=arguments.check,
        )
    except (
        ProgressError,
        InventoryError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    print(f"functions:          {progress['function_count']}")
    print(f"function bytes:     {progress['function_bytes']:#x}")
    print(
        "handwritten:        "
        f"{progress['handwritten_function_count']} functions, "
        f"{progress['handwritten_function_bytes']:#x} bytes"
    )
    print(
        "SDK/startup:        "
        f"{progress['sdk_function_count']} functions, "
        f"{progress['sdk_function_bytes']:#x} bytes"
    )
    print(f"assembly functions: {progress['assembly_function_bytes']:#x} bytes")
    print(f"matching C:         {progress['matching_c_bytes']:#x} bytes")
    print(f"unassigned text:    {progress['unassigned_text_bytes']:#x} bytes")
    print(f"report:             {output.relative_to(root)}")
    print(f"README.md:          {readme_status}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
