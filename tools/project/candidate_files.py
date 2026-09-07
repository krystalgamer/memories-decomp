"""Maintain the per-function resident candidate store.

`notes/candidates.md` was a single serial-append file. Every agent that stored
a candidate appended to its end, so independent entries collided textually even
though they never overlap in content, and the same file blocked three or four
otherwise-ready pull requests for several cycles running.

A serial-append file also hides a worse failure than a conflict. An append
whose entry already exists on master merges with *no conflict at all*, so a
duplicate heading can land silently and `mergeable_state` cannot see it.

This script owns the replacement layout: one file per function under
`notes/candidates/`, named for the function's address, plus a generated index
in `notes/candidates/README.md`. Two agents storing different candidates now
touch different files, and a duplicate is a create-file collision rather than a
silent merge.

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
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LEGACY = ROOT / "notes/candidates.md"
STORE = ROOT / "notes/candidates"
INDEX = STORE / "README.md"
RULES = STORE / "rules.md"

INDEX_START = "<!-- BEGIN GENERATED CANDIDATE INDEX -->"
INDEX_END = "<!-- END GENERATED CANDIDATE INDEX -->"

HEADING = re.compile(r"^## `([^`]+)` at (0x[0-9A-Fa-f]{8})\s*$", re.M)


def parse_entries(text):
    """Split a candidates document into (name, address, body) triples."""
    marks = [(m.start(), m.group(1), m.group(2)) for m in HEADING.finditer(text)]
    out = []
    for i, (pos, name, addr) in enumerate(marks):
        end = marks[i + 1][0] if i + 1 < len(marks) else len(text)
        out.append((name, addr.lower(), text[pos:end].rstrip("\n") + "\n"))
    return out


def preamble(text):
    """Everything before the first entry heading."""
    m = HEADING.search(text)
    return text[: m.start()].rstrip("\n") + "\n" if m else text


def entry_path(address):
    return STORE / f"func_{address[2:].upper()}.md"


def load_store():
    """Return [(name, address, path)] for every stored candidate."""
    found = []
    for path in sorted(STORE.glob("func_*.md")):
        text = path.read_text(encoding="utf-8")
        m = HEADING.search(text)
        if m is None:
            raise SystemExit(
                f"{path.relative_to(ROOT)}: no '## `name` at 0xADDRESS' heading"
            )
        if len(HEADING.findall(text)) != 1:
            raise SystemExit(
                f"{path.relative_to(ROOT)}: expected exactly one entry heading"
            )
        addr = m.group(2).lower()
        expected = entry_path(addr)
        if path != expected:
            raise SystemExit(
                f"{path.relative_to(ROOT)}: address {addr} belongs in "
                f"{expected.relative_to(ROOT)}"
            )
        found.append((m.group(1), addr, path))
    return found


def summarise(path):
    """First non-empty line after the heading, used as the index summary."""
    lines = path.read_text(encoding="utf-8").splitlines()
    for i, line in enumerate(lines):
        if HEADING.match(line + "\n"):
            for follow in lines[i + 1:]:
                if follow.strip():
                    return " ".join(follow.split())
            break
    return ""


def render_index(entries):
    rows = [
        "| function | address | state |",
        "| --- | --- | --- |",
    ]
    for name, addr, path in sorted(entries, key=lambda e: e[1]):
        state = summarise(path).replace("|", r"\|")
        rows.append(
            f"| [`{name}`]({path.name}) | `{addr.upper()}` | {state} |"
        )
    rows.append("")
    rows.append(f"{len(entries)} stored candidate"
                f"{'' if len(entries) == 1 else 's'}.")
    return "\n".join(rows)


def write_index(entries):
    """Produce the whole index file from the tracked rules and the entries."""
    if not RULES.is_file():
        raise SystemExit(f"{RULES.relative_to(ROOT)} not found")
    generated = (
        f"{RULES.read_text(encoding='utf-8').rstrip()}\n\n"
        f"{INDEX_START}\n\n{render_index(entries)}\n\n{INDEX_END}\n"
    )
    if not INDEX.is_file() or INDEX.read_text(encoding="utf-8") != generated:
        INDEX.write_text(generated, encoding="utf-8")
        return True
    return False


def split():
    if not LEGACY.is_file():
        raise SystemExit(f"{LEGACY.relative_to(ROOT)} not found")
    text = LEGACY.read_text(encoding="utf-8")
    entries = parse_entries(text)
    if not entries:
        raise SystemExit("no entries found in the legacy file")
    STORE.mkdir(parents=True, exist_ok=True)

    seen = {}
    for name, addr, body in entries:
        if addr in seen:
            raise SystemExit(
                f"duplicate entry for {addr}: '{seen[addr]}' and '{name}'"
            )
        seen[addr] = name
        entry_path(addr).write_text(body, encoding="utf-8")

    RULES.write_text(
        preamble(text)
        + "\n"
        + "Each stored candidate is one file in this directory, named for the\n"
        + "function's address. `README.md` is a generated index of them and is\n"
        + "not tracked; run `make candidate-index` to produce it.\n",
        encoding="utf-8",
    )
    write_index(load_store())
    LEGACY.unlink()
    print(f"split {len(entries)} entries into {STORE.relative_to(ROOT)}")
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--split", action="store_true",
                        help="one-time migration from notes/candidates.md")
    parser.add_argument("--check", action="store_true",
                        help="verify the index is current")
    args = parser.parse_args()

    if args.split:
        return split()

    entries = load_store()
    if args.check:
        # load_store already rejects a missing or duplicated heading, an
        # address that disagrees with its filename, and more than one entry per
        # file. The index is generated, so there is no committed copy to
        # compare against and nothing here can be stale.
        seen = {}
        for name, addr, path in entries:
            if addr in seen:
                raise SystemExit(
                    f"duplicate entry for {addr}: {seen[addr]} and {path.name}"
                )
            seen[addr] = path.name
        if not RULES.is_file():
            raise SystemExit(f"{RULES.relative_to(ROOT)} not found")
        print(f"candidate store: OK ({len(entries)} entries)")
        return 0

    changed = write_index(entries)
    print(f"{'wrote' if changed else 'unchanged'}: "
          f"{INDEX.relative_to(ROOT)} ({len(entries)} entries)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
