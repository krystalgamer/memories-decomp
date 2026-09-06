#!/usr/bin/env python3

from __future__ import annotations

import csv
import json
import re
import sys
from pathlib import Path
from typing import Any

import yaml

from workspace import WorkspaceError, require_workspace_root, resolve_within


class GenerationError(RuntimeError):
    pass


PSX_HEADER_SIZE = 0x800
LOAD_ADDRESS = 0x80010000
OVERLAY_REGION_KIND = "overlay_load_slot"
OVERLAY_REGION_PREFIX = "overlay_"


def parse_integer(value: Any, description: str) -> int:
    if isinstance(value, int) and not isinstance(value, bool):
        return value
    if isinstance(value, str):
        try:
            return int(value, 0)
        except ValueError as error:
            raise GenerationError(
                f"{description} is not a valid integer: {value}"
            ) from error
    raise GenerationError(f"{description} must be an integer or integer string")


def load_json(root: Path, relative_path: str) -> dict[str, Any]:
    path = resolve_within(root, relative_path, must_exist=True)
    with path.open("r", encoding="utf-8") as handle:
        value = json.load(handle)
    if not isinstance(value, dict) or value.get("schema") != 1:
        raise GenerationError(f"{path}: unsupported schema")
    return value


def load_inventory(root: Path) -> dict[int, dict[str, str]]:
    path = resolve_within(
        root, "config/slus_01411/functions.csv", must_exist=True
    )
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        rows = list(reader)
    inventory: dict[int, dict[str, str]] = {}
    for row in rows:
        address = parse_integer(row["address"], "function address")
        if address in inventory:
            raise GenerationError(f"duplicate inventory address {address:#010x}")
        inventory[address] = row
    return inventory


def load_profiles(root: Path) -> set[str]:
    configuration = load_json(
        root, "config/slus_01411/compiler_profiles.json"
    )
    profiles = configuration.get("profiles")
    if not isinstance(profiles, dict) or not profiles:
        raise GenerationError("compiler profile configuration is empty")
    return set(profiles)


def load_matching_functions(root: Path) -> list[dict[str, Any]]:
    configuration = load_json(root, "config/slus_01411/matching_c.json")
    functions = configuration.get("functions")
    if not isinstance(functions, list):
        raise GenerationError("matching C functions must be a list")

    inventory = load_inventory(root)
    profiles = load_profiles(root)
    parsed: list[dict[str, Any]] = []
    seen_addresses: set[int] = set()
    source_root = resolve_within(root, "src", must_exist=True)

    for index, function in enumerate(functions):
        if not isinstance(function, dict):
            raise GenerationError(f"matching function {index} must be an object")
        address = parse_integer(
            function.get("address"), f"matching function {index} address"
        )
        size = parse_integer(
            function.get("size"), f"matching function {index} size"
        )
        source_value = function.get("source")
        profile = function.get("profile")
        if not isinstance(source_value, str) or not isinstance(profile, str):
            raise GenerationError(
                f"matching function {address:#010x} has invalid source/profile"
            )
        if address in seen_addresses:
            raise GenerationError(
                f"duplicate matching function address at {address:#010x}"
            )
        if profile not in profiles:
            raise GenerationError(
                f"matching function {address:#010x} uses unknown profile {profile}"
            )

        source = resolve_within(root, source_value, must_exist=True)
        try:
            source_relative = source.relative_to(source_root)
        except ValueError as error:
            raise GenerationError(f"{source_value}: source must be under src/") from error
        if source.suffix != ".c":
            raise GenerationError(f"{source_value}: matching source must be C")

        inventory_row = inventory.get(address)
        if inventory_row is None:
            raise GenerationError(
                f"matching function {address:#010x} is absent from inventory"
            )
        if parse_integer(inventory_row["size"], "inventory size") != size:
            raise GenerationError(
                f"matching function {address:#010x} size disagrees with inventory"
            )
        if inventory_row["status"] != "matching_c":
            raise GenerationError(
                f"matching function {address:#010x} is not marked matching_c"
            )

        parsed.append(
            {
                "address": address,
                "size": size,
                "source": source_value,
                "segment": source_relative.with_suffix("").as_posix(),
                "profile": profile,
            }
        )
        seen_addresses.add(address)

    parsed.sort(key=lambda function: function["address"])
    previous_end = 0
    for function in parsed:
        if function["address"] < previous_end:
            raise GenerationError(
                f"overlapping matching function at {function['address']:#010x}"
            )
        previous_end = function["address"] + function["size"]

    by_source: dict[str, list[dict[str, Any]]] = {}
    for function in parsed:
        by_source.setdefault(function["source"], []).append(function)
    for source_value, members in by_source.items():
        if len(members) == 1:
            continue
        profiles_used = {member["profile"] for member in members}
        if len(profiles_used) != 1:
            raise GenerationError(
                f"{source_value}: grouped functions use multiple profiles"
            )
        source_text = resolve_within(
            root, source_value, must_exist=True
        ).read_text(encoding="utf-8")
        expected = members[0]["address"]
        for member in members:
            if member["address"] != expected:
                raise GenerationError(
                    f"{source_value}: grouped functions are not contiguous"
                )
            name = inventory[member["address"]]["name"]
            if re.search(rf"\b{re.escape(name)}\s*\(", source_text) is None:
                raise GenerationError(
                    f"{source_value}: does not define grouped symbol {name}"
                )
            expected += member["size"]
    return parsed


