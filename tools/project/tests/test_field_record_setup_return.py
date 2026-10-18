"""Check the typed result/publication boundary with opaque setup/object helpers."""

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
FIELD_FACTORY = "func_80024C1C"
HEADERS = r"""
#include "src/types.h"
#include "src/game/duel_deck_card.h"
#include "src/game/duel_card_record_lifecycle.h"
#include "src/game/duel_card_staging.h"
#include "src/game/display_object.h"
#define DUEL_SCREEN_TABLES_TYPED_POSITIONS
#include "src/game/duel_screen_tables.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef union { DisplayObject full; DuelCardDisplayObject card; } ObjectViews;
typedef struct { u8 before[16]; ObjectViews views; u8 after[16]; } GuardedObject;
typedef struct {
    u8 before[16];
    DuelFieldPosition positions[DUEL_SCREEN_CARD_POSITION_COUNT];
    u16 ids[DUEL_CARD_RECORD_COUNT];
    u8 gap_1[16];
    GuardedObject objects[2];
    u8 after[16];
} PublicationBacking;
typedef struct {
    u8 before[16];
    DuelCardStagingDeckView view;
    u8 after[16];
} StagingBacking;
extern PublicationBacking backing;
extern StagingBacking staging;
#define gDuel_aCardRecords staging.view.field_cards
#define D_800908A0 backing.positions
#define gDuel_abCardStagingBuffer ((u8 *)&staging.view)
"""
WITNESS = r"""
PublicationBacking backing, expected;
StagingBacking staging, expected_staging;
static u32 low, which, selected, index, raw_a, raw_b, phase, error;
static s32 sign16(u32 value)
{ value &= 65535; return value < 32768 ? (s32)value : (s32)value - 65536; }
static void write16(u8 *p, u32 offset, u32 value)
{ p[offset] = value; p[offset + 1] = value >> 8; }
static void write32(u8 *p, u32 offset, u32 value)
{ write16(p, offset, value); write16(p, offset + 2, value >> 16); }
static s32 same(void)
{
    u32 i;
    for (i = 0; i < sizeof(backing); i++)
        if (((u8 *)&backing)[i] != ((u8 *)&expected)[i]) return 0;
    for (i = 0; i < sizeof(staging.before); i++)
        if (staging.before[i] != expected_staging.before[i] ||
            staging.after[i] != expected_staging.after[i]) return 0;
    for (i = 0; i < sizeof(staging.view.field_cards); i++)
        if (((u8 *)staging.view.field_cards)[i] !=
            ((u8 *)expected_staging.view.field_cards)[i]) return 0;
    return 1;
}
DuelCardRecord *Duel_SetupCardRecord(s32 a, s32 b)
{
    if (phase++ != 0 || (u32)a != raw_a || (u32)b != raw_b || !same()) error = 1;
    staging.view.field_cards[selected].flags = 0xA55A;
    write16((u8 *)&expected_staging.view.field_cards[selected], 0x16, 0xA55A);
    return &staging.view.field_cards[selected];
}
DuelCardDisplayObject *FIELD_FACTORY(s32 card_id, s32 x, s32 y)
{
    if (phase++ != 1 || card_id != sign16(low) ||
        x != sign16(low ^ 0x8000) || y != sign16(low ^ 0x5A5A) || !same()) error = 1;
    backing.objects[which].views.card.field_68 = 0x1D;
    ((u8 *)&expected.objects[which].views.full)[0x68] = 0x1D;
    return &backing.objects[which].views.card;
}
int main(void)
{
    static const u32 returned[4] = {0, 5, 20, 29};
    static const u32 tagged[5] = {0, 0x80, 0x12340080, 0x80000080, 0xFFFFFF80};
    u32 record_choice, i, mode, cases = 0;
    for (i = 0; i < sizeof(staging); i++) ((u8 *)&staging)[i] = i * 37 + 0x5A;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; i++)
        staging.view.field_cards[i].data = &backing.ids[i];
    for (i = 0; i < sizeof(staging); i++) ((u8 *)&expected_staging)[i] = ((u8 *)&staging)[i];
    for (record_choice = 0; record_choice < 4; record_choice++)
        for (which = 0; which < 2; which++)
            for (low = 0; low < 65536; low++) {
                for (i = 0; i < sizeof(backing); i++) ((u8 *)&backing)[i] = i * 37 + low + which;
                selected = returned[record_choice];
                mode = low % 5;
                if (mode == 0) { raw_a = low % 30; index = raw_a; }
                else { raw_a = tagged[mode] | (low % 15); index = low % 15 + 15; }
                raw_b = (which ? 0x80000000U : 0x12340000U) | low;
                backing.ids[index] = low;
                backing.positions[index].x = low ^ 0x8000;
                backing.positions[index].y = low ^ 0x5A5A;
                for (i = 0; i < sizeof(backing); i++) ((u8 *)&expected)[i] = ((u8 *)&backing)[i];
                for (i = 0; i < sizeof(staging.view.field_cards); i++)
                    ((u8 *)expected_staging.view.field_cards)[i] = ((u8 *)staging.view.field_cards)[i];
                phase = error = 0;
                func_80024D34((s32)raw_a, (s32)raw_b);
                write32((u8 *)&expected_staging.view.field_cards[selected], 0, (u32)&backing.objects[which].views.card);
                ((u8 *)&expected.objects[which].views.full)[0x6A] = index;
                if (phase != 2 || error || !same()) return 1;
                cases++;
            }
    for (i = 0; i < sizeof(staging); i++)
        if (((u8 *)&staging)[i] != ((u8 *)&expected_staging)[i]) return 1;
    return cases == 524288 ? 0 : 2;
}
"""


