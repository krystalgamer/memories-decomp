"""Check the actual nine-object loops against both declared pointer views."""

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
HEADERS = r"""
#include "src/types.h"
#include "src/game/library_state.h"
#include "src/game/display_object.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
"""
START = r"""
.text
.globl _start
_start:
    andl $-16, %esp
    call main
    movl %eax, %ebx
    movl $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""
WITNESS = HEADERS + r"""
typedef struct { u8 before[16]; LibraryState state; u8 after[16]; } GuardedState;
typedef struct { u8 before[16]; DisplayObject object; u8 after[16]; } GuardedObject;
typedef char state_offset[__builtin_offsetof(GuardedState, state) == 16 ? 1 : -1];
typedef char object_offset[__builtin_offsetof(GuardedObject, object) == 16 ? 1 : -1];
static GuardedState state_block;
static GuardedObject objects[9];
static u8 expected_state[sizeof(state_block)], expected_objects[sizeof(objects)];
static const u32 mappings[3][9] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8},
    {8, 7, 6, 5, 4, 3, 2, 1, 0},
    {0, 0, 1, 2, 3, 4, 5, 6, 8}
};
static const u32 edges[8] = {0, 1, 0x3F, 0x40, 0x7F, 0x8000, 0xFFBF, 0xFFFF};
void clear_objects(LibraryState *state);
void set_objects(LibraryState *state);

static u32 halfword(const u8 *p) { return p[0] | ((u32)p[1] << 8); }
static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
static s32 equal(const u8 *a, const u8 *b, u32 count)
{
    while (count--) if (*a++ != *b++) return 0;
    return 1;
}
static s32 matches(void)
{
    return equal((u8 *)&state_block, expected_state, sizeof(state_block))
        && equal((u8 *)objects, expected_objects, sizeof(objects));
}
int main(void)
{
    LibraryDisplayObjects *window = &state_block.state.motion.display_objects;
    u8 *actual = (u8 *)objects;
    u32 shape, sample, word, i, index, offset, cases = 0;
    for (shape = 0; shape < 3; shape++) {
        for (i = 0; i < sizeof(state_block); i++)
            ((u8 *)&state_block)[i] = i * 37 + shape * 13;
        for (i = 0; i < sizeof(objects); i++) actual[i] = i * 19 + shape * 7;
        for (i = 0; i < 8; i++)
            window->by_role.slots[i] = &objects[mappings[shape][i]].object;
        window->by_role.render = &objects[mappings[shape][8]].object;
        for (i = 0; i < 9; i++)
            if (window->all[i] != &objects[mappings[shape][i]].object) return 4;
        for (i = 0; i < sizeof(state_block); i++)
            expected_state[i] = ((u8 *)&state_block)[i];
        for (i = 0; i < sizeof(objects); i++) expected_objects[i] = actual[i];
        for (sample = 0; sample < (shape == 0 ? 65536 : 8); sample++) {
            word = shape == 0 ? sample : edges[sample];
            for (i = 0; i < 9; i++) {
                offset = i * sizeof(GuardedObject) + 16 + 8;
                put16(actual + offset, word ^ (i * 0x1111));
                put16(expected_objects + offset, word ^ (i * 0x1111));
            }
            for (i = 0; i < 9; i++) {
                index = mappings[shape][i];
                offset = index * sizeof(GuardedObject) + 16 + 8;
                put16(expected_objects + offset,
                      halfword(expected_objects + offset) & 0xFFBF);
            }
            clear_objects(&state_block.state);
            if (!matches()) return 1;
            for (i = 0; i < 9; i++) {
                index = mappings[shape][i];
                offset = index * sizeof(GuardedObject) + 16 + 8;
                put16(expected_objects + offset,
                      halfword(expected_objects + offset) | 0x40);
            }
            set_objects(&state_block.state);
            if (!matches()) return 2;
            cases++;
        }
    }
    return cases == 65552 ? 0 : 3;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine() in ("x86_64", "i386", "i686")
    and shutil.which("cc"),
    "requires a Linux x86 compiler and ILP32 execution",
)
class LibraryDisplayObjectsTests(unittest.TestCase):
    def run_witness(self, optimization, mutation=None):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        text = (ROOT / "src/game/library_runtime.c").read_text()
        function = re.search(
            r"(?ms)^void Library_UpdateCardView\(LibraryState \*state\)\s*\{.*?^\}",
            text,
        )
        self.assertIsNotNone(function)
        text = function.group()
        loops = []
        initializers = []
        for statement in (
            "state->motion.display_objects.all[i]->flags &= 0xFFBF;",
            "state->motion.display_objects.all[i]->flags |= 0x40;",
        ):
            match = re.search(
                r"(?m)^[ \t]*do \{\s*" + re.escape(statement)
                + r"\s*i\+\+;\s*\} while \(i < 9\);", text
            )
            self.assertIsNotNone(match, statement)
            assignments = list(re.finditer(
                r"(?m)^[ \t]*i = ([^;\n]+);", text[:match.start()]
            ))
            self.assertTrue(assignments)
            initializer = assignments[-1]
            self.assertEqual(initializer.group(1), "0")
            self.assertNotRegex(
                text[initializer.end():match.start()],
                r"\bi\s*(?:\+\+|--|[+\-*/%&|^]?=)",
            )
            initializers.append(initializer.group())
            loops.append(match.group())
        if mutation == "missing-ninth":
            self.assertEqual(loops[0].count("i < 9"), 1)
            loops[0] = loops[0].replace("i < 9", "i < 8")
        elif mutation == "lost-unrelated-bit":
            self.assertEqual(loops[0].count("0xFFBF"), 1)
            loops[0] = loops[0].replace("0xFFBF", "0xFFBE")
        else:
            self.assertIsNone(mutation)
        code = HEADERS + "\n".join(
            f"void {name}(LibraryState *state) {{ s32 i;\n{initializer}\n{loop}\n}}\n"
            for name, initializer, loop in zip(
                ("clear_objects", "set_objects"), initializers, loops
            )
        )
        with tempfile.TemporaryDirectory(
            prefix="library-display-objects-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            source = directory / "operations.c"
            source.write_text(code)
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
                    str(source), str(witness), str(start), "-o", str(binary),
                ],
                cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
            )
            self.assertEqual(result.returncode, 0, result.stderr[-4000:])
            return subprocess.run(
                [str(binary)], cwd=ROOT, env=environment, timeout=180
            ).returncode

    def test_actual_loops_and_regression_controls(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                self.assertEqual(self.run_witness(optimization), 0)
        for mutation in ("missing-ninth", "lost-unrelated-bit"):
            with self.subTest(mutation=mutation):
                self.assertEqual(self.run_witness("-O2", mutation), 1)


if __name__ == "__main__":
    unittest.main()
