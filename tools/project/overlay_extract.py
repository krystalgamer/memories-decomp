#!/usr/bin/env python3

from __future__ import annotations

import argparse
from contextlib import ExitStack
import csv
import hashlib
import json
import os
import sys
from pathlib import Path
from typing import Any, BinaryIO

from hashing import sha256_stream
from overlay_sources import OverlaySourceError, c_segments
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

OVERLAY_MANIFESTS = {
    "usa": "config/slus_01411/overlays.json",
    "japan": "config/slpm_86398/overlays.json",
    "europe": "config/sles_03947/overlays.json",
    "spain": "config/sles_03951/overlays.json",
    "france": "config/sles_03948/overlays.json",
    "italy": "config/sles_03950/overlays.json",
    "germany": "config/sles_03949/overlays.json",
}


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


def load_manifest(root: Path, region: str = "usa") -> tuple[int, list[dict[str, Any]]]:
    path = resolve_within(
        root, OVERLAY_MANIFESTS[region], must_exist=True
    )
    with path.open("r", encoding="utf-8") as handle:
        manifest = json.load(handle)

    if not isinstance(manifest, dict) or manifest.get("schema") != 1:
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
    return read_modules(root, sector_size, [module])[0]


def _archive_state(stat: os.stat_result) -> tuple[int, int, int, int, int]:
    return stat.st_dev, stat.st_ino, stat.st_size, stat.st_mtime_ns, stat.st_ctime_ns


def read_modules(
    root: Path, sector_size: int, modules: list[dict[str, Any]]
) -> list[tuple[Path, bytes]]:
    """Verify each archive once per batch, then read through the same open file."""
    with ExitStack() as stack:
        archives: dict[Path, tuple[BinaryIO, str, tuple[int, int, int, int, int]]] = {}
        sources: list[tuple[dict[str, Any], Path]] = []
        for module in modules:
            name = require_string(module, "name")
            archive = resolve_within(root, require_string(module, "archive"), must_exist=True)
            expected_hash = require_string(module, "archive_sha256")
            if archive not in archives:
                handle = stack.enter_context(archive.open("rb"))
                initial = _archive_state(os.fstat(handle.fileno()))
                archives[archive] = handle, sha256_stream(handle), initial
            _handle, actual_hash, _initial = archives[archive]
            if actual_hash != expected_hash:
                raise OverlayError(
                    f"{name}: archive SHA-256 is {actual_hash}, expected {expected_hash}"
                )
            sources.append((module, archive))

        payloads = [
            _read_module_payload(root, sector_size, module, archives[archive][0])
            for module, archive in sources
        ]
        for archive, (handle, _actual_hash, initial) in archives.items():
            if (
                _archive_state(os.fstat(handle.fileno())) != initial
                or _archive_state(archive.stat()) != initial
            ):
                raise OverlayError(f"archive changed while reading overlays: {archive.relative_to(root)}")
        return payloads


