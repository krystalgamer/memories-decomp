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

It also reports an entry where one artifact states a figure and the other does
not. That shape is the one this check originally missed: it compared a pair of
numbers only when it could find both, so an entry that changed *which* metric it
led with - `func_800528AC` moving from "16 non-relocation word mismatches" to
"279 differing positions" - left nothing to compare and was recorded as agreeing.
**Absence was read as agreement**, which is quietest exactly when an entry is
being rewritten and most likely to drift.

    tools/environments/python/bin/python tools/project/check_candidate_headlines.py
"""

from __future__ import annotations

import argparse
import csv
import re
from pathlib import Path

# Resolved from this file rather than through `require_workspace_root`, whose
# markers include the retail image. This check reads only tracked text, so
# demanding an ignored binary would keep it out of the metadata workflow -- the
# one place it can watch `notes/**`. `candidate_files.py` resolves its root the
# same way for the same reason.
ROOT = Path(__file__).resolve().parents[2]

FUNCTIONS_CSV = "config/slus_01411/functions.csv"
CANDIDATES_DIR = "notes/candidates"
HEADLINE_LINES = 12

DISTANCE = re.compile(
    r"(?:opcode\s+(?:multiset\s+)?distance|distance)\s*\**\s*(\d+)", re.I)
# Deliberately not anchored to "positions" or "words". Rows in the wild also
# write "172 differing of 384", and requiring the noun made this check miss a row
# that agreed with its note perfectly well.
DIFFERING = re.compile(
    r"\**(\d+)\**\s+(?:of\s+\d+\s+\w+\s+)?differing|"
    r"\**(\d+)\**\s+differing", re.I)


def figures(text: str) -> tuple[str | None, str | None]:
    distance = DISTANCE.search(text)
    differing = DIFFERING.search(text)
    value = next((g for g in differing.groups() if g), None) if differing else None
    return (distance.group(1) if distance else None, value)


def compare(headline: str, row: str) -> list[str] | None:
    """Complaints about one entry, or None when neither artifact states figures.

    A figure stated on one side and absent on the other is a complaint. Reading
    that as agreement is what let `func_800528AC` change which metric it led with
    while its row kept the old one.
    """
    note_distance, note_differing = figures(headline)
    row_distance, row_differing = figures(row)
    if not any((note_distance, note_differing, row_distance, row_differing)):
        return None
    if not row.strip():
        return ["the note states figures and the functions.csv row has no "
                "notes at all"]
    disagree = []
    for label, note_value, row_value in (
            ("differing count", note_differing, row_differing),
            ("opcode distance", note_distance, row_distance)):
        if note_value is not None and row_value is None:
            disagree.append(
                f"the note states a {label} of {note_value} and the row "
                f"states none")
        elif row_value is not None and note_value is None:
            disagree.append(
                f"the row states a {label} of {row_value} and the note "
                f"states none")
        elif (note_value is not None and row_value is not None
                and note_value != row_value):
            disagree.append(
                f"{label} {note_value} in the note, {row_value} in the row")
    return disagree


CONTROLS = (
    # The shape this check used to miss: the metric was renamed, not changed.
    ("279 differing positions", "opcode multiset distance 1 (one addu), 16 "
     "non-relocation word mismatches", True),
    # Rows and notes in the tree write the count several ways; none is a fault.
    ("172 differing positions of 384", "172 differing of 384", False),
    ("opcode multiset distance 2, 93 of 394 words differing",
     "opcode multiset distance 2, 93 of 394 words differing", False),
    # A real disagreement must still be caught.
    ("5 differing positions", "18 differing positions", True),
    # An entry with no figures on either side is out of scope, not a fault.
    ("no figures here", "narrative only", None),
)


def self_test() -> int:
    failures = 0
    for headline, row, expected in CONTROLS:
        got = compare(headline, row)
        if expected is None:
            ok = got is None
        else:
            ok = got is not None and bool(got) == expected
        if not ok:
            failures += 1
            print(f"self-test: {headline!r} vs {row!r}: expected "
                  f"{expected}, got {got}")
    if failures:
        print(f"self-test: {failures} of {len(CONTROLS)} controls failed")
        return 1
    print(f"self-test: OK ({len(CONTROLS)} controls)")
    return 0


def inventory(root: Path) -> dict[str, str]:
    notes = {}
    with (root / FUNCTIONS_CSV).open() as handle:
        for row in csv.DictReader(handle):
            notes[row["name"]] = row["notes"]
    return notes


def main() -> int:
    parser = argparse.ArgumentParser(
        description="check candidate headlines against their inventory rows")
    parser.add_argument("--quiet", action="store_true",
                        help="only report disagreements")
    parser.add_argument("--self-test", action="store_true",
                        help="check the comparison against known-good and "
                             "known-bad pairs, including the shape this check "
                             "originally missed")
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    root = ROOT
    rows = inventory(root)
    candidates = sorted((root / CANDIDATES_DIR).glob("func_*.md"))

    checked = 0
    problems = []
    for note in candidates:
        name = note.stem
        if name not in rows:
            continue
        headline = "\n".join(note.read_text().splitlines()[:HEADLINE_LINES])
        disagree = compare(headline, rows[name])
        if disagree is None:
            continue
        checked += 1
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
    raise SystemExit(main())
