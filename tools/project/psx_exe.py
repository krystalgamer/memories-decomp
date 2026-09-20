#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import shutil
import struct
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any

from hashing import sha256_file
from verify_inputs import (
    VerificationError,
    load_target_manifest,
    validate_psx_executable,
)
from workspace import WorkspaceError, require_workspace_root, resolve_within


class PsxExeError(RuntimeError):
    pass


PSX_EXE_HEADER_SIZE = 0x800


@dataclass(frozen=True)
class PsxExeHeader:
    initial_pc: int
    initial_gp: int
    load_address: int
    payload_size: int
    data_address: int
    data_size: int
    bss_address: int
    bss_size: int
    stack_base: int
    stack_offset: int
    region_text: str

    @property
    def file_size(self) -> int:
        return PSX_EXE_HEADER_SIZE + self.payload_size

    @property
    def load_end(self) -> int:
        return self.load_address + self.payload_size

    @property
    def initial_stack(self) -> int:
        return self.stack_base + self.stack_offset

    def file_offset_to_vram(self, file_offset: int) -> int:
        if not PSX_EXE_HEADER_SIZE <= file_offset < self.file_size:
            raise PsxExeError(
                f"file offset {file_offset:#x} is outside the loaded payload"
            )
        return self.load_address + file_offset - PSX_EXE_HEADER_SIZE

    def vram_to_file_offset(self, address: int) -> int:
        if not self.load_address <= address < self.load_end:
            raise PsxExeError(
                f"address {address:#010x} is outside the loaded payload"
            )
        return PSX_EXE_HEADER_SIZE + address - self.load_address

    def to_metadata(self) -> dict[str, Any]:
        values = asdict(self)
        values.update(
            {
                "header_size": PSX_EXE_HEADER_SIZE,
                "file_size": self.file_size,
                "load_end": self.load_end,
                "initial_stack": self.initial_stack,
                "entry_file_offset": self.vram_to_file_offset(self.initial_pc),
            }
        )
        return values


def parse_header(header: bytes) -> PsxExeHeader:
    if len(header) != PSX_EXE_HEADER_SIZE:
        raise PsxExeError(
            f"PS-X EXE header must be {PSX_EXE_HEADER_SIZE:#x} bytes"
        )
    if header[:8] != b"PS-X EXE":
        raise PsxExeError("missing PS-X EXE signature")

    (
        initial_pc,
        initial_gp,
        load_address,
        payload_size,
        data_address,
        data_size,
        bss_address,
        bss_size,
        stack_base,
        stack_offset,
    ) = struct.unpack_from("<10I", header, 0x10)

    region_bytes = header[0x4C:].rstrip(b"\0")
    try:
        region_text = region_bytes.decode("ascii")
    except UnicodeDecodeError as error:
        raise PsxExeError("PS-X EXE region text is not ASCII") from error

    return PsxExeHeader(
        initial_pc=initial_pc,
        initial_gp=initial_gp,
        load_address=load_address,
        payload_size=payload_size,
        data_address=data_address,
        data_size=data_size,
        bss_address=bss_address,
        bss_size=bss_size,
        stack_base=stack_base,
        stack_offset=stack_offset,
        region_text=region_text,
    )


def load_verified_executable(
    root: Path,
    target_manifest: str = "config/slus_01411/target.yaml",
) -> tuple[Path, dict[str, Any], bytes, PsxExeHeader]:
    target_path = resolve_within(root, target_manifest, must_exist=True)
    manifest = load_target_manifest(target_path)
    executable = manifest["executable"]
    source = resolve_within(root, str(executable["path"]), must_exist=True)

    expected_sha256 = str(executable["sha256"])
    actual_sha256 = sha256_file(source)
    if actual_sha256 != expected_sha256:
        raise PsxExeError(
            f"{source}: SHA-256 is {actual_sha256}, expected {expected_sha256}"
        )

    validate_psx_executable(source, executable)
    with source.open("rb") as handle:
        header_bytes = handle.read(PSX_EXE_HEADER_SIZE)
    header = parse_header(header_bytes)
    return source, executable, header_bytes, header


def atomic_write(path: Path, data: bytes) -> None:
    temporary = path.with_name(f"{path.name}.tmp")
    try:
        temporary.write_bytes(data)
        temporary.replace(path)
    except OSError:
        temporary.unlink(missing_ok=True)
        raise


def atomic_copy_payload(
    source: Path, destination: Path, expected_size: int
) -> None:
    temporary = destination.with_name(f"{destination.name}.tmp")
    try:
        with source.open("rb") as source_handle:
            source_handle.seek(PSX_EXE_HEADER_SIZE)
            with temporary.open("wb") as destination_handle:
                shutil.copyfileobj(
                    source_handle,
                    destination_handle,
                    length=1024 * 1024,
                )
        actual_size = temporary.stat().st_size
        if actual_size != expected_size:
            raise PsxExeError(
                f"extracted payload is {actual_size} bytes, expected {expected_size}"
            )
        temporary.replace(destination)
    except (OSError, PsxExeError):
        temporary.unlink(missing_ok=True)
        raise


def extract(root: Path, output_path: str) -> None:
    source, executable, header_bytes, header = load_verified_executable(root)
    output_relative = Path(output_path)
    output = resolve_within(root, output_relative)
    output.mkdir(parents=True, exist_ok=True)

    header_path = resolve_within(root, output_relative / "header.bin")
    payload_path = resolve_within(root, output_relative / "payload.bin")
    metadata_path = resolve_within(root, output_relative / "metadata.json")

    atomic_write(header_path, header_bytes)
    atomic_copy_payload(source, payload_path, header.payload_size)

    metadata = {
        "source": str(executable["path"]),
        "sha256": str(executable["sha256"]),
        "header": header.to_metadata(),
    }
    atomic_write(
        metadata_path,
        (json.dumps(metadata, indent=2, sort_keys=True) + "\n").encode("utf-8"),
    )

    print(f"header:  {header_path.relative_to(root)}")
    print(f"payload: {payload_path.relative_to(root)}")
    print(f"metadata: {metadata_path.relative_to(root)}")


def print_info(root: Path) -> None:
    source, executable, _header_bytes, header = load_verified_executable(root)
    entry_offset = header.vram_to_file_offset(header.initial_pc)
    print(f"path:          {source.relative_to(root)}")
    print(f"sha256:        {executable['sha256']}")
    print(f"file size:     {header.file_size:#x}")
    print(f"payload:       {header.payload_size:#x}")
    print(f"load range:    {header.load_address:#010x}-{header.load_end:#010x}")
    print(f"entry point:   {header.initial_pc:#010x}")
    print(f"entry offset:  {entry_offset:#x}")
    print(f"initial gp:    {header.initial_gp:#010x}")
    print(f"initial stack: {header.initial_stack:#010x}")
    print(f"region:        {header.region_text}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Inspect or extract the verified SLUS-01411 PS-X EXE."
    )
    subparsers = parser.add_subparsers(dest="command", required=True)
    subparsers.add_parser("info", help="print PS-X EXE header information")
    extract_parser = subparsers.add_parser(
        "extract", help="extract the header and loaded payload"
    )
    extract_parser.add_argument(
        "--output",
        default="tmp/extract/slus_01411",
        help="output directory relative to the repository root",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        if args.command == "info":
            print_info(root)
        elif args.command == "extract":
            extract(root, args.output)
        else:
            raise PsxExeError(f"unsupported command: {args.command}")
    except (
        PsxExeError,
        VerificationError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
