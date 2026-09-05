#!/usr/bin/env python3

"""Report repeated identical loads that no store separates.

A compiler eliminates a second identical load unless something between the two
may have changed the memory it reads. Two loads separated by a store are
therefore ordinary aliasing and say nothing about the source. Two loads with no
store between them are different: the value was re-read because the source
named it twice and the read could not be eliminated, which in practice means a
`volatile` object.

That distinction is the whole point of this scan, because the weaker pattern is
common and the stronger one is rare. Run it on a function before reaching for a
volatile qualifier.
"""

from __future__ import annotations

import argparse
import csv
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from overlay_diff import (  # noqa: E402
    OverlayDiffError,
    disassemble,
    inventory_entry,
    load_module,
    target_words,
)
from workspace import WorkspaceError, require_workspace_root, resolve_within  # noqa: E402

LOAD = re.compile(r"^(lw|lh|lhu|lb|lbu)\s+\w+,(-?\d+)\((\w+)\)$")
STORE = re.compile(r"^(sw|sh|sb|swl|swr)\b")
FLOW = re.compile(r"^(b|j|jal|jr)")

MODULES = (
    "free_duel",
    "password",
    "main_menu",
    "overworld_before_coup",
    "overworld_after_coup",
)


def basic_blocks(instructions: list[str]) -> list[list[str]]:
    blocks: list[list[str]] = [[]]
    for text in instructions:
        blocks[-1].append(text)
        if FLOW.match(text):
            blocks.append([])
    return blocks


def worst_reload(instructions: list[str]) -> tuple[int, str | None]:
    """Largest number of identical loads in one block with no store between."""
    best = 0
    culprit = None
    for block in basic_blocks(instructions):
        seen: dict[tuple[str, str, str], int] = {}
        for text in block:
            if STORE.match(text):
                seen.clear()
                continue
            match = LOAD.match(text)
            if match is None:
                continue
            key = (match.group(1), match.group(2), match.group(3))
            seen[key] = seen.get(key, 0) + 1
            if seen[key] > best:
                best = seen[key]
                culprit = text
    return best, culprit


def unmatched_rows(root: Path, module: str) -> list[dict[str, str]]:
    path = resolve_within(
        root, f"config/slus_01411/overlays/{module}_functions.csv", must_exist=True
    )
    with path.open(encoding="utf-8", newline="") as handle:
        return [row for row in csv.DictReader(handle) if row["status"] != "matching_c"]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("module", nargs="?", choices=MODULES)
    parser.add_argument("address", nargs="?")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        modules = [args.module] if args.module else list(MODULES)
        hits = 0
        for name in modules:
            module = load_module(root, name)
            if args.address:
                address = int(args.address, 16)
                rows = [inventory_entry(root, name, address)]
            else:
                rows = unmatched_rows(root, name)
            for row in rows:
                address = int(row["address"], 16)
                words = target_words(root, module, address, int(row["size"], 16))
                instructions = disassemble(root, words)
                count, culprit = worst_reload(instructions)
                flag = ""
                if count >= 2:
                    hits += 1
                    flag = f"  <== volatile signature: {culprit}"
                print(
                    f"{name:22s} {row['address']} "
                    f"{len(instructions):5d} instructions  "
                    f"reloads-without-store={count}{flag}"
                )
        print(f"\n{hits} function(s) show the signature")
        return 0
    except (OverlayDiffError, WorkspaceError, OSError, KeyError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