class FieldRecordSetupReturnTests(unittest.TestCase):
    def test_real_record_owner_and_direct_member_publication(self):
        owner = (ROOT / "src/game/duel_deck_card.h").read_text()
        self.assertIn('#include "duel_card.h"', owner)
        self.assertIn("DuelCardRecord *Duel_SetupCardRecord(s32, s32);", owner)
        source = (ROOT / "src/game/duel_card_record_lifecycle.c").read_text()
        self.assertIn("DuelCardRecord *Duel_SetupCardRecord(s32 a, s32 b)", source)
        self.assertIn("DuelCardRecord *slot;", source)
        self.assertIn("slot->object = obj;", source)
        self.assertNotIn("*(DuelCardDisplayObject **)slot", source)

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_actual_owner_return_width_arity_and_lost_owner_contracts(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="record-return-type-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for owner, declaration, accepted, diagnostic in (
                (True, "DuelCardRecord *(*checked)(s32, s32) = Duel_SetupCardRecord;", True, ""),
                (True, "u8 *(*checked)(s32, s32) = Duel_SetupCardRecord;", False, "incompatible pointer type"),
                (True, "DuelDeckCardRecord *(*checked)(s32, s32) = Duel_SetupCardRecord;",
                 False, "incompatible pointer type"),
                (True, "DuelCardRecord *(*checked)(s16, s16) = Duel_SetupCardRecord;",
                 False, "incompatible pointer type"),
                (True, "DuelCardRecord *use(s32 a) { return Duel_SetupCardRecord(a); }",
                 False, "too few arguments"),
                (False, "__typeof__(&Duel_SetupCardRecord) checked = Duel_SetupCardRecord;",
                 False, "undeclared"),
            ):
                with self.subTest(owner=owner, declaration=declaration):
                    source = directory / "probe.c"
                    header = "duel_deck_card.h" if owner else "duel_card.h"
                    source.write_text(f'#include "../../src/game/{header}"\n' + declaration + "\n")
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
                        self.assertIn(diagnostic, result.stderr)

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires Linux x86 compiler and ILP32 execution",
    )
    def test_actual_publisher_with_full_stage_and_independent_returned_record(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_card_record_lifecycle.c").read_text()
        match = re.search(r"void func_80024D34\([^)]*\)\n\{.*?\n\}", source, re.S)
        self.assertIsNotNone(match)
        publisher = match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wrong-record"),
            ("-O2", "unsigned-card"), ("-O2", "unsigned-x"),
            ("-O2", "wrong-index"), ("-O2", "wrong-object"), ("-O2", "guard-store"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                candidate = publisher
                if mutation == "wrong-record":
                    candidate = candidate.replace("slot->object = obj;", "gDuel_aCardRecords[idx].object = obj;")
                elif mutation == "unsigned-card":
                    candidate = candidate.replace("*(s16 *)replay->record.data", "*(u16 *)replay->record.data")
                elif mutation == "unsigned-x":
                    candidate = candidate.replace("D_800908A0[idx].x", "(u16)D_800908A0[idx].x")
                elif mutation == "wrong-index":
                    candidate = candidate.replace("obj->card_index = idx;", "obj->card_index = idx + 1;")
                elif mutation == "wrong-object":
                    candidate = candidate.replace("slot->object = obj;", "slot->object = obj + 1;")
                elif mutation == "guard-store":
                    candidate = candidate.replace("obj->card_index = idx;",
                                                  "obj->card_index = idx;\n    ((u8 *)obj)[sizeof(DisplayObject)] ^= 0x80;")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(candidate, publisher)
                with tempfile.TemporaryDirectory(prefix="record-publisher-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "publisher.c"
                    operations.write_text(HEADERS + candidate + "\n")
                    witness = directory / "witness.c"
                    witness.write_text(HEADERS + f"\n#define FIELD_FACTORY {FIELD_FACTORY}\n" + WITNESS)
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
