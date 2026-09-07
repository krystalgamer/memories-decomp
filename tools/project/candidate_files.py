"""Maintain the per-function resident candidate store.

`notes/candidates.md` was a single serial-append file. Every agent that stored
a candidate appended to its end, so independent entries collided textually even
though they never overlap in content, and the same file blocked three or four
otherwise-ready pull requests for several cycles running.

A serial-append file also hides a worse failure than a conflict. An append
whose entry already exists on master merges with *no conflict at all*, so a
duplicate heading can land silently and `mergeable_state` cannot see it.

This script owns the replacement layout: one file per function under a store
directory, named for the function's address, plus a generated index in that
directory's README.md. Two agents storing different candidates now touch
different files, and a duplicate is a create-file collision rather than a
silent merge.

There are two stores. The resident one lives in `notes/candidates/`. The
overlay one lives in `notes/overlays/candidates/` and was a single serial-append
file for far longer, because this script's heading pattern never matched it and
so `--check` silently validated only the resident store. Overlay entries carry
a module name in the heading and are reconciled against the per-module
inventories rather than the resident one.

The index itself is a generated artifact rather than a tracked file. Every
candidate pull request used to append one line to a committed index, which
reintroduced the same serialization at a lower cost: a branch whose base
predated another candidate landing had to rebase purely to regenerate it.
`notes/candidates/README.md` is therefore untracked and produced on demand from
`rules.md` plus the stored entries, so a pull request adding a candidate touches
exactly one file that nobody else touches.

Modes:

  --split   one-time migration from the old single file
  (default) regenerate notes/candidates/README.md from rules.md and the entries
  --check   verify the store is well formed, without reading a committed index
"""

import argparse
import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

INDEX_START = "<!-- BEGIN GENERATED CANDIDATE INDEX -->"
INDEX_END = "<!-- END GENERATED CANDIDATE INDEX -->"


class Store:
    """One candidate store: where it lives and how its headings read."""

    def __init__(self, key, legacy, directory, heading, inventories):
        self.key = key
        self.legacy = ROOT / legacy
        self.dir = ROOT / directory
        self.heading = re.compile(heading, re.M)
        self.inventories = inventories

    @property
    def index(self):
        return self.dir / "README.md"

    @property
    def rules(self):
        return self.dir / "rules.md"

    def entry_path(self, address):
        return self.dir / f"func_{address[2:].upper()}.md"


STORES = {
    "resident": Store(
        "resident",
        "notes/candidates.md",
        "notes/candidates",
        r"^## `(?P<name>[^`]+)` at (?P<addr>0x[0-9A-Fa-f]{8})\s*$",
        ["config/slus_01411/functions.csv"],
    ),
    "overlays": Store(
        "overlays",
        "notes/overlays/candidates.md",
        "notes/overlays/candidates",
        r"^## (?P<module>\S+) `(?P<name>[^`]+)` at (?P<addr>0x[0-9A-Fa-f]{8})\s*$",
        sorted(
            str(p.relative_to(ROOT))
            for p in (ROOT / "config/slus_01411/overlays").glob("*_functions.csv")
        ),
    ),
}


def parse_entries(store, text):
    """Split a candidates document into (name, address, body) triples."""
    marks = [(m.start(), m.group("name"), m.group("addr"))
             for m in store.heading.finditer(text)]
    out = []
    for i, (pos, name, addr) in enumerate(marks):
        end = marks[i + 1][0] if i + 1 < len(marks) else len(text)
        out.append((name, addr.lower(), text[pos:end].rstrip("\n") + "\n"))
    return out


def preamble(store, text):
    """Everything before the first entry heading."""
    m = store.heading.search(text)
    return text[: m.start()].rstrip("\n") + "\n" if m else text


def load_store(store):
    """Return [(name, address, path)] for every stored candidate."""
    found = []
    for path in sorted(store.dir.glob("func_*.md")):
        text = path.read_text(encoding="utf-8")
        m = store.heading.search(text)
        if m is None:
            raise SystemExit(
                f"{path.relative_to(ROOT)}: no entry heading matching "
                f"{store.heading.pattern}"
            )
        if len(store.heading.findall(text)) != 1:
            raise SystemExit(
                f"{path.relative_to(ROOT)}: expected exactly one entry heading"
            )
        addr = m.group("addr").lower()
        expected = store.entry_path(addr)
        if path != expected:
            raise SystemExit(
                f"{path.relative_to(ROOT)}: address {addr} belongs in "
                f"{expected.relative_to(ROOT)}"
            )
        found.append((m.group("name"), addr, path))
    return found


def summarise(store, path):
    """First non-empty line after the heading, used as the index summary."""
    lines = path.read_text(encoding="utf-8").splitlines()
    for i, line in enumerate(lines):
        if store.heading.match(line + "\n"):
            for follow in lines[i + 1:]:
                if follow.strip():
                    return " ".join(follow.split())
            break
    return ""