def _read_module_payload(
    root: Path, sector_size: int, module: dict[str, Any], handle: BinaryIO
) -> tuple[Path, bytes]:
    name = require_string(module, "name")
    archive_size = os.fstat(handle.fileno()).st_size

    sector_offset = require_nonnegative_int(module, "sector_offset")
    sector_count = require_nonnegative_int(module, "sector_count")
    if sector_count == 0:
        raise OverlayError(f"{name}: sector_count must be positive")
    byte_offset = sector_offset * sector_size
    byte_count = sector_count * sector_size
    if byte_offset + byte_count > archive_size:
        raise OverlayError(f"{name}: requested sectors exceed the archive size")

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
    duplicate_offsets = module.get("duplicate_sector_offsets", [])
    if not isinstance(duplicate_offsets, list):
        raise OverlayError(f"{name}: duplicate_sector_offsets must be a list")
    seen_offsets = {sector_offset}
    for offset in duplicate_offsets:
        if not isinstance(offset, int) or isinstance(offset, bool) or offset < 0:
            raise OverlayError(
                f"{name}: duplicate sector offsets must be non-negative integers"
            )
        if offset in seen_offsets:
            raise OverlayError(f"{name}: repeated sector offset {offset}")
        seen_offsets.add(offset)
        if offset * sector_size + byte_count > archive_size:
            raise OverlayError(
                f"{name}: duplicate sectors at {offset} exceed the archive size"
            )
        handle.seek(offset * sector_size)
        if handle.read(byte_count) != payload:
            raise OverlayError(
                f"{name}: duplicate module at sector {offset} differs from the primary"
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
    for module, (output, payload) in zip(modules, read_modules(root, sector_size, modules)):
        write_payload(output, payload)
        print(
            f"overlay: {module['name']} -> {output.relative_to(root)} "
            f"({len(payload):#x} bytes at {module['load_address']})"
        )


def verify(root: Path, sector_size: int, modules: list[dict[str, Any]]) -> None:
    for module, (output, payload) in zip(modules, read_modules(root, sector_size, modules)):
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
    verify_metadata(root)


def verify_manifest_format(root: Path) -> None:
    """Check every matching_c manifest is canonically formatted.

    Nothing generates these files, so their layout is a convention that drifts
    whenever someone rewrites one with a different json.dumps call. Pinning it
    keeps entry-adding diffs to the entry that was added.
    """
    config = resolve_within(root, "config/slus_01411", must_exist=True)
    paths = [config / "matching_c.json"]
    paths.append(resolve_within(root, "config/sles_03951/matching_c.json", must_exist=True))
    paths.extend(sorted((config / "overlays").glob("*_matching_c.json")))
    japanese = resolve_within(root, "config/slpm_86398/overlays", must_exist=True)
    paths.extend(sorted(japanese.glob("*_matching_c.json")))
    european = resolve_within(root, "config/sles_03947/overlays", must_exist=True)
    paths.extend(sorted(european.glob("*_matching_c.json")))
    spanish = resolve_within(root, "config/sles_03951/overlays", must_exist=True)
    paths.extend(sorted(spanish.glob("*_matching_c.json")))
    french = resolve_within(root, "config/sles_03948/overlays", must_exist=True)
    paths.extend(sorted(french.glob("*_matching_c.json")))
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


METADATA_MARKERS = (".git", "config/slus_01411/target.yaml")


def require_metadata_root() -> Path:
    """Locate the repository root for checks that read only tracked metadata.

    The usual workspace guard also requires game/SLUS_014.11, because almost
    every tool here needs the retail executable. These checks do not read it,
    so requiring it would stop them running anywhere it is absent -- which is
    the one place they are most useful.
    """
    root = Path.cwd().resolve(strict=True)
    missing = [
        marker
        for marker in METADATA_MARKERS
        if not resolve_within(root, marker).exists()
    ]
    if missing:
        raise OverlayError(
            "run this command from the repository root; missing "
            + ", ".join(missing)
        )
    return root


def verify_metadata(root: Path) -> None:
    """Run every tracked-metadata check.

    These read files under config/ and notes/ and parse them. They touch no
    overlay image, need no retail data and need no secrets, which is why they
    are also reachable on their own -- a job that runs only these can cover
    paths the overlay build deliberately ignores.
    """
    verify_manifest_format(root)
    verify_csv_format(root)
    verify_sources_wired(root)


def verify_sources_wired(root: Path) -> None:
    """Check every overlay C source is mapped and has a compiler profile.

    A source that no subsegment names is never compiled, and nothing else
    notices. The module still hashes, because the original assembly is still
    in the build, so `make match-overlays` and `verify-overlays` both stay
    green while the file sits on disk doing nothing -- and a per-function
    `overlay_diff` reports MATCH for the same reason, since it compiles the
    candidate itself rather than reading the module.

    So a green build is not evidence that a newly added source is being used.
    The invariant is bidirectional wiring between C sources, manifests and
    `c` or owned-data subsegments. Two modules share `src/overlays/overworld`,
    so the subsegments are gathered across every module yaml before comparing.
    """
    sources_root = resolve_within(root, "src/overlays", must_exist=True)

    wired: dict[str, Path] = {}
    overlay_directories = {
        Path(manifest).with_suffix("") for manifest in OVERLAY_MANIFESTS.values()
    }
    for relative in sorted(overlay_directories):
        overlays = resolve_within(root, relative.as_posix(), must_exist=True)
        for path in sorted(overlays.glob("*.yaml")):
            try:
                segments = c_segments(root, path)
            except OverlaySourceError as error:
                raise OverlayError(str(error)) from error
            for segment in segments:
                wired.setdefault(segment["source"], path)

    present = {
        p.relative_to(root).as_posix(): p
        for p in sorted(sources_root.rglob("*.c"))
    }

    orphans = sorted(set(present) - set(wired))
    if orphans:
        listed = ", ".join(present[name].relative_to(root).as_posix() for name in orphans)
        raise OverlayError(
            f"overlay sources not wired into any split: {listed}; "
            "add a C/data subsegment and its manifest entry, or the file is never compiled"
        )

    missing = sorted(set(wired) - set(present))
    if missing:
        listed = ", ".join(f"{name} ({wired[name].name})" for name in missing)
        raise OverlayError(f"C/data subsegments with no source file: {listed}")

    print(f"overlay sources wired: OK ({len(present)} sources)")


def tracked_csv_paths(root: Path) -> list[Path]:
    """Every CSV the repository tracks, found by walking rather than listing.

    A new CSV is guarded the day it is added; nothing has to remember to
    register it.
    """
    paths: list[Path] = []
    for directory in ("config", "notes"):
        base = resolve_within(root, directory, must_exist=True)
        paths.extend(p for p in base.rglob("*.csv") if p.is_file())
    return sorted(set(paths))


def verify_csv_format(root: Path) -> None:
    """Check every tracked CSV keeps the column count of its header.

    The last column of these tables is prose. An unquoted comma in it splits
    the row, and the text after the comma is silently discarded because there
    is no column for it to land in -- the file still parses, and every check
    still passes. Quote the field instead.

    Note that a byte round-trip through csv.writer does NOT catch this: a row
    that has grown an extra field round-trips to itself exactly. The column
    count is the invariant that matters.
    """
    paths = tracked_csv_paths(root)
    if not paths:
        raise OverlayError("no tracked CSV files found")
    for path in paths:
        name = path.relative_to(root)
        with path.open("r", encoding="utf-8", newline="") as handle:
            rows = list(csv.reader(handle))
        if not rows:
            raise OverlayError(f"{name}: empty CSV")
        width = len(rows[0])
        for number, row in enumerate(rows[1:], start=2):
            if not row:
                continue
            if len(row) != width:
                key = row[0] if row else "?"
                raise OverlayError(
                    f"{name}: line {number} ({key}) has {len(row)} columns, "
                    f"expected {width}; quote the field if it contains a comma"
                )
    print(f"tracked CSV tables: OK ({len(paths)} well formed)")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Extract and verify runtime overlay module images."
    )
    parser.add_argument(
        "command", choices=("extract", "verify", "verify-metadata")
    )
    parser.add_argument("--region", choices=tuple(OVERLAY_MANIFESTS), default="usa")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        if args.command == "verify-metadata":
            verify_metadata(require_metadata_root())
            return 0
        root = require_workspace_root()
        sector_size, modules = load_manifest(root, args.region)
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
