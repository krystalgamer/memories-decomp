#!/usr/bin/env python3

from __future__ import annotations

import argparse
import re
import struct
import sys
from pathlib import Path
from typing import Any

from hashing import sha256_file
from workspace import WorkspaceError, require_workspace_root, resolve_within


class VerificationError(RuntimeError):
    pass


SHA256_PATTERN = re.compile(r"^[0-9a-f]{64}$")
KNOWN_PATCHED_INPUTS = {
    (
        "game/DATA/WA_MRG.MRG",
        "a519a1f5b27f4c6702613ba48c10bf945d25c9c51611ae3d2fab6e5d562b2d66",
    ),
    (
        "game/rpg-yfm.bin",
        "c020cb7cef2f55a0433324444f9052d0660c3eb247730d1ab1ec7204962d6c59",
    ),
}


def parse_scalar(value: str) -> str | int:
    value = value.strip()
    if len(value) >= 2 and value[0] == value[-1] == '"':
        return value[1:-1]
    if value.startswith("0x"):
        return int(value, 16)
    if value.isdecimal():
        return int(value, 10)
    return value


def parse_key_value(text: str, path: Path, line_number: int) -> tuple[str, Any]:
    key, separator, value = text.partition(":")
    if not separator or not key or not value.strip():
        raise VerificationError(
            f"{path}:{line_number}: expected a key and scalar value"
        )
    return key.strip(), parse_scalar(value)


def load_target_manifest(path: Path) -> dict[str, Any]:
    manifest: dict[str, Any] = {}
    executable: dict[str, Any] = {}
    inputs: list[dict[str, Any]] = []
    section: str | None = None
    current_input: dict[str, Any] | None = None

    with path.open("r", encoding="utf-8") as handle:
        for line_number, raw_line in enumerate(handle, start=1):
            line = raw_line.rstrip("\n")
            if not line.strip() or line.lstrip().startswith("#"):
                continue

            if not line.startswith(" "):
                key, separator, value = line.partition(":")
                if not separator:
                    raise VerificationError(
                        f"{path}:{line_number}: malformed top-level entry"
                    )
                section = key.strip()
                current_input = None
                if value.strip():
                    manifest[section] = parse_scalar(value)
                    section = None
                continue

            if section == "executable" and line.startswith("  "):
                key, value = parse_key_value(line.strip(), path, line_number)
                executable[key] = value
                continue

            if section == "inputs":
                if line.startswith("  - "):
                    current_input = {}
                    inputs.append(current_input)
                    key, value = parse_key_value(line[4:], path, line_number)
                    current_input[key] = value
                    continue
                if line.startswith("    ") and current_input is not None:
                    key, value = parse_key_value(line.strip(), path, line_number)
                    current_input[key] = value
                    continue

            raise VerificationError(
                f"{path}:{line_number}: unsupported manifest structure"
            )

    manifest["executable"] = executable
    manifest["inputs"] = inputs
    return manifest


def load_checksum_manifest(path: Path) -> dict[str, str]:
    checksums: dict[str, str] = {}
    with path.open("r", encoding="ascii") as handle:
        for line_number, raw_line in enumerate(handle, start=1):
            line = raw_line.rstrip("\n")
            if not line:
                continue
            checksum, separator, relative_path = line.partition("  ")
            if (
                not separator
                or not SHA256_PATTERN.fullmatch(checksum)
                or not relative_path
            ):
                raise VerificationError(
                    f"{path}:{line_number}: malformed SHA-256 entry"
                )
            if relative_path in checksums:
                raise VerificationError(
                    f"{path}:{line_number}: duplicate path {relative_path}"
                )
            checksums[relative_path] = checksum
    return checksums


def require_fields(
    values: dict[str, Any], fields: tuple[str, ...], description: str
) -> None:
    missing = [field for field in fields if field not in values]
    if missing:
        raise VerificationError(
            f"{description} is missing required fields: {', '.join(missing)}"
        )


def require_string(values: dict[str, Any], field: str, description: str) -> str:
    value = values[field]
    if not isinstance(value, str):
        raise VerificationError(f"{description} field {field} must be a string")
    return value


def require_positive_int(
    values: dict[str, Any], field: str, description: str
) -> int:
    value = values[field]
    if not isinstance(value, int) or isinstance(value, bool) or value <= 0:
        raise VerificationError(
            f"{description} field {field} must be a positive integer"
        )
    return value


