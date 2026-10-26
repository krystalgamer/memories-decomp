"""Observe the actual factory with stubbed pool/helpers, not engine execution."""

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
NAME = "Duel_CreateCardDisplayObject"
HEADERS = r"""
#include "src/types.h"
#include "src/game/duel_card_display_state.h"
#include "src/game/display_object.h"
#include "src/game/display_object_core.h"
#include "src/game/display_object_helpers.h"
#include "src/game/display_object_layout.h"
#include "src/game/duel_draw_card_output_position.h"
#include "src/game/display_object_set_resource_variant_from_sign.h"
#include "src/game/card_constants.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef union {
    DisplayObject full;
    DuelCardDisplayObject card;
} ObjectViews;
typedef struct {
    u8 before[16];
    ObjectViews views;
    u8 after[16];
} GuardedObject;
typedef struct {
    u8 before[16];
    DuelCardRecord cards[DUEL_CARD_RECORD_COUNT];
    u8 gap_0[16];
    u8 data[DUEL_CARD_RECORD_COUNT][8];
    u8 gap_1[16];
    s32 stats[722];
    u8 gap_2[16];
    GuardedObject objects[2];
    u8 after[16];
} FactoryBacking;
extern FactoryBacking backing;
#define gDuel_aCardRecords backing.cards
#define gDuel_adwCardStats backing.stats
"""
WITNESS = r"""
FactoryBacking backing, expected;
static u32 low, which, record_index, phase, error;
static s32 x, y;
static u8 *expected_object;
static DuelCardDisplayObject *selected;
static void write16(u8 *p, u32 offset, u32 value)
{ p[offset] = value; p[offset + 1] = value >> 8; }
static u32 read32(u8 *p, u32 offset)
{ return p[offset] | p[offset + 1] << 8 | p[offset + 2] << 16 | (u32)p[offset + 3] << 24; }
static void write32(u8 *p, u32 offset, u32 value)
{ write16(p, offset, value); write16(p, offset + 2, value >> 16); }
static s32 same(void)
{
    u32 i;
    for (i = 0; i < sizeof(backing); i++)
        if (((u8 *)&backing)[i] != ((u8 *)&expected)[i]) return 0;
    return 1;
}
s32 DisplayObject_FindFreeGeneralSlot(void)
{
    if (phase++ != 0 || !same()) error = 1;
    return 19;
}
void *DisplayObject_AcquireSlot(s32 index, s32 key)
{
    if (phase++ != 1 || index != 19 || key != 6 || !same()) error = 1;
    return &backing.objects[which].views.full;
}
void DisplayObject_SetResourceVariantFromSign(DisplayObjectConfig *object, u32 value)
{ (void)object; (void)value; error = 1; }
void Duel_DrawCardAtOutputPosition(DisplayObjectPosition *object, s32 table)
{ (void)object; (void)table; error = 1; }
void DisplayObject_SelectOrderingTable1(DisplayObject *object)
{
    u32 stat = (u32)backing.stats[backing.cards[record_index].card_id - 1];
    expected_object[0x67] = 0;
    expected_object[0x68] = stat >> 26 & 31;
    expected_object[0x69] = 0;
    expected_object[0x6A] = record_index;
    expected_object[0x6B] = backing.data[record_index][2];
    write16(expected_object, 0x30, (u32)x);
    write16(expected_object, 0x32, (u32)y);
    write32(expected_object, 4, read32(expected_object, 4) | 0x01000000U);
    write32(expected_object, 0x10, (u32)DisplayObject_SetResourceVariantFromSign);
    if (phase++ != 2 || object != &backing.objects[which].views.full || !same()) error = 1;
}
void Duel_UpdateCardDisplayState(DuelCardDisplayObject *object)
{
    write32(expected_object, 0x4C, (u32)Duel_DrawCardAtOutputPosition);
    if (phase++ != 3 || object != selected || !same()) error = 1;
    object->field_5D = 0xA5;
    expected_object[0x5D] = 0xA5;
}
void Duel_UpdateCardDisplayMarker(DuelCardDisplayObject *object)
{
    if (phase++ != 4 || object != selected || !same()) error = 1;
    object->field_67 = 0xFE;
    expected_object[0x67] = 0xFE;
}
int main(void)
{
    u32 i, cases = 0;
    DuelCardDisplayObject *result;
    for (i = 0; i < sizeof(backing); i++) ((u8 *)&backing)[i] = i * 37 + 0x5A;
    for (i = 0; i < 722; i++) backing.stats[i] = i * 0x24000000U ^ 0xA55A5A5AU;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; i++) backing.cards[i].data = backing.data[i];
    for (which = 0; which < 2; which++)
        for (low = 0; low < 65536; low++) {
            record_index = (low * 7 + which) % DUEL_CARD_RECORD_COUNT;
            backing.cards[record_index].card_id = low % 722 + 1;
            backing.data[record_index][2] = low ^ 0x5A;
            for (i = 0; i < sizeof(backing.objects); i++)
                ((u8 *)backing.objects)[i] = i * 37 + low;
            selected = &backing.objects[which].views.card;
            x = (s32)(0x12340000U | low);
            y = (s32)(0x80000000U | (low ^ 0xFFFF));
            for (i = 0; i < sizeof(backing); i++) ((u8 *)&expected)[i] = ((u8 *)&backing)[i];
            expected_object = (u8 *)&expected.objects[which].views.full;
            phase = error = 0;
            result = FACTORY(&backing.cards[record_index], x, y);
            if (result != selected || phase != 5 || error || !same()) return 1;
            cases++;
        }
    return cases == 131072 ? 0 : 2;
}
"""


