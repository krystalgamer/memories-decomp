#!/usr/bin/env python3

from __future__ import annotations

import argparse
from contextlib import contextmanager
import json
import os
from pathlib import Path
import re
import statistics
import subprocess
import sys
import time
from typing import Any, Iterator

import build_baseline
from build_incremental import sha256
from generate_build_config import write_json
from split_incremental import CACHE_PATH, C_FUNC_RE
from workspace import WorkspaceError, local_environment, require_workspace_root, resolve_within


class IncrementalCheckError(RuntimeError):
    pass


REPORT_DIRECTORY = "tmp/incremental-check"
DRIVER = "tools/project/build_incremental.py"
MATCH = "tools/project/match.py"


class Checks:
    def __init__(self, root: Path) -> None:
        self.root = root
        self.directory = resolve_within(root, REPORT_DIRECTORY)
        self.directory.mkdir(parents=True, exist_ok=True)
        self.environment = os.environ.copy()
        self.environment.update(local_environment(root))
        self.environment["MAKEFLAGS"] = "-j2"
        self.environment["PYTHONPATH"] = os.pathsep.join(filter(None, (
            str(root / "tools/project"), self.environment.get("PYTHONPATH"),
        )))
        self.results: dict[str, Any] = {"schema": 1, "commands": []}

    def run(self, name: str, commands: list[list[str]]) -> dict[str, Any]:
        path = self.directory / f"{name}.log"
        timings = []
        with path.open("w", encoding="utf-8") as log:
            for command in commands:
                log.write(f"$ {' '.join(command)}\n")
                log.flush()
                started = time.perf_counter()
                result = subprocess.run(
                    command, cwd=self.root, env=self.environment,
                    stdout=log, stderr=subprocess.STDOUT, check=False,
                )
                timings.append(time.perf_counter() - started)
                if result.returncode:
                    raise IncrementalCheckError(
                        f"{name} failed ({result.returncode}); see {path.relative_to(self.root)}"
                    )
        entry = {"name": name, "seconds": sum(timings), "command_seconds": timings}
        self.results["commands"].append(entry)
        print(f"incremental check: {name}: {entry['seconds']:.3f}s", flush=True)
        return entry

    def match(self, name: str, split: str) -> dict[str, Any]:
        entry = self.run(name, [["make", "match-incremental"]])
        output = (self.directory / f"{name}.log").read_text(encoding="utf-8")
        if f"incremental split: {split}" not in output:
            raise IncrementalCheckError(f"{name}: expected split {split}; see {name}.log")
        entry["objects"] = object_counts(output)
        return entry


def object_counts(output: str) -> dict[str, int]:
    match = re.search(
        r"^incremental build: rebuilt=(\d+) reused=(\d+) retained=(\d+) materialized=(\d+) ",
        output, re.MULTILINE,
    )
    if match is None:
        raise IncrementalCheckError("missing incremental object counters")
    counts = dict(zip(
        ("rebuilt", "reused", "retained", "materialized"), map(int, match.groups()),
    ))
    if counts["reused"] != counts["retained"] + counts["materialized"]:
        raise IncrementalCheckError("inconsistent incremental object counters")
    return counts


def object_snapshot(root: Path) -> dict[str, tuple[int, int, int]]:
    # Metadata is only a measurement of object replacement. Production cache
    # validity is checked independently using file contents.
    result = {}
    directory = resolve_within(root, build_baseline.SPLAT_BUILD_DIRECTORY, must_exist=True)
    for path in sorted(directory.rglob("*.o")):
        stat = path.stat()
        result[path.relative_to(root).as_posix()] = (stat.st_ino, stat.st_mtime_ns, stat.st_size)
    if not result:
        raise IncrementalCheckError("the matching build has no installed objects")
    return result


def changed_objects(
    before: dict[str, tuple[int, int, int]], after: dict[str, tuple[int, int, int]],
) -> set[str]:
    if before.keys() != after.keys():
        raise IncrementalCheckError("the installed object set changed unexpectedly")
    return {name for name in before if before[name] != after[name]}


