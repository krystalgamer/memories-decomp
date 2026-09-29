#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
from pathlib import Path
import shutil
import stat
import sys
import tempfile
import zipfile

from hashing import sha256_file
from overlay_extract import OverlayError, load_manifest, require_string
from verify_inputs import KNOWN_PATCHED_INPUTS, VerificationError, load_checksum_manifest
from workspace import WorkspaceError, require_workspace_root, resolve_within


REGIONS = {
    "usa": ("usa", "slus_01411", "SLUS_014.11"),
    "europe": ("eur", "sles_03947", "SLES_039.47"),
    "france": ("fra", "sles_03948", "SLES_039.48"),
    "germany": ("ger", "sles_03949", "SLES_039.49"),
    "italy": ("ita", "sles_03950", "SLES_039.50"),
    "spain": ("esp", "sles_03951", "SLES_039.51"),
    "japanese": ("jap", "slpm_86398", "SLPM_863.98"),
}


def stage_inputs(
    root: Path, archive: str, region: str, *,
    archives_only: bool = False, executable_only: bool = False,
) -> None:
    if archives_only and executable_only:
        raise VerificationError("archives-only and executable-only are mutually exclusive")
    code, config, executable = REGIONS[region]
    directory = "game" if region == "usa" else f"game/{region}"
    archive_path = resolve_within(root, archive, must_exist=True)
    checksums = load_checksum_manifest(
        resolve_within(root, f"config/{config}/files.sha256", must_exist=True)
    )
    selected = []
    if not archives_only:
        selected.append(f"{directory}/{executable}")
    if not executable_only:
        selected.extend(
            f"{directory}/DATA/{name}" for name in ("SU.MRG", "WA_MRG.MRG", "MODEL.MRG")
        )
        _, modules = load_manifest(root, "japan" if region == "japanese" else region)
        for module in modules:
            relative = require_string(module, "archive")
            if Path(relative).parent.as_posix() != f"{directory}/DATA":
                raise VerificationError(f"overlay archive must be in {directory}/DATA: {relative}")
            if relative not in checksums:
                raise VerificationError(f"missing input checksum: {relative}")
            if require_string(module, "archive_sha256") != checksums[relative]:
                raise VerificationError(f"overlay archive checksum disagrees with files.sha256: {relative}")
            if relative not in selected:
                selected.append(relative)
    for relative in selected:
        if relative not in checksums:
            raise VerificationError(f"missing input checksum: {relative}")
    scratch = resolve_within(root, "tmp")
    scratch.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="ci-inputs-", dir=scratch) as temporary:
        pending = []
        with zipfile.ZipFile(archive_path) as bundle:
            names = bundle.namelist()
            for relative in selected:
                digest = checksums[relative]
                destination = resolve_within(root, relative)
                member = f"ci_files/{code}/{Path(relative).name}"
                if names.count(member) != 1:
                    raise VerificationError(f"expected exactly one ZIP member: {member}")
                info = bundle.getinfo(member)
                if info.is_dir() or stat.S_ISLNK(info.external_attr >> 16):
                    raise VerificationError(f"ZIP member is not a regular input: {member}")
                staged = Path(temporary) / destination.name
                with bundle.open(info) as source, staged.open("wb") as output:
                    shutil.copyfileobj(source, output)
                actual = sha256_file(staged)
                if (relative, actual) in KNOWN_PATCHED_INPUTS:
                    raise VerificationError(
                        f"known patched retail input: {member}; supply an untouched dump"
                    )
                if actual != digest:
                    raise VerificationError(f"SHA-256 mismatch: {member}")
                if destination.exists() and sha256_file(destination) != digest:
                    raise VerificationError(f"refusing to replace different retail input: {relative}")
                pending.append((staged, destination, relative))
        # Do not install any member until the complete selection has been verified.
        for staged, destination, relative in pending:
            if not destination.exists():
                destination.parent.mkdir(parents=True, exist_ok=True)
                staged.replace(destination)
            print(f"{relative}: OK")


def main() -> int:
    parser = argparse.ArgumentParser(description="Stage hash-verified regional CI inputs.")
    parser.add_argument("--archive", required=True, help="repository-relative CI ZIP path")
    parser.add_argument("--region", required=True, choices=REGIONS)
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument("--archives-only", action="store_true")
    selection.add_argument("--executable-only", action="store_true")
    args = parser.parse_args()
    try:
        root = require_workspace_root()
        stage_inputs(
            root, args.archive, args.region,
            archives_only=args.archives_only, executable_only=args.executable_only,
        )
    except (OSError, WorkspaceError, VerificationError, OverlayError,
            json.JSONDecodeError, zipfile.BadZipFile) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
