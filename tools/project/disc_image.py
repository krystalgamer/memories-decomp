#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, BinaryIO

from hashing import sha256_file
from workspace import WorkspaceError, require_workspace_root, resolve_within


class DiscError(RuntimeError):
    pass


RAW_SECTOR_SIZE = 2352
MODE2_USER_OFFSET = 24
LOGICAL_BLOCK_SIZE = 2048
CD_LEAD_IN_SECTORS = 150
CUE_FILE_PATTERN = re.compile(r'^FILE\s+"(?P<name>[^"]+)"\s+BINARY$', re.I)
CUE_TRACK_PATTERN = re.compile(
    r"^TRACK\s+(?P<number>\d+)\s+(?P<mode>\S+)$",
    re.I,
)
CUE_INDEX_PATTERN = re.compile(
    r"^INDEX\s+01\s+(?P<minute>\d+):(?P<second>\d+):(?P<frame>\d+)$",
    re.I,
)


@dataclass(frozen=True)
class CueTrack:
    cue_path: Path
    bin_path: Path
    referenced_bin_name: str
    track_number: int
    mode: str
    index_lba: int


@dataclass(frozen=True)
class DirectoryRecord:
    name: str
    extent_lba: int
    size: int
    flags: int

    @property
    def is_directory(self) -> bool:
        return bool(self.flags & 0x02)


class Mode2Image:
    def __init__(self, path: Path):
        self.path = path
        size = path.stat().st_size
        if size % RAW_SECTOR_SIZE:
            raise DiscError(
                f"{path}: size {size} is not divisible by {RAW_SECTOR_SIZE}"
            )
        self.sector_count = size // RAW_SECTOR_SIZE

    def read_user_sector(self, handle: BinaryIO, lba: int) -> bytes:
        if not 0 <= lba < self.sector_count:
            raise DiscError(f"{self.path}: LBA {lba} is outside the image")
        handle.seek(lba * RAW_SECTOR_SIZE)
        sector = handle.read(RAW_SECTOR_SIZE)
        if len(sector) != RAW_SECTOR_SIZE:
            raise DiscError(f"{self.path}: short read at LBA {lba}")
        if sector[:12] != b"\x00" + b"\xFF" * 10 + b"\x00":
            raise DiscError(f"{self.path}: invalid sync pattern at LBA {lba}")
        if sector[15] != 2:
            raise DiscError(f"{self.path}: LBA {lba} is not MODE2")
        if sector[16:20] != sector[20:24]:
            raise DiscError(f"{self.path}: mismatched XA subheaders at LBA {lba}")
        return sector[
            MODE2_USER_OFFSET : MODE2_USER_OFFSET + LOGICAL_BLOCK_SIZE
        ]

    def read_extent(self, lba: int, size: int) -> bytes:
        if size < 0:
            raise DiscError("extent size must be non-negative")
        remaining = size
        chunks: list[bytes] = []
        with self.path.open("rb") as handle:
            while remaining:
                block = self.read_user_sector(handle, lba)
                chunks.append(block[:remaining])
                remaining -= min(remaining, LOGICAL_BLOCK_SIZE)
                lba += 1
        return b"".join(chunks)

    def hash_extent(self, lba: int, size: int) -> str:
        digest = hashlib.sha256()
        remaining = size
        with self.path.open("rb") as handle:
            while remaining:
                block = self.read_user_sector(handle, lba)
                chunk = block[:remaining]
                digest.update(chunk)
                remaining -= len(chunk)
                lba += 1
        return digest.hexdigest()

    def write_extent(self, lba: int, size: int, destination: Path) -> str:
        digest = hashlib.sha256()
        remaining = size
        destination.parent.mkdir(parents=True, exist_ok=True)
        temporary = destination.with_name(f"{destination.name}.tmp")
        try:
            with self.path.open("rb") as handle:
                with temporary.open("wb") as output:
                    while remaining:
                        block = self.read_user_sector(handle, lba)
                        chunk = block[:remaining]
                        output.write(chunk)
                        digest.update(chunk)
                        remaining -= len(chunk)
                        lba += 1
            temporary.replace(destination)
        except (DiscError, OSError):
            temporary.unlink(missing_ok=True)
            raise
        return digest.hexdigest()


