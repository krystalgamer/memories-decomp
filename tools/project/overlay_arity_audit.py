"""Compare per-call argument-register usage between target and candidate.

A value the target loads into a0-a3 shortly before a `jal` is an argument to
that call.  That is a fact about the target rather than a hypothesis about the
source, so it pins the callee's arity independently of anything the candidate
believes.

This matters because a missing argument does not present as a missing
argument.  `CampaignMap_UpdateLocationTransition` sat at five differing
positions that all read as register allocation: the same instruction mix, an
opcode distance of zero, two `lui`s in the other order and one value in `v1`
where the target had `a1`.  The call took two arguments and the candidate
passed one, and no amount of reordering can move a value into an argument
register.

Neither the differing-position count nor the opcode distance can see this, and
neither can the register-blind multiset in overlay_blind_audit.py, because that
one deliberately discards exactly the register fields this depends on.

Reported per call site: the highest argument register written in the run of
instructions before each `jal`, which is a lower bound on the arity.

Two caveats keep this honest.  The count is a lower bound, because an argument
already in the right register needs no instruction at all, and `a0`-`a3` are
caller-saved, so GCC may write one as a scratch register for reasons unrelated
to any call.  A target that sets *more* argument registers than the candidate
is therefore the actionable direction; the reverse is advisory.  When the two
streams differ in length, calls are paired positionally and may not correspond.

Validated against three sites whose answers are known independently:
`CampaignMap_UpdateLocationTransition` calling `0x801688BC`, where the audit
reports the two-argument shortfall that closed that match, and its calls to
`TextBox_GetGlyphAt` and `func_80168CDC`, where the reported arities of three
and two agree with the matched definitions already in the tree.
"""
from __future__ import annotations

import contextlib
import io
import sys
from pathlib import Path

sys.path.insert(0, "tools/project")

from overlay_blind_audit import MODULES, candidates  # noqa: E402
from overlay_diff import (  # noqa: E402
    candidate_words,
    inventory_entry,
    load_module,
    target_words,
)
from workspace import require_workspace_root  # noqa: E402

ARG_REGS = {4: "a0", 5: "a1", 6: "a2", 7: "a3"}
JAL = 3
SPECIAL = 0
JR = 0x08
JALR = 0x09
# Opcodes that write their rt field: the loads, plus addiu/slti/andi/ori/lui.
RT_WRITERS = set(range(0x20, 0x28)) | {0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}


def written_register(word: int) -> int | None:
    """The register this instruction defines, or None."""
    op = word >> 26
    if op == SPECIAL:
        funct = word & 0x3F
        if funct in (JR, JALR):
            return None
        return (word >> 11) & 0x1F  # rd
    if op in RT_WRITERS:
        return (word >> 16) & 0x1F  # rt
    return None


def is_transfer(word: int) -> bool:
    op = word >> 26
    if op in (2, 3):
        return True
    if op == SPECIAL:
        return (word & 0x3F) in (JR, JALR)
    return 0x04 <= op <= 0x07 or op == 0x01


def call_arity(words: list[int]) -> list[tuple[int, int, int]]:
    """For each `jal`, the (index, highest argument register index, callee).

    The scan walks back to the previous transfer, which bounds the basic block
    the argument setup has to live in.  A register written twice still counts
    once; the result is the count of distinct argument registers defined, which
    is a lower bound on the arity because an argument already in place needs no
    instruction at all.

    The callee is decoded from the `jal` field, so it is meaningful on the
    target side and zero on an unlinked candidate.
    """
    sites = []
    for i, word in enumerate(words):
        if (word >> 26) != JAL:
            continue
        # Walk back to the previous transfer to bound the basic block.
        j = i - 1
        while j >= 0 and not is_transfer(words[j]):
            j -= 1
        # words[j] is that transfer and words[j + 1] is *its* delay slot, which
        # belongs to the previous call, so argument setup for this one starts
        # at j + 2.  Skipping the slot is not the same as discarding whatever
        # register it wrote, because this block may legitimately write it again.
        start = 0 if j < 0 else min(j + 2, i)
        seen = set()
        for k in range(start, i):
            reg = written_register(words[k])
            if reg in ARG_REGS:
                seen.add(reg)
        # This call's own delay slot, by contrast, can set an argument.
        if i + 1 < len(words):
            reg = written_register(words[i + 1])
            if reg in ARG_REGS:
                seen.add(reg)
        callee = 0x80000000 | ((word & 0x03FFFFFF) << 2)
        sites.append((i, max(seen) - 3 if seen else 0, callee))
    return sites


def main() -> int:
    root = require_workspace_root()
    scratch = Path("tmp/arity-audit")
    scratch.mkdir(parents=True, exist_ok=True)
    findings = 0
    for group, name, address, source in candidates(root):
        module = MODULES[group][0]
        path = scratch / f"{name}.c"
        path.write_text(source, encoding="utf-8")
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
        left = call_arity(list(target))
        right = call_arity(list(cand))
        aligned = len(cand) == len(target)
        note = "" if aligned else "  (lengths differ; pairing is positional and advisory)"
        print(f"{name:<40} calls target={len(left):<3} candidate={len(right)}{note}")
        if len(left) != len(right):
            print("    call counts differ; sites are not comparable at all")
            findings += 1
            continue
        for (ti, ta, callee), (_, ca, _) in zip(left, right):
            if ta == ca:
                continue
            findings += 1
            if ta > ca:
                # The target has to set those registers for the call to work,
                # so this direction is a real shortfall in the candidate.
                mark = "MISSING ARGUMENT" if aligned else "missing argument?"
                detail = f"callee takes at least {ta}, candidate passes {ca}"
            else:
                # a0-a3 are caller-saved, so GCC may write one as a scratch
                # register for reasons that have nothing to do with the call.
                mark = "over-count"
                detail = f"candidate sets {ca} against the target's {ta}; may be scratch use"
            print(f"    index {ti:>4} calls 0x{callee:08X}  {mark}: {detail}")
    print(f"\n{findings} arity finding(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
