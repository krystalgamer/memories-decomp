"""Check the actual grid-state selectors against the complete guarded owner."""

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
HEADERS = r"""
#include "src/types.h"
#include "src/game/library_runtime.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
"""
WITNESS = HEADERS + r"""
typedef struct { u8 before[16]; LibraryState state; u8 after[16]; } GuardedState;
static GuardedState block;
static u8 expected[sizeof(block)];
int main(void)
{
    u32 value, i, offset, cases = 0;
    offset = (u8 *)&block.state.motion.state - (u8 *)&block;
    for (value = 0; value < 256; value++) {
        for (i = 0; i < sizeof(block); i++)
            ((u8 *)&block)[i] = i * 37 + value;
        block.state.motion.state = value;
        for (i = 0; i < sizeof(block); i++)
            expected[i] = ((u8 *)&block)[i];
        expected[offset] = 1;
        Library_EnterGridState(&block.state);
        for (i = 0; i < sizeof(block); i++)
            if (((u8 *)&block)[i] != expected[i]) return 1;
        cases++;
    }
    return cases == 256 ? 0 : 2;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine() in ("x86_64", "i386", "i686")
    and shutil.which("cc"),
    "requires a Linux x86 compiler and ILP32 execution",
)
class LibraryGridStateTests(unittest.TestCase):
    def run_witness(self, relative, optimization, mutation=None):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / relative).read_text()
        function = re.search(
            r"(?ms)^void Library_EnterGridState\(LibraryState \*state\)\s*\{.*?^\}",
            source,
        )
        self.assertIsNotNone(function)
        code = function.group()
        self.assertEqual(code.count("state->motion.state = 1;"), 1)
        if mutation == "retained-high-flags":
            code = code.replace(
                "state->motion.state = 1;",
                "state->motion.state = (state->motion.state & 0xF0) | 1;",
            )
        elif mutation == "word-store":
            code = code.replace(
                "state->motion.state = 1;", "*(u32 *)&state->motion.state = 1;"
            )
        else:
            self.assertIsNone(mutation)
        with tempfile.TemporaryDirectory(
            prefix="library-grid-state-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            operations = directory / "operations.c"
            operations.write_text(HEADERS + code)
            witness = directory / "witness.c"
            witness.write_text(WITNESS)
            start = directory / "start.S"
            start.write_text(START)
            binary = directory / "witness"
            environment = dict(os.environ, TMPDIR=str(directory))
            result = subprocess.run(
                [
                    shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                    "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                    "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                    "-Werror=incompatible-pointer-types", "-I", str(ROOT),
                    str(operations), str(witness), str(start), "-o", str(binary),
                ],
                cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
            )
            self.assertEqual(result.returncode, 0, result.stderr[-4000:])
            return subprocess.run(
                [str(binary)], cwd=ROOT, env=environment, timeout=180
            ).returncode

    def test_actual_selectors_and_regression_controls(self):
        for relative in ("src/game/library_runtime.c",):
            for optimization in ("-O0", "-O2"):
                with self.subTest(source=relative, optimization=optimization):
                    self.assertEqual(self.run_witness(relative, optimization), 0)
            for mutation in ("retained-high-flags", "word-store"):
                with self.subTest(source=relative, mutation=mutation):
                    self.assertEqual(self.run_witness(relative, "-O2", mutation), 1)


if __name__ == "__main__":
    unittest.main()
