"""Check direct/callback card-call boundaries, not card rendering or RTPS."""

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
#include "src/game/duel_card.h"
#include "src/game/func_80016784.h"
#include "src/game/duel_draw_card_output_position.h"
#define ORDERING_TABLE_SLOT2_ARRAY
#include "src/game/ordering_tables.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef union {
    DisplayObject full;
    DisplayObjectPosition position;
} ObjectViews;
typedef struct {
    u8 before[16];
    DuelCardRecord card;
    u8 gap_0[16];
    ObjectViews objects[2];
    u8 gap_1[16];
    GsOT descriptors[4];
    GsOT *G32 table_slot[2];
    u8 after[16];
} GuardedCardCall;
extern GuardedCardCall guarded;
#define D_800E9D98 guarded.table_slot
"""
WITNESS = r"""
GuardedCardCall guarded, expected;
static DisplayObject *selected_object;
static GsOT *selected_table;
static u32 calls, error, low;
void select_projected(DuelCardRecord *card);
void select_output(DisplayObjectPosition *object, s32 arg1);
static s32 signed_half(u32 value)
{ value &= 65535; return value < 32768 ? (s32)value : (s32)value - 65536; }
static s32 same(void)
{
    u32 i;
    for (i = 0; i < sizeof(guarded); i++)
        if (((u8 *)&guarded)[i] != ((u8 *)&expected)[i]) return 0;
    return 1;
}
void func_80016784(DisplayObject *object, GsOT *table, s32 x, s32 y)
{
    s32 wanted_x = signed_half(calls == 0 ? low : low ^ 0xFFFF);
    s32 wanted_y = signed_half(calls == 0 ? low ^ 0x8000 : low ^ 0x5A5A);
    if (calls > 1 || object != selected_object || table != selected_table ||
        x != wanted_x || y != wanted_y || !same()) error = 1;
    calls++;
}
int main(void)
{
    u32 slot, which, i, cases = 0;
    for (slot = 0; slot < 4; slot++)
        for (which = 0; which < 2; which++)
            for (low = 0; low < 65536; low++) {
                for (i = 0; i < sizeof(guarded); i++) ((u8 *)&guarded)[i] = i * 37 + low;
                selected_object = &guarded.objects[which].full;
                selected_table = &guarded.descriptors[slot];
                guarded.table_slot[0] = selected_table;
                guarded.table_slot[1] = &guarded.descriptors[(slot + 1) & 3];
                guarded.card.object = selected_object;
                guarded.card.screen_x = low;
                guarded.card.screen_y = low ^ 0x8000;
                guarded.objects[which].position.out_x = low ^ 0xFFFF;
                guarded.objects[which].position.out_y = low ^ 0x5A5A;
                for (i = 0; i < sizeof(guarded); i++) ((u8 *)&expected)[i] = ((u8 *)&guarded)[i];
                calls = error = 0;
                select_projected(&guarded.card);
                select_output(&guarded.objects[which].position, (s32)selected_table);
                if (error || calls != 2 || !same()) return 1;
                cases++;
            }
    return cases == 524288 ? 0 : 2;
}
"""


class CardFrameTablePointerTests(unittest.TestCase):
    def test_real_descriptor_and_unchanged_word_callback_boundary(self):
        source = (ROOT / "src/game/duel_card_frame_draw.c").read_text()
        self.assertIn("func_80016784(DisplayObject *object, GsOT *ordering_table, s32 arg2, s32 arg3)", source)
        self.assertEqual(source.count("POLY_FT4_BYTES(y), ordering_table, fl,"), 10)
        self.assertIn("Duel_DrawCardAtOutputPosition(DisplayObjectPosition *object, s32 arg1)", source)
        self.assertIn("func_80016784((DisplayObject *)object, (GsOT *)arg1, object->out_x, object->out_y);", source)
        projected = (ROOT / "src/game/func_80015DFC.c").read_text()
        self.assertIn("func_80016784((DisplayObject *)card->object, D_800E9D98[0], card->screen_x,", projected)

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_direct_and_callback_contracts_keep_their_distinct_views(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="card-table-type-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for symbol, parameters, accepted in (
                ("func_80016784", "DisplayObject *, GsOT *, s32, s32", True),
                ("func_80016784", "DisplayObject *, s32, s32, s32", False),
                ("func_80016784", "DisplayObject *, GsOT_TAG *, s32, s32", False),
                ("func_80016784", "DisplayObject *, GsOT *, s16, s16", False),
                ("Duel_DrawCardAtOutputPosition", "DisplayObjectPosition *, s32", True),
                ("Duel_DrawCardAtOutputPosition", "DisplayObjectPosition *, GsOT *", False),
            ):
                with self.subTest(symbol=symbol, parameters=parameters):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/game/func_80016784.h"\n'
                        '#include "../../src/game/duel_draw_card_output_position.h"\n'
                        f"void (*checked)({parameters}) = {symbol};\n"
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
    def test_actual_projected_and_word_wrapper_calls_with_negatives(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        projected = (ROOT / "src/game/func_80015DFC.c").read_text()
        wrapper = (ROOT / "src/game/duel_card_frame_draw.c").read_text()
        match = re.search(
            r"func_80016784\(\(DisplayObject \*\)card->object, D_800E9D98\[0\], card->screen_x,\n\s*card->screen_y\);",
            projected,
        )
        self.assertIsNotNone(match)
        direct = match.group()
        forward = "func_80016784((DisplayObject *)object, (GsOT *)arg1, object->out_x, object->out_y);"
        self.assertEqual(wrapper.count(forward), 1)
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wrong-table-slot"),
            ("-O2", "unsigned-output-x"), ("-O2", "wrong-object-view"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                caller, callback = direct, forward
                if mutation == "wrong-table-slot":
                    caller = caller.replace("D_800E9D98[0]", "D_800E9D98[1]")
                elif mutation == "unsigned-output-x":
                    callback = callback.replace("object->out_x", "(u16)object->out_x")
                elif mutation == "wrong-object-view":
                    callback = callback.replace("(DisplayObject *)object", "(DisplayObject *)(object + 1)")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual((caller, callback), (direct, forward))
                with tempfile.TemporaryDirectory(prefix="card-table-calls-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        HEADERS + "\nvoid select_projected(DuelCardRecord *card) {\n" + caller
                        + "\n}\nvoid select_output(DisplayObjectPosition *object, s32 arg1) {\n"
                        + callback + "\n}\n"
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