@contextmanager
def edited_input(root: Path, name: str, replacement: bytes) -> Iterator[None]:
    path = resolve_within(root, name, must_exist=True)
    original = path.read_bytes()
    if replacement == original:
        raise IncrementalCheckError(f"probe does not change {name}")
    backup = resolve_within(root, f"{REPORT_DIRECTORY}/backups/{name}")
    backup.parent.mkdir(parents=True, exist_ok=True)
    backup.write_bytes(original)
    stat = path.stat()
    try:
        path.write_bytes(replacement)
        os.utime(path, ns=(stat.st_atime_ns, stat.st_mtime_ns))
        yield
    finally:
        if path.read_bytes() != replacement:
            raise IncrementalCheckError(
                f"{name} changed concurrently; original preserved in {backup.relative_to(root)}"
            )
        path.write_bytes(original)
        os.utime(path, ns=(stat.st_atime_ns, stat.st_mtime_ns))
        backup.unlink()


def source_probe(root: Path) -> tuple[str, bytes]:
    mapping = json.loads(resolve_within(
        root, "config/slus_01411/matching_c.json", must_exist=True
    ).read_text(encoding="utf-8"))
    sizes: dict[str, int] = {}
    for function in mapping["functions"]:
        name = function["source"]
        value = function["size"]
        sizes[name] = sizes.get(name, 0) + (int(value, 0) if isinstance(value, str) else value)
    for name in sorted(sizes, key=lambda name: (sizes[name], name)):
        text = resolve_within(root, name, must_exist=True).read_text(encoding="utf-8")
        match = C_FUNC_RE.search(text)
        if match is not None:
            position = match.end()
            changed = text[:position] + "\n/* incremental body probe */" + text[position:]
            return name, changed.encode("utf-8")
    raise IncrementalCheckError("no matching C source is suitable for the edit probe")


def baseline_driver(root: Path, revision: str | None) -> tuple[str, str | None]:
    if revision is None:
        return DRIVER, None
    commit = subprocess.run(
        ["git", "rev-parse", "--verify", "--end-of-options", f"{revision}^{{commit}}"],
        cwd=root, stdout=subprocess.PIPE, check=True, text=True,
    ).stdout.strip()
    source = subprocess.run(
        ["git", "show", f"{commit}:{DRIVER}"], cwd=root, stdout=subprocess.PIPE, check=True,
    ).stdout
    path = resolve_within(root, f"{REPORT_DIRECTORY}/baseline_driver.py")
    path.write_bytes(source)
    return path.relative_to(root).as_posix(), commit


