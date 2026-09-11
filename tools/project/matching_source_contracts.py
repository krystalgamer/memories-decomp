#!/usr/bin/env python3
"""Enforce the source-quality contract for every matching C function."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Any

from integrate_verified_match import (
    COMMENT_PATTERN,
    REGISTER_AGGREGATE_PIN_PATTERN,
    REGISTER_PIN_PATTERN,
    load_tracked_symbol_names,
    uses_asm_extension,
)
from workspace import WorkspaceError, require_workspace_root, resolve_within


class MatchingSourceContractError(RuntimeError):
    pass


G_FLAG = re.compile(r"^-G(?P<value>\d+)$")


def profile_g_value(flags: Any, description: str) -> int:
    if not isinstance(flags, list) or not all(isinstance(flag, str) for flag in flags):
        raise MatchingSourceContractError(f"{description}: expected a string list")
    values: list[int] = []
    for index, flag in enumerate(flags):
        match = G_FLAG.fullmatch(flag)
        if match is not None:
            values.append(int(match.group("value")))
        elif flag == "-G":
            if index + 1 >= len(flags) or not flags[index + 1].isdigit():
                raise MatchingSourceContractError(
                    f"{description}: -G must be followed by a non-negative integer"
                )
            values.append(int(flags[index + 1]))
    if len(values) != 1:
        raise MatchingSourceContractError(
            f"{description}: expected exactly one -G value, found {len(values)}"
        )
    return values[0]


def source_violations(source: str, tracked_symbol_names: set[str]) -> list[str]:
    text = COMMENT_PATTERN.sub("", source)
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
                compiler_g = profile_g_value(
                    profile.get("compiler_flags"),
                    f"{profile_name} compiler_flags",
                )
                maspsx_g = profile_g_value(
                    profile.get("maspsx_flags"),
                    f"{profile_name} maspsx_flags",
                )
            except MatchingSourceContractError as error:
                problems.append(f"{label}: {error}")
            else:
                if compiler_g != maspsx_g:
                    problems.append(
                        f"{label}: profile {profile_name} mixes "
                        f"compiler -G{compiler_g} with MASPSX -G{maspsx_g}"
                    )

            source_key = (source_name, profile_name)
            if source_key in checked_sources:
                continue
            checked_sources.add(source_key)
            try:
                source_path = resolve_within(root, source_name, must_exist=True)
                source = source_path.read_text(encoding="utf-8")
            except (WorkspaceError, OSError, UnicodeError) as error:
                problems.append(f"{label}: {error}")
                continue
            for violation in source_violations(source, tracked_symbol_names):
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
    try:
        root = require_workspace_root()
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
