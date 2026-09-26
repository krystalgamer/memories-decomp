#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

from classify_functions import (
    ClassificationError,
    classify_function,
    load_regions,
    validate_coverage,
)
from function_inventory import (
    Function,
    InventoryError,
    load_inventory,
    parse_generated_function_tree,
    validate_function_order,
)
from overlay_extract import OverlayError, load_manifest as load_overlay_manifest
from regional_inventory import load_matching_ranges
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


def load_image_map(root: Path, config: str) -> dict[str, Any]:
    path = resolve_within(
        root, f"{config}/image_map.json", must_exist=True
    )
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def load_text_size(root: Path, config: str = "config/slus_01411") -> int:
    image_map = load_image_map(root, config)
    for region in image_map["regions"]:
        if region["name"] == "text":
            start = parse_integer(region["file_start"], "text.file_start")
            end = parse_integer(region["file_end"], "text.file_end")
            return end - start
    raise ProgressError("image map has no text region")


def format_address_sample(addresses: set[int], limit: int = 5) -> str:
    ordered = sorted(addresses)
    shown = ", ".join(f"{address:#010x}" for address in ordered[:limit])
    if len(ordered) > limit:
        shown += f", and {len(ordered) - limit} more"
    return shown


def describe_inventory_mismatch(
    generated: set[int], expected: set[int]
) -> str:
    extra = generated - expected
    missing = expected - generated
    parts: list[str] = []
    if extra:
        parts.append(
            "the split defines "
            f"{len(extra)} function(s) the inventory does not list "
            f"({format_address_sample(extra)}); a symbol declared inside the "
            "text range but outside any function, such as a segment boundary "
            "marker, produces this and should carry ignore:True in symbols.txt"
        )
    if missing:
        parts.append(
            "the inventory lists "
            f"{len(missing)} non-matching function(s) the split does not "
            f"define ({format_address_sample(missing)}); run make inventory"
        )
    return "; ".join(parts)


def coalesce_sdk_fragments(
    generated: list[Function], inventory: list[Function]
) -> list[Function]:
    """Prefer authoritative SDK extents over call-target split heuristics."""
    sdk_by_address = {
        function.address: function
        for function in inventory
        if function.status == "sdk_asm"
    }
    ordered = sorted(generated, key=lambda function: function.address)
    coalesced: list[Function] = []
    index = 0
    while index < len(ordered):
        first = ordered[index]
        sdk = sdk_by_address.get(first.address)
        if sdk is None or first.name != sdk.name:
            coalesced.append(first)
            index += 1
            continue

        end = sdk.address + sdk.size
        cursor = sdk.address
        next_index = index
        while (
            next_index < len(ordered)
            and ordered[next_index].address < end
            and ordered[next_index].address == cursor
        ):
            cursor += ordered[next_index].size
            next_index += 1
        if cursor != end:
            coalesced.append(first)
            index += 1
            continue

        coalesced.append(
            Function(
                address=sdk.address,
                size=sdk.size,
                name=sdk.name,
                status="unmatched_asm",
            )
        )
        index = next_index
    return coalesced


def validate_inventory(
    generated: list[Function], inventory: list[Function]
) -> None:
    generated = coalesce_sdk_fragments(generated, inventory)
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
            "function inventory does not match the generated split; "
            + describe_inventory_mismatch(
                set(generated_by_address), expected_generated
            )
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


def load_japanese_overlay_inventories(root: Path) -> dict[str, dict[str, int]]:
    sector_size, modules = load_overlay_manifest(root, "japan")
    directory = resolve_within(root, "config/slpm_86398/overlays", must_exist=True)
    overlays: dict[str, dict[str, int]] = {}
    for module in modules:
        layout_name = module.get("layout")
        if not isinstance(layout_name, str) or not layout_name:
            raise ProgressError(f"{module['name']}: missing overlay layout")
        layout = resolve_within(root, layout_name, must_exist=True)
        name = layout.stem
        if layout.parent != directory or module["name"] != f"japanese_{name}":
            raise ProgressError(f"{module['name']}: invalid Japanese overlay layout")
        if name in overlays:
            raise ProgressError(f"duplicate Japanese overlay layout: {name}")

        manifest_path = resolve_within(
            root,
            layout.with_name(f"{name}_matching_c.json").relative_to(root),
            must_exist=True,
        )
        start = parse_integer(module["load_address"], f"{name}.load_address")
        sectors = parse_integer(module["sector_count"], f"{name}.sector_count")
        if sectors <= 0:
            raise ProgressError(f"{name}: sector_count must be positive")
        end = start + sectors * sector_size
        ranges = load_matching_ranges(manifest_path)
        inventory_path = resolve_within(
            root,
            layout.with_name(f"{name}_functions.csv").relative_to(root),
            must_exist=True,
        )
        functions = load_inventory(inventory_path)
        validate_overlay_inventory(
            functions, ranges, start=start, end=end, name=name
        )
        matched = [function for function in functions if function.status == "matching_c"]
        overlays[name] = {
            "function_count": len(functions),
            "function_bytes": sum(function.size for function in functions),
            "matching_c_function_count": len(matched),
            "matching_c_bytes": sum(function.size for function in matched),
        }
    return overlays


