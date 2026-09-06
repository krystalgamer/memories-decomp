#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import json
import sys
from collections import defaultdict
from pathlib import Path
from typing import Any

from function_inventory import InventoryError, load_inventory
from workspace import WorkspaceError, require_workspace_root, resolve_within


class CandidateError(RuntimeError):
    pass


ATTEMPT_FIELDS = ("address", "attempt", "compiler", "flags", "result", "summary")
TERMINAL_RESULTS = {"matched", "deferred"}
MAX_ATTEMPTS = 6


def parse_integer(value: str) -> int:
    try:
        return int(value, 0)
    except ValueError as error:
        raise argparse.ArgumentTypeError(f"invalid integer: {value}") from error


def load_attempts(path: Path) -> dict[int, list[dict[str, str]]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if tuple(reader.fieldnames or ()) != ATTEMPT_FIELDS:
            raise CandidateError(f"{path}: unexpected CSV fields")
        grouped: dict[int, list[dict[str, str]]] = defaultdict(list)
        for row in reader:
            try:
                address = int(row["address"], 0)
            except ValueError as error:
                raise CandidateError(
                    f"{path}: invalid address {row['address']}"
                ) from error
            grouped[address].append(row)

    for address, rows in grouped.items():
        if len(rows) > MAX_ATTEMPTS:
            raise CandidateError(
                f"{address:#010x}: canonical ledger has more than "
                f"{MAX_ATTEMPTS} rows, so the campaign history is malformed"
            )
        for expected, row in enumerate(rows, start=1):
            try:
                attempt = int(row["attempt"], 10)
            except ValueError as error:
                raise CandidateError(
                    f"{address:#010x}: invalid attempt {row['attempt']}"
                ) from error
            if attempt != expected:
                raise CandidateError(
                    f"{address:#010x}: expected attempt {expected}, "
                    f"found {attempt}"
                )
            if row["result"] in TERMINAL_RESULTS and expected != len(rows):
                raise CandidateError(
                    f"{address:#010x}: terminal result is not last"
                )
    return grouped


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="List unmatched game functions safe for a new attempt batch."
    )
    parser.add_argument(
        "--start",
        type=parse_integer,
        default=0,
        help="inclusive address lower bound",
    )
    parser.add_argument(
        "--end",
        type=parse_integer,
        default=0x1_0000_0000,
        help="exclusive address upper bound",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=30,
        help="maximum candidates after size/address sorting",
    )
    parser.add_argument(
        "--include-partial",
        action="store_true",
        help="include functions that already have a canonical attempt history",
    )
    parser.add_argument(
        "--format",
        choices=("csv", "addresses", "json"),
        default="csv",
        help="output format",
    )
    return parser.parse_args()


def select(root: Path, args: argparse.Namespace) -> list[dict[str, Any]]:
    if args.start < 0 or args.end <= args.start:
        raise CandidateError("address range must satisfy 0 <= start < end")
    if args.limit < 1:
        raise CandidateError("limit must be at least one")

    inventory = load_inventory(
        resolve_within(
            root, "config/slus_01411/functions.csv", must_exist=True
        )
    )
    attempts = load_attempts(
        resolve_within(
            root, "config/slus_01411/attempts.csv", must_exist=True
        )
    )

    candidates: list[dict[str, Any]] = []
    for function in inventory:
        if (
            function.module != "game"
            or function.status != "unmatched_asm"
            or not args.start <= function.address < args.end
        ):
            continue
        history = attempts.get(function.address, [])
        # A closed canonical ledger records that the first campaign ended, not
        # that the function is finished. Only an exact match means there is
        # nothing left to explore.
        if history and history[-1]["result"] == "matched":
            continue
        if history and not args.include_partial:
            continue
        candidates.append(
            {
                "address": f"0x{function.address:08X}",
                "size": f"0x{function.size:X}",
                "attempt_count": len(history),
            }
        )

    candidates.sort(
        key=lambda candidate: (
            int(candidate["size"], 0),
            int(candidate["address"], 0),
        )
    )
    return candidates[: args.limit]


def print_candidates(
    candidates: list[dict[str, Any]], output_format: str
) -> None:
    if output_format == "addresses":
        print(", ".join(candidate["address"] for candidate in candidates))
        return
    if output_format == "json":
        print(json.dumps({"schema": 1, "candidates": candidates}, indent=2))
        return

    writer = csv.DictWriter(
        sys.stdout,
        fieldnames=(
            "address",
            "size",
            "attempt_count",
        ),
        lineterminator="\n",
    )
    writer.writeheader()
    writer.writerows(candidates)


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        candidates = select(root, args)
        print_candidates(candidates, args.format)
    except (
        CandidateError,
        InventoryError,
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
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