def render_index(store, entries):
    rows = [
        "| function | address | state |",
        "| --- | --- | --- |",
    ]
    for name, addr, path in sorted(entries, key=lambda e: e[1]):
        state = summarise(store, path).replace("|", r"\|")
        rows.append(
            f"| [`{name}`]({path.name}) | `{addr.upper()}` | {state} |"
        )
    rows.append("")
    rows.append(f"{len(entries)} stored candidate"
                f"{'' if len(entries) == 1 else 's'}.")
    return "\n".join(rows)


def write_index(store, entries):
    """Produce the whole index file from the tracked rules and the entries."""
    if not store.rules.is_file():
        raise SystemExit(f"{store.rules.relative_to(ROOT)} not found")
    generated = (
        f"{store.rules.read_text(encoding='utf-8').rstrip()}\n\n"
        f"{INDEX_START}\n\n{render_index(store, entries)}\n\n{INDEX_END}\n"
    )
    if not store.index.is_file() or store.index.read_text(encoding="utf-8") != generated:
        store.index.write_text(generated, encoding="utf-8")
        return True
    return False


def split(store):
    if not store.legacy.is_file():
        raise SystemExit(f"{store.legacy.relative_to(ROOT)} not found")
    text = store.legacy.read_text(encoding="utf-8")
    entries = parse_entries(store, text)
    if not entries:
        raise SystemExit("no entries found in the legacy file")
    store.dir.mkdir(parents=True, exist_ok=True)

    seen = {}
    for name, addr, body in entries:
        if addr in seen:
            raise SystemExit(
                f"duplicate entry for {addr}: '{seen[addr]}' and '{name}'"
            )
        seen[addr] = name
        store.entry_path(addr).write_text(body, encoding="utf-8")

    store.rules.write_text(
        preamble(store, text)
        + "\n"
        + "Each stored candidate is one file in this directory, named for the\n"
        + "function's address. `README.md` is a generated index of them and is\n"
        + "not tracked; run `make candidate-index` to produce it.\n",
        encoding="utf-8",
    )
    write_index(store, load_store(store))
    store.legacy.unlink()
    print(f"split {len(entries)} entries into {store.dir.relative_to(ROOT)}")
    return 0


def matched_entries(store, entries):
    """Entries whose function is already matching_c in the inventory.

    The store's own rule is to delete an entry once its source is promoted, but
    nothing enforced it, so a stale entry could be picked up and worked on. It
    reads exactly like an open near miss.
    """
    status = {}
    for name in store.inventories:
        inventory = ROOT / name
        if not inventory.is_file():
            continue
        with inventory.open(encoding="utf-8", newline="") as handle:
            for row in csv.DictReader(handle):
                status[row["address"].lower()] = row["status"]
    return [
        (path.name, addr)
        for _, addr, path in entries
        if status.get(addr) == "matching_c"
    ]


def check(store):
    entries = load_store(store)
    # load_store already rejects a missing or duplicated heading, an address
    # that disagrees with its filename, and more than one entry per file. The
    # index is generated, so there is no committed copy to compare against and
    # nothing here can be stale.
    seen = {}
    for name, addr, path in entries:
        if addr in seen:
            raise SystemExit(
                f"duplicate entry for {addr}: {seen[addr]} and {path.name}"
            )
        seen[addr] = path.name
    if not store.rules.is_file():
        raise SystemExit(f"{store.rules.relative_to(ROOT)} not found")
    if store.legacy.is_file():
        raise SystemExit(
            f"{store.legacy.relative_to(ROOT)} still exists; entries belong in "
            f"{store.dir.relative_to(ROOT)}"
        )
    stale = matched_entries(store, entries)
    if stale:
        listing = ", ".join(f"{n} ({a})" for n, a in stale)
        raise SystemExit(
            "candidate entries for functions that already match, remove "
            f"them: {listing}"
        )
    print(f"candidate store ({store.key}): OK ({len(entries)} entries)")
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--split", action="store_true",
                        help="one-time migration from the legacy single file")
    parser.add_argument("--check", action="store_true",
                        help="verify the store is well formed")
    parser.add_argument("--store", choices=sorted(STORES) + ["all"],
                        default="all", help="which candidate store to act on")
    args = parser.parse_args()

    if args.store == "all":
        if args.split:
            raise SystemExit("--split needs an explicit --store")
        chosen = [STORES[k] for k in sorted(STORES)]
    else:
        chosen = [STORES[args.store]]

    if args.split:
        return split(chosen[0])

    for store in chosen:
        if args.check:
            check(store)
        else:
            entries = load_store(store)
            changed = write_index(store, entries)
            print(f"{'wrote' if changed else 'unchanged'}: "
                  f"{store.index.relative_to(ROOT)} ({len(entries)} entries)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