def validate_overlay_inventory(
    functions: list[Function],
    ranges: list[tuple[int, int]],
    *,
    start: int,
    end: int,
    name: str,
) -> None:
    validate_function_order(functions, f"{name} function inventory")
    actual_matches: dict[int, int] = {}
    for function in functions:
        if not start <= function.address < function.address + function.size <= end:
            raise ProgressError(f"{name}: function outside overlay image")
        if function.module != f"overlay/{name}":
            raise ProgressError(f"{name}: unexpected function module {function.module}")
        if function.status not in {"matching_c", "unmatched_asm", "handwritten_asm"}:
            raise ProgressError(f"{name}: unexpected function status {function.status}")
        if function.status == "matching_c":
            actual_matches[function.address] = function.size
    if actual_matches != dict(ranges):
        raise ProgressError(
            f"{name}: matching-C inventory does not agree with the matching manifest"
        )


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
        matched = overlay["matching_c_bytes"]
        total_count = overlay["function_count"]
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


def render_readme_progress(
    progress: dict[str, Any],
    source_description: str = (
        "`config/slus_01411/functions.csv` and "
        "`config/slus_01411/overlays/*_functions.csv`"
    ),
) -> str:
    game_count = progress["game_function_count"]
    target_count = progress["decompilation_target_function_count"]
    target_bytes = progress["decompilation_target_function_bytes"]
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
                "| Game C-decompilation targets matched | "
                f"**{matching_count:,} / {target_count:,} "
                f"({format_percentage(matching_count, target_count)})** |"
            ),
            (
                "| Game C-decompilation target bytes matched | "
                f"**{format_bytes(matching_bytes)} / "
                f"{format_bytes(target_bytes)} "
                f"({format_percentage(matching_bytes, target_bytes)})** |"
            ),
            (
                "| Remaining game C-decompilation targets | "
                f"{assembly_count:,} "
                f"{'function' if assembly_count == 1 else 'functions'}, "
                f"{format_bytes(assembly_bytes)} |"
            ),
            (
                "| Evidence-backed handwritten game assembly | "
                f"{handwritten_count:,} functions, "
                f"{format_bytes(handwritten_bytes)} |"
            ),
            f"| Total game-owned functions | {game_count:,} |",
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
                f"_Generated from {source_description} by "
                "`tools/project/progress.py`._"
            ),
        )
    )


def render_japanese_progress(progress: dict[str, Any]) -> str:
    return "\n".join(
        (
            "### Japanese (`SLPM-86398`)",
            "",
            f"Target SHA-256: `{progress['target_sha256']}`",
            "",
            render_readme_progress(
                progress,
                (
                    "`config/slpm_86398/functions.csv` and "
                    "`config/slpm_86398/overlays/*_functions.csv`, "
                    "validated against their matching-C manifests"
                ),
            ),
        )
    )


def render_regional_progress(
    north_american: dict[str, Any], japanese: dict[str, Any]
) -> str:
    return "\n\n".join(
        (
            "### North American (`SLUS-01411`)\n\n"
            f"Target SHA-256: `{north_american['target_sha256']}`\n\n"
            + render_readme_progress(north_american),
            render_japanese_progress(japanese),
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
    root: Path,
    progress: dict[str, Any],
    japanese: dict[str, Any],
    *,
    check: bool,
) -> str:
    path = resolve_within(root, "README.md", must_exist=True)
    current = path.read_text(encoding="utf-8")
    expected = expected_readme(
        current, render_regional_progress(progress, japanese)
    )
    if check:
        if current != expected:
            raise ProgressError("README.md progress is stale; run make progress")
        return "current"

    if current == expected:
        return "current"
    atomic_write_text(path, expected)
    return "updated"


def summarize_functions(functions: list[Function], text_bytes: int) -> dict[str, Any]:
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
    decompilation_target_count = len(matching) + len(assembly)
    decompilation_target_bytes = matching_bytes + assembly_bytes
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
        "function_count": len(functions),
        "function_bytes": function_bytes,
        "game_function_count": len(game),
        "game_function_bytes": game_bytes,
        "decompilation_target_function_count": decompilation_target_count,
        "decompilation_target_function_bytes": decompilation_target_bytes,
        "handwritten_function_count": len(handwritten),
        "handwritten_function_bytes": handwritten_bytes,
        "assembly_function_count": len(assembly),
        "assembly_function_bytes": assembly_bytes,
        "sdk_function_count": len(sdk),
        "sdk_function_bytes": sdk_bytes,
        "matching_c_function_count": len(matching),
        "matching_c_bytes": matching_bytes,
        "modules": modules,
        "unassigned_text_bytes": text_bytes - function_bytes,
    }


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
    image_map = load_image_map(root, "config/slus_01411")
    text_bytes = load_text_size(root)
    return {
        "target": "SLUS-01411",
        "target_sha256": image_map["target_sha256"],
        "text_bytes": text_bytes,
        **summarize_functions(functions, text_bytes),
        "overlays": load_overlay_inventories(root),
    }


