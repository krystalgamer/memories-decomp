#!/usr/bin/env python3
"""Reject calls that compile only through an implicit declaration.

GCC 2.8.1 accepts a call to a function with no visible declaration, treating
it as `int f()`. The build then stays byte-exact, so neither the executable
hash nor the candidate fingerprints notice when a caller loses sight of a
prototype, for example because the prototype moved to another header the
caller does not include. The declaration contracts the other checks enforce
(unmatched_contracts.py, candidate canonical contracts) are only real where
the caller actually sees them.

This compiles every matching C source (resident and overlay manifests) and
every build-integrated candidate to discarded assembly with
-Wimplicit-function-declaration under its own profile's flags, and fails on
each implicit declaration unless that exact source/function pair is recorded,
with a measured reason, in config/slus_01411/implicit_declaration_exceptions.json.
A recorded pair that no longer occurs is also an error, so the list cannot go
stale.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONFIG = Path("config/slus_01411")
PROFILES = CONFIG / "compiler_profiles.json"
EXCEPTIONS = CONFIG / "implicit_declaration_exceptions.json"
IMPLICIT = re.compile(r"implicit declaration of function `([^']+)'")
# Only flags that decide what the preprocessor and front end see.
FRONT_END_FLAG = re.compile(r"^-(?:D|U|I|G|m|O|f)")


class VisibilityError(RuntimeError):
    pass


def sources(root: Path) -> dict[str, str]:
    """Every compiled C source mapped to its profile."""
    result: dict[str, str] = {}
    manifests = [root / CONFIG / "matching_c.json"]
    manifests += sorted((root / CONFIG / "overlays").glob("*_matching_c.json"))
    for manifest in manifests:
        for entry in json.loads(manifest.read_text(encoding="utf-8"))["functions"]:
            result[entry["source"]] = entry["profile"]
    candidates = root / CONFIG / "candidates.json"
    if candidates.is_file():
        for entry in json.loads(candidates.read_text(encoding="utf-8"))["candidates"]:
            result[entry["source"]] = entry["profile"]
    return result


def compiler(root: Path, profiles: dict[str, dict[str, object]]) -> Path:
    paths = {str(profile["compiler"]) for profile in profiles.values()}
    if len(paths) != 1:
        raise VisibilityError(f"expected one compiler, profiles name {sorted(paths)}")
    path = root / paths.pop()
    if not path.is_file():
        raise VisibilityError(f"{path.relative_to(root)} is absent; run make tools")
    return path


def implicit_calls(
    root: Path,
    gcc: Path,
    source: str,
    profile: dict[str, object],
) -> set[str]:
    flags = [
        flag
        for flag in profile["compiler_flags"]  # type: ignore[index]
        if FRONT_END_FLAG.match(str(flag))
    ]
    completed = subprocess.run(
        [
            str(gcc),
            # -S stops before the assembler, which rejects -G on its own
            # command line; the output is discarded.
            "-S",
            "-o",
            os.devnull,
            "-Wimplicit-function-declaration",
            *flags,
            source,
        ],
        cwd=root,
        capture_output=True,
        text=True,
        errors="replace",
    )
    if completed.returncode != 0:
        raise VisibilityError(
            f"{source}: does not compile: " + " ".join(completed.stderr.split())[:400]
        )
    return set(IMPLICIT.findall(completed.stderr))


def load_exceptions(root: Path) -> set[tuple[str, str]]:
    path = root / EXCEPTIONS
    if not path.is_file():
        return set()
    data = json.loads(path.read_text(encoding="utf-8"))
    if data.get("schema") != 1 or not isinstance(data.get("exceptions"), list):
        raise VisibilityError(f"{EXCEPTIONS}: expected schema 1 exception list")
    pairs: set[tuple[str, str]] = set()
    for index, item in enumerate(data["exceptions"]):
        if not isinstance(item, dict) or set(item) != {"source", "function", "reason"}:
            raise VisibilityError(
                f"{EXCEPTIONS}: exception {index} needs source, function, reason"
            )
        if not all(isinstance(item[key], str) and item[key] for key in item):
            raise VisibilityError(f"{EXCEPTIONS}: exception {index} has an empty field")
        pairs.add((item["source"], item["function"]))
    return pairs


def validate(root: Path = ROOT, jobs: int | None = None) -> tuple[list[str], int]:
    profiles = json.loads((root / PROFILES).read_text(encoding="utf-8"))["profiles"]
    gcc = compiler(root, profiles)
    units = sources(root)
    with ThreadPoolExecutor(max_workers=jobs or os.cpu_count() or 1) as pool:
        found = dict(
            zip(
                units,
                pool.map(
                    lambda item: implicit_calls(root, gcc, item[0], profiles[item[1]]),
                    units.items(),
                ),
            )
        )
    allowed = load_exceptions(root)
    errors: list[str] = []
    seen: set[tuple[str, str]] = set()
    for source in sorted(found):
        for function in sorted(found[source]):
            seen.add((source, function))
            if (source, function) not in allowed:
                errors.append(
                    f"{source}: calls {function} through an implicit declaration; "
                    "include the header that declares it"
                )
    for source, function in sorted(allowed - seen):
        errors.append(
            f"{EXCEPTIONS}: stale exception {source}: {function} is no longer "
            "called implicitly"
        )
    return errors, len(units)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--jobs", type=int, default=None)
    args = parser.parse_args()
    if Path.cwd().resolve() != ROOT:
        print(f"error: run from repository root {ROOT}", file=sys.stderr)
        return 1
    try:
        errors, checked = validate(jobs=args.jobs)
    except (VisibilityError, OSError, KeyError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    for error in errors:
        print(f"error: {error}", file=sys.stderr)
    if errors:
        return 1
    print(f"declaration visibility: OK ({checked} sources)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
