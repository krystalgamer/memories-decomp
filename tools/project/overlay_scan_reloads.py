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
STORE_ADDR = re.compile(r"^(sw|sh|sb)\s+(\w+),(-?\d+)\((\w+)\)$")
WIDTH = {"sw": {"lw"}, "sh": {"lh", "lhu"}, "sb": {"lb", "lbu"}}
FLOW = re.compile(r"^(b|j|jal|jr)")
TARGET = re.compile(r",(?:\s*)(?:0x)?([0-9a-f]+)(?:\s|$|<)")
JUMP = re.compile(r"^j\s+0x([0-9a-f]+)$")

MODULES = (
    "free_duel",
    "password",
    "main_menu",
    "overworld_before_coup",
    "overworld_after_coup",
)


def branch_targets(instructions: list[str], base: int = 0) -> set[int]:
    """Instruction indices that some branch or jump can land on.

    A load at a branch target may be reached without executing the store that
    precedes it in the listing, so a block must start there. Splitting only at
    branch instructions merges the two and invents reloads that are not
    redundant at all.
    """
    targets: set[int] = set()
    for text in instructions:
        if not FLOW.match(text):
            continue
        # A conditional branch prints an offset from the start of the
        # function; an unconditional j prints the raw jump field, which is
        # only useful once the function's own address is known.
        jump = JUMP.match(text)
        if jump is not None:
            index = (int(jump.group(1), 16) - (base & 0x0FFFFFFF)) // 4
            if 0 <= index < len(instructions):
                targets.add(index)
            continue
        match = TARGET.search(text)
        if match is None:
            continue
        offset = int(match.group(1), 16)
        if offset % 4 == 0 and offset // 4 < len(instructions):
            targets.add(offset // 4)
    return targets


def basic_blocks(instructions: list[str], base: int = 0) -> list[list[str]]:
    targets = branch_targets(instructions, base)
    blocks: list[list[str]] = [[]]
    for index, text in enumerate(instructions):
        # A delay slot belongs with its branch, so a target one past a branch
        # still opens its block at the target itself.
        if index in targets and blocks[-1]:
            blocks.append([])
        blocks[-1].append(text)
        if FLOW.match(text):
            blocks.append([])
    return blocks


def worst_reload(instructions: list[str], base: int = 0) -> tuple[int, str | None]:
    """Largest number of identical loads in one block with no store between."""
    best = 0
    culprit = None
    for block in basic_blocks(instructions, base):
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


def reload_after_own_store(instructions: list[str], base: int = 0) -> str | None:
    """A load of the address a store just wrote is a second volatile tell.

    A store tells the compiler what the location now holds, so re-reading it
    immediately is redundant unless the object is `volatile`. This case is
    invisible to `worst_reload`, which treats any store as a legitimate reason
    to reload and clears its record.
    """
    for block in basic_blocks(instructions, base):
        stored: tuple[str, str, str, str] | None = None
        for text in block:
            store = STORE_ADDR.match(text)
            if store is not None:
                stored = store.groups()
                continue
            match = LOAD.match(text)
            if match is None:
                continue
            if stored is not None:
                opcode, source, offset, base = stored
                # Same address and width, but a different destination register:
                # the value is already in `source`, so the reload buys nothing.
                # A narrower load is a truncation and a reload into the same
                # register is an ordinary re-read of a field just written.
                same_place = (offset, base) == (match.group(2), match.group(3))
                if (
                    same_place
                    and match.group(1) in WIDTH.get(opcode, set())
                    and text.split()[1].split(",")[0] != source
                ):
                    return text
            stored = None
    return None


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
                count, culprit = worst_reload(instructions, address)
                after_store = reload_after_own_store(instructions, address)
                flag = ""
                if count >= 2:
                    hits += 1
                    flag = f"  <== volatile signature: {culprit}"
                elif after_store is not None:
                    hits += 1
                    flag = f"  <== volatile signature, reload after own store: {after_store}"
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
