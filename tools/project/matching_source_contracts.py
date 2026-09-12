#!/usr/bin/env python3
"""Enforce the source-quality contract for every matching C function."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

from integrate_verified_match import (
    IntegrationError,
    REGISTER_AGGREGATE_PIN_PATTERN,
    REGISTER_PIN_PATTERN,
    load_tracked_symbol_names,
    preprocess_source,
    profile_g_value,
    strip_c_comments,
    uses_asm_extension,
    validate_effective_profile,
)
from workspace import WorkspaceError, resolve_within


class MatchingSourceContractError(RuntimeError):
    pass


def source_violations(source: str, tracked_symbol_names: set[str]) -> list[str]:
    text = strip_c_comments(source)
    violations: list[str] = []
    if (
        REGISTER_AGGREGATE_PIN_PATTERN.search(text) is not None
        or REGISTER_PIN_PATTERN.search(text) is not None
    ):
        violations.append("contains a hard-register variable")
    if uses_asm_extension(
        source,
        allow_register_pins=True,
        allow_symbol_aliases=True,
        tracked_symbol_names=tracked_symbol_names,
    ):
        violations.append(
            "contains statement-level assembly or an untracked assembler alias"
        )
    return violations


def load_object(path: Path, key: str) -> dict[str, Any]:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise MatchingSourceContractError(f"{path}: {error}") from error
    if not isinstance(value, dict):
        raise MatchingSourceContractError(f"{path}: expected a JSON object")
    child = value.get(key)
    if not isinstance(child, dict):
        raise MatchingSourceContractError(f"{path}: missing {key} object")
    return child


def manifest_paths(root: Path) -> list[Path]:
    config = resolve_within(root, "config/slus_01411", must_exist=True)
    paths = [config / "matching_c.json"]
    paths.extend(sorted((config / "overlays").glob("*_matching_c.json")))
    return paths


def audit(root: Path) -> list[str]:
    profiles_path = resolve_within(
        root, "config/slus_01411/compiler_profiles.json", must_exist=True
    )
    profiles = load_object(profiles_path, "profiles")
    tracked_symbol_names = load_tracked_symbol_names(root)
    problems: list[str] = []
    checked_sources: set[tuple[str, str]] = set()
    function_count = 0

    for manifest in manifest_paths(root):
        try:
            value = json.loads(manifest.read_text(encoding="utf-8"))
        except (OSError, UnicodeError, json.JSONDecodeError) as error:
            raise MatchingSourceContractError(f"{manifest}: {error}") from error
        entries = value.get("functions") if isinstance(value, dict) else None
        if not isinstance(entries, list):
            raise MatchingSourceContractError(f"{manifest}: missing functions list")
        for entry in entries:
            function_count += 1
            if not isinstance(entry, dict):
                problems.append(f"{manifest.relative_to(root)}: non-object function entry")
                continue
            address = entry.get("address", "<unknown>")
            source_name = entry.get("source")
            profile_name = entry.get("profile")
            label = f"{manifest.relative_to(root)}:{address}"
            if not isinstance(source_name, str) or not isinstance(profile_name, str):
                problems.append(f"{label}: source and profile must be strings")
                continue
            profile = profiles.get(profile_name)
            if not isinstance(profile, dict):
                problems.append(f"{label}: unknown profile {profile_name}")
                continue
            try:
                validate_effective_profile(profile, profile_name)
            except IntegrationError as error:
                problems.append(f"{label}: {error}")

            source_key = (source_name, profile_name)
            if source_key in checked_sources:
                continue
            checked_sources.add(source_key)
            try:
                source_path = resolve_within(root, source_name, must_exist=True)
                source = source_path.read_text(encoding="utf-8")
                preprocessed = preprocess_source(root, source_path, profile)
            except (IntegrationError, WorkspaceError, OSError, UnicodeError) as error:
                problems.append(f"{label}: {error}")
                continue
            violations = source_violations(source, tracked_symbol_names)
            for violation in source_violations(preprocessed, tracked_symbol_names):
                if violation not in violations:
                    violations.append(violation)
            for violation in violations:
                problems.append(f"{source_name}: {violation}")

    if not problems:
        print(
            "matching source contracts: OK "
            f"({function_count} functions, {len(checked_sources)} source/profile pairs)"
        )
    return problems


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Check matching C for pins, inline assembly, and mixed -G profiles."
    )
    return parser.parse_args()


def main() -> int:
    parse_args()
    root = Path(__file__).resolve().parents[2]
    if Path.cwd().resolve() != root:
        print(f"error: run from repository root {root}", file=sys.stderr)
        return 1
    try:
        problems = audit(root)
    except (MatchingSourceContractError, WorkspaceError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    if problems:
        for problem in problems:
            print(f"error: {problem}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
