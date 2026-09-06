#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import json
import re
import sys
from pathlib import Path
from typing import Any

import audit_unchiga_candidates
from workspace import WorkspaceError, require_workspace_root, resolve_within


WORK_PATH = "tmp/agents/remaining-profile-pass"
FUNCTIONS_PATH = "config/slus_01411/functions.csv"
ATTEMPTS_PATH = "config/slus_01411/attempts.csv"
EXTERNAL_ATTEMPTS_PATH = "config/slus_01411/external_attempts.csv"
MATCHING_PATH = "config/slus_01411/matching_c.json"
PROFILES_PATH = "config/slus_01411/compiler_profiles.json"
SOURCE_PATTERN = re.compile(r"(?:^|\s)source=([^\s]+)")
PROFILE_PATTERN = re.compile(r"profile=(gcc_[A-Za-z0-9_]+)")
ASM_PATTERN = re.compile(r"\b(?:asm|__asm|__asm__)\b")
COMMENT_PATTERN = re.compile(r"/\*.*?\*/|//[^\n]*", re.DOTALL)
RESULT_FIELDS = (
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


class ProfilePassError(RuntimeError):
    pass


def read_csv(path: Path) -> list[dict[str, str]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        return list(csv.DictReader(handle))


def write_csv(path: Path, rows: list[dict[str, Any]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(f"{path.name}.tmp")
    with temporary.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(
            handle,
            fieldnames=RESULT_FIELDS,
            extrasaction="ignore",
            lineterminator="\n",
        )
        writer.writeheader()
        writer.writerows(rows)
    temporary.replace(path)


def write_json(path: Path, value: Any) -> None:
    temporary = path.with_name(f"{path.name}.tmp")
    temporary.write_text(
        json.dumps(value, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    temporary.replace(path)


def normalize_source(root: Path, value: str) -> Path | None:
    path = Path(value)
    if path.is_absolute():
        try:
            path = path.relative_to(root)
        except ValueError:
            return None
    try:
        return resolve_within(root, path, must_exist=True)
    except WorkspaceError:
        return None


def source_from_attempt(root: Path, row: dict[str, str]) -> Path | None:
    match = SOURCE_PATTERN.search(row["flags"])
    if match is None:
        return None
    return normalize_source(root, match.group(1))


def source_is_pure(path: Path) -> bool:
    text = path.read_text(encoding="utf-8", errors="replace")
    return ASM_PATTERN.search(COMMENT_PATTERN.sub("", text)) is None


def profile_group(history: list[dict[str, str]]) -> str | None:
    profiles = [
        profile
        for row in history
        for profile in PROFILE_PATTERN.findall(row["flags"])
    ]
    for profile in reversed(profiles):
        if "_g8" in profile:
            return "g8"
        if "_g0" in profile:
            return "g0"
    return None


def selected_profile(group: str, variant: str) -> str:
    if variant == "gcc272":
        return f"gcc_2_7_2_{group}"
    if variant == "no-sched2":
        return f"gcc_2_8_1_{group}_no_sched2"
    if variant == "o1":
        return f"gcc_2_8_1_o1_{group}"
    suffix = {
        "default": "",
        "split": "_split",
        "no-split": "_no_split",
    }[variant]
    return f"gcc_2_8_1_{group}{suffix}"


def attempted_profiles(history: list[dict[str, str]]) -> set[str]:
    return {
        profile
        for row in history
        for profile in PROFILE_PATTERN.findall(row["flags"])
    }


def candidates(
    root: Path,
    *,
    scope: str,
    variant: str,
    limit: int,
    repair_gprel: bool,
) -> list[dict[str, Any]]:
    functions = read_csv(resolve_within(root, FUNCTIONS_PATH, must_exist=True))
    functions_by_address = {
        int(row["address"], 0): row for row in functions
    }
    if scope == "inline":
        matching = json.loads(
            resolve_within(root, MATCHING_PATH, must_exist=True).read_text(
                encoding="utf-8"
            )
        )["functions"]
        external = read_csv(
            resolve_within(root, EXTERNAL_ATTEMPTS_PATH, must_exist=True)
        )
        by_address: dict[int, list[dict[str, str]]] = {}
        for row in external:
            if row["mode"] == "inline_refinement":
                by_address.setdefault(int(row["address"], 0), []).append(row)
        result: list[dict[str, Any]] = []
        for entry in matching:
            address = int(entry["address"], 0)
            source = resolve_within(root, entry["source"], must_exist=True)
            if source_is_pure(source):
                continue
            history = by_address.get(address, [])
            if (
                not history
                or len(history) >= 6
                or history[-1]["result"] in {"matched"}
            ):
                continue
            candidate = normalize_source(root, history[-1]["candidate_source"])
            if (
                candidate is None
                or not candidate.is_file()
                or not source_is_pure(candidate)
            ):
                continue
            group = "g8" if "_g8" in history[-1]["profile"] else "g0"
            profile = selected_profile(group, variant)
            tried = {row["profile"] for row in history}
            if profile in tried:
                continue
            function = functions_by_address[address]
            result.append(
                {
                    "address": address,
                    "size": int(entry["size"], 0),
                    "name": function["name"],
                    "attempt_count": len(history),
                    "profile": profile,
                    "source": candidate,
                    "mode": "inline_refinement",
                }
            )
        result.sort(key=lambda row: (row["size"], row["address"]))
        return result[:limit]

    attempts = read_csv(resolve_within(root, ATTEMPTS_PATH, must_exist=True))
    by_address: dict[int, list[dict[str, str]]] = {}
    for row in attempts:
        by_address.setdefault(int(row["address"], 0), []).append(row)

    result: list[dict[str, Any]] = []
    for function in functions:
        address = int(function["address"], 0)
        history = by_address.get(address, [])
        summary = history[-1]["summary"].lower() if history else ""
        tool_error = (
            "tool error" in summary
            or "compile/build failed" in summary
            or "profile-pass tool error" in summary
        )
        gprel_error = (
            "gprel16" in summary
            or "small-data" in summary
            or "out-of-range gprel" in summary
        )
        if (
            function["module"] != "game"
            or function["status"] != "unmatched_asm"
            or not history
            or len(history) >= 6
            or history[-1]["result"] in {"matched"}
            or tool_error and not (repair_gprel and gprel_error)
        ):
            continue
        source = source_from_attempt(root, history[-1])
        if source is None or not source.is_file() or not source_is_pure(source):
            continue
        group = "g0" if repair_gprel else profile_group(history)
        if group is None:
            continue
        profile = selected_profile(group, variant)
        if profile in attempted_profiles(history):
            continue
        result.append(
            {
                "address": address,
                "size": int(function["size"], 0),
                "name": function["name"],
                "attempt_count": len(history),
                "profile": profile,
                "source": source,
                "mode": "collaborator_match",
            }
        )
    result.sort(key=lambda row: (row["size"], row["address"]))
    return result[:limit]


def run_pass(
    root: Path,
    selected: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    work = resolve_within(root, WORK_PATH)
    work.mkdir(parents=True, exist_ok=True)
    audit_unchiga_candidates.ensure_baseline(root, work)
    profiles = json.loads(
        resolve_within(root, PROFILES_PATH, must_exist=True).read_text(
            encoding="utf-8"
        )
    )["profiles"]
    symbols = audit_unchiga_candidates.all_symbols(root, work)
    rows: list[dict[str, Any]] = []
    for candidate in selected:
        result = audit_unchiga_candidates.verify_one(
            root,
            {
                "address": f"0x{candidate['address']:08X}",
                "size": f"0x{candidate['size']:X}",
                "current_name": candidate["name"],
                "mode": candidate["mode"],
                "candidate_source": str(candidate["source"].relative_to(root)),
                "profile": candidate["profile"],
            },
            profiles,
            symbols,
        )
        rows.append(
            {
                "address": f"0x{candidate['address']:08X}",
                "size": f"0x{candidate['size']:X}",
                "attempt_count": candidate["attempt_count"],
                "profile": candidate["profile"],
                "source": str(candidate["source"].relative_to(root)),
                "exact": result.get("exact", False),
                "bytes_match": result.get("bytes_match", False),
                "relocations_match": result.get("relocations_match", False),
                "actual_size": result.get("actual_size", ""),
                "first_difference": result.get("first_difference", ""),
                "error": result.get("error", ""),
            }
        )
    return rows


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Sequentially test one untried compiler-profile variant for "
            "preserved pure-C unmatched candidates."
        )
    )
    parser.add_argument(
        "--scope",
        choices=("unmatched", "inline"),
        default="unmatched",
    )
    parser.add_argument(
        "--variant",
        choices=(
            "default",
            "split",
            "no-split",
            "gcc272",
            "no-sched2",
            "o1",
        ),
        required=True,
    )
    parser.add_argument("--limit", type=int, default=100)
    parser.add_argument(
        "--repair-gprel",
        action="store_true",
        help=(
            "include unmatched pure candidates whose latest tool error is "
            "a G8 small-data relocation failure, and test them as G0"
        ),
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        if args.limit < 1:
            raise ProfilePassError("--limit must be positive")
        root = require_workspace_root()
        selected = candidates(
            root,
            scope=args.scope,
            variant=args.variant,
            limit=args.limit,
            repair_gprel=args.repair_gprel,
        )
        rows = run_pass(root, selected)
        work = resolve_within(root, WORK_PATH)
        repair_suffix = "-gprel" if args.repair_gprel else ""
        output_name = (
            f"{args.scope}-{args.variant}{repair_suffix}-results"
        )
        write_csv(work / f"{output_name}.csv", rows)
        write_json(
            work / f"{output_name}.json",
            {
                "schema": 1,
                "scope": args.scope,
                "variant": args.variant,
                "results": rows,
            },
        )
        exact = sum(bool(row["exact"]) for row in rows)
        errors = sum(bool(row["error"]) for row in rows)
        print(
            f"profile pass: scope={args.scope} variant={args.variant} "
            f"selected={len(rows)} "
            f"exact={exact} nonmatch={len(rows) - exact - errors} "
            f"errors={errors}"
        )
    except (
        ProfilePassError,
        WorkspaceError,
        OSError,
        UnicodeError,
        KeyError,
        TypeError,
        ValueError,
        csv.Error,
        json.JSONDecodeError,
    ) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