def validate_psx_executable(path: Path, metadata: dict[str, Any]) -> None:
    require_fields(
        metadata,
        (
            "file_size",
            "header_size",
            "payload_size",
            "load_address",
            "entry_point",
            "initial_stack",
        ),
        "executable metadata",
    )

    header_size = require_positive_int(metadata, "header_size", "executable metadata")
    expected_file_size = require_positive_int(
        metadata, "file_size", "executable metadata"
    )
    expected_payload_size = require_positive_int(
        metadata, "payload_size", "executable metadata"
    )
    expected_load_address = require_positive_int(
        metadata, "load_address", "executable metadata"
    )
    expected_entry_address = require_positive_int(
        metadata, "entry_point", "executable metadata"
    )
    expected_stack_address = require_positive_int(
        metadata, "initial_stack", "executable metadata"
    )

    with path.open("rb") as handle:
        header = handle.read(header_size)

    if len(header) != header_size:
        raise VerificationError(
            f"{path}: expected a {header_size:#x}-byte PS-X EXE header"
        )
    if header[:8] != b"PS-X EXE":
        raise VerificationError(f"{path}: missing PS-X EXE signature")

    (
        actual_entry_point,
        _global_pointer,
        actual_load_address,
        actual_payload_size,
        _data_address,
        _data_size,
        _bss_address,
        _bss_size,
        actual_initial_stack,
        _stack_size,
    ) = struct.unpack_from("<10I", header, 0x10)

    expected_fields = (
        ("entry point", actual_entry_point, expected_entry_address),
        ("load address", actual_load_address, expected_load_address),
        ("payload size", actual_payload_size, expected_payload_size),
        ("initial stack", actual_initial_stack, expected_stack_address),
    )
    for label, actual, expected in expected_fields:
        if actual != expected:
            raise VerificationError(
                f"{path}: {label} is {actual:#010x}, expected {expected:#010x}"
            )

    actual_size = path.stat().st_size
    if actual_size != expected_file_size:
        raise VerificationError(
            f"{path}: size is {actual_size}, expected {expected_file_size}"
        )
    if expected_file_size != header_size + expected_payload_size:
        raise VerificationError(
            "target metadata file size does not equal header plus payload size"
        )


def expected_files(manifest: dict[str, Any]) -> list[dict[str, Any]]:
    executable = manifest["executable"]
    require_fields(
        executable,
        ("path", "sha256", "file_size"),
        "executable metadata",
    )

    executable_path = require_string(executable, "path", "executable metadata")
    executable_sha256 = require_string(
        executable, "sha256", "executable metadata"
    )
    executable_size = require_positive_int(
        executable, "file_size", "executable metadata"
    )
    result = [
        {
            "path": executable_path,
            "sha256": executable_sha256,
            "size": executable_size,
            "executable": True,
        }
    ]
    for index, item in enumerate(manifest["inputs"]):
        description = f"input {index}"
        require_fields(item, ("path", "sha256", "size"), description)
        result.append(
            {
                "path": require_string(item, "path", description),
                "sha256": require_string(item, "sha256", description),
                "size": require_positive_int(item, "size", description),
                "executable": False,
            }
        )
    return result


def verify(
    root: Path,
    target_manifest_path: Path,
    checksum_manifest_path: Path,
    *,
    executable_only: bool,
) -> None:
    target = load_target_manifest(target_manifest_path)
    checksums = load_checksum_manifest(checksum_manifest_path)
    files = expected_files(target)
    if executable_only:
        files = [item for item in files if item["executable"]]
    expected_paths = {str(item["path"]) for item in expected_files(target)}

    if not executable_only and set(checksums) != expected_paths:
        missing = sorted(expected_paths - set(checksums))
        unexpected = sorted(set(checksums) - expected_paths)
        details: list[str] = []
        if missing:
            details.append(f"missing checksum paths: {', '.join(missing)}")
        if unexpected:
            details.append(f"unexpected checksum paths: {', '.join(unexpected)}")
        raise VerificationError("; ".join(details))

    for item in files:
        relative_path = str(item["path"])
        declared_checksum = str(item["sha256"])
        if not SHA256_PATTERN.fullmatch(declared_checksum):
            raise VerificationError(
                f"target manifest has an invalid SHA-256 for {relative_path}"
            )
        if checksums[relative_path] != declared_checksum:
            raise VerificationError(
                f"checksum manifests disagree for {relative_path}"
            )

        path = resolve_within(root, relative_path, must_exist=True)
        if not path.is_file():
            raise VerificationError(f"{relative_path}: expected a regular file")

        actual_size = path.stat().st_size
        expected_size = item["size"]
        if actual_size != expected_size:
            raise VerificationError(
                f"{relative_path}: size is {actual_size}, expected {expected_size}"
            )

        actual_checksum = sha256_file(path)
        if actual_checksum != declared_checksum:
            if (relative_path, actual_checksum) in KNOWN_PATCHED_INPUTS:
                raise VerificationError(
                    f"{relative_path}: this is the known dump with the "
                    "anti-piracy branch patched out; provide an untouched "
                    "North American dump"
                )
            raise VerificationError(
                f"{relative_path}: SHA-256 is {actual_checksum}, "
                f"expected {declared_checksum}"
            )

        if item["executable"]:
            validate_psx_executable(path, target["executable"])
        print(f"OK  {relative_path}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate the immutable SLUS-01411 project inputs."
    )
    parser.add_argument(
        "--target",
        default="config/slus_01411/target.yaml",
        help="target metadata path relative to the repository root",
    )
    parser.add_argument(
        "--checksums",
        default="config/slus_01411/files.sha256",
        help="SHA-256 manifest path relative to the repository root",
    )
    parser.add_argument(
        "--executable-only",
        action="store_true",
        help="validate only the PS-X executable needed for a matching build",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        target_manifest = resolve_within(root, args.target, must_exist=True)
        checksum_manifest = resolve_within(root, args.checksums, must_exist=True)
        verify(
            root,
            target_manifest,
            checksum_manifest,
            executable_only=args.executable_only,
        )
    except (VerificationError, WorkspaceError, OSError, UnicodeError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