def exercise(checks: Checks, runs: int, revision: str | None) -> None:
    root = checks.root
    checks.run("workspace", [["make", "workspace"]])
    checks.run("initial-match", [[sys.executable, MATCH]])
    source, changed_source = source_probe(root)
    header = "src/game/input.h"
    symbols = "config/slus_01411/symbols.txt"
    config = "config/slus_01411/split.yaml"
    checks.results["probe_inputs"] = [source, header, symbols, config]
    checks.results["optimized_driver_sha256"] = sha256(root / DRIVER)
    checks.run("unchanged-probe-inputs", [
        ["git", "diff", "--exit-code", "HEAD", "--", source, header, symbols, config],
    ])
    original_driver, commit = baseline_driver(root, revision)
    checks.results["baseline"] = {
        "revision": commit,
        "driver_sha256": sha256(resolve_within(root, original_driver, must_exist=True)),
        "description": "unconditional make split followed by the baseline incremental driver",
    }
    checks.run("baseline-seed", [[sys.executable, original_driver, "--seed-existing"]])
    baseline = [
        checks.run(f"baseline-{index + 1}", [
            ["make", "split"], [sys.executable, original_driver], [sys.executable, MATCH],
        ])["seconds"]
        for index in range(runs)
    ]

    checks.run("optimized-seed", [[sys.executable, DRIVER, "--seed-existing"]])
    resolve_within(root, CACHE_PATH).unlink(missing_ok=True)
    checks.match("optimized-cold-split", "regenerating")
    optimized = []
    for index in range(runs):
        before = object_snapshot(root)
        entry = checks.match(f"optimized-warm-{index + 1}", "reused")
        changed = changed_objects(before, object_snapshot(root))
        if changed or entry["objects"] != {
            "rebuilt": 0, "reused": len(before), "retained": len(before), "materialized": 0,
        }:
            raise IncrementalCheckError("warm build did not retain every unchanged object")
        entry["retained_objects"] = len(before)
        optimized.append(entry["seconds"])

    old_median = statistics.median(baseline)
    new_median = statistics.median(optimized)
    checks.results["performance"] = {
        "runs": runs,
        "baseline_median_seconds": old_median,
        "optimized_median_seconds": new_median,
        "speedup": old_median / new_median,
        "reduction_percent": 100 * (1 - new_median / old_median),
    }

    before = object_snapshot(root)
    with edited_input(root, source, changed_source):
        entry = checks.match("source-body-edit", "reused")
        changed = changed_objects(before, object_snapshot(root))
        expected = build_baseline.splat_object(source)
        if changed != {expected}:
            raise IncrementalCheckError(f"source edit replaced unexpected objects: {sorted(changed)}")
        if entry["objects"]["rebuilt"] != 1 or entry["objects"]["materialized"] != 0:
            raise IncrementalCheckError("source edit did not rebuild exactly its compilation unit")
        entry["replaced_objects"] = len(changed)
    checks.match("source-restored", "reused")

    before = object_snapshot(root)
    with edited_input(root, header, (root / header).read_bytes() + b"\n/* header probe */\n"):
        entry = checks.match("header-edit", "reused")
        changed = changed_objects(before, object_snapshot(root))
        if not 0 < len(changed) < len(before):
            raise IncrementalCheckError("header edit did not rebuild just its dependent subset")
        if entry["objects"]["rebuilt"] != len(changed) or entry["objects"]["materialized"] != 0:
            raise IncrementalCheckError("header edit replaced objects outside its rebuild set")
        entry["replaced_objects"] = len(changed)
    checks.match("header-restored", "reused")

    for label, name, comment in (
        ("symbols", symbols, b"\n// incremental split probe\n"),
        ("configuration", config, b"\n# incremental split probe\n"),
    ):
        with edited_input(root, name, (root / name).read_bytes() + comment):
            checks.match(f"{label}-edit", "regenerating")
        checks.match(f"{label}-restored", "regenerating")

    damaged = (
        "tmp/splat/asm/header.s",
        "tmp/splat/slus_01411.ld",
        "tmp/splat/assets/reserved_zero.bin",
    )
    hashes = {name: sha256(resolve_within(root, name, must_exist=True)) for name in damaged}
    (root / damaged[0]).write_bytes(b"corrupt generated assembly\n")
    for name in damaged[1:]:
        (root / name).unlink()
    checks.match("generated-output-recovery", "regenerating")
    if any(sha256(root / name) != digest for name, digest in hashes.items()):
        raise IncrementalCheckError("generated output was not restored exactly")

    obj = resolve_within(root, build_baseline.splat_object(source), must_exist=True)
    digest = sha256(obj)
    obj.write_bytes(b"corrupt installed object\n")
    entry = checks.match("installed-object-recovery", "reused")
    if entry["objects"]["rebuilt"] != 0 or entry["objects"]["materialized"] != 1:
        raise IncrementalCheckError("installed-object damage did not restore exactly one cache hit")
    if sha256(obj) != digest:
        raise IncrementalCheckError("the installed object was not restored exactly")

    checks.run("restored-probe-inputs", [
        ["git", "diff", "--exit-code", "HEAD", "--", source, header, symbols, config],
    ])
    checks.run("final-clean", [["make", "clean"]])
    checks.run("final-clean-match", [["make", "match"]])
    if new_median >= old_median:
        raise IncrementalCheckError("warm incremental builds did not improve the baseline median")
    if original_driver != DRIVER:
        resolve_within(root, original_driver, must_exist=True).unlink()
    checks.results["complete"] = True
    print(
        f"incremental performance: {old_median:.3f}s -> {new_median:.3f}s "
        f"({old_median / new_median:.2f}x faster, {100 * (1 - new_median / old_median):.1f}% less time)"
    )


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Measure incremental builds and exercise invalidation after a clean match."
    )
    parser.add_argument("--baseline-ref", help="Git revision supplying the pre-change incremental driver")
    parser.add_argument("--runs", type=int, default=3, help="no-change samples per implementation (default: 3)")
    args = parser.parse_args()
    if args.runs < 1:
        parser.error("--runs must be positive")
    checks = None
    try:
        checks = Checks(require_workspace_root())
        exercise(checks, args.runs, args.baseline_ref)
    except (
        IncrementalCheckError, WorkspaceError, OSError, ValueError,
        subprocess.CalledProcessError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    finally:
        if checks is not None:
            write_json(checks.directory / "report.json", checks.results)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
