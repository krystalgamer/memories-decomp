#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import sys
import tarfile
import urllib.request
from pathlib import Path, PurePosixPath
from typing import Any

import old_gcc

from workspace import WorkspaceError, require_workspace_root, resolve_within


CONFIG_PATH = "tools/bootstrap/old_gcc_prebuilt.json"
REAL_GCC_PATH = "libexec/gcc-2.8.1-psx"
EXPECTED_ARCHIVE_FILES = {"gcc", "cc1", "cpp", "g++", "cc1plus"}


class PrebuiltCompilerError(RuntimeError):
    pass


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def load_json(path: Path) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as handle:
        value = json.load(handle)
    if not isinstance(value, dict) or value.get("schema") != 1:
        raise PrebuiltCompilerError(f"{path}: unsupported schema")
    return value


def load_config(root: Path, path: str = CONFIG_PATH) -> dict[str, Any]:
    config = load_json(resolve_within(root, path, must_exist=True))
    required = {"release", "url", "archive", "sha256", "target", "files"}
    if not required.issubset(config):
        raise PrebuiltCompilerError("prebuilt GCC config is incomplete")
    files = config["files"]
    if not isinstance(files, dict) or set(files) != {"gcc", "cc1", "cpp"}:
        raise PrebuiltCompilerError("prebuilt GCC file hashes are incomplete")
    return config


def load_lock(root: Path, config: dict[str, Any]) -> dict[str, Any]:
    """The installed prefix, driver target and version. A config for the
    default GCC 2.8.1 release reads them from old_gcc.json; a config for
    another old-gcc release carries its own `lock` object."""
    lock = config.get("lock")
    if lock is None:
        return old_gcc.load_lock(root)
    required = {"prefix", "target", "version"}
    if not isinstance(lock, dict) or not required.issubset(lock):
        raise PrebuiltCompilerError("prebuilt GCC lock is incomplete")
    return lock


def real_gcc_path(lock: dict[str, Any]) -> str:
    return str(lock.get("real_gcc", REAL_GCC_PATH))


def download(root: Path, config: dict[str, Any]) -> Path:
    archive = resolve_within(root, str(config["archive"]))
    if archive.is_file() and sha256(archive) == config["sha256"]:
        return archive
    archive.parent.mkdir(parents=True, exist_ok=True)
    temporary = archive.with_name(f"{archive.name}.part")
    temporary.unlink(missing_ok=True)
    try:
        with urllib.request.urlopen(str(config["url"])) as response:
            with temporary.open("wb") as handle:
                shutil.copyfileobj(response, handle, 1024 * 1024)
        actual = sha256(temporary)
        if actual != config["sha256"]:
            raise PrebuiltCompilerError(
                f"prebuilt GCC archive SHA-256 is {actual}, "
                f"expected {config['sha256']}"
            )
        temporary.replace(archive)
    finally:
        temporary.unlink(missing_ok=True)
    return archive


def archive_files(
    archive: tarfile.TarFile,
    config: dict[str, Any],
) -> dict[str, bytes]:
    result: dict[str, bytes] = {}
    members = archive.getmembers()
    names = {member.name for member in members}
    if names != EXPECTED_ARCHIVE_FILES:
        raise PrebuiltCompilerError(
            f"unexpected prebuilt archive members: {sorted(names)}"
        )
    for member in members:
        path = PurePosixPath(member.name)
        if (
            path.is_absolute()
            or len(path.parts) != 1
            or not member.isfile()
            or member.name not in EXPECTED_ARCHIVE_FILES
        ):
            raise PrebuiltCompilerError(
                f"unsafe prebuilt archive member: {member.name}"
            )
        source = archive.extractfile(member)
        if source is None:
            raise PrebuiltCompilerError(
                f"cannot read prebuilt archive member: {member.name}"
            )
        with source:
            result[member.name] = source.read()
    for name in ("gcc", "cc1", "cpp"):
        actual = hashlib.sha256(result[name]).hexdigest()
        if actual != config["files"][name]:
            raise PrebuiltCompilerError(
                f"prebuilt {name} SHA-256 is {actual}, "
                f"expected {config['files'][name]}"
            )
    return result


def wrapper_text(target: str, version: str, real: str = REAL_GCC_PATH) -> str:
    return f"""#!/bin/sh
set -eu
prefix=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
exec "$prefix/{real}" \\
  -B"$prefix/lib/gcc-lib/{target}/{version}/" "$@"
"""


