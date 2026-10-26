"""Check the actual signed coordinate reads without changing the copied buffer."""

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
#include "src/psyq/libgte.h"
#include "src/psyq/libgpu.h"
#include "src/psyq/libgs.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
"""
WITNESS = HEADERS + r"""
typedef struct { u8 before[16]; SVECTOR vector; u8 after[16]; } GuardedVector;
typedef struct { u8 before[16]; GsRVIEW2 view; u8 after[16]; } GuardedView;
static GuardedVector input;
static GuardedView actual, expected;
static u8 expected_input[sizeof(input)];
void read_target(SVECTOR *p, GsRVIEW2 *view);

static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
static s32 signed16(u32 value)
{
    return value & 0x8000 ? (s32)value - 0x10000 : (s32)value;
}
static s32 equal(const u8 *a, const u8 *b, u32 count)
{
    while (count--) if (*a++ != *b++) return 0;
    return 1;
}
int main(void)
{
    u32 value, i, cases = 0;
    for (value = 0; value < 0x10000; value++) {
        for (i = 0; i < sizeof(input); i++)
            ((u8 *)&input)[i] = i * 19 + value;
        put16((u8 *)&input.vector, value);
        put16((u8 *)&input.vector + 2, value ^ 0x8000);
        put16((u8 *)&input.vector + 4, value ^ 0xFFFF);
        for (i = 0; i < sizeof(input); i++)
            expected_input[i] = ((u8 *)&input)[i];
        for (i = 0; i < sizeof(actual); i++) {
            ((u8 *)&actual)[i] = i * 37 + value;
            ((u8 *)&expected)[i] = ((u8 *)&actual)[i];
        }
        expected.view.vrx = signed16(value);
        expected.view.vry = signed16(value ^ 0x8000);
        expected.view.vrz = signed16(value ^ 0xFFFF);
        read_target(&input.vector, &actual.view);
        if (!equal((u8 *)&actual, (u8 *)&expected, sizeof(actual))) return 1;
        if (!equal((u8 *)&input, expected_input, sizeof(input))) return 2;
        cases++;
    }
    return cases == 65536 ? 0 : 3;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine() in ("x86_64", "i386", "i686")
    and shutil.which("cc"),
    "requires a Linux x86 compiler and ILP32 execution",
)
class LibraryViewTargetTests(unittest.TestCase):
    def run_witness(self, optimization, mutation=None):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/library_runtime.c").read_text()
        start = source.index("void Library_UpdateCardView(")
        body = source[start:source.index("\n}\n", start)]
        self.assertRegex(body, r"\bSVECTOR\s*\*\s*p\s*;")
        match = re.search(
            r"Model_CopySlotPosition\(0, \(u16 \*\)0x80181000\);\s*"
            r"p = \(SVECTOR \*\)0x80181000;\s*"
            r"(?P<reads>(?:gViewState_Current\.view\.\w+\s*=\s*[^;]+;\s*){3})",
            body,
        )
        self.assertIsNotNone(match)
        reads = match["reads"].replace("gViewState_Current.view.", "view->")
        if mutation == "unsigned-read":
            self.assertEqual(reads.count("p->vx"), 1)
            reads = reads.replace("p->vx", "(u16)p->vx")
        elif mutation == "axis-swap":
            self.assertEqual(reads.count("p->vy"), 1)
            reads = reads.replace("p->vy", "p->vz")
        elif mutation == "padding-write":
            reads += "\np->pad = 0;\n"
        else:
            self.assertIsNone(mutation)
        code = HEADERS + (
            "\nvoid read_target(SVECTOR *p, GsRVIEW2 *view) {\n" + reads + "\n}\n"
        )
        with tempfile.TemporaryDirectory(
            prefix="library-view-target-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            operations = directory / "operations.c"
            operations.write_text(code)
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

    def test_actual_reads_and_regression_controls(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                self.assertEqual(self.run_witness(optimization), 0)
        for mutation, result in (
            ("unsigned-read", 1), ("axis-swap", 1), ("padding-write", 2),
        ):
            with self.subTest(mutation=mutation):
                self.assertEqual(self.run_witness("-O2", mutation), result)


if __name__ == "__main__":
    unittest.main()
