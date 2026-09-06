#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import re
import sys
from pathlib import Path

import record_attempt
import record_external_attempt
from workspace import WorkspaceError, require_workspace_root, resolve_within


EXPECTED_FIELDS = (
    "address",
    "size",
    "attempt_count",
    "profile",
    "source",
    "exact",
    "bytes_match",
    "relocations_match",
    "actual_size",
    "first_difference",
    "error",
)
PROFILE_PATTERN = re.compile(r"profile=(gcc_[A-Za-z0-9_]+)")
SOURCE_PATTERN = re.compile(r"(?:^|\s)source=([^\s]+)")


class ImportError(RuntimeError):
    pass


def read_results(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if tuple(reader.fieldnames or ()) != EXPECTED_FIELDS:
            raise ImportError(f"{path}: unexpected result fields")
        return list(reader)


def compiler_for_profile(profile: str) -> str:
    if profile.startswith("gcc_2_8_1_"):
        return "gcc-2.8.1-psx"
    if profile.startswith("gcc_2_7_2_"):
        return "gcc-2.7.2-mips"
    raise ImportError(f"unsupported compiler profile: {profile}")


def summary(row: dict[str, str]) -> str:
    if row["error"]:
        return f"profile-pass tool error: {row['error']}"
    detail = row["first_difference"] or "relocation-only mismatch"
    return (
        f"profile-pass nonmatch: bytes={row['bytes_match']}, "
        f"relocations={row['relocations_match']}, "
        f"size={row['actual_size']}/{int(row['size'], 0)}; {detail}"
    )


def import_unmatched_results(root: Path, path: Path) -> int:
    functions_path = resolve_within(
        root, "config/slus_01411/functions.csv", must_exist=True
    )
    attempts_path = resolve_within(
        root, "config/slus_01411/attempts.csv", must_exist=True
    )
    functions = record_attempt.load_function_addresses(functions_path)
    rows = record_attempt.load_attempts(attempts_path)
    imported = 0
    for result in read_results(path):
        if result["exact"] == "True":
            raise ImportError(
                f"{result['address']}: exact results require integration review"
            )
        address = record_attempt.parse_address(result["address"])
        if address not in functions:
            raise ImportError(f"{address:#010x}: unknown function")
        history = record_attempt.validate_history(rows, address)
        if history and history[-1]["result"] in {"matched", "deferred"}:
            continue
        profile = result["profile"]
        source = resolve_within(root, result["source"], must_exist=True)
        source_value = str(source.relative_to(root))
        duplicate = any(
            profile in PROFILE_PATTERN.findall(row["flags"])
            and source_value in SOURCE_PATTERN.findall(row["flags"])
            for row in history
        )
        if duplicate:
            continue
        attempt = len(history) + 1
        outcome = "nonmatch"
        rows.append(
            {
                "address": f"0x{address:08X}",
                "attempt": str(attempt),
                "compiler": compiler_for_profile(profile),
                "flags": (
                    f"profile={profile} label=remaining-profile-pass "
                    f"source={source_value}"
                ),
                "result": outcome,
                "summary": summary(result),
            }
        )
        imported += 1
    record_attempt.write_attempts(attempts_path, rows)
    return imported


def import_inline_results(root: Path, path: Path) -> int:
    ledger_path = resolve_within(
        root,
        "config/slus_01411/external_attempts.csv",
        must_exist=True,
    )
    rows = record_external_attempt.load_rows(ledger_path)
    functions = record_external_attempt.load_functions(
        resolve_within(
            root, "config/slus_01411/functions.csv", must_exist=True
        )
    )
    matching = record_external_attempt.load_matching_addresses(
        resolve_within(
            root, "config/slus_01411/matching_c.json", must_exist=True
        )
    )
    profiles = record_external_attempt.load_profiles(
        resolve_within(
            root,
            "config/slus_01411/compiler_profiles.json",
            must_exist=True,
        )
    )
    imported = 0
    for result in read_results(path):
        if result["exact"] == "True":
            raise ImportError(
                f"{result['address']}: exact results require integration review"
            )
        address = record_external_attempt.parse_address(result["address"])
        history = [
            row
            for row in rows
            if row["mode"] == "inline_refinement"
            and record_external_attempt.parse_address(row["address"]) == address
        ]
        if history and history[-1]["result"] in {"matched", "deferred"}:
            continue
        profile = result["profile"]
        source = resolve_within(root, result["source"], must_exist=True)
        source_value = str(source.relative_to(root))
        if any(
            row["profile"] == profile
            and row["candidate_source"] == source_value
            for row in history
        ):
            continue
        attempt = len(history) + 1
        outcome = "nonmatch"
        rows.append(
            {
                "mode": "inline_refinement",
                "address": f"0x{address:08X}",
                "attempt": str(attempt),
                "reference_path": "",
                "reference_sha256": "",
                "profile": profile,
                "candidate_source": source_value,
                "candidate_sha256": record_external_attempt.sha256(source),
                "result": outcome,
                "summary": summary(result),
            }
        )
        imported += 1
    record_external_attempt.validate_rows(
        rows,
        functions,
        matching,
        profiles,
    )
    record_external_attempt.write_rows(ledger_path, rows)
    return imported


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Import reviewed remaining-profile pass results."
    )
    parser.add_argument("results", help="result CSV beneath tmp/")
    parser.add_argument(
        "--scope",
        choices=("unmatched", "inline"),
        default="unmatched",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        path = resolve_within(root, args.results, must_exist=True)
        temporary_root = resolve_within(root, "tmp", must_exist=True)
        path.relative_to(temporary_root)
        imported = (
            import_inline_results(root, path)
            if args.scope == "inline"
            else import_unmatched_results(root, path)
        )
    except (
        ImportError,
        record_attempt.AttemptError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        csv.Error,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print(f"imported profile-pass results: {imported}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