class CardDisplayConstructorReturnTests(unittest.TestCase):

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_owner_accepts_only_the_recovered_return_view(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="card-return-type-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for result_type, parameters, accepted in (
                ("DuelCardDisplayObject *", "DuelCardRecord *, s32, s32", True),
                ("u8 *", "DuelCardRecord *, s32, s32", False),
                ("DisplayObject *", "DuelCardRecord *, s32, s32", False),
                ("DuelCardDisplayObject *", "DuelCardRecord *, s16, s16", False),
            ):
                with self.subTest(result_type=result_type, parameters=parameters):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/game/duel_card_display_state.h"\n'
                        '#include "../../src/game/display_object.h"\n'
                        + f"{result_type}(*checked)({parameters}) = {NAME};\n"
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
    def test_complete_actual_factory_with_full_guarded_backing_and_negatives(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_init_scene.c").read_text()
        match = re.search(r"DuelCardDisplayObject \*" + NAME + r"\([^)]*\)\n\{.*?\n\}", source, re.S)
        self.assertIsNotNone(match)
        factory = match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wrong-return"),
            ("-O2", "wrong-index"), ("-O2", "wrong-position"),
            ("-O2", "helper-order"), ("-O2", "guard-store"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                candidate = factory
                if mutation == "wrong-return":
                    candidate = candidate.replace("return p;", "return p + 1;")
                elif mutation == "wrong-index":
                    candidate = candidate.replace("/ DUEL_CARD_RECORD_SIZE;", "/ DUEL_CARD_RECORD_SIZE + 1;")
                elif mutation == "wrong-position":
                    candidate = candidate.replace("p->out_x = arg1;", "p->out_x = arg2;")
                elif mutation == "helper-order":
                    candidate = candidate.replace("Duel_UpdateCardDisplayState(p);\n    Duel_UpdateCardDisplayMarker(p);",
                                                  "Duel_UpdateCardDisplayMarker(p);\n    Duel_UpdateCardDisplayState(p);")
                elif mutation == "guard-store":
                    candidate = candidate.replace("return p;", "((u8 *)p)[sizeof(DisplayObject)] ^= 0x80;\n    return p;")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(candidate, factory)
                with tempfile.TemporaryDirectory(prefix="card-return-factory-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "factory.c"
                    operations.write_text(HEADERS + candidate + "\n")
                    witness = directory / "witness.c"
                    witness.write_text(HEADERS + f"\n#define FACTORY {NAME}\n" + WITNESS)
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
