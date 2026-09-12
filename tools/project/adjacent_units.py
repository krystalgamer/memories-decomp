#!/usr/bin/env python3
"""Report the source files that are adjacent in the image but still apart.

Issue #39 asks for sources to be merged by meaning and closeness. Closeness is
cheap to measure and nobody was measuring it: two sources whose functions abut
in the address space can be joined without moving anything, because the linker
script is regenerated from object sizes and a concatenation in address order
produces the same bytes.

Two things gate such a merge, and both are mechanical:

  * the profile. generate_build_config.py refuses a grouped source whose
    members disagree ("grouped functions use multiple profiles"), so a boundary
    between two profiles needs a clean build proving the two sides are
    byte-identical under one of them before any source moves. Some are -- three
    merges in #39 so far turned out to be recorded history rather than a
    requirement -- and some are not: the -fno-strength-reduce boundaries in the
    AI search run resist unification in both directions.
  * the meaning. Adjacency alone is not a reason. The strongest evidence that
    two sources are one unit is a header that already declares every function
    of both, which happens whenever a unit was split by #3859 and one of its
    members has since been re-matched.

The other direction is weaker than it first looks, and the verdict name says
so. A source whose functions are declared in a header that is not its own is
worth reading before merging -- func_800134B4.c abuts view_state_orbit.c and
its own file comment says it belongs with the per-frame service pump in
main_services.c -- but the same flag fires on a source that simply has no
header of its own and is declared in its subsystem's. Eight of today's nine
OTHER-HEADER rows are that second shape, and #4131 merged one of them
(func_8004A6F8.c into sound_voice_envelope.c) on its merits. So OTHER-HEADER
means "read the headers first", not "do not merge". Which of the two shapes a
row is cannot be told apart mechanically here: the headers that gave the
strongest JOIN evidence -- duel_effect_command.h, sound_voice_selection.h,
mem_card_io_result_callbacks.h -- each span several sources too, precisely
because their unit is split, so "spans many sources" separates nothing.

This prints those verdicts. It checks nothing and fails nothing; it is a
worklist, and the meaning call at the end of it is a human one.
"""

from __future__ import annotations

import argparse
import collections
import csv
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MATCHING = ROOT / "config/slus_01411/matching_c.json"
INVENTORY = ROOT / "config/slus_01411/functions.csv"

JOIN = "JOIN"
PROFILE = "PROFILE"
ELSEWHERE = "OTHER-HEADER"
ONLY = "ADJACENT-ONLY"


def verdict(profiles_a: set[str], profiles_b: set[str],
            headers_a: set[str], headers_b: set[str],
            own_a: str, own_b: str) -> str:
    """Classify one adjacent boundary.

    profiles_*  every profile recorded for that source
    headers_*   basenames of the headers declaring ALL of that source's functions
    own_*       that source's own header basename, whether or not it exists
    """
    if len(profiles_a) != 1 or profiles_a != profiles_b:
        return PROFILE
    if headers_a & headers_b:
        return JOIN
    foreign_a = headers_a - {own_a}
    foreign_b = headers_b - {own_b}
    if foreign_a or foreign_b:
        return ELSEWHERE
    return ONLY


# (profiles A, profiles B, headers A, headers B, own A, own B, expected)
CONTROLS = (
    # One shared header and one profile is the case worth acting on.
    ({"g0"}, {"g0"}, {"u.h"}, {"u.h"}, "a.h", "b.h", JOIN),
    # A source's own header naming only itself is no evidence either way.
    ({"g0"}, {"g0"}, {"a.h"}, {"b.h"}, "a.h", "b.h", ONLY),
    ({"g0"}, {"g0"}, set(), set(), "a.h", "b.h", ONLY),
    # A header that is not the source's own is flagged for reading, on either
    # side. It is not a refusal: the same shape covers a source that has no
    # header of its own and is declared in its subsystem's.
    ({"g0"}, {"g0"}, {"main_services.h"}, set(), "a.h", "b.h", ELSEWHERE),
    ({"g0"}, {"g0"}, set(), {"sound.h"}, "a.h", "b.h", ELSEWHERE),
    # A source declared in both its own header and another still counts.
    ({"g0"}, {"g0"}, {"a.h", "sound.h"}, set(), "a.h", "b.h", ELSEWHERE),
    # The profile gate comes first: a shared header does not skip it.
    ({"g0"}, {"g8"}, {"u.h"}, {"u.h"}, "a.h", "b.h", PROFILE),
    # A source recorded at two profiles cannot absorb anything either.
    ({"g0", "g8"}, {"g0", "g8"}, {"u.h"}, {"u.h"}, "a.h", "b.h", PROFILE),
)


