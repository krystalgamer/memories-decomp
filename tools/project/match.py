#!/usr/bin/env python3

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from hashing import sha256_file
from psx_exe import PSX_EXE_HEADER_SIZE, load_verified_executable
from workspace import WorkspaceError, require_workspace_root, resolve_within


class MatchError(RuntimeError):
    pass


def first_difference(expected: Path, actual: Path) -> tuple[int, int | None, int | None]:
    offset = 0
    with expected.open("rb") as expected_handle:
        with actual.open("rb") as actual_handle:
            while True:
                expected_chunk = expected_handle.read(1024 * 1024)
                actual_chunk = actual_handle.read(1024 * 1024)
                shared = min(len(expected_chunk), len(actual_chunk))
                for index in range(shared):
                    if expected_chunk[index] != actual_chunk[index]:
                        return (
                            offset + index,
                            expected_chunk[index],
                            actual_chunk[index],
                        )
                if len(expected_chunk) != len(actual_chunk):
                    expected_byte = expected_chunk[shared] if len(expected_chunk) > shared else None
                    actual_byte = actual_chunk[shared] if len(actual_chunk) > shared else None
                    return offset + shared, expected_byte, actual_byte
                if not expected_chunk:
                    raise MatchError("files differ but no differing byte was found")
                offset += len(expected_chunk)


def compare(root: Path, target_path: str, output_path: str) -> None:
    expected, executable, _header_bytes, header = load_verified_executable(
        root, target_path
    )
    actual = resolve_within(root, output_path, must_exist=True)
    if not actual.is_file():
        raise MatchError(f"rebuilt output is not a file: {output_path}")

    expected_size = expected.stat().st_size
    actual_size = actual.stat().st_size
    expected_sha256 = str(executable["sha256"])
    actual_sha256 = sha256_file(actual)
    if expected_size == actual_size and expected_sha256 == actual_sha256:
        print(f"MATCH  {actual.relative_to(root)}")
        print(f"sha256 {actual_sha256}")
        return

    offset, expected_byte, actual_byte = first_difference(expected, actual)
    location = f"file offset {offset:#x}"
    if offset >= PSX_EXE_HEADER_SIZE and offset < header.file_size:
        location += f", VRAM {header.file_offset_to_vram(offset):#010x}"
    expected_text = "EOF" if expected_byte is None else f"{expected_byte:#04x}"
    actual_text = "EOF" if actual_byte is None else f"{actual_byte:#04x}"
    raise MatchError(
        f"mismatch at {location}: expected {expected_text}, got {actual_text}; "
        f"size {actual_size:#x}/{expected_size:#x}; "
        f"SHA-256 {actual_sha256}/{expected_sha256}"
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compare a rebuilt PS-X executable with a configured target."
    )
    parser.add_argument(
        "--target",
        default="config/slus_01411/target.yaml",
        help="target metadata path relative to the repository root",
    )
    parser.add_argument(
        "--output",
        default="tmp/project-build/SLUS_014.11",
        help="rebuilt executable path relative to the repository root",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        compare(root, args.target, args.output)
    except (MatchError, WorkspaceError, OSError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
