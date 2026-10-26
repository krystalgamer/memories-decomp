"""Check actual strip/sheet caller statements and their signed depth ABI."""

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
GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
HEADERS = r"""
#include "src/types.h"
#include "src/game/display_object_runtime.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct {
    u8 before[16];
    DisplayObject objects[2];
    u8 gap[16];
    GsOT descriptors[4];
    GsOT *G32 tables[4];
    u8 after[16];
} GuardedRenderTables;
extern GuardedRenderTables guarded;
"""
WITNESS = r"""
GuardedRenderTables guarded, expected;
static DisplayObject *selected_object;
static GsOT *selected_table;
static s32 selected_depth;
static u32 calls, error;
void select_strip(DisplayObject *object, GsOT *G32 *table);
void select_sheet(DisplayObject *p, GsOT *G32 *t);
static s32 same(void)
{
    u32 i;
    for (i = 0; i < sizeof(guarded); i++)
        if (((u8 *)&guarded)[i] != ((u8 *)&expected)[i]) return 0;
    return 1;
}
static void observe(u32 kind, DisplayObject *object, GsOT *table, s32 depth)
{
    if (kind != calls || object != selected_object || table != selected_table ||
        depth != selected_depth || !same()) error = 1;
    calls++;
}
void DisplayObject_RenderSpriteStrips(DisplayObject *object, GsOT *table, s32 mode)
{ observe(0, object, table, mode); }
void DisplayObject_RenderSpriteSheet(DisplayObject *object, GsOT *table, s32 depth)
{ observe(1, object, table, depth); }
int main(void)
{
    u32 slot, which, low, i, cases = 0;
    for (slot = 0; slot < 4; slot++)
        for (which = 0; which < 2; which++)
            for (low = 0; low < 65536; low++) {
                for (i = 0; i < sizeof(guarded); i++) ((u8 *)&guarded)[i] = i * 37 + low;
                for (i = 0; i < 4; i++) guarded.tables[i] = &guarded.descriptors[i];
                selected_object = &guarded.objects[which];
                selected_object->ot_index = slot;
                selected_object->ot_depth = low;
                selected_table = &guarded.descriptors[slot];
                selected_depth = low < 32768 ? (s32)low : (s32)low - 65536;
                for (i = 0; i < sizeof(guarded); i++) ((u8 *)&expected)[i] = ((u8 *)&guarded)[i];
                calls = error = 0;
                select_strip(selected_object, guarded.tables);
                select_sheet(selected_object, guarded.tables);
                if (error || calls != 2 || !same()) return 1;
                cases++;
            }
    return cases == 524288 ? 0 : 2;
}
"""


class SpriteRendererTablePointerTests(unittest.TestCase):

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_descriptor_type_rejects_old_word_and_narrow_third_argument(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="sprite-table-type-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for symbol, header in (
                ("DisplayObject_RenderSpriteStrips", "display_object_runtime.h"),
                ("DisplayObject_RenderSpriteSheet", "display_object_runtime.h"),
            ):
                for table, depth, accepted in (
                    ("GsOT *", "s32", True), ("s32", "s32", False),
                    ("GsOT *", "u16", False), ("GsOT_TAG *", "s32", False),
                ):
                    with self.subTest(symbol=symbol, table=table, depth=depth):
                        source = directory / "probe.c"
                        source.write_text(
                            f'#include "../../src/game/{header}"\n'
                            f"void (*checked)(DisplayObject *, {table}, {depth}) = {symbol};\n"
                        )
                        result = subprocess.run(
                            [str(GCC), "-S", "-O2", "-G8", "-Werror", "-o",
                             str(directory / "probe.s"), str(source)],
                            cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)),
                            text=True, capture_output=True, timeout=60,
                        )
                        if accepted:
                            self.assertEqual(result.returncode, 0, result.stderr)
                        else:
                            self.assertNotEqual(result.returncode, 0, result.stderr)
                            self.assertIn("incompatible pointer type", result.stderr)

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires Linux x86 compiler and ILP32 execution",
    )
    def test_actual_callers_preserve_descriptor_identity_and_full_signed_depth(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        strips = (ROOT / "src/game/display_object_runtime.c").read_text()
        sheets = (ROOT / "src/game/display_object_render_sprite_sheet_list.c").read_text()
        strip_match = re.search(
            r"DisplayObject_RenderSpriteStrips\(object, table\[object->ot_index\],\n\s*\(s16\)object->ot_depth\);",
            strips,
        )
        sheet_match = re.search(
            r"DisplayObject_RenderSpriteSheet\(\n\s*\(DisplayObject \*\)q, t\[p->ot_index\],\n\s*\(s16\)p->ot_depth\n\s*\);",
            sheets,
        )
        self.assertIsNotNone(strip_match)
        self.assertIsNotNone(sheet_match)
        originals = strip_match.group(), sheet_match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "unsigned-depth"),
            ("-O2", "wrong-table"), ("-O2", "wrong-object"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                strip, sheet = originals
                if mutation == "unsigned-depth":
                    strip = strip.replace("(s16)object->ot_depth", "(u16)object->ot_depth")
                elif mutation == "wrong-table":
                    strip = strip.replace("table[object->ot_index]", "table[(object->ot_index + 1) & 3]")
                elif mutation == "wrong-object":
                    strip = strip.replace("Strips(object,", "Strips(object + 1,")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual((strip, sheet), originals)
                with tempfile.TemporaryDirectory(prefix="sprite-table-calls-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        HEADERS + "\nvoid select_strip(DisplayObject *object, GsOT *G32 *table) {\n"
                        + strip + "\n}\nvoid select_sheet(DisplayObject *p, GsOT *G32 *t) {\n"
                        "u8 *q = (u8 *)p;\n" + sheet + "\n}\n"
                    )
                    witness = directory / "witness.c"
                    witness.write_text(HEADERS + WITNESS)
                    startup = directory / "start.S"
                    startup.write_text(START)
                    binary = directory / "witness"
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT),
                         str(operations), str(witness), str(startup), "-o", str(binary)],
                        cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)),
                        text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run(
                        [str(binary)], cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)), timeout=180,
                    )
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
