#!/usr/bin/env python3
"""Prepare an encrypted Actions cache; consumers never download from the origin."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import zipfile

from overlay_extract import OverlayError, load_manifest
from stage_ci_inputs import REGIONS, stage_inputs
from verify_inputs import VerificationError
from workspace import WorkspaceError, require_workspace_root, resolve_within


CACHE_PATH = "tmp/ci-retail-cache/ci-files.zip.gpg"
CACHE_VERSION = "retail-encrypted-v1"
CREDENTIALS = ("YGOFM_CI_FILES", "YGOFM_CI_FILES_USERNAME", "YGOFM_CI_FILES_PASSWORD")


class BundleError(Exception):
    pass


def passphrase() -> bytes:
    values = []
    for name in CREDENTIALS:
        value = os.environ.get(name, "")
        if not value:
            raise BundleError(f"Configure the {name} repository secret.")
        values.append(value)
    # JSON escaping keeps embedded credential newlines off the passphrase protocol.
    return json.dumps(values, ensure_ascii=True, separators=(",", ":")).encode() + b"\n"


def cache_key(root: Path) -> str:
    secret = passphrase()
    digest = hashlib.sha256(CACHE_VERSION.encode())
    for _, config, _ in sorted(REGIONS.values()):
        relative = f"config/{config}/files.sha256"
        digest.update(b"\0" + relative.encode() + b"\0")
        digest.update(resolve_within(root, relative, must_exist=True).read_bytes())
    for region in sorted(REGIONS):
        _, modules = load_manifest(root, "japan" if region == "japanese" else region)
        archives = sorted({module["archive"] for module in modules})
        digest.update(b"\0" + region.encode() + b"\0" + json.dumps(archives).encode())
    # Public cache keys must not provide a fast password-guessing oracle.
    fingerprint = hashlib.pbkdf2_hmac("sha256", secret, digest.digest(), 600_000)
    return f"{CACHE_VERSION}-{fingerprint.hex()}"


def crypt(source: Path, destination: Path, home: Path, *, decrypt: bool) -> None:
    home.mkdir(mode=0o700)
    command = [
        "gpg", "--no-options", "--homedir", str(home), "--batch", "--yes", "--no-tty",
        "--no-autostart", "--pinentry-mode", "loopback", "--no-symkey-cache",
        "--passphrase-fd", "0", "--status-fd", "1", "--output", str(destination),
    ]
    if decrypt:
        command += ["--decrypt", str(source)]
    else:
        command += [
            "--symmetric", "--cipher-algo", "AES256", "--force-mdc",
            "--s2k-mode", "3", "--s2k-digest-algo", "SHA256", "--s2k-count", "65011712",
            "--compress-algo", "none", str(source),
        ]
    result = subprocess.run(command, input=passphrase(), capture_output=True, check=False)
    statuses = set(result.stdout.splitlines())
    authenticated = (b"[GNUPG:] DECRYPTION_OKAY" in statuses
                     and b"[GNUPG:] GOODMDC" in statuses)
    if result.returncode or (decrypt and not authenticated):
        operation = "decrypt" if decrypt else "encrypt"
        raise BundleError(
            f"Cannot {operation} the private CI bundle (GnuPG exit {result.returncode}). "
            "Check credentials and cache integrity; delete a damaged cache and rerun all jobs."
        )


def encrypt_bundle(root: Path, archive: str) -> None:
    passphrase()
    source = resolve_within(root, archive, must_exist=True)
    for region in REGIONS:
        stage_inputs(root, archive, region, verify_only=True)
    scratch = resolve_within(root, "tmp")
    scratch.mkdir(parents=True, exist_ok=True)
    destination = resolve_within(root, CACHE_PATH)
    destination.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="ci-encrypt-", dir=scratch) as temporary:
        temporary = Path(temporary)
        encrypted = temporary / "bundle.gpg"
        crypt(source, encrypted, temporary / "gnupg", decrypt=False)
        encrypted.replace(destination)
    print("Verified all regions and encrypted the CI bundle for caching.")


def stage_cached_bundle(
    root: Path, region: str, *, archives_only: bool = False, executable_only: bool = False,
) -> None:
    passphrase()
    source = resolve_within(root, CACHE_PATH)
    if not source.is_file():
        raise BundleError("Shared CI bundle cache is missing. Rerun all jobs to prepare it.")
    scratch = resolve_within(root, "tmp")
    scratch.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="ci-decrypt-", dir=scratch) as temporary:
        temporary = Path(temporary)
        archive = temporary / "inputs.zip"
        # GnuPG must authenticate the complete ciphertext before any input is staged.
        crypt(source, archive, temporary / "gnupg", decrypt=True)
        stage_inputs(
            root, archive.relative_to(root).as_posix(), region,
            archives_only=archives_only, executable_only=executable_only,
        )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("key")
    encrypt = commands.add_parser("encrypt")
    encrypt.add_argument("--archive", required=True)
    stage = commands.add_parser("stage")
    stage.add_argument("--region", required=True, choices=REGIONS)
    selection = stage.add_mutually_exclusive_group()
    selection.add_argument("--archives-only", action="store_true")
    selection.add_argument("--executable-only", action="store_true")
    args = parser.parse_args()
    try:
        root = require_workspace_root()
        if args.command == "key":
            print(f"cache-key={cache_key(root)}")
        elif args.command == "encrypt":
            encrypt_bundle(root, args.archive)
        else:
            stage_cached_bundle(
                root, args.region, archives_only=args.archives_only,
                executable_only=args.executable_only,
            )
    except (BundleError, OSError, WorkspaceError, VerificationError, OverlayError,
            json.JSONDecodeError, zipfile.BadZipFile) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
