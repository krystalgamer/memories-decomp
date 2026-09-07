"""Compile many overlay candidates at once and rank them by the match metrics.

`overlay_diff.py` answers "how close is this one candidate", which is the
right question when a lever is already chosen.  Finding the lever needs the
other question: "which of these several hundred spellings is closest".  Asked
serially that costs about thirty seconds a candidate, so in practice each axis
gets swept on its own and the combinations never get tried -- which is exactly
where matches hide, because levers are not independent.

This module compiles the whole cross product in parallel and sorts the result,
so a few hundred cells cost a couple of minutes rather than a couple of hours.

Used as a library, `sweep()` takes a dict of label to source text and a list of
profiles, and returns rows sorted best-first::

    from overlay_sweep import sweep, show
    rows = sweep("free_duel", 0x80168090, "FreeDuel_PlaceCursor",
                 {"a": source_a, "b": source_b}, profiles=["gcc_2_8_1_g0_split"])
    show(rows)

Used from the command line it sweeps one source across every profile, which is
the common "is this the wrong profile" check::

    tools/project/overlay_sweep.py free_duel 0x80168090 cand.c --all-profiles
"""

from __future__ import annotations

import argparse
import hashlib
import io
import itertools
import json
import os
import sys
from concurrent.futures import ProcessPoolExecutor
from contextlib import redirect_stdout
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from overlay_diff import (  # noqa: E402
    candidate_words,
    disassemble,
    inventory_entry,
    load_module,
    opcode_distance,
    target_words,
)
from workspace import require_workspace_root  # noqa: E402

SCRATCH = Path("tmp/overlay-sweep")

_TARGET: dict[tuple[str, int], tuple[list[int], int]] = {}


def target_for(root: Path, module: str, address: int) -> tuple[list[int], int]:
    key = (module, address)
    if key not in _TARGET:
        size = int(inventory_entry(root, module, address)["size"], 16)
        words = target_words(root, load_module(root, module), address, size)
        _TARGET[key] = (words, size)
    return _TARGET[key]


def profile_names(root: Path) -> list[str]:
    path = root / "config/slus_01411/compiler_profiles.json"
    return list(json.loads(path.read_text())["profiles"].keys())


def _cell(job):
    label, text, profile, module, address, name = job
    root = require_workspace_root()
    words, size = target_for(root, module, address)
    # The scratch file name is derived from the label, and every character the
    # file system might object to becomes an underscore.  Two labels that differ
    # only in punctuation therefore collapse to the same name -- "a==1|b<2" and
    # "a>=1|b<2" both become "a__1_b_2" -- and the parallel workers then race on
    # one file, so some cells measure a neighbour's source and the whole sweep is
    # quietly wrong.  A digest of the untouched label keeps the readable stem and
    # makes the collision impossible.
    key = f"{label}__{profile}"
    safe = "".join(c if c.isalnum() or c in "._-" else "_" for c in key)
    stem = f"{safe[:80]}__{hashlib.sha1(key.encode()).hexdigest()[:8]}"
    path = SCRATCH / f"{stem}.c"
    path.write_text(text)
    try:
        # A candidate that will not compile is a real measurement, not a crash.
        with redirect_stdout(io.StringIO()):
            cand, _, masks = candidate_words(root, str(path), profile, address, name, size)
    except Exception as exc:
        # Seven fields, like every other return: a build failure is a row in the
        # table, not an absence of one, and both consumers index the seventh.
        return (
            label,
            profile,
            None,
            None,
            None,
            f"{type(exc).__name__}: {exc}"[:110],
            None,
        )

    def masked(ws):
        return [w & masks[i] if i < len(masks) else w for i, w in enumerate(ws)]

    left, right = masked(words), masked(cand)
    if left == right:
        return (label, profile, len(cand), 0, 0, "MATCH", len(cand))
    positions = sum(
        1
        for i in range(max(len(left), len(right)))
        if i >= len(left) or i >= len(right) or left[i] != right[i]
    )
    return (
        label,
        profile,
        len(cand),
        opcode_distance(words, cand),
        positions,
        "",
        prefix(root, words, cand),
    )


