#!/usr/bin/env python3
"""Check that notes/grouped-translation-units.md matches the build config.

The table in that note is an index of every grouped translation unit: its
source path, its compiler profile, and the addresses it covers. All three
drift silently when translation units are merged or renamed, because nothing
compiles the note. This checks each row against
config/slus_01411/matching_c.json, which is what the build actually uses.
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NOTE = ROOT / "notes/grouped-translation-units.md"
MATCHING = ROOT / "config/slus_01411/matching_c.json"

ROW = re.compile(r"\|\s*`(?P<source>src/[^`]+)`\s*\|\s*`(?P<profile>[^`]+)`\s*\|(?P<rest>.*)")
ADDRESS = re.compile(r"`(0x[0-9A-Fa-f]{8})`")


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
    if Path.cwd().resolve() != ROOT:
        print("error: run this command from the repository root", file=sys.stderr)
        return 1

    entries = load_entries()
    by_source: dict[str, set[str]] = {}
    for address, entry in entries.items():
        by_source.setdefault(entry["source"], set()).add(address)

    problems: list[str] = []
    rows = 0

    for number, line in enumerate(NOTE.read_text(encoding="utf-8").splitlines(), 1):
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
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
