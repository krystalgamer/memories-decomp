#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import shutil
import stat
import sys
import tempfile
import zipfile

from hashing import sha256_file
from verify_inputs import VerificationError, load_checksum_manifest
from workspace import WorkspaceError, require_workspace_root, resolve_within


REGIONS = {
    "europe": ("eur", "sles_03947"),
    "france": ("fra", "sles_03948"),
    "germany": ("ger", "sles_03949"),
    "italy": ("ita", "sles_03950"),
    "spain": ("esp", "sles_03951"),
    "japanese": ("jap", "slpm_86398"),
}


def stage_inputs(
    root: Path, archive: str, region: str, *, archives_only: bool = False
) -> None:
    code, config = REGIONS[region]
    archive_path = resolve_within(root, archive, must_exist=True)
    checksums = load_checksum_manifest(
        resolve_within(root, f"config/{config}/files.sha256", must_exist=True)
    )
    selected = {
        name: digest for name, digest in checksums.items()
        if not archives_only or name.endswith(("/SU.MRG", "/WA_MRG.MRG"))
    }
    if len(selected) != (2 if archives_only else 3):
        raise VerificationError("expected the regional executable and two archive hashes")
    scratch = resolve_within(root, "tmp")
    scratch.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="ci-inputs-", dir=scratch) as temporary:
        pending = []
        with zipfile.ZipFile(archive_path) as bundle:
            names = bundle.namelist()
            for relative, digest in selected.items():
                if not relative.startswith(f"game/{region}/"):
                    raise VerificationError(f"unexpected regional input path: {relative}")
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
                if sha256_file(staged) != digest:
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
    parser.add_argument("--archives-only", action="store_true")
    args = parser.parse_args()
    try:
        root = require_workspace_root()
        stage_inputs(root, args.archive, args.region, archives_only=args.archives_only)
    except (OSError, WorkspaceError, VerificationError, zipfile.BadZipFile) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
