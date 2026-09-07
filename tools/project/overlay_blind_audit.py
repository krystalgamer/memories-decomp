"""Register-blind content audit of every stored overlay candidate.

The differing-position count stops responding once a block is displaced, and
the opcode distance never sees an immediate at all.  This compares the two
instruction streams as multisets with the register fields masked out, which
leaves opcode, funct, shamt and immediate -- so a wrong struct offset, a wrong
divisor or a wrong constant shows up even when both headline metrics are flat.

Relocated fields are noise here (the candidate's are still zero), so entries
whose candidate-side immediate is zero are reported separately.
"""
from __future__ import annotations

import collections
import contextlib
import io
import re
import sys
from pathlib import Path

sys.path.insert(0, "tools/project")

from overlay_diff import (  # noqa: E402
    candidate_words,
    inventory_entry,
    load_module,
    target_words,
)
from workspace import require_workspace_root  # noqa: E402

HEADING = re.compile(r"^## (\S+) `(\w+)` at (0x[0-9A-Fa-f]+)", re.MULTILINE)
MODULES = {
    "password": ["password"],
    "main_menu": ["main_menu"],
    "free_duel": ["free_duel"],
    "overworld": ["overworld_before_coup", "overworld_after_coup"],
}


def strip_regs(word: int) -> int:
    """Zero the register fields, keeping opcode, funct, shamt and immediate."""
    op = word >> 26
    if op == 0:
        return word & 0x000007FF
    if op in (2, 3):
        return word
    return (op << 26) | (word & 0xFFFF)


def candidates(root: Path):
    store = root / "notes/overlays/candidates"
    for path in sorted(store.glob("func_*.md")):
        text = path.read_text(encoding="utf-8")
        for match in HEADING.finditer(text):
            rest = text[match.end():]
            start = rest.index("```c") + 5
            end = rest.index("```", start)
            yield (match.group(1), match.group(2),
                   int(match.group(3), 16), rest[start:end])


def main() -> int:
    root = require_workspace_root()
    scratch = Path("tmp/blind-audit")
    scratch.mkdir(parents=True, exist_ok=True)
    for group, name, address, source in candidates(root):
        module = MODULES[group][0]
        path = scratch / f"{name}.c"
        # The store keeps the include an integrated source uses, which resolves
        # from src/overlays/<module>/. This scratch directory is somewhere else,
        # so rewrite it on the way out rather than storing a path that would
        # break the moment a candidate is promoted.
        path.write_text(
            source.replace('#include "../../types.h"',
                           '#include "../../src/types.h"'),
            encoding="utf-8",
        )
        size = int(inventory_entry(root, module, address)["size"], 16)
        target = target_words(root, load_module(root, module), address, size)
        try:
            with contextlib.redirect_stdout(io.StringIO()):
                cand, _, _ = candidate_words(
                    root, str(path), "gcc_2_8_1_g0_split", address, name, size
                )
        except Exception as exc:  # a candidate that no longer builds is a finding
            print(f"{name}: BUILD FAILED {exc}"[:160])
            continue
        left = collections.Counter(strip_regs(w) for w in target)
        right = collections.Counter(strip_regs(w) for w in cand)
        blind = sum((left - right).values()) + sum((right - left).values())
        # An immediate the candidate actually chose, rather than a zeroed relocation.
        chosen = {
            k: v for k, v in (right - left).items()
            if (k >> 26) == 0 or (k & 0xFFFF) != 0
        }
        print(f"{name:<40} n={len(cand):>3}/{len(target):<3} blind={blind:>4}")
        for key, count in sorted(chosen.items()):
            op = key >> 26
            print(f"    candidate chose {count}x op={op:<2} funct/imm=0x{key & 0xFFFF:04x}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
