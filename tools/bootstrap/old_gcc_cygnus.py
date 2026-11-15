#!/usr/bin/env python3
"""Build the Cygnus GCC 2.7.2 (970404) compiler of the Sony safechk module.

The source is the cygnus-2.7.2-970404 tree published by decompals/old-gcc
(b18.tar.gz, the compiler of Psy-Q 4.1 CC1PSX SN32.3.7.0004), with the
decompals CDK build patches and two historical reverts kept under
tools/bootstrap/patches/ (see their headers and notes/overlays/safecheck2003.md).
The C parser is generated with a pinned byacc built from source.
"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import sys
import tarfile
import urllib.request
from pathlib import Path
from typing import Any

from old_gcc import CompilerError, run, sha256_file, validate_tar_members, verify_hash

from workspace import WorkspaceError, require_workspace_root, resolve_within


CONFIG_PATH = "tools/bootstrap/old_gcc_cygnus.json"
PARSERS = ("c-parse.c", "bi-parser.c", "cexp.c")
SMOKE_TESTS = (
    # A 32-bit HOST_WIDE_INT prints the constant as the target sees it.
    ("host_wide_int.c", "unsigned int f(void) { return 1u << 31; }\n", "# 0x80000000"),
    # The target is little-endian.
    ("little_endian.c", "unsigned char f(unsigned int *p) { return *p; }\n", "lbu\t$2,0($4)"),
)


def load_config(root: Path) -> dict[str, Any]:
    path = resolve_within(root, CONFIG_PATH, must_exist=True)
    with path.open("r", encoding="utf-8") as handle:
        config = json.load(handle)
    if not isinstance(config, dict) or config.get("schema") != 1:
        raise CompilerError(f"{path}: unsupported schema")
    return config


def download(root: Path, url: str, archive_path: str, expected: str) -> Path:
    archive = resolve_within(root, archive_path)
    if archive.is_file():
        verify_hash(archive, expected)
        return archive
    archive.parent.mkdir(parents=True, exist_ok=True)
    partial = archive.with_name(f"{archive.name}.part")
    partial.unlink(missing_ok=True)
    request = urllib.request.Request(
        url, headers={"User-Agent": "yugioh-decomp-compiler-bootstrap/1"}
    )
    try:
        with urllib.request.urlopen(request, timeout=60) as response:
            with partial.open("wb") as handle:
                while chunk := response.read(1024 * 1024):
                    handle.write(chunk)
        verify_hash(partial, expected)
    except (OSError, CompilerError):
        partial.unlink(missing_ok=True)
        raise
    partial.replace(archive)
    return archive


def patch_files(root: Path, config: dict[str, Any], *, fetch: bool) -> list[Path]:
    paths = []
    for patch in config["patches"]:
        if fetch and "url" in patch:
            path = download(root, patch["url"], patch["path"], patch["sha256"])
        else:
            path = resolve_within(root, patch["path"], must_exist=True)
            verify_hash(path, patch["sha256"])
        paths.append(path)
    return paths


def expected_marker(config: dict[str, Any]) -> dict[str, Any]:
    return {
        "source_sha256": config["source"]["sha256"],
        "patches": {patch["path"]: patch["sha256"] for patch in config["patches"]},
    }


def check_source(root: Path, config: dict[str, Any]) -> Path:
    source = resolve_within(root, config["source"]["destination"], must_exist=True)
    marker = source / ".project-source.json"
    if not marker.is_file() or not (source / "configure").is_file():
        raise CompilerError(f"incomplete Cygnus GCC source tree: {source}")
    with marker.open("r", encoding="utf-8") as handle:
        if json.load(handle) != expected_marker(config):
            raise CompilerError(f"{source}: source marker does not match the config")
    return source


def extract(archive_path: Path, staging: Path, root_name: str) -> Path:
    if staging.exists():
        shutil.rmtree(staging)
    staging.mkdir(parents=True)
    with tarfile.open(archive_path, "r:gz") as archive:
        validate_tar_members(archive)
        archive.extractall(staging)
    extracted = staging / root_name
    if not extracted.is_dir():
        raise CompilerError(f"{archive_path}: missing {root_name} directory")
    for path in extracted.rglob("*"):
        if path.is_file() and not path.is_symlink():
            path.chmod(path.stat().st_mode | 0o200)
    return extracted


def prepare_source(root: Path, config: dict[str, Any]) -> Path:
    destination = resolve_within(root, config["source"]["destination"])
    if destination.exists():
        return check_source(root, config)
    archive = download(
        root,
        config["source"]["url"],
        config["source"]["archive"],
        config["source"]["sha256"],
    )
    patches = patch_files(root, config, fetch=True)
    staging = resolve_within(root, "tmp/bootstrap/gcc-cygnus-source")
    extracted = extract(archive, staging, config["source"]["root"])
    for patch, path in zip(config["patches"], patches):
        run(
            root,
            ["patch", *[str(argument) for argument in patch["arguments"]], "-i", str(path)],
            cwd=extracted,
        )
    (extracted / ".project-source.json").write_text(
        json.dumps(expected_marker(config), indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.move(str(extracted), str(destination))
    shutil.rmtree(staging)
    return check_source(root, config)


def build_yacc(root: Path, config: dict[str, Any]) -> Path:
    yacc = config["yacc"]
    build = resolve_within(root, f"tmp/build/{yacc['root']}")
    binary = build / "yacc"
    if binary.is_file():
        return binary
    archive = download(root, yacc["url"], yacc["archive"], yacc["sha256"])
    staging = resolve_within(root, "tmp/bootstrap/byacc-source")
    extracted = extract(archive, staging, yacc["root"])
    if build.exists():
        shutil.rmtree(build)
    build.parent.mkdir(parents=True, exist_ok=True)
    shutil.move(str(extracted), str(build))
    shutil.rmtree(staging)
    # byacc 20140715 calls open(..., O_CREAT) without a mode, which the
    # glibc fortify headers reject.
    run(
        root,
        ["./configure", "CFLAGS=-O2 -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0"],
        cwd=build,
        capture_output=True,
    )
    run(root, ["make"], cwd=build, capture_output=True)
    if not binary.is_file():
        raise CompilerError(f"byacc build did not produce {binary}")
    return binary


def compiler_paths(root: Path, config: dict[str, Any]) -> dict[str, Path]:
    prefix = resolve_within(root, config["prefix"])
    library = prefix / "lib" / "gcc-lib" / config["target"] / config["version"]
    return {
        "prefix": prefix,
        "gcc": prefix / "bin" / f"{config['target']}-gcc",
        "real_gcc": prefix / config["real_gcc"],
        "cc1": library / "cc1",
        "cpp": library / "cpp",
        "manifest": prefix / "build-manifest.json",
    }


def wrapper_text(config: dict[str, Any]) -> str:
    return f"""#!/bin/sh