def validate_japanese_inventory(
    root: Path, functions: list[Function], image_map: dict[str, Any]
) -> None:
    config = "config/slpm_86398"
    regions_path = f"{config}/function_regions.json"
    regions = load_regions(root, regions_path)
    text_region = next(
        (
            region
            for region in image_map["regions"]
            if region.get("name") == "text"
        ),
        None,
    )
    if text_region is None:
        raise ProgressError("Japanese image map has no text region")
    text_start = parse_integer(
        text_region.get("vram_start"), "Japanese text.vram_start"
    )
    text_end = parse_integer(
        text_region.get("vram_end"), "Japanese text.vram_end"
    )
    if regions[0]["start"] != text_start or regions[-1]["end"] != text_end:
        raise ProgressError(
            f"{regions_path}: regions must cover Japanese resident text "
            f"{text_start:#010x}..{text_end:#010x}"
        )
    validate_function_order(functions, "Japanese function inventory")
    validate_coverage(functions, regions)
    for function in functions:
        classified = classify_function(function, regions)
        if classified != function:
            raise ProgressError(
                f"Japanese function {function.address:#010x} disagrees with "
                f"{regions_path}"
            )
        if function.module == "game":
            if function.status not in {
                "matching_c", "unmatched_asm", "handwritten_asm"
            }:
                raise ProgressError(
                    f"Japanese game function {function.address:#010x} is not game code"
                )
        elif function.status != "sdk_asm" or function.module not in {
            "psyq/crt", "psyq/sdk"
        }:
            raise ProgressError(
                f"Japanese non-game function {function.address:#010x} is not SDK code"
            )
        if (
            function.status == "matching_c"
            and any(
                region["start"] <= function.address < region["end"]
                and region["status"] is not None
                for region in regions
            )
        ):
            raise ProgressError(
                f"Japanese matching C {function.address:#010x} is in assembly region"
            )

    manifest = resolve_within(root, f"{config}/matching_c.json", must_exist=True)
    expected = dict(load_matching_ranges(manifest))
    actual = {
        function.address: function.size
        for function in functions if function.status == "matching_c"
    }
    if actual != expected:
        raise ProgressError(
            "Japanese matching-C inventory does not agree with the matching manifest"
        )


def calculate_japanese(root: Path) -> dict[str, Any]:
    config = "config/slpm_86398"
    image_map = load_image_map(root, config)
    assembly_root = resolve_within(
        root, "tmp/splat/slpm_86398/asm", must_exist=True
    )
    generated = parse_generated_function_tree(assembly_root)
    inventory_path = resolve_within(
        root, f"{config}/functions.csv", must_exist=True
    )
    functions = load_inventory(inventory_path)
    validate_inventory(generated, functions)
    validate_japanese_inventory(root, functions, image_map)
    text_bytes = load_text_size(root, config)
    metrics = summarize_functions(functions, text_bytes)
    return {
        "target": "SLPM-86398",
        "target_sha256": image_map["target_sha256"],
        "text_bytes": text_bytes,
        **metrics,
        "overlays": load_japanese_overlay_inventories(root),
        "fallback_bytes": text_bytes - metrics["matching_c_bytes"],
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
        japanese = calculate_japanese(root)
        output = resolve_within(root, "tmp/reports/progress.json")
        atomic_write_json(output, {**progress, "japanese": japanese})
        readme_status = sync_readme(
            root,
            progress,
            japanese,
            check=arguments.check,
        )
    except (
        ClassificationError,
        ProgressError,
        InventoryError,
        OverlayError,
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
    print(
        "Japanese matching:  "
        f"{japanese['matching_c_function_count']} functions, "
        f"{japanese['matching_c_bytes']:#x} bytes"
    )
    print(f"unassigned text:    {progress['unassigned_text_bytes']:#x} bytes")
    print(f"report:             {output.relative_to(root)}")
    print(f"README.md:          {readme_status}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
