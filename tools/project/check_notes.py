#!/usr/bin/env python3
"""Check that notes/grouped-translation-units.md matches the build config.

The table in that note is an index of every grouped translation unit: its
source path, its compiler profile, and the addresses it covers. All three
drift silently when translation units are merged or renamed, because nothing
compiles the note. This checks each row against
config/slus_01411/matching_c.json, which is what the build actually uses.

It also rejects leftover merge-conflict markers. That is not hypothetical
tidiness: the note conflicts on almost every coalesce PR, because two units
merged in the same cycle add adjacent rows, and markers reached review three
times on one branch. Nothing else can catch them - notes/** is paths-ignored
by the build workflow, and the row parser below skips any line that is not a
table row, so `<<<<<<< HEAD` used to be ignored rather than rejected.

It also checks the per-family MODEL variant layout: the semantic map
(notes/semantic-symbol-map.csv plus notes/semantic-symbol-map.d/*.csv) carries
no conflict markers, every per-family file holds only its own family's rows
under the shared header, and no family row is left in the shared map.
"""

from __future__ import annotations

import argparse
import csv
import io
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NOTE = ROOT / "notes/grouped-translation-units.md"
MATCHING = ROOT / "config/slus_01411/matching_c.json"
SEMANTIC_MAP = ROOT / "notes/semantic-symbol-map.csv"
SEMANTIC_MAP_DIR = ROOT / "notes/semantic-symbol-map.d"
FAMILY = re.compile(r"^g?(?P<region>French|Spanish)?ModelVariant(?P<number>\d+)_")

# Git writes each marker as exactly seven characters at the start of a line:
# "<<<<<<< label", "||||||| label" (diff3), "=======" alone, ">>>>>>> label".
CONFLICT = re.compile(r"^(?:<{7}|>{7}|\|{7})(?:\s|$)|^={7}\s*$")

ROW = re.compile(r"\|\s*`(?P<source>src/[^`]+)`\s*\|\s*`(?P<profile>[^`]+)`\s*\|(?P<rest>.*)")
ADDRESS = re.compile(r"`(0x[0-9A-Fa-f]{8})`")


def conflict_markers(text: str) -> list[tuple[int, str]]:
    return [
        (number, line)
        for number, line in enumerate(text.splitlines(), 1)
        if CONFLICT.match(line)
    ]


def family_stem(name: str) -> str | None:
    """Return the per-family file stem for a MODEL variant name, else None."""
    match = FAMILY.match(name)
    if match is None:
        return None
    region = match.group("region")
    prefix = f"{region.lower()}-" if region else ""
    return f"{prefix}model-variant-{match.group('number')}"


def family_files() -> list[Path]:
    files: list[Path] = []
    if SEMANTIC_MAP_DIR.is_dir():
        files.extend(sorted(SEMANTIC_MAP_DIR.glob("*.csv")))
    return files


def family_layout_problems() -> list[str]:
    problems: list[str] = []

    def where(path: Path, number: int) -> str:
        return f"{path.relative_to(ROOT)}:{number}"

    map_lines = SEMANTIC_MAP.read_text(encoding="utf-8").splitlines()
    map_header = map_lines[0] if map_lines else ""
    for number, line in enumerate(map_lines[1:], 2):
        row = next(csv.reader(io.StringIO(line)), [])
        if (len(row) > 2 and row[0].startswith("overlay/")
                and family_stem(row[2]) is not None):
            problems.append(
                f"{where(SEMANTIC_MAP, number)}: {row[2]} belongs in "
                f"notes/semantic-symbol-map.d/{family_stem(row[2])}.csv"
            )

    for path in family_files():
        lines = path.read_text(encoding="utf-8").splitlines()
        if path.suffix == ".csv":
            if not lines or lines[0] != map_header:
                problems.append(
                    f"{where(path, 1)}: header differs from "
                    f"{SEMANTIC_MAP.relative_to(ROOT)}"
                )
                continue
            for number, line in enumerate(lines[1:], 2):
                row = next(csv.reader(io.StringIO(line)), [])
                name = row[2] if len(row) > 2 else ""
                if not row[0:1] or not row[0].startswith("overlay/"):
                    problems.append(f"{where(path, number)}: not an overlay row")
                elif family_stem(name) != path.stem:
                    problems.append(
                        f"{where(path, number)}: {name or '(no name)'} "
                        f"does not belong in {path.name}"
                    )

    return problems