def prefix(root: Path, words: list[int], cand: list[int]) -> int:
    """Length of the leading run of instructions whose mnemonics agree.

    Opcode distance is an L1 distance between two multisets, so it is blind to
    order and, worse, it cancels: an error that adds an instruction and an
    error that drops one sum to nothing. Once register allocation is the
    dominant residual that cancellation is ordinary, and the metric then
    punishes a correction for exposing the error that had been hiding behind
    it. A leading run cannot cancel, because it stops at the first
    disagreement, so it is the metric to steer by while the shape of the
    function is still being settled.

    Mnemonics rather than encodings, because the registers are exactly what is
    still wrong when the shape is right, and comparing encodings would stop at
    the first differing allocation and report nothing useful.
    """
    lhs = [line.split()[0] for line in disassemble(root, words)]
    rhs = [line.split()[0] for line in disassemble(root, cand)]
    i = 0
    while i < min(len(lhs), len(rhs)) and lhs[i] == rhs[i]:
        i += 1
    return i


def sweep(
    module,
    address,
    name,
    variants,
    profiles=("gcc_2_8_1_g0_split",),
    workers=None,
    sort="distance",
):
    """Compile every (variant, profile) cell.  Returns rows sorted best-first.

    A row is (label, profile, instructions, distance, positions, note, prefix),
    where note is "MATCH", a build error, or empty.  Rows that failed to build
    sort last rather than being dropped, because a spelling the compiler
    rejects is still an answer about that spelling.

    `sort` selects which metric leads.  "distance" is right while the candidate
    is still gaining and losing whole instructions; "prefix" is right once
    register allocation dominates, because opcode distance cancels there and
    can rank a strict improvement below what it improved on.
    """
    root = require_workspace_root()
    SCRATCH.mkdir(parents=True, exist_ok=True)
    target_for(root, module, address)
    jobs = [
        (label, text, profile, module, address, name)
        for (label, text), profile in itertools.product(variants.items(), profiles)
    ]
    workers = workers or min(len(jobs), os.cpu_count() or 4)
    with ProcessPoolExecutor(max_workers=workers) as pool:
        rows = list(pool.map(_cell, jobs))
    far = 1 << 30
    # positions of 0 is the best possible value, so it must not be treated as
    # missing; `or` would send an exact match to the bottom of the list.
    if sort == "prefix":
        key = lambda r: (r[3] is None, -(r[6] or 0), r[4] if r[4] is not None else far)
    elif sort == "distance":
        key = lambda r: (
            r[3] is None,
            r[3] if r[3] is not None else far,
            r[4] if r[4] is not None else far,
        )
    else:
        raise ValueError(f"unknown sort {sort!r}")
    rows.sort(key=key)
    return rows


def show(rows, limit=25):
    print(f"{'dist':>4} {'pfx':>4} {'pos':>4} {'n':>4}  {'label':38s} profile")
    for label, profile, n, distance, positions, note, pfx in rows[:limit]:
        if distance is None:
            print(f"{'ERR':>4} {'':>4} {'':>4} {'':>4}  {label:38s} {profile}  {note}")
            continue
        mark = "  *** MATCH ***" if note == "MATCH" else ""
        print(
            f"{distance:4d} {pfx:4d} {positions:4d} {n:4d}  "
            f"{label:38s} {profile}{mark}"
        )
    matches = [r for r in rows if r[5] == "MATCH"]
    print(f"({len(rows)} cells, {len(matches)} matching)")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("module")
    parser.add_argument("address")
    parser.add_argument("source")
    parser.add_argument("--name", help="function symbol; defaults to func_<address>")
    parser.add_argument("--profile", action="append", dest="profiles")
    parser.add_argument("--all-profiles", action="store_true")
    parser.add_argument("--limit", type=int, default=25)
    parser.add_argument("--sort", choices=("distance", "prefix"), default="distance")
    args = parser.parse_args()

    root = require_workspace_root()
    address = int(args.address, 16)
    name = args.name or inventory_entry(root, args.module, address)["name"]
    if args.all_profiles:
        profiles = profile_names(root)
    else:
        profiles = args.profiles or ["gcc_2_8_1_g0_split"]
    variants = {Path(args.source).stem: Path(args.source).read_text()}
    show(
        sweep(args.module, address, name, variants, profiles=profiles, sort=args.sort),
        limit=args.limit,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
