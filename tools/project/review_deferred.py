#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import json
import re
import sys
from collections import defaultdict
from pathlib import Path
from typing import Any

from function_inventory import InventoryError, load_inventory
from workspace import WorkspaceError, require_workspace_root, resolve_within


class ReviewError(RuntimeError):
    pass


ATTEMPT_FIELDS = ("address", "attempt", "compiler", "flags", "result", "summary")
SOURCE_PATTERN = re.compile(r"(?:^|\s)source=([^\s]+)")


def parse_integer(value: str) -> int:
    try:
        return int(value, 0)
    except ValueError as error:
        raise argparse.ArgumentTypeError(f"invalid integer: {value}") from error


def load_attempts(path: Path) -> dict[int, list[dict[str, str]]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if tuple(reader.fieldnames or ()) != ATTEMPT_FIELDS:
            raise ReviewError(f"{path}: unexpected CSV fields")
        grouped: dict[int, list[dict[str, str]]] = defaultdict(list)
        for row in reader:
            try:
                address = int(row["address"], 0)
            except ValueError as error:
                raise ReviewError(
                    f"{path}: invalid address {row['address']}"
                ) from error
            grouped[address].append(row)
    return grouped


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "List deferred game functions for hypothesis review without "
            "reopening their canonical attempt histories."
        )
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
        "--contains",
        default="",
        help="case-insensitive filter over names, compilers, flags, and summaries",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=30,
        help="maximum functions after size/address sorting",
    )
    parser.add_argument(
        "--format",
        choices=("csv", "addresses", "json"),
        default="csv",
        help="output format; JSON includes the complete canonical history",
    )
    return parser.parse_args()


def select(root: Path, args: argparse.Namespace) -> list[dict[str, Any]]:
    if args.start < 0 or args.end <= args.start:
        raise ReviewError("address range must satisfy 0 <= start < end")
    if args.limit < 1:
        raise ReviewError("limit must be at least one")

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
    needle = args.contains.casefold()
    candidates: list[dict[str, Any]] = []

    for function in inventory:
        history = attempts.get(function.address, [])
        if (
            function.module != "game"
            or function.status != "unmatched_asm"
            or not args.start <= function.address < args.end
            or not history
            or history[-1]["result"] != "deferred"
        ):
            continue

        searchable = "\n".join(
            (
                function.name,
                *(row[field] for row in history for field in ATTEMPT_FIELDS[2:]),
            )
        ).casefold()
        if needle and needle not in searchable:
            continue

        last = history[-1]
        source_variants = {
            match.group(1)
            for row in history
            if (match := SOURCE_PATTERN.search(row["flags"])) is not None
        }
        candidates.append(
            {
                "address": f"0x{function.address:08X}",
                "size": f"0x{function.size:X}",
                "name": function.name,
                "attempt_count": len(history),
                "distinct_compilers": len(
                    {row["compiler"] for row in history}
                ),
                "distinct_sources": len(source_variants),
                "tool_error_attempts": sum(
                    "tool error" in row["summary"].casefold()
                    for row in history
                ),
                "last_compiler": last["compiler"],
                "last_flags": last["flags"],
                "last_summary": last["summary"],
                "history": history,
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
        print(json.dumps({"schema": 1, "deferred": candidates}, indent=2))
        return

    fields = (
        "address",
        "size",
        "name",
        "attempt_count",
        "distinct_compilers",
        "distinct_sources",
        "tool_error_attempts",
        "last_compiler",
        "last_flags",
        "last_summary",
    )
    writer = csv.DictWriter(sys.stdout, fieldnames=fields, lineterminator="\n")
    writer.writeheader()
    writer.writerows(
        {field: candidate[field] for field in fields}
        for candidate in candidates
    )


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        candidates = select(root, args)
        print_candidates(candidates, args.format)
    except (
        ReviewError,
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