set -eu
prefix=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
exec "$prefix/{config['real_gcc']}" \\
  -B"$prefix/lib/gcc-lib/{config['target']}/{config['version']}/" "$@"
"""


def smoke_test(root: Path, build: Path) -> None:
    for name, text, expected in SMOKE_TESTS:
        source = build / name
        source.write_text(text, encoding="ascii")
        output = source.with_suffix(".s")
        run(root, [str(build / "cc1"), "-quiet", "-O2", str(source), "-o", str(output)], cwd=build)
        if expected not in output.read_text(encoding="ascii", errors="replace"):
            raise CompilerError(f"cc1 smoke test {name} did not produce {expected!r}")


def build_compiler(root: Path, config: dict[str, Any], source: Path, yacc: Path) -> None:
    paths = compiler_paths(root, config)
    if paths["prefix"].exists():
        raise CompilerError(
            f"incomplete compiler prefix exists; remove it before retrying: {paths['prefix']}"
        )
    build = resolve_within(root, config["build"])
    if build.exists():
        shutil.rmtree(build)
    shutil.copytree(source, build, symlinks=True)

    run(
        root,
        [
            "./configure",
            f"--target={config['target']}",
            "--prefix=/opt/cross",
            "--with-endian-little",
            "--with-gnu-as",
            "--disable-gprof",
            "--disable-gdb",
            "--disable-werror",
            "--host=i686-pc-linux-gnu",
            "--build=i686-pc-linux-gnu",
        ],
        cwd=build,
        capture_output=True,
    )
    make_variables = [f"CFLAGS={config['host_cflags']}", f"BISON={yacc}", "BISONFLAGS="]
    # The parsers share y.tab.c/y.tab.h, so generate them one at a time.
    for parser in PARSERS:
        run(root, ["make", parser, *make_variables], cwd=build, capture_output=True)
    jobs = max(1, os.cpu_count() or 1)
    run(
        root,
        ["make", f"-j{jobs}", "cpp", "cc1", "xgcc", *make_variables],
        cwd=build,
        capture_output=True,
    )
    smoke_test(root, build)

    staging = resolve_within(root, "tmp/bootstrap/gcc-2.7.2-cygnus-prefix")
    if staging.exists():
        shutil.rmtree(staging)
    staged = {
        name: staging / path.relative_to(paths["prefix"])
        for name, path in paths.items()
        if name != "prefix"
    }
    for path in staged.values():
        path.parent.mkdir(parents=True, exist_ok=True)
    staged["gcc"].write_text(wrapper_text(config), encoding="ascii")
    shutil.copy2(build / "xgcc", staged["real_gcc"])
    shutil.copy2(build / "cc1", staged["cc1"])
    shutil.copy2(build / "cpp", staged["cpp"])
    for name in ("gcc", "real_gcc", "cc1", "cpp"):
        staged[name].chmod(0o755)
    licenses = staging / "share" / "licenses" / "gcc-2.7.2-cygnus"
    licenses.mkdir(parents=True)
    shutil.copy2(build / "COPYING", licenses / "COPYING")
    manifest = {
        "schema": 1,
        "distribution": config["name"],
        "source_sha256": config["source"]["sha256"],
        "yacc_sha256": config["yacc"]["sha256"],
        "patches": expected_marker(config)["patches"],
        "host_cflags": config["host_cflags"],
        "target": config["target"],
        "version": config["version"],
        "sha256": {
            name: sha256_file(staged[name]) for name in ("gcc", "real_gcc", "cc1", "cpp")
        },
    }
    staged["manifest"].write_text(
        json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    paths["prefix"].parent.mkdir(parents=True, exist_ok=True)
    shutil.move(str(staging), str(paths["prefix"]))


def check_compiler(root: Path, config: dict[str, Any]) -> None:
    paths = compiler_paths(root, config)
    for name in ("gcc", "real_gcc", "cc1", "cpp", "manifest"):
        if not paths[name].is_file():
            raise CompilerError(f"missing Cygnus GCC file: {paths[name]}")
    with paths["manifest"].open("r", encoding="utf-8") as handle:
        manifest = json.load(handle)
    if (
        not isinstance(manifest, dict)
        or manifest.get("distribution") != config["name"]
        or manifest.get("source_sha256") != config["source"]["sha256"]
        or manifest.get("yacc_sha256") != config["yacc"]["sha256"]
        or manifest.get("patches") != expected_marker(config)["patches"]
        or manifest.get("host_cflags") != config["host_cflags"]
    ):
        raise CompilerError("installed Cygnus GCC was built from a different configuration")
    for name in ("gcc", "real_gcc", "cc1", "cpp"):
        if sha256_file(paths[name]) != manifest.get("sha256", {}).get(name):
            raise CompilerError(f"installed Cygnus GCC {name} differs from its manifest")
    machine = run(root, [str(paths["gcc"]), "-dumpmachine"], capture_output=True)
    version = run(root, [str(paths["gcc"]), "-dumpversion"], capture_output=True)
    if machine != config["target"] or version != config["dumpversion"]:
        raise CompilerError(f"Cygnus GCC identifies as {machine} {version}")
    cc1 = Path(
        run(root, [str(paths["gcc"]), "-print-prog-name=cc1"], capture_output=True)
    ).resolve(strict=True)
    if cc1 != paths["cc1"].resolve(strict=True):
        raise CompilerError("Cygnus GCC resolves cc1 outside its prefix")


def install(root: Path, config: dict[str, Any]) -> None:
    try:
        check_compiler(root, config)
        return
    except (CompilerError, WorkspaceError, OSError, ValueError):
        pass
    source = prepare_source(root, config)
    yacc = build_yacc(root, config)
    build_compiler(root, config, source, yacc)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "--check",
        action="store_true",
        help="verify the installed compiler without downloading or building",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        config = load_config(root)
        if args.check:
            patch_files(root, config, fetch=False)
            check_source(root, config)
        else:
            install(root, config)
        check_compiler(root, config)
    except (
        CompilerError,
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
    print(f"{config['target']} {config['dumpversion']} ({config['name']}): OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
