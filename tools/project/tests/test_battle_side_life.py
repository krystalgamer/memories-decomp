"""Guard the actual unsigned life updates and their post-narrowing signed clamp."""

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
#include "src/game/duel_side_state.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef char side_size[sizeof(DuelSideState) == 0x20 ? 1 : -1];
typedef char life_offset[(u32)&((DuelSideState *)0)->life_points == 0x14 ? 1 : -1];
typedef char life_width[sizeof(((DuelSideState *)0)->life_points.unsigned_value) == 2 ? 1 : -1];
typedef struct { u8 before[16]; DuelSideState sides[2]; u8 after[16]; } GuardedSides;
extern GuardedSides test_sides;
extern u8 gDuel_bActiveSide;
#define gDuel_aSideStates test_sides.sides
"""
WITNESS = HEADERS + r"""
GuardedSides test_sides;
u8 gDuel_bActiveSide;
static u8 expected[sizeof(test_sides)];
static const s32 results[2][8] = {
    {1, 16, 30000, 32767, 32768, 65535, 65536, 2147483647},
    {-2, -16, -30000, -32768, -32769, -65535, -65536, (-2147483647 - 1)}
};
void opponent_loss(s32 result);
void active_loss(s32 result);
static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
int main(void)
{
    u32 mode, side, sample, value, i, target, offset, half, magnitude, cases = 0;
    for (mode = 0; mode < 2; mode++) {
        for (side = 0; side < 2; side++) {
            gDuel_bActiveSide = side;
            target = mode == 0 ? side ^ 1 : side;
            offset = (u8 *)&test_sides.sides[target] - (u8 *)&test_sides + 0x14;
            for (sample = 0; sample < 8; sample++) {
                magnitude = mode == 0 ? (u32)results[mode][sample]
                                     : 0 - (u32)results[mode][sample];
                for (value = 0; value < 65536; value++) {
                    for (i = 0; i < sizeof(test_sides); i++)
                        ((u8 *)&test_sides)[i] = i * 37 + value + side * 13;
                    put16((u8 *)&test_sides + offset, value);
                    for (i = 0; i < sizeof(test_sides); i++)
                        expected[i] = ((u8 *)&test_sides)[i];
                    half = (value - magnitude) & 0xFFFF;
                    if (half & 0x8000) half = 0;
                    put16(expected + offset, half);
                    if (mode == 0) opponent_loss(results[mode][sample]);
                    else active_loss(results[mode][sample]);
                    for (i = 0; i < sizeof(test_sides); i++)
                        if (((u8 *)&test_sides)[i] != expected[i]) return 1;
                    cases++;
                }
            }
        }
    }
    return cases == 2097152 ? 0 : 2;
}
"""


class BattleSideLifeTests(unittest.TestCase):
    def fragments(self):
        source = (ROOT / "src/game/duel_scene_battle.c").read_text()
        self.assertIn("DuelSideState *view;", source)
        fragments = re.findall(
            r"view = &gDuel_aSideStates\[gDuel_bActiveSide(?: \^ 1)?\];\s*"
            r"life = view->life_points\.unsigned_value [-+] result;\s*"
            r"view->life_points\.unsigned_value = life;\s*"
            r"if \(\(s16\)life < 0\) \{\s*"
            r"view->life_points\.unsigned_value = 0;\s*\}",
            source,
        )
        self.assertEqual(len(fragments), 2)
        self.assertIn("[gDuel_bActiveSide ^ 1]", fragments[0])
        self.assertIn(" - result;", fragments[0])
        self.assertIn("[gDuel_bActiveSide]", fragments[1])
        self.assertIn(" + result;", fragments[1])
        return fragments


    @unittest.skipUnless(
        platform.system() == "Linux"
        and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"),
        "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_fragments_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        fragments = self.fragments()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None),
            ("-O2", "wrong-side"), ("-O2", "unsigned-clamp"),
            ("-O2", "wide-store"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                altered = list(fragments)
                if mutation == "wrong-side":
                    altered[0] = altered[0].replace("[gDuel_bActiveSide ^ 1]", "[gDuel_bActiveSide]")
                elif mutation == "unsigned-clamp":
                    altered = [text.replace("(s16)life < 0", "life < 0") for text in altered]
                elif mutation == "wide-store":
                    altered = [
                        text.replace(
                            "view->life_points.unsigned_value = life;",
                            "*(u32 *)&view->life_points.unsigned_value = life;",
                        ) for text in altered
                    ]
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(altered, fragments)
                operations = HEADERS + "\n".join(
                    f"void {name}(s32 result) {{ DuelSideState *view; u16 life;\n{code}\n}}\n"
                    for name, code in zip(("opponent_loss", "active_loss"), altered)
                )
                with tempfile.TemporaryDirectory(
                    prefix="battle-side-life-", dir=ROOT / "tmp"
                ) as name:
                    directory = Path(name)
                    source = directory / "operations.c"
                    source.write_text(operations)
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
                        cwd=ROOT, env=environment, text=True,
                        capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run(
                        [str(binary)], cwd=ROOT, env=environment, timeout=180
                    )
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
