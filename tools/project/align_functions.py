#!/usr/bin/env python3
"""Align a candidate's instructions against the retail target.

A positional diff answers "which offsets disagree". That is the wrong question
once the two sides have different instruction counts: one inserted or missing
instruction shifts everything after it, so the count then measures the offset
rather than the difference, and window comparisons across the shift are not
meaningful at all.

This aligns the two instruction streams instead and reports what is *inserted*
or *missing*, which is the question worth asking in that regime. Immediates are
stripped so that alignment keys on opcode and registers only; a relocated or
otherwise differing constant should not stop two instructions from pairing up.

Usage:
    align_functions.py <rom_offset_hex> <size_hex> <candidate>

<candidate> is either an ELF object, whose .text is used, or a raw binary.
Both offset and size are hexadecimal, as they appear in the inventory. The ROM
offset of a resident address is `addr - 0x80010000 + 0x800`.
"""
from __future__ import annotations

import difflib
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from workspace import require_workspace_root  # noqa: E402

OBJDUMP = "tools/toolchains/binutils-2.42/bin/mipsel-none-elf-objdump"
TARGET = "game/SLUS_014.11"
LINE = re.compile(r"\s*([0-9a-f]+):\s+([0-9a-f]{8})\s+(.*)")
NUMBER = re.compile(r"-?\b(?:0x)?[0-9a-fA-F]+\b")


def disassemble(root: Path, data: bytes, scratch: Path) -> list[str]:
    scratch.parent.mkdir(parents=True, exist_ok=True)
    scratch.write_bytes(data)
    out = subprocess.run(
        [str(root / OBJDUMP), "-D", "-b", "binary", "-m", "mips:3000", "-EL",
         str(scratch)],
        capture_output=True, text=True, check=True).stdout
    return [re.sub(r"\s+", " ", m.group(3).strip())
            for line in out.splitlines() if (m := LINE.match(line))]


def candidate_text(root: Path, path: Path) -> bytes:
    data = path.read_bytes()
    if not data.startswith(b"\x7fELF"):
        return data
    out = root / "tmp/align/candidate.text.bin"
    out.parent.mkdir(parents=True, exist_ok=True)
    objcopy = str(root / OBJDUMP).replace("objdump", "objcopy")
    subprocess.run([objcopy, "-O", "binary", "--only-section=.text",
                    str(path), str(out)], check=True)
    return out.read_bytes()


def key(line: str) -> str:
    """Opcode and registers, with every numeric literal removed."""
    return NUMBER.sub("#", line.split(";")[0].strip())


def main(argv: list[str]) -> int:
    if len(argv) != 4:
        print(__doc__, file=sys.stderr)
        return 2
    root = require_workspace_root()
    offset, size = int(argv[1], 16), int(argv[2], 16)
    target = disassemble(
        root, (root / TARGET).read_bytes()[offset:offset + size],
        root / "tmp/align/target.bin")
    candidate = disassemble(
        root, candidate_text(root, Path(argv[3])),
        root / "tmp/align/candidate.bin")

    a, b = [key(x) for x in target], [key(x) for x in candidate]
    matcher = difflib.SequenceMatcher(None, a, b, autojunk=False)
    print(f"target {len(a)} instructions, candidate {len(b)}"
          f" ({len(b) - len(a):+d})")
    aligned = 0
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == "equal":
            aligned += i2 - i1
            continue
        delta = (i2 - i1) - (j2 - j1)
        print(f"\n@@ target[{i1}:{i2}] candidate[{j1}:{j2}] {tag}"
              f" delta={delta:+d}")
        for k in range(i1, i2):
            print(f"   -{k:5d} {target[k]}")
        for k in range(j1, j2):
            print(f"   +{k:5d} {candidate[k]}")
    print(f"\naligned on opcode and registers: {aligned}/{len(a)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