# (line, is a conflict marker)
CONTROLS = (
    ("<<<<<<< HEAD", True),
    ("=======", True),
    (">>>>>>> 3d3054dc (Coalesce the developer effect-preview screen (#39))", True),
    ("||||||| merged common ancestors", True),
    # A table row is never a marker, including one whose text contains them.
    ("| `src/game/main_services.c` | `gcc_2_8_1_g8_split` | The pump |", False),
    ("| `src/game/x.c` | `p` | writes ======= and <<<<<<< inline |", False),
    # Nor is ordinary prose or a shorter run of the same characters.
    ("The four are the whole run between graphics_frame.c and func_800134E0",
     False),
    ("=====", False),
    ("<<<<<< six is not a marker", False),
)


def self_test() -> int:
    failures = 0
    for line, expected in CONTROLS:
        if bool(CONFLICT.match(line)) != expected:
            failures += 1
            print(f"self-test: {line!r}: expected {expected}")
    if failures:
        print(f"self-test: {failures} of {len(CONTROLS)} controls failed")
        return 1
    print(f"self-test: OK ({len(CONTROLS)} controls)")
    return 0


def load_entries() -> dict[str, dict[str, str]]:
    entries: dict[str, dict[str, str]] = {}

    def walk(node: object) -> None:
        if isinstance(node, dict):
            if "address" in node and "source" in node:
                entries[str(node["address"]).upper()] = {
                    "source": str(node["source"]),
                    "profile": str(node.get("profile", "")),
                }
            for value in node.values():
                walk(value)
        elif isinstance(node, list):
            for value in node:
                walk(value)

    walk(json.loads(MATCHING.read_text(encoding="utf-8")))
    return entries


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--self-test", action="store_true",
                        help="check the conflict-marker controls and exit")
    args = parser.parse_args()
    if args.self_test:
        return self_test()

    if Path.cwd().resolve() != ROOT:
        print("error: run this command from the repository root", file=sys.stderr)
        return 1

    note_text = NOTE.read_text(encoding="utf-8")

    # Before anything else, and on both inputs: a file with markers in it is
    # not worth validating, and the row parser would skip them silently.
    marker_problems = []
    marker_inputs = [(NOTE, note_text),
                     (MATCHING, MATCHING.read_text(encoding="utf-8"))]
    marker_inputs.extend(
        (path, path.read_text(encoding="utf-8"))
        for path in (SEMANTIC_MAP, *family_files())
    )
    for path, text in marker_inputs:
        for number, line in conflict_markers(text):
            marker_problems.append(
                f"{path.relative_to(ROOT)}:{number}: "
                f"unresolved merge conflict marker: {line.rstrip()}"
            )
    if marker_problems:
        for problem in marker_problems:
            print(f"error: {problem}", file=sys.stderr)
        return 1

    entries = load_entries()
    by_source: dict[str, set[str]] = {}
    for address, entry in entries.items():
        by_source.setdefault(entry["source"], set()).add(address)

    problems: list[str] = family_layout_problems()
    rows = 0

    for number, line in enumerate(note_text.splitlines(), 1):
        match = ROW.match(line)
        if match is None:
            continue
        rows += 1
        source = match.group("source")
        profile = match.group("profile")
        where = f"{NOTE.relative_to(ROOT)}:{number}"

        if not (ROOT / source).is_file():
            problems.append(f"{where}: {source} does not exist")
        if source not in by_source:
            problems.append(f"{where}: {source} is not a matching_c source")
            continue

        profiles = {entries[a]["profile"] for a in by_source[source]}
        if profile not in profiles:
            problems.append(
                f"{where}: profile {profile} is not {sorted(profiles)}"
            )

        for address in ADDRESS.findall(match.group("rest")):
            owner = entries.get(address.upper())
            if owner is None:
                problems.append(f"{where}: {address} is not a matched function")
            elif owner["source"] != source:
                problems.append(
                    f"{where}: {address} belongs to {owner['source']}"
                )

    for problem in problems:
        print(f"error: {problem}", file=sys.stderr)
    if problems:
        return 1
    print(f"grouped translation units: OK ({rows} rows)")
    print(f"MODEL variant family files: OK ({len(family_files())} files)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