def resolve_cue_bin(cue_path: Path, referenced_name: str) -> Path:
    exact = cue_path.parent / referenced_name
    if exact.is_file():
        return exact.resolve(strict=True)
    matches = [
        path
        for path in cue_path.parent.iterdir()
        if path.is_file() and path.name.casefold() == referenced_name.casefold()
    ]
    if len(matches) != 1:
        raise DiscError(
            f"{cue_path}: cannot resolve referenced BIN {referenced_name}"
        )
    return matches[0].resolve(strict=True)


def parse_cue(cue_path: Path) -> CueTrack:
    referenced_name: str | None = None
    track_number: int | None = None
    mode: str | None = None
    index_lba: int | None = None

    with cue_path.open("r", encoding="ascii", newline=None) as handle:
        for raw_line in handle:
            line = raw_line.strip()
            if not line:
                continue
            file_match = CUE_FILE_PATTERN.fullmatch(line)
            if file_match:
                referenced_name = file_match.group("name")
                continue
            track_match = CUE_TRACK_PATTERN.fullmatch(line)
            if track_match:
                track_number = int(track_match.group("number"), 10)
                mode = track_match.group("mode").upper()
                continue
            index_match = CUE_INDEX_PATTERN.fullmatch(line)
            if index_match:
                minute = int(index_match.group("minute"), 10)
                second = int(index_match.group("second"), 10)
                frame = int(index_match.group("frame"), 10)
                if second >= 60 or frame >= 75:
                    raise DiscError(f"{cue_path}: invalid INDEX timestamp")
                index_lba = (minute * 60 + second) * 75 + frame
                continue
            raise DiscError(f"{cue_path}: unsupported CUE line: {line}")

    if referenced_name is None or track_number is None or mode is None:
        raise DiscError(f"{cue_path}: incomplete track definition")
    if index_lba is None:
        raise DiscError(f"{cue_path}: missing INDEX 01")
    if track_number != 1 or mode != "MODE2/2352":
        raise DiscError(
            f"{cue_path}: expected one TRACK 01 MODE2/2352 definition"
        )
    return CueTrack(
        cue_path=cue_path,
        bin_path=resolve_cue_bin(cue_path, referenced_name),
        referenced_bin_name=referenced_name,
        track_number=track_number,
        mode=mode,
        index_lba=index_lba,
    )


def both_endian_u16(data: bytes, offset: int, description: str) -> int:
    little = struct.unpack_from("<H", data, offset)[0]
    big = struct.unpack_from(">H", data, offset + 2)[0]
    if little != big:
        raise DiscError(f"{description}: little/big-endian values disagree")
    return little


def both_endian_u32(data: bytes, offset: int, description: str) -> int:
    little = struct.unpack_from("<I", data, offset)[0]
    big = struct.unpack_from(">I", data, offset + 4)[0]
    if little != big:
        raise DiscError(f"{description}: little/big-endian values disagree")
    return little


def parse_directory_record(data: bytes, offset: int) -> DirectoryRecord:
    length = data[offset]
    if length < 34 or offset + length > len(data):
        raise DiscError(f"invalid ISO9660 directory record at offset {offset:#x}")
    extent_lba = both_endian_u32(data, offset + 2, "directory extent")
    size = both_endian_u32(data, offset + 10, "directory size")
    flags = data[offset + 25]
    name_length = data[offset + 32]
    name_bytes = data[offset + 33 : offset + 33 + name_length]
    if len(name_bytes) != name_length:
        raise DiscError("truncated ISO9660 file identifier")
    if name_bytes == b"\x00":
        name = "."
    elif name_bytes == b"\x01":
        name = ".."
    else:
        try:
            name = name_bytes.decode("ascii")
        except UnicodeDecodeError as error:
            raise DiscError("non-ASCII ISO9660 file identifier") from error
        if ";" in name:
            name = name.split(";", 1)[0]
    return DirectoryRecord(
        name=name,
        extent_lba=extent_lba,
        size=size,
        flags=flags,
    )


