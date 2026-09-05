#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path
from typing import Any

from hashing import sha256_file
from workspace import WorkspaceError, require_workspace_root, resolve_within


class OverlayError(RuntimeError):
    pass


REQUIRED_MODULE_FIELDS = (
    "name",
    "archive",
    "archive_sha256",
    "sector_offset",
    "sector_count",
    "load_address",
    "output",
    "sha256",
)


def require_string(module: dict[str, Any], field: str) -> str:
    value = module[field]
    if not isinstance(value, str) or not value:
        raise OverlayError(f"overlay field {field} must be a non-empty string")
    return value


def require_nonnegative_int(module: dict[str, Any], field: str) -> int:
    value = module[field]
    if not isinstance(value, int) or isinstance(value, bool) or value < 0:
        raise OverlayError(f"overlay field {field} must be a non-negative integer")
    return value


def load_manifest(root: Path) -> tuple[int, list[dict[str, Any]]]:
    path = resolve_within(
        root, "config/slus_01411/overlays.json", must_exist=True
    )
    with path.open("r", encoding="utf-8") as handle:
        manifest = json.load(handle)

    if manifest.get("schema") != 1:
        raise OverlayError(f"{path.relative_to(root)}: unsupported schema")
    sector_size = manifest.get("sector_size")
    if not isinstance(sector_size, int) or isinstance(sector_size, bool):
        raise OverlayError("overlay sector_size must be an integer")
    if sector_size <= 0:
        raise OverlayError("overlay sector_size must be positive")

    modules = manifest.get("modules")
    if not isinstance(modules, list) or not modules:
        raise OverlayError("overlay manifest must contain at least one module")

    names: set[str] = set()
    outputs: set[str] = set()
    for module in modules:
        if not isinstance(module, dict):
            raise OverlayError("overlay module entries must be objects")
        missing = [field for field in REQUIRED_MODULE_FIELDS if field not in module]
        if missing:
            raise OverlayError(
                f"overlay module is missing fields: {', '.join(missing)}"
            )
        name = require_string(module, "name")
        output = require_string(module, "output")
        if name in names:
            raise OverlayError(f"duplicate overlay module name: {name}")
        if output in outputs:
            raise OverlayError(f"duplicate overlay output path: {output}")
        names.add(name)
        outputs.add(output)
    return sector_size, modules


def read_module(
    root: Path, sector_size: int, module: dict[str, Any]
) -> tuple[Path, bytes]:
    name = require_string(module, "name")
    archive = resolve_within(
        root, require_string(module, "archive"), must_exist=True
    )
    expected_archive_hash = require_string(module, "archive_sha256")
    actual_archive_hash = sha256_file(archive)
    if actual_archive_hash != expected_archive_hash:
        raise OverlayError(
            f"{name}: archive SHA-256 is {actual_archive_hash}, "
            f"expected {expected_archive_hash}"
        )

    sector_offset = require_nonnegative_int(module, "sector_offset")
    sector_count = require_nonnegative_int(module, "sector_count")
    if sector_count == 0:
        raise OverlayError(f"{name}: sector_count must be positive")
    byte_offset = sector_offset * sector_size
    byte_count = sector_count * sector_size
    if byte_offset + byte_count > archive.stat().st_size:
        raise OverlayError(f"{name}: requested sectors exceed the archive size")

    with archive.open("rb") as handle:
        handle.seek(byte_offset)
        payload = handle.read(byte_count)
    if len(payload) != byte_count:
        raise OverlayError(
            f"{name}: read {len(payload)} bytes, expected {byte_count}"
        )

    expected_hash = require_string(module, "sha256")
    actual_hash = hashlib.sha256(payload).hexdigest()
    if actual_hash != expected_hash:
        raise OverlayError(
            f"{name}: payload SHA-256 is {actual_hash}, expected {expected_hash}"
        )
    output = resolve_within(root, require_string(module, "output"))
    return output, payload


def write_payload(path: Path, payload: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(f"{path.name}.tmp")
    try:
        temporary.write_bytes(payload)
        temporary.replace(path)
    except OSError:
        temporary.unlink(missing_ok=True)
        raise


def extract(root: Path, sector_size: int, modules: list[dict[str, Any]]) -> None:
    for module in modules:
        output, payload = read_module(root, sector_size, module)
        write_payload(output, payload)
        print(
            f"overlay: {module['name']} -> {output.relative_to(root)} "
            f"({len(payload):#x} bytes at {module['load_address']})"
        )


def verify(root: Path, sector_size: int, modules: list[dict[str, Any]]) -> None:
    for module in modules:
        output, payload = read_module(root, sector_size, module)
        if not output.is_file():
            raise OverlayError(
                f"{module['name']}: missing output {output.relative_to(root)}; "
                "run make overlays"
            )
        actual = output.read_bytes()
        if actual != payload:
            raise OverlayError(
                f"{module['name']}: extracted output does not match the archive"
            )
        print(f"overlay: {module['name']} OK")
    verify_manifest_format(root)


def verify_manifest_format(root: Path) -> None:
    """Check every matching_c manifest is canonically formatted.

    Nothing generates these files, so their layout is a convention that drifts
    whenever someone rewrites one with a different json.dumps call. Pinning it
    keeps entry-adding diffs to the entry that was added.
    """
    config = resolve_within(root, "config/slus_01411", must_exist=True)
    paths = [config / "matching_c.json"]
    paths.extend(sorted((config / "overlays").glob("*_matching_c.json")))
    for path in paths:
        name = path.relative_to(root)
        if not path.is_file():
            raise OverlayError(f"{name}: missing matching_c manifest")
        text = path.read_text(encoding="utf-8")
        canonical = json.dumps(json.loads(text), indent=2, sort_keys=True) + "\n"
        if text != canonical:
            raise OverlayError(
                f"{name}: not canonical; rewrite it with "
                "json.dumps(data, indent=2, sort_keys=True) plus a trailing newline"
            )
    print(f"matching_c manifests: OK ({len(paths)} canonical)")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Extract and verify runtime overlay module images."
    )
    parser.add_argument("command", choices=("extract", "verify"))
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        sector_size, modules = load_manifest(root)
        if args.command == "extract":
            extract(root, sector_size, modules)
        else:
            verify(root, sector_size, modules)
    except (
        OverlayError,
        WorkspaceError,
        OSError,
        KeyError,
        TypeError,
        ValueError,
        json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
