#!/usr/bin/env python3
"""Content-addressed store for overlay C objects.

Thousands of overlay modules link the same few C units, and the seven regions
share most of them. A unit's object depends only on its preprocessed source,
its compiler profile and the compile pipeline, so it is compiled once into
`tmp/overlay-object-cache` and copied into every module that links it.
Objects come from tracked sources only; the store never holds retail bytes.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import shutil
import subprocess
import tempfile
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
from typing import Any, Iterable

from build_baseline import (
    BuildError,
    compile_c,
    load_compiler_profiles,
    tool,
)
from overlay_sources import OverlaySourceError, c_segments
from workspace import (
    WorkspaceError,
    local_environment,
    require_workspace_root,
    resolve_within,
)

STORE_DIRECTORY = "tmp/overlay-object-cache"
FORMAT = "overlay-objects-v2"
# The scripts and vendored tools a compiled object depends on besides its
# source and toolchain: build_baseline.compile_c and its imports, plus the
# MASPSX entry point it executes and the package that entry point imports.
PIPELINE_FILES = (
    "tools/project/build_baseline.py",
    "tools/project/overlay_objects.py",
    "tools/project/workspace.py",
    "tools/vendor/maspsx/maspsx.py",
)
PIPELINE_DIRECTORIES = ("tools/vendor/maspsx/maspsx",)
WRAPPER_EXEC = re.compile(rb"^exec (/\S+) \"\$@\"$", re.MULTILINE)


class OverlayObjectError(RuntimeError):
    pass


def _hash_file(digest: Any, path: Path) -> None:
    digest.update(str(len(path.name)).encode() + b":" + path.name.encode())
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)


def executed_files(path: Path) -> list[Path]:
    """Return a tool and, for a system-binutils shell wrapper, the binary it runs."""
    path = path.resolve()
    with path.open("rb") as handle:
        head = handle.read(4096)
    if not head.startswith(b"#!"):
        return [path]
    return [path, *(Path(m.decode()).resolve() for m in WRAPPER_EXEC.findall(head))]


def _hash_tree(digest: Any, directory: Path) -> None:
    for path in sorted(p for p in directory.rglob("*") if p.is_file()):
        if "__pycache__" in path.parts:
            continue
        digest.update(str(path.relative_to(directory)).encode() + b"\0")
        _hash_file(digest, path)


class ObjectStore:
    def __init__(self, root: Path, directory: str = STORE_DIRECTORY) -> None:
        self.root = root
        self.directory = resolve_within(root, directory)
        self.profiles = load_compiler_profiles(root)
        self._pipeline: str | None = None
        self._profile_hashes: dict[str, str] = {}
        self._keys: dict[tuple[str, str], str] = {}

    def pipeline_fingerprint(self) -> str:
        if self._pipeline is None:
            digest = hashlib.sha256(FORMAT.encode())
            for relative in PIPELINE_FILES:
                _hash_file(digest, resolve_within(self.root, relative, must_exist=True))
            for relative in PIPELINE_DIRECTORIES:
                _hash_tree(digest, resolve_within(self.root, relative, must_exist=True))
            for name in ("as", "objcopy"):
                for path in executed_files(tool(self.root, name)):
                    _hash_file(digest, path)
            digest.update(
                f"system={os.environ.get('USE_SYSTEM_MIPS_BINUTILS') == '1'}".encode()
            )
            digest.update(sys.version.encode())
            self._pipeline = digest.hexdigest()
        return self._pipeline

    def profile_fingerprint(self, name: str) -> str:
        cached = self._profile_hashes.get(name)
        if cached is not None:
            return cached
        profile = self.profiles.get(name)
        if profile is None:
            raise OverlayObjectError(f"unknown compiler profile: {name}")
        digest = hashlib.sha256(json.dumps(profile, sort_keys=True).encode())
        compiler = resolve_within(self.root, str(profile["compiler"]), must_exist=True)
        # The driver runs the cpp/cc1 programs installed beside it.
        _hash_tree(digest, compiler.parent.parent)
        assembly_filter = profile.get("assembly_filter")
        if assembly_filter is not None:
            _hash_file(
                digest, resolve_within(self.root, str(assembly_filter), must_exist=True)
            )
        self._profile_hashes[name] = digest.hexdigest()
        return self._profile_hashes[name]

    def _preprocess(self, source: Path, profile: dict[str, Any]) -> bytes:
        compiler = resolve_within(self.root, str(profile["compiler"]), must_exist=True)
        environment = os.environ.copy()
        environment.update(local_environment(self.root))
        result = subprocess.run(
            [str(compiler), "-E", *profile["compiler_flags"], str(source)],
            cwd=self.root,
            env=environment,
            capture_output=True,
            check=False,
        )
        if result.returncode != 0:
            raise BuildError(
                f"preprocessor failed with exit code {result.returncode}: "
                f"{source.relative_to(self.root)}\n"
                + result.stderr.decode(errors="replace")
            )
        return result.stdout

    def key(self, segment: dict[str, str]) -> str:
        identity = (segment["source"], segment["profile"])
        cached = self._keys.get(identity)
        if cached is not None:
            return cached
        profile_hash = self.profile_fingerprint(segment["profile"])
        source = resolve_within(self.root, segment["source"], must_exist=True)
        digest = hashlib.sha256(self.pipeline_fingerprint().encode())
        digest.update(profile_hash.encode())
        # compile_c names the source by absolute path, which GCC records.
        digest.update(str(source).encode() + b"\0")
        digest.update(self._preprocess(source, self.profiles[segment["profile"]]))
        self._keys[identity] = digest.hexdigest()
        return self._keys[identity]

    def path(self, key: str) -> Path:
        return self.directory / "objects" / key[:2] / f"{key}.o"

    def _compile(self, segment: dict[str, str], key: str) -> None:
        # Concurrent builds may compile the same unit; give each its own work
        # directory so one never reads or replaces another's partial output.
        parent = self.directory / "work"
        parent.mkdir(parents=True, exist_ok=True)
        work = Path(tempfile.mkdtemp(prefix=f"{key}.", dir=parent))
        relative = work.relative_to(self.root)
        output = compile_c(
            self.root,
            tool(self.root, "as"),
            segment,
            self.profiles,
            object_directory=str(relative),
            asm_directory=str(relative / "asm"),
        )
        destination = self.path(key)
        destination.parent.mkdir(parents=True, exist_ok=True)
        os.replace(output, destination)
        shutil.rmtree(work, ignore_errors=True)

    def keys(self, segments: Iterable[dict[str, str]], jobs: int) -> list[dict[str, str]]:
        """Key every distinct unit in parallel; return the distinct units."""
        distinct: dict[tuple[str, str], dict[str, str]] = {}
        for segment in segments:
            distinct.setdefault((segment["source"], segment["profile"]), segment)
        units = list(distinct.values())
        with ThreadPoolExecutor(max_workers=max(1, jobs)) as pool:
            list(pool.map(self.key, units))
        return units

    def ensure(self, segments: Iterable[dict[str, str]], jobs: int) -> dict[str, int]:
        """Key every distinct unit and compile the ones the store lacks."""
        units = self.keys(segments, jobs)
        missing = [
            (unit, self.key(unit)) for unit in units
            if not self.path(self.key(unit)).is_file()
        ]
        with ThreadPoolExecutor(max_workers=max(1, jobs)) as pool:
            for future in [pool.submit(self._compile, *item) for item in missing]:
                future.result()
        return {"units": len(units), "compiled": len(missing)}

    def materialize(self, segment: dict[str, str], destination: Path) -> Path:
        cached = self.path(self.key(segment))
        if not cached.is_file():
            raise OverlayObjectError(
                f"object store lacks {segment['source']} ({segment['profile']})"
            )
        destination.parent.mkdir(parents=True, exist_ok=True)
        staging = destination.with_name(f".{destination.name}.{os.getpid()}")
        shutil.copyfile(cached, staging)
        os.replace(staging, destination)
        return destination

    def prune(self, keep: Iterable[str]) -> int:
        wanted = {self.path(key) for key in keep}
        removed = 0
        objects = self.directory / "objects"
        if objects.is_dir():
            for path in objects.rglob("*.o"):
                if path not in wanted:
                    path.unlink()
                    removed += 1
        work = self.directory / "work"
        if work.exists():
            shutil.rmtree(work)
        return removed


def module_segments(root: Path, modules: Iterable[dict[str, Any]]) -> list[dict[str, str]]:
    segments: list[dict[str, str]] = []
    for module in modules:
        layout = resolve_within(root, str(module["layout"]), must_exist=True)
        segments.extend(c_segments(root, layout))
    return segments


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Fill or prune the shared overlay C object store."
    )
    parser.add_argument("command", choices=("prepare", "prune"))
    parser.add_argument(
        "--region", action="append", required=True,
        help="overlay region whose units to keep (repeatable)",
    )
    parser.add_argument(
        "--github-output", action="store_true",
        help="print the kept-object digest as a GitHub Actions output",
    )
    return parser.parse_args()


def main() -> int:
    from overlay_build import job_count, load_modules

    args = parse_args()
    try:
        root = require_workspace_root()
        os.environ.update(local_environment(root))
        store = ObjectStore(root)
        segments = module_segments(
            root, [m for region in args.region for m in load_modules(root, region)]
        )
        if args.command == "prepare":
            counts = store.ensure(segments, job_count())
            print(
                f"overlay objects: {counts['units']} units, "
                f"compiled {counts['compiled']}"
            )
            return 0
        keys = sorted({store.key(unit) for unit in store.keys(segments, job_count())})
        removed = store.prune(keys)
        digest = hashlib.sha256("\n".join(keys).encode()).hexdigest()
        print(f"overlay objects: kept {len(keys)}, removed {removed}", file=sys.stderr)
        if args.github_output:
            print(f"digest={digest}")
        else:
            print(digest)
    except (
        OverlayObjectError, OverlaySourceError, BuildError, WorkspaceError,
        OSError, KeyError, TypeError, ValueError, json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