def install(
    root: Path,
    lock: dict[str, Any],
    config: dict[str, Any],
) -> None:
    try:
        check_prebuilt(root, lock, config)
        return
    except (
        PrebuiltCompilerError,
        old_gcc.CompilerError,
        WorkspaceError,
        OSError,
        ValueError,
    ):
        pass

    prefix = resolve_within(root, str(lock["prefix"]))
    if prefix.exists():
        raise PrebuiltCompilerError(
            f"incomplete compiler prefix exists: {prefix}"
        )
    archive_path = download(root, config)
    with tarfile.open(archive_path, "r:gz") as archive:
        files = archive_files(archive, config)

    staging = resolve_within(
        root, f"tmp/bootstrap/{Path(str(lock['prefix'])).name}-prebuilt"
    )
    if staging.exists():
        shutil.rmtree(staging)
    try:
        binary = staging / "bin" / f"{lock['target']}-gcc"
        library = (
            staging
            / "lib"
            / "gcc-lib"
            / str(lock["target"])
            / str(lock["version"])
        )
        real = staging / real_gcc_path(lock)
        binary.parent.mkdir(parents=True)
        library.mkdir(parents=True)
        real.parent.mkdir(parents=True)
        binary.write_text(
            wrapper_text(
                str(lock["target"]), str(lock["version"]), real_gcc_path(lock)
            ),
            encoding="ascii",
        )
        real.write_bytes(files["gcc"])
        (library / "cc1").write_bytes(files["cc1"])
        (library / "cpp").write_bytes(files["cpp"])
        for path in (binary, real, library / "cc1", library / "cpp"):
            path.chmod(0o755)
        manifest = {
            "schema": 1,
            "distribution": "decompals-old-gcc-prebuilt",
            "release": config["release"],
            "archive_sha256": config["sha256"],
            "target": lock["target"],
            "compiler_target": config["target"],
            "version": lock["version"],
            "real_gcc_sha256": sha256(real),
            "sha256": {
                "gcc": sha256(binary),
                "cc1": sha256(library / "cc1"),
                "cpp": sha256(library / "cpp"),
            },
        }
        (staging / "build-manifest.json").write_text(
            json.dumps(manifest, indent=2, sort_keys=True) + "\n",
            encoding="utf-8",
        )
        prefix.parent.mkdir(parents=True, exist_ok=True)
        staging.replace(prefix)
    finally:
        if staging.exists():
            shutil.rmtree(staging)


def check_prebuilt(
    root: Path,
    lock: dict[str, Any],
    config: dict[str, Any],
) -> None:
    prefix = resolve_within(root, str(lock["prefix"]), must_exist=True)
    manifest_path = prefix / "build-manifest.json"
    with manifest_path.open("r", encoding="utf-8") as handle:
        manifest = json.load(handle)
    if not isinstance(manifest, dict):
        raise PrebuiltCompilerError(
            f"{manifest_path}: compiler manifest must be an object"
        )
    if manifest.get("distribution") != "decompals-old-gcc-prebuilt":
        old_gcc.check_compiler(root, lock)
        return
    if manifest.get("schema") != 1:
        raise PrebuiltCompilerError(
            f"{manifest_path}: unsupported prebuilt manifest schema"
        )
    if (
        manifest.get("release") != config["release"]
        or manifest.get("archive_sha256") != config["sha256"]
        or manifest.get("compiler_target") != config["target"]
    ):
        raise PrebuiltCompilerError("installed prebuilt GCC release differs")
    paths = old_gcc.compiler_paths(root, lock)
    for name in ("gcc", "cc1", "cpp", "manifest"):
        path = paths[name]
        if not path.is_file():
            raise PrebuiltCompilerError(f"missing prebuilt compiler file: {path}")
    machine = old_gcc.run(
        root,
        [str(paths["gcc"]), "-dumpmachine"],
        capture_output=True,
    )
    version = old_gcc.run(
        root,
        [str(paths["gcc"]), "-dumpversion"],
        capture_output=True,
    )
    if machine != config["target"] or version != config.get(
        "dumpversion", lock["version"]
    ):
        raise PrebuiltCompilerError(
            f"prebuilt compiler identifies as {machine} {version}"
        )
    cc1 = Path(
        old_gcc.run(
            root,
            [str(paths["gcc"]), "-print-prog-name=cc1"],
            capture_output=True,
        )
    ).resolve(strict=True)
    if cc1 != paths["cc1"].resolve(strict=True):
        raise PrebuiltCompilerError("prebuilt GCC resolves cc1 outside its prefix")
    for name in ("gcc", "cc1", "cpp"):
        if sha256(paths[name]) != manifest.get("sha256", {}).get(name):
            raise PrebuiltCompilerError(f"installed prebuilt {name} differs")
    real = prefix / real_gcc_path(lock)
    if not real.is_file() or sha256(real) != config["files"]["gcc"]:
        raise PrebuiltCompilerError("installed prebuilt GCC driver differs")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Install a pinned decompals old-gcc binary release."
    )
    parser.add_argument(
        "--config",
        default=CONFIG_PATH,
        help="release config to install (default: the GCC 2.8.1 release)",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="validate an installed compiler without downloading or building",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        config = load_config(root, args.config)
        lock = load_lock(root, config)
        if args.check:
            check_prebuilt(root, lock, config)
        else:
            install(root, lock, config)
            check_prebuilt(root, lock, config)
    except (
        PrebuiltCompilerError,
        old_gcc.CompilerError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        json.JSONDecodeError,
        tarfile.TarError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print(
        f"{config['target']} GCC {lock['version']} prebuilt compiler: OK"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
