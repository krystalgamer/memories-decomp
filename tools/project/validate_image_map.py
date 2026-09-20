#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path
from typing import Any

from psx_exe import PSX_EXE_HEADER_SIZE, load_verified_executable
from workspace import WorkspaceError, require_workspace_root, resolve_within


class ImageMapError(RuntimeError):
    pass


def parse_integer(value: Any, description: str) -> int:
    if isinstance(value, int) and not isinstance(value, bool):
        return value
    if isinstance(value, str):
        try:
            return int(value, 0)
        except ValueError as error:
            raise ImageMapError(
                f"{description} is not a valid integer: {value}"
            ) from error
    raise ImageMapError(f"{description} must be an integer or integer string")


def load_image_map(root: Path, relative_path: str) -> dict[str, Any]:
    path = resolve_within(root, relative_path, must_exist=True)
    with path.open("r", encoding="utf-8") as handle:
        image_map = json.load(handle)
    if image_map.get("schema") != 1:
        raise ImageMapError(f"{path}: unsupported image map schema")
    return image_map


def validate_regions(
    image: bytes,
    image_map: dict[str, Any],
    load_address: int,
) -> dict[str, dict[str, Any]]:
    regions = image_map.get("regions")
    if not isinstance(regions, list) or not regions:
        raise ImageMapError("image map must contain a non-empty regions list")

    cursor = 0
    by_name: dict[str, dict[str, Any]] = {}
    for index, region in enumerate(regions):
        if not isinstance(region, dict):
            raise ImageMapError(f"region {index} must be an object")
        name = region.get("name")
        if not isinstance(name, str) or not name:
            raise ImageMapError(f"region {index} has no valid name")
        if name in by_name:
            raise ImageMapError(f"duplicate region name: {name}")

        start = parse_integer(region.get("file_start"), f"{name}.file_start")
        end = parse_integer(region.get("file_end"), f"{name}.file_end")
        if start != cursor:
            raise ImageMapError(
                f"{name} starts at {start:#x}; expected contiguous offset {cursor:#x}"
            )
        if end <= start or end > len(image):
            raise ImageMapError(f"{name} has invalid file range {start:#x}-{end:#x}")

        chunk = image[start:end]
        expected_sha256 = region.get("sha256")
        actual_sha256 = sha256_file_from_bytes(chunk)
        if actual_sha256 != expected_sha256:
            raise ImageMapError(
                f"{name} SHA-256 is {actual_sha256}, expected {expected_sha256}"
            )

        if start < PSX_EXE_HEADER_SIZE:
            if name != "header" or start != 0 or end != PSX_EXE_HEADER_SIZE:
                raise ImageMapError("only the PS-X EXE header may precede the payload")
        else:
            vram_start = parse_integer(
                region.get("vram_start"), f"{name}.vram_start"
            )
            vram_end = parse_integer(region.get("vram_end"), f"{name}.vram_end")
            expected_vram_start = load_address + start - PSX_EXE_HEADER_SIZE
            expected_vram_end = load_address + end - PSX_EXE_HEADER_SIZE
            if (vram_start, vram_end) != (
                expected_vram_start,
                expected_vram_end,
            ):
                raise ImageMapError(
                    f"{name} VRAM range does not match its file offsets"
                )

        if "fill" in region:
            fill = parse_integer(region["fill"], f"{name}.fill")
            if not 0 <= fill <= 0xFF:
                raise ImageMapError(f"{name}.fill must be one byte")
            if any(byte != fill for byte in chunk):
                raise ImageMapError(
                    f"{name} contains bytes other than its declared fill {fill:#04x}"
                )

        region["_file_start"] = start
        region["_file_end"] = end
        by_name[name] = region
        cursor = end

    if cursor != len(image):
        raise ImageMapError(
            f"image map ends at {cursor:#x}; executable ends at {len(image):#x}"
        )
    return by_name


def sha256_file_from_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def decode_lui_addiu(first: int, second: int) -> int:
    if first >> 26 != 0x0F or second >> 26 != 0x09:
        raise ImageMapError("startup clear range is not encoded as lui/addiu")
    register = (first >> 16) & 0x1F
    if ((second >> 21) & 0x1F) != register or ((second >> 16) & 0x1F) != register:
        raise ImageMapError("startup clear range uses inconsistent registers")
    upper = first & 0xFFFF
    lower = second & 0xFFFF
    signed_lower = lower if lower < 0x8000 else lower - 0x10000
    return ((upper << 16) + signed_lower) & 0xFFFFFFFF


def validate_structural_evidence(
    image: bytes,
    regions: dict[str, dict[str, Any]],
    entry_file_offset: int,
) -> None:
    initial_data = regions["initial_data"]
    text = regions["text"]
    if initial_data["_file_end"] != entry_file_offset:
        raise ImageMapError("initial_data does not end at the executable entry point")
    if text["_file_start"] != entry_file_offset:
        raise ImageMapError("text does not start at the executable entry point")

    text_bytes = image[text["_file_start"] : text["_file_end"]]
    final_instruction, delay_slot = struct.unpack_from("<2I", text_bytes, len(text_bytes) - 8)
    if (final_instruction, delay_slot) != (0x03E00008, 0x00000000):
        raise ImageMapError("text does not end with the expected jr ra delay slot")

    startup = struct.unpack_from("<4I", image, entry_file_offset)
    clear_start = decode_lui_addiu(startup[0], startup[1])
    clear_end = decode_lui_addiu(startup[2], startup[3])
    bss = regions["bss_image"]
    if clear_start != parse_integer(bss["vram_start"], "bss_image.vram_start"):
        raise ImageMapError("startup clear start does not match bss_image")
    if clear_end != parse_integer(bss["vram_end"], "bss_image.vram_end"):
        raise ImageMapError("startup clear end does not match bss_image")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate one configured executable image map."
    )
    parser.add_argument(
        "--target",
        default="config/slus_01411/target.yaml",
        help="target metadata path relative to the repository root",
    )
    parser.add_argument(
        "--image-map",
        default="config/slus_01411/image_map.json",
        help="image map path relative to the repository root",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        source, executable, _header_bytes, header = load_verified_executable(
            root, args.target
        )
        image = source.read_bytes()
        if sha256_file_from_bytes(image) != str(executable["sha256"]):
            raise ImageMapError("target executable changed during map validation")
        image_map = load_image_map(root, args.image_map)
        if image_map.get("target_sha256") != executable["sha256"]:
            raise ImageMapError("image map targets a different executable hash")

        regions = validate_regions(image, image_map, header.load_address)
        validate_structural_evidence(
            image,
            regions,
            header.vram_to_file_offset(header.initial_pc),
        )

        for region in image_map["regions"]:
            start = parse_integer(region["file_start"], "file_start")
            end = parse_integer(region["file_end"], "file_end")
            print(f"OK  {region['name']:<18} {start:#08x}-{end:#08x}")
    except (
        ImageMapError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        struct.error,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