def parse_directory(image: Mode2Image, record: DirectoryRecord) -> list[DirectoryRecord]:
    data = image.read_extent(record.extent_lba, record.size)
    entries: list[DirectoryRecord] = []
    offset = 0
    while offset < len(data):
        length = data[offset]
        if length == 0:
            offset = ((offset // LOGICAL_BLOCK_SIZE) + 1) * LOGICAL_BLOCK_SIZE
            continue
        entry = parse_directory_record(data, offset)
        entries.append(entry)
        offset += length
    return entries


def walk_iso(
    image: Mode2Image,
    root_record: DirectoryRecord,
) -> list[tuple[str, DirectoryRecord]]:
    results: list[tuple[str, DirectoryRecord]] = []
    visited_directories: set[tuple[int, int]] = set()

    def visit(prefix: str, directory: DirectoryRecord) -> None:
        key = (directory.extent_lba, directory.size)
        if key in visited_directories:
            return
        visited_directories.add(key)
        for entry in parse_directory(image, directory):
            if entry.name in (".", ".."):
                continue
            path = f"{prefix}/{entry.name}" if prefix else entry.name
            if entry.is_directory:
                visit(path, entry)
            else:
                if entry.flags & 0x80:
                    raise DiscError(f"{path}: multi-extent files are unsupported")
                results.append((path, entry))

    visit("", root_record)
    return sorted(results, key=lambda item: (item[1].extent_lba, item[0]))


def lba_to_msf(lba: int) -> str:
    absolute = lba + CD_LEAD_IN_SECTORS
    minute, remainder = divmod(absolute, 75 * 60)
    second, frame = divmod(remainder, 75)
    return f"{minute:02d}:{second:02d}:{frame:02d}"


def local_file_map(root: Path) -> dict[str, Path]:
    paths = (
        "game/SLUS_014.11",
        "game/DATA/MASTER.XA",
        "game/DATA/MODEL.MRG",
        "game/DATA/MOVIE.STR",
        "game/DATA/SD_BGM.DAT",
        "game/DATA/SD_SE.DAT",
        "game/DATA/SU.MRG",
        "game/DATA/WA_MRG.MRG",
    )
    return {
        path.removeprefix("game/").upper(): resolve_within(
            root, path, must_exist=True
        )
        for path in paths
    }


def scan(root: Path) -> dict[str, Any]:
    cue_path = resolve_within(root, "game/rpg-yfm.cue", must_exist=True)
    track = parse_cue(cue_path)
    try:
        track.bin_path.relative_to(root)
    except ValueError as error:
        raise DiscError("CUE resolves to a BIN outside the workspace") from error
    image = Mode2Image(track.bin_path)

    pvd = image.read_extent(16, LOGICAL_BLOCK_SIZE)
    if pvd[0] != 1 or pvd[1:6] != b"CD001" or pvd[6] != 1:
        raise DiscError("LBA 16 is not an ISO9660 primary volume descriptor")
    volume_blocks = both_endian_u32(pvd, 80, "volume block count")
    block_size = both_endian_u16(pvd, 128, "logical block size")
    if block_size != LOGICAL_BLOCK_SIZE:
        raise DiscError(f"unsupported logical block size {block_size}")
    root_record = parse_directory_record(pvd, 156)
    if not root_record.is_directory:
        raise DiscError("primary volume root record is not a directory")

    local_files = local_file_map(root)
    found_local: set[str] = set()
    files: list[dict[str, Any]] = []
    for iso_path, record in walk_iso(image, root_record):
        iso_key = iso_path.upper()
        disc_sha256 = image.hash_extent(record.extent_lba, record.size)
        local_path = local_files.get(iso_key)
        local_relative: str | None = None
        if local_path is not None:
            found_local.add(iso_key)
            if local_path.stat().st_size != record.size:
                raise DiscError(
                    f"{iso_path}: disc size {record.size} differs from "
                    f"{local_path.relative_to(root)} size {local_path.stat().st_size}"
                )
            local_sha256 = sha256_file(local_path)
            if local_sha256 != disc_sha256:
                raise DiscError(
                    f"{iso_path}: extracted file SHA-256 {local_sha256} "
                    f"differs from disc SHA-256 {disc_sha256}"
                )
            local_relative = str(local_path.relative_to(root))

        files.append(
            {
                "path": iso_path,
                "lba": record.extent_lba,
                "msf": lba_to_msf(record.extent_lba),
                "size": record.size,
                "logical_blocks": (
                    record.size + LOGICAL_BLOCK_SIZE - 1
                )
                // LOGICAL_BLOCK_SIZE,
                "flags": record.flags,
                "sha256": disc_sha256,
                "local_path": local_relative,
            }
        )

    missing_local = sorted(set(local_files) - found_local)
    if missing_local:
        raise DiscError(
            "extracted files missing from disc: " + ", ".join(missing_local)
        )

    volume_identifier = pvd[40:72].decode("ascii").rstrip(" ")
    return {
        "schema": 1,
        "cue_path": str(cue_path.relative_to(root)),
        "cue_sha256": sha256_file(cue_path),
        "referenced_bin_name": track.referenced_bin_name,
        "resolved_bin_path": str(track.bin_path.relative_to(root)),
        "bin_sha256": sha256_file(track.bin_path),
        "track": {
            "number": track.track_number,
            "mode": track.mode,
            "index_lba": track.index_lba,
        },
        "raw_sector_size": RAW_SECTOR_SIZE,
        "raw_sector_count": image.sector_count,
        "logical_block_size": LOGICAL_BLOCK_SIZE,
        "volume_identifier": volume_identifier,
        "volume_block_count": volume_blocks,
        "files": files,
    }


def load_manifest(root: Path) -> dict[str, Any]:
    manifest_path = resolve_within(root, "config/slus_01411/disc_layout.json")
    if not manifest_path.is_file():
        raise DiscError(
            f"missing disc layout manifest: {manifest_path.relative_to(root)}"
        )
    with manifest_path.open("r", encoding="utf-8") as handle:
        layout = json.load(handle)
    if layout.get("schema") != 1:
        raise DiscError("unsupported disc layout schema")
    if layout.get("raw_sector_size") != RAW_SECTOR_SIZE:
        raise DiscError("disc layout raw sector size is not 2352")
    if layout.get("logical_block_size") != LOGICAL_BLOCK_SIZE:
        raise DiscError("disc layout logical block size is not 2048")
    return layout


def tracked_extents(layout: dict[str, Any]) -> list[dict[str, Any]]:
    return [
        file for file in layout["files"] if file.get("local_path") is not None
    ]


def select_extents(
    layout: dict[str, Any], names: list[str] | None
) -> list[dict[str, Any]]:
    extents = tracked_extents(layout)
    if not names:
        return extents
    by_key: dict[str, dict[str, Any]] = {}
    for file in extents:
        by_key[str(file["path"]).upper()] = file
        by_key[str(file["local_path"]).upper()] = file
        by_key[Path(str(file["local_path"])).name.upper()] = file
    selected: list[dict[str, Any]] = []
    for name in names:
        file = by_key.get(name.upper())
        if file is None:
            known = ", ".join(
                sorted(Path(str(f["local_path"])).name for f in extents)
            )
            raise DiscError(f"{name} is not a tracked disc file; known: {known}")
        if file not in selected:
            selected.append(file)
    return selected


def extract_files(root: Path, names: list[str] | None, force: bool) -> None:
    layout = load_manifest(root)
    bin_path = resolve_within(root, str(layout["resolved_bin_path"]))
    if not bin_path.is_file():
        raise DiscError(
            f"missing disc image: {bin_path.relative_to(root)}; place the "
            "MODE2/2352 dump there to extract the tracked files from it"
        )
    actual_bin_sha256 = sha256_file(bin_path)
    if actual_bin_sha256 != layout["bin_sha256"]:
        raise DiscError(
            f"{bin_path.relative_to(root)} SHA-256 {actual_bin_sha256} "
            f"differs from the tracked disc image {layout['bin_sha256']}"
        )
    image = Mode2Image(bin_path)
    if image.sector_count != layout["raw_sector_count"]:
        raise DiscError("disc image sector count differs from the manifest")

    for file in select_extents(layout, names):
        destination = resolve_within(root, str(file["local_path"]))
        relative = destination.relative_to(root)
        expected = str(file["sha256"])
        if (
            not force
            and destination.is_file()
            and destination.stat().st_size == file["size"]
            and sha256_file(destination) == expected
        ):
            print(f"disc extract: {relative} already present")
            continue
        actual = image.write_extent(
            int(file["lba"]), int(file["size"]), destination
        )
        if actual != expected:
            destination.unlink(missing_ok=True)
            raise DiscError(
                f"{relative}: extracted SHA-256 {actual} differs from the "
                f"tracked {expected}"
            )
        print(f"disc extract: {relative} {file['size']} bytes")


def atomic_write_json(path: Path, value: dict[str, Any]) -> None:
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


def print_summary(layout: dict[str, Any]) -> None:
    print(
        f"disc: {layout['volume_identifier']} "
        f"{layout['raw_sector_count']} raw sectors"
    )
    for file in layout["files"]:
        print(
            f"{file['path']:<24} "
            f"LBA {file['lba']:>6} "
            f"MSF {file['msf']} "
            f"{file['size']:>9} bytes"
        )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Scan and verify the original MODE2/2352 disc image."
    )
    subparsers = parser.add_subparsers(dest="command", required=True)
    subparsers.add_parser("info", help="scan and print the ISO9660 file layout")
    subparsers.add_parser("write", help="write the tracked disc layout manifest")
    subparsers.add_parser("verify", help="verify the tracked disc layout manifest")
    extract_parser = subparsers.add_parser(
        "extract",
        help="extract the tracked disc files from the MODE2/2352 image",
    )
    extract_parser.add_argument(
        "name",
        nargs="*",
        help=(
            "disc path, tracked local path, or file name to extract; "
            "defaults to every tracked file"
        ),
    )
    extract_parser.add_argument(
        "--force",
        action="store_true",
        help="re-extract files that are already present and correct",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        if args.command == "extract":
            extract_files(root, args.name, args.force)
            return 0
        layout = scan(root)
        manifest_path = resolve_within(
            root, "config/slus_01411/disc_layout.json"
        )
        if args.command == "write":
            atomic_write_json(manifest_path, layout)
            print(f"disc layout: {manifest_path.relative_to(root)}")
        elif args.command == "verify":
            if not manifest_path.is_file():
                raise DiscError(
                    f"missing disc layout manifest: {manifest_path.relative_to(root)}"
                )
            with manifest_path.open("r", encoding="utf-8") as handle:
                expected = json.load(handle)
            if expected != layout:
                raise DiscError(
                    "disc layout changed; regenerate with make disc-layout"
                )
            print("disc layout: OK")
        elif args.command == "info":
            print_summary(layout)
        else:
            raise DiscError(f"unsupported command: {args.command}")
    except (
        DiscError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        json.JSONDecodeError,
        struct.error,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
