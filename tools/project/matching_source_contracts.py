#!/usr/bin/env python3
"""Enforce the source-quality contract for every matching C function."""

from __future__ import annotations

import argparse
import csv
import json
import re
import sys
from pathlib import Path
from typing import Any

from integrate_verified_match import (
    contains_register_pin,
    IntegrationError,
    load_tracked_symbol_names,
    preprocess_source,
    profile_g_value,
    strip_c_comments,
    uses_asm_extension,
    uses_disallowed_psyq_inline_asm,
    validate_effective_profile,
)
from workspace import WorkspaceError, resolve_within


class MatchingSourceContractError(RuntimeError):
    pass


def source_violations(
    source: str,
    tracked_symbol_names: set[str],
    *,
    allow_psyq_inline_macros: bool = False,
    psyq_inline_macro: str = "rtps",
    allow_register_pins: bool = False,
) -> list[str]:
    text = strip_c_comments(source)
    violations: list[str] = []
    if not allow_register_pins and contains_register_pin(text):
        violations.append("contains a hard-register variable")
    if (
        uses_disallowed_psyq_inline_asm(
            source,
            macro_family=psyq_inline_macro,
            allow_register_pins=True,
            allow_symbol_aliases=True,
            tracked_symbol_names=tracked_symbol_names,
        )
        if allow_psyq_inline_macros
        else uses_asm_extension(
            source,
            allow_register_pins=True,
            allow_symbol_aliases=True,
            tracked_symbol_names=tracked_symbol_names,
        )
    ):
        violations.append(
            "contains statement-level assembly or an untracked assembler alias"
        )
    return violations


INLINE_C = "src/psyq/inline_c.h"
RESIDENT_MANIFEST = "config/slus_01411/matching_c.json"
STRING_LITERAL = re.compile(r'"(?:[^"\\]|\\.)*"')


def asm_statements(text: str) -> list[str]:
    """Every __asm__ statement's parenthesised body (comments removed)."""
    text = strip_c_comments(text)
    bodies = []
    for match in re.finditer(r"\b__asm__\b(?:\s+(?:volatile|__volatile__))?\s*\(", text):
        depth, index = 1, match.end()
        while depth and index < len(text):
            char = text[index]
            if char == '"':
                index = STRING_LITERAL.match(text, index).end()
                continue
            depth += {"(": 1, ")": -1}.get(char, 0)
            index += 1
        bodies.append(text[match.end() : index - 1])
    return bodies


def asm_shape(body: str) -> tuple[str, ...]:
    """An asm statement with its C operand expressions removed: the template,
    then each section's constraint and clobber strings."""
    sections: list[list[str]] = [[]]
    depth, index = 0, 0
    while index < len(body):
        char = body[index]
        if char == '"':
            end = STRING_LITERAL.match(body, index).end()
            if depth == 0:
                sections[-1].append(re.sub(r"\s+", " ", body[index + 1 : end - 1]))
            index = end
            continue
        if char == ":" and depth == 0:
            sections.append([])
        depth += {"(": 1, ")": -1}.get(char, 0)
        index += 1
    template = "".join(sections[0])
    return (template, *("|".join(section) for section in sections[1:]))


def inline_c_shapes(root: Path) -> set[tuple[str, ...]]:
    text = resolve_within(root, INLINE_C, must_exist=True).read_text(encoding="utf-8")
    return {asm_shape(body) for body in asm_statements(text.replace("\\\n", " "))}


def massaged_unit_violations(
    root: Path,
    source_name: str,
    source: str,
    preprocessed: str,
    entries: list[tuple[str, int, int]],
) -> list[str]:
    """A massaged unit is plain C: no register pins and only inline_c.h's
    own asm statements. Its edit directory holds exactly one valid edit file
    per manifest function."""
    import asm_massage
    import asm_massage_rules

    violations = []
    allowed = inline_c_shapes(root)
    for body in asm_statements(preprocessed):
        if asm_shape(body) not in allowed:
            violations.append(
                "massaged unit uses asm that is not a stock inline_c.h macro: "
                + " ".join(body.split())[:80]
            )
    directory = asm_massage.edit_directory(resolve_within(root, source_name, must_exist=True))
    try:
        edits = asm_massage.read_edits(directory)
    except asm_massage.MassageError as error:
        return violations + [str(error)]
    names = {name for name, _address, _size in entries}
    for name in sorted(set(edits) - names):
        violations.append(f"edit file for {name} has no manifest function")
    for name, address, size in entries:
        edit = edits.get(name)
        if edit is None:
            violations.append(f"{name} has no edit file in {directory.relative_to(root)}")
            continue
        if edit.settings["source"] != source_name:
            violations.append(f"{edit.path.relative_to(root)}: source is not {source_name}")
        if (int(edit.settings["address"], 16), int(edit.settings["size"], 16)) != (address, size):
            violations.append(f"{edit.path.relative_to(root)}: address/size disagree with the inventory")
        unknown = [rule for rule in edit.settings["rules"].split() if rule not in asm_massage_rules.RULES]
        if unknown:
            violations.append(f"{edit.path.relative_to(root)}: unknown rules " + ", ".join(unknown))
    return violations


def inventory_names(root: Path) -> dict[int, str]:
    path = resolve_within(root, "config/slus_01411/functions.csv", must_exist=True)
    with path.open(encoding="utf-8", newline="") as handle:
        return {int(row["address"], 16): row["name"] for row in csv.DictReader(handle)}


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
    names = inventory_names(root)
    massaged: dict[str, list[tuple[str, int, int]]] = {}
    massaged_text: dict[str, tuple[str, str]] = {}

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
            allow_psyq_inline_macros = profile.get(
                "allow_psyq_inline_macros", False
            )
            if not isinstance(allow_psyq_inline_macros, bool):
                problems.append(
                    f"{label}: allow_psyq_inline_macros must be a boolean"
                )
                continue

            asm_massage = profile.get("asm_massage") is True
            if asm_massage:
                if str(manifest.relative_to(root)) != RESIDENT_MANIFEST:
                    problems.append(f"{label}: massaged profiles are resident-only")
                    continue
                try:
                    address_value = int(str(address), 0)
                    size_value = int(str(entry.get("size")), 0)
                except ValueError:
                    problems.append(f"{label}: invalid address or size")
                    continue
                massaged.setdefault(source_name, []).append(
                    (names.get(address_value, f"{address_value:#010x}"), address_value, size_value)
                )

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
            if asm_massage:
                # Its asm is checked against inline_c.h below.
                preprocessed_violations = (
                    ["contains a hard-register variable"]
                    if contains_register_pin(strip_c_comments(preprocessed))
                    else []
                )
            else:
                preprocessed_violations = source_violations(
                    preprocessed,
                    tracked_symbol_names,
                    allow_psyq_inline_macros=allow_psyq_inline_macros,
                    psyq_inline_macro=profile.get("psyq_inline_macro", "rtps"),
                )
            for violation in preprocessed_violations:
                if violation not in violations:
                    violations.append(violation)
            if asm_massage:
                massaged_text[source_name] = (source, preprocessed)
            for violation in violations:
                problems.append(f"{source_name}: {violation}")

    for source_name, entries in sorted(massaged.items()):
        if source_name not in massaged_text:
            continue
        source, preprocessed = massaged_text[source_name]
        for violation in massaged_unit_violations(
            root, source_name, source, preprocessed, entries
        ):
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
