from __future__ import annotations

from pathlib import Path
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import align_functions

OBJDUMP = REPOSITORY / align_functions.OBJDUMP

# lui $v0, 0 / nop / nop / nop / jr $ra, little-endian. The three zero words in
# the middle are what objdump abbreviates to "..." unless it is told not to.
NOP = b"\x00\x00\x00\x00"
PROGRAM = b"\x00\x00\x02\x3c" + NOP * 3 + b"\x08\x00\xe0\x03"


@unittest.skipUnless(OBJDUMP.is_file(), "needs the binutils 2.42 toolchain")
class DisassembleTests(unittest.TestCase):
    """A repeated run of identical words must not disappear.

    objdump abbreviates a run of identical words to "...", so a disassembly
    taken without -z is missing them. That is silent: both sides of a
    comparison lose the same rows, the counts still look plausible, and a
    candidate whose only fault is a differing number of delay-slot nops
    compares as if it had no fault at all. The retail function at 0x800528AC
    is 288 words and reports 276 without -z.
    """

    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="align-functions-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.scratch = Path(temporary.name) / "program.bin"

    def test_every_word_is_disassembled(self) -> None:
        lines = align_functions.disassemble(REPOSITORY, PROGRAM, self.scratch)
        self.assertEqual(len(lines), len(PROGRAM) // 4)

    def test_the_repeated_run_is_kept(self) -> None:
        lines = align_functions.disassemble(REPOSITORY, PROGRAM, self.scratch)
        self.assertEqual(sum(1 for line in lines if line == "nop"), 3)
        self.assertNotIn("...", lines)


class KeyTests(unittest.TestCase):
    """The key must keep every register name and erase every literal.

    objdump spells the argument registers a0-a3 and ra as words made of hex
    digits, so a literal regex that means "a word of hex digits" erased them
    and reported `mult a1,a2` and `mult a2,a1` as the same row (#5358). The
    rows below are the control table from that issue: the first four pairs
    must differ, the last three must be equal.
    """

    def test_argument_registers_survive(self) -> None:
        for left, right in (
            ("mult\ta1,a2", "mult\ta2,a1"),
            ("mflo\tt1", "mflo\ta1"),
            ("sra\tv0,t1,0x7", "sra\ta3,a1,0x7"),
            ("lw\tv1,-19368(v1)", "lw\tv0,-19368(v0)"),
        ):
            with self.subTest(left=left, right=right):
                self.assertNotEqual(align_functions.key(left), align_functions.key(right))

    def test_literals_are_erased(self) -> None:
        for line, expected in (
            ("addiu\tv0,v0,4095", "addiu\tv0,v0,#"),
            ("sra\tv0,t1,0x7", "sra\tv0,t1,#"),
            ("lw\tv1,-19368(v1)", "lw\tv1,#(v1)"),
            ("lui\ta0,0x800f", "lui\ta0,#"),
            ("jal\t0x86e50", "jal\t#"),
            ("sw\tra,76(sp)", "sw\tra,#(sp)"),
        ):
            with self.subTest(line=line):
                self.assertEqual(align_functions.key(line), expected)


if __name__ == "__main__":
    unittest.main()