def self_test() -> int:
    failures = 0
    for *args, expected in CONTROLS:
        got = verdict(*args)
        if got != expected:
            failures += 1
            print(f"self-test: {args!r}: expected {expected}, got {got}")
    if failures:
        print(f"self-test: {failures} of {len(CONTROLS)} controls failed")
        return 1
    print(f"self-test: OK ({len(CONTROLS)} controls)")
    return 0


def load() -> tuple[list[dict], dict[int, str]]:
    functions = json.loads(MATCHING.read_text(encoding="utf-8"))["functions"]
    functions.sort(key=lambda entry: int(str(entry["address"]), 0))
    with INVENTORY.open("r", encoding="utf-8", newline="") as handle:
        names = {int(row["address"], 0): row["name"]
                 for row in csv.DictReader(handle)}
    return functions, names


def headers_declaring(names: list[str], texts: dict[str, str]) -> set[str]:
    """Headers that declare every one of these names. Empty if any name is
    unknown -- a partial match is not evidence that a header owns the unit."""
    if not names or not all(names):
        return set()
    patterns = [re.compile(r"\b" + re.escape(name) + r"\s*\(") for name in names]
    return {Path(path).name for path, text in texts.items()
            if all(pattern.search(text) for pattern in patterns)}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--self-test", action="store_true",
                        help="check the verdict controls and exit")
    parser.add_argument("--verdict", action="append", choices=[JOIN, PROFILE,
                        ELSEWHERE, ONLY], help="only print these verdicts")
    parser.add_argument("--prefix", default="src/game/",
                        help="source prefix to report on (default src/game/)")
    args = parser.parse_args()
    if args.self_test:
        return self_test()
    if Path.cwd().resolve() != ROOT:
        print("error: run this command from the repository root", file=sys.stderr)
        return 1

    functions, names = load()
    by_address = {int(str(entry["address"]), 0): entry for entry in functions}
    profiles: dict[str, set[str]] = collections.defaultdict(set)
    members: dict[str, list[int]] = collections.defaultdict(list)
    for entry in functions:
        address = int(str(entry["address"]), 0)
        profiles[entry["source"]].add(entry["profile"])
        members[entry["source"]].append(address)

    texts = {str(path.relative_to(ROOT)): path.read_text(encoding="utf-8")
             for path in sorted((ROOT / args.prefix).glob("*.h"))}
    owners = {source: headers_declaring([names.get(a, "") for a in addresses],
                                        texts)
              for source, addresses in members.items()
              if source.startswith(args.prefix)}

    wanted = set(args.verdict) if args.verdict else None
    counts: collections.Counter = collections.Counter()
    seen: set[tuple[str, str]] = set()
    rows: list[tuple[str, str, str, str]] = []
    for entry in functions:
        left = entry["source"]
        if not left.startswith(args.prefix):
            continue
        end = int(str(entry["address"]), 0) + int(str(entry["size"]), 0)
        neighbour = by_address.get(end)
        if neighbour is None:
            continue
        right = neighbour["source"]
        if right == left or not right.startswith(args.prefix):
            continue
        if (left, right) in seen:
            continue
        seen.add((left, right))
        call = verdict(profiles[left], profiles[right],
                       owners.get(left, set()), owners.get(right, set()),
                       Path(left).with_suffix(".h").name,
                       Path(right).with_suffix(".h").name)
        counts[call] += 1
        if wanted is None or call in wanted:
            shared = owners.get(left, set()) & owners.get(right, set())
            note = ", ".join(sorted(shared)) if shared else ""
            if call == PROFILE:
                note = "%s | %s" % (",".join(sorted(profiles[left])),
                                    ",".join(sorted(profiles[right])))
            elif call == ELSEWHERE:
                foreign = ((owners.get(left, set()) -
                            {Path(left).with_suffix(".h").name}) |
                           (owners.get(right, set()) -
                            {Path(right).with_suffix(".h").name}))
                note = ", ".join(sorted(foreign))
            rows.append((call, Path(left).name, Path(right).name, note))

    width = max((len(row[1]) for row in rows), default=0)
    for call, left, right, note in rows:
        print("%-16s %-*s -> %-34s %s" % (call, width, left, right, note))
    print("adjacent boundaries under %s: %d (%s)" % (
        args.prefix, sum(counts.values()),
        ", ".join("%s %d" % (key, counts[key])
                  for key in (JOIN, PROFILE, ELSEWHERE, ONLY) if counts[key])))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