def group_translation_units(
    functions: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    units: list[dict[str, Any]] = []
    index = 0
    while index < len(functions):
        first = functions[index]
        members = [first]
        index += 1
        while (
            index < len(functions)
            and functions[index]["source"] == first["source"]
        ):
            members.append(functions[index])
            index += 1
        units.append(
            {
                "address": first["address"],
                "size": sum(member["size"] for member in members),
                "source": first["source"],
                "segment": first["segment"],
                "profile": first["profile"],
                "members": [member["address"] for member in members],
            }
        )
    return units


def load_image_regions(root: Path) -> dict[str, dict[str, Any]]:
    configuration = load_json(root, "config/slus_01411/image_map.json")
    regions = configuration.get("regions")
    if not isinstance(regions, list):
        raise GenerationError("image map regions must be a list")
    result = {
        str(region["name"]): region
        for region in regions
        if isinstance(region, dict) and "name" in region
    }
    required = {
        "text",
        "initialized_data",
        "bss_image",
        "reserved_zero",
        "tail_data",
    }
    missing = sorted(required - set(result))
    if missing:
        raise GenerationError("image map is missing regions: " + ", ".join(missing))
    overlay_regions = [
        name
        for name, region in result.items()
        if region.get("kind") == OVERLAY_REGION_KIND
    ]
    if not overlay_regions:
        raise GenerationError("image map has no overlay load-slot regions")
    invalid = [
        name
        for name in overlay_regions
        if not name.startswith(OVERLAY_REGION_PREFIX)
    ]
    if invalid:
        raise GenerationError(
            "overlay load-slot regions need overlay_ names: "
            + ", ".join(invalid)
        )
    return result


def file_offset(address: int) -> int:
    return PSX_HEADER_SIZE + address - LOAD_ADDRESS


def write_json(path: Path, value: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(f"{path.name}.tmp")
    try:
        temporary.write_text(
            json.dumps(value, indent=2, sort_keys=True) + "\n",
            encoding="utf-8",
        )
        temporary.replace(path)
    except OSError:
        temporary.unlink(missing_ok=True)
        raise


def write_yaml(path: Path, value: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(f"{path.name}.tmp")
    try:
        temporary.write_text(
            yaml.safe_dump(value, sort_keys=False, width=1000),
            encoding="utf-8",
        )
        temporary.replace(path)
    except OSError:
        temporary.unlink(missing_ok=True)
        raise


def generate(root: Path) -> tuple[Path, Path]:
    template_path = resolve_within(
        root, "config/slus_01411/split.yaml", must_exist=True
    )
    with template_path.open("r", encoding="utf-8") as handle:
        split_config = yaml.safe_load(handle)
    if not isinstance(split_config, dict):
        raise GenerationError(f"{template_path}: invalid Splat configuration")

    functions = load_matching_functions(root)
    units = group_translation_units(functions)
    regions = load_image_regions(root)
    text_start = parse_integer(regions["text"]["file_start"], "text start")
    text_end = parse_integer(regions["text"]["file_end"], "text end")
    segments = split_config.get("segments")
    if not isinstance(segments, list):
        raise GenerationError("Splat configuration has no segments list")
    main_segments = [
        segment
        for segment in segments
        if isinstance(segment, dict) and segment.get("name") == "main"
    ]
    if len(main_segments) != 1:
        raise GenerationError("Splat configuration must contain one main segment")
    main_segment_template = main_segments[0]
    cursor = text_start

    # The region before the text is the compiler's read-only data. It is
    # declared in the Splat template, one entry per owning object, the way the
    # ff8 and silent-hill configurations declare theirs, so adding a function
    # that owns rodata is a template edit and nothing else.
    template_subsegments = main_segment_template.get("subsegments") or []
    leading: list[list[Any]] = []
    for entry in template_subsegments:
        if not isinstance(entry, list) or not entry:
            raise GenerationError("subsegment entries must be non-empty lists")
        if parse_integer(entry[0], "subsegment start") >= text_start:
            break
        leading.append(list(entry))
    if not leading:
        raise GenerationError("the template declares no leading data region")
    subsegments: list[list[Any]] = [list(entry) for entry in leading]

    text_sources: list[dict[str, Any]] = []

    for unit in units:
        start = file_offset(unit["address"])
        end = start + unit["size"]
        if start < cursor or end > text_end:
            raise GenerationError(
                f"matching unit {unit['address']:#010x} is outside text"
            )
        if start > cursor:
            name = f"generated/text_{cursor:06x}"
            subsegments.append([cursor, "asm", name])
            text_sources.append(
                {
                    "kind": "asm",
                    "source": f"tmp/splat/asm/{name}.s",
                    "object": f"asm_{cursor:06x}.o",
                }
            )
        subsegments.append([start, "c", unit["segment"]])
        text_sources.append(
            {
                "kind": "c",
                "source": unit["source"],
                "object": f"c_{unit['address']:08x}.o",
                "profile": unit["profile"],
                "members": [
                    f"0x{address:08X}" for address in unit["members"]
                ],
            }
        )
        cursor = end

    if cursor < text_end:
        name = f"generated/text_{cursor:06x}"
        subsegments.append([cursor, "asm", name])
        text_sources.append(
            {
                "kind": "asm",
                "source": f"tmp/splat/asm/{name}.s",
                "object": f"asm_{cursor:06x}.o",
            }
        )

    subsegments.extend(
        [
            [text_end, "pad"],
            [
                parse_integer(
                    regions["initialized_data"]["file_start"],
                    "initialized data start",
                ),
                "data",
                "initialized_data",
            ],
            [
                parse_integer(regions["bss_image"]["file_start"], "BSS start"),
                "bin",
                "bss_image",
            ],
            [
                parse_integer(
                    regions["reserved_zero"]["file_start"],
                    "reserved zero start",
                ),
                "bin",
                "reserved_zero",
            ],
            *[
                [
                    parse_integer(
                        region["file_start"],
                        f"{name} start",
                    ),
                    "bin",
                    f"overlays/{name[len(OVERLAY_REGION_PREFIX):]}",
                ]
                for name, region in regions.items()
                if region.get("kind") == OVERLAY_REGION_KIND
            ],
            [
                parse_integer(regions["tail_data"]["file_start"], "tail data start"),
                "bin",
                "tail_data",
            ],
        ]
    )

    main_segments[0]["subsegments"] = subsegments

    generated_directory = resolve_within(root, "tmp/generated")
    split_path = generated_directory / "slus_01411.split.yaml"
    text_path = generated_directory / "text_sources.json"
    write_yaml(split_path, split_config)
    write_json(text_path, {"schema": 1, "segments": text_sources})

    # The linker sections for the leading region are emitted from the same
    # template entries Splat uses, so the declaration lives in one place.
    owner_by_segment = {
        function["segment"].rsplit("/", 1)[-1]: function["address"]
        for function in functions
    }
    lines = ["/* Generated by tools/project/generate_build_config.py. */", ""]
    for index, entry in enumerate(leading):
        piece_start = parse_integer(entry[0], "subsegment start")
        piece_end = (
            parse_integer(leading[index + 1][0], "subsegment start")
            if index + 1 < len(leading)
            else text_start
        )
        kind = str(entry[1])
        if kind == "pad":
            # Left as a gap between sections, exactly as the reference PSX
            # configurations treat padding inside a read-only data region.
            continue
        vram = 0x80010000 + piece_start - 0x800
        if kind == ".rodata":
            segment_name = str(entry[2])
            owner = owner_by_segment.get(segment_name)
            if owner is None:
                raise GenerationError(
                    f"{segment_name}: rodata subsegment has no matching C function"
                )
            name = f".rodata_{piece_start:06x}"
            member = f"KEEP(*c_{owner:08x}.o(.rodata))"
            align = " SUBALIGN(4)"
        else:
            name = f".{entry[2]}"
            member = f"KEEP(*{entry[2]}.o(.data))"
            align = ""
        lines.append(f"    {name} {vram:#010x} : AT({piece_start:#08x}){align}")
        lines.append("    {")
        lines.append(f"        {member}")
        lines.append("    }")
        lines.append(
            f'    ASSERT(SIZEOF({name}) == {piece_end - piece_start:#x},'
            f' "{name} size mismatch")'
        )
        lines.append("")
    (generated_directory / "initial_data.ld").write_text(
        "\n".join(lines), encoding="utf-8"
    )
    return split_path, text_path


def main() -> int:
    try:
        root = require_workspace_root()
        split_path, text_path = generate(root)
    except (
        GenerationError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        csv.Error,
        yaml.YAMLError,
        json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print(f"generated: {split_path.relative_to(root)}")
    print(f"generated: {text_path.relative_to(root)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
