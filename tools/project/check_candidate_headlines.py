#!/usr/bin/env python3
"""Check each candidate note's headline against its `functions.csv` row.

A candidate's figures live in two places: the entry under `notes/candidates/`
and the inventory row. Nothing kept them in step, and they drifted - three
entries disagreed when this check was first written, and one inventory row
disagreed with *itself*, opening with one figure and closing with another it had
been given later.

The failure is quiet. A reader who opens the inventory to pick work sees a
number nobody has measured for months, and a reader who opens the note sees a
different one; neither has any reason to suspect the other exists.

This compares the differing-word count and the opcode distance as each artifact
states them, and reports any entry where they disagree. It does not rebuild
anything, so it is cheap enough to run on every change.

    tools/environments/python/bin/python tools/project/check_candidate_headlines.py
"""

from __future__ import annotations

import argparse
import csv
import re
import sys
from pathlib import Path

from workspace import (
    WorkspaceError,
    require_workspace_root,
    resolve_within,
)

FUNCTIONS_CSV = "config/slus_01411/functions.csv"
CANDIDATES_DIR = "notes/candidates"
HEADLINE_LINES = 12

DISTANCE = re.compile(
    r"(?:opcode\s+(?:multiset\s+)?distance|distance)\s*\**\s*(\d+)", re.I)
DIFFERING = re.compile(r"\**(\d+)\**\s+differing\s+(?:positions|words)", re.I)


def figures(text: str) -> tuple[str | None, str | None]:
    distance = DISTANCE.search(text)
    differing = DIFFERING.search(text)
    return (distance.group(1) if distance else None,
            differing.group(1) if differing else None)


def inventory(root: Path) -> dict[str, str]:
    notes = {}
    with resolve_within(root, FUNCTIONS_CSV).open() as handle:
        for row in csv.DictReader(handle):
            notes[row["name"]] = row["notes"]
    return notes


def main() -> int:
    parser = argparse.ArgumentParser(
        description="check candidate headlines against their inventory rows")
    parser.add_argument("--quiet", action="store_true",
                        help="only report disagreements")
    args = parser.parse_args()

    root = require_workspace_root()
    rows = inventory(root)
    candidates = sorted(resolve_within(root, CANDIDATES_DIR).glob("func_*.md"))

    checked = 0
    problems = []
    for note in candidates:
        name = note.stem
        if name not in rows:
            continue
        headline = "\n".join(note.read_text().splitlines()[:HEADLINE_LINES])
        note_distance, note_differing = figures(headline)
        row_distance, row_differing = figures(rows[name])
        if note_differing is None and row_differing is None:
            continue
        checked += 1
        disagree = []
        if (note_distance is not None and row_distance is not None
                and note_distance != row_distance):
            disagree.append(
                f"distance {note_distance} in the note, {row_distance} in the row")
        if (note_differing is not None and row_differing is not None
                and note_differing != row_differing):
            disagree.append(
                f"{note_differing} differing in the note, "
                f"{row_differing} in the row")
        if disagree:
            problems.append((name, disagree))

    for name, disagree in problems:
        print(f"{name}: " + "; ".join(disagree))
    if problems:
        print(f"\ncandidate headlines: {len(problems)} of {checked} disagree "
              f"with their functions.csv row")
        return 1
    if not args.quiet:
        print(f"candidate headlines: OK ({checked} entries agree with their "
              f"functions.csv rows)")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except WorkspaceError as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(1)
