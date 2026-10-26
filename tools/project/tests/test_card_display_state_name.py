"""Check the actual field-state helper, not graphics or animation execution."""

import csv
import json
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest

from tools.project.tests.test_card_display_constructor_name import REGIONS
from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
NAME = "Duel_UpdateCardDisplayState"
HEADERS = r"""
#include "src/types.h"
#include "src/game/duel_card_display_state.h"
#include "src/game/display_object.h"
#include "src/game/display_object_layout.h"
#include "src/game/duel_display.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef union { DisplayObject full; DuelCardDisplayObject card; } ObjectViews;
typedef struct { u8 before[16]; ObjectViews views; u8 after[16]; } GuardedObject;
typedef struct {
    u8 before[16];
    DuelCardRecord cards[DUEL_CARD_RECORD_COUNT];
    u8 gap[16];
    GuardedObject objects[2];
    u8 after[16];
} StateBacking;
extern StateBacking backing;
#define gDuel_aCardRecords backing.cards
"""
WITNESS = r"""
StateBacking backing, expected;
static void write16(u8 *p, u32 offset, u32 value)
{ p[offset] = value; p[offset + 1] = value >> 8; }
static u32 read16(u8 *p, u32 offset)
{ return p[offset] | p[offset + 1] << 8; }
static void write32(u8 *p, u32 offset, u32 value)
{ write16(p, offset, value); write16(p, offset + 2, value >> 16); }
int main(void)
{
    u32 value, which, index, i, flags, cases = 0;
    u8 *object;
    for (i = 0; i < sizeof(backing); i++) ((u8 *)&backing)[i] = i * 37 + 0x5A;
    for (i = 0; i < DUEL_CARD_RECORD_COUNT; i++) backing.cards[i].flags = 0xFFFF;
    for (which = 0; which < 2; which++)
        for (value = 0; value < 65536; value++) {
            index = (value * 7 + which) % DUEL_CARD_RECORD_COUNT;
            backing.cards[index].flags = value;
            for (i = 0; i < sizeof(backing.objects); i++)
                ((u8 *)backing.objects)[i] = i * 37 + value + which * 17;
            backing.objects[which].views.card.card_index = index;
            for (i = 0; i < sizeof(backing); i++) ((u8 *)&expected)[i] = ((u8 *)&backing)[i];
            object = (u8 *)&expected.objects[which].views.full;
            if (!(value & 0x2000)) object[0x67] = 0;
            flags = read16(object, 8) & 0xFFFB;
            if (value & 0x1800) {
                flags |= 4;
                object[0x21] = value & 0x1000 ? 0x80 : 0;
                object[0x22] = value & 0x800 ? 0xC0 : 0;
            }
            write16(object, 8, flags);
            write32(object, 0x0C, value & 0x4000 ? 0x404040 : 0x808080);
            STATE_HELPER(&backing.objects[which].views.card);
            for (i = 0; i < sizeof(backing); i++)
                if (((u8 *)&backing)[i] != ((u8 *)&expected)[i]) return 1;
            cases++;
        }
    return cases == 131072 ? 0 : 2;
}
"""


class CardDisplayStateNameTests(unittest.TestCase):
    def test_seven_bindings_retain_actual_group_owners_order_and_profiles(self):
        for region, constructor, owner, expected_group in REGIONS:
            with self.subTest(region=region):
                address = constructor - 0xC8
                directory = ROOT / "config" / region
                with (directory / "functions.csv").open(newline="") as handle:
                    entry, = [row for row in csv.DictReader(handle)
                              if int(row["address"], 0) == address]
                self.assertEqual(
                    (entry["name"], entry["status"], entry["module"], int(entry["size"], 0)),
                    (NAME, "matching_c", "game", 0xC8),
                )
                rows = json.loads((directory / "matching_c.json").read_text())["functions"]
                group = sorted((row for row in rows if row["source"] == owner),
                               key=lambda row: int(row["address"], 0))[:len(expected_group)]
                self.assertEqual([(int(row["address"], 0), int(row["size"], 0)) for row in group],
                                 list(expected_group))
                self.assertTrue(all(row["profile"] == "gcc_2_8_1_g8_split" for row in group))
                self.assertIsNotNone(
                    re.search(rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func",
                              (directory / "symbols.txt").read_text()), region,
                )
        self.assertIn("game/japanese/duel_init_scene",
                      (ROOT / "config/slpm_86398/split.yaml").read_text())
        self.assertFalse((ROOT / "src/game/japanese/duel_update_card_display_state.c").exists())
        self.assertFalse((ROOT / "src/game/japanese/func_80017cb8.c").exists())
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entry, = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual((entry["address"], entry["kind"], entry["confidence"]),
                         ("0x80017E3C", "function", "high"))

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_real_owner_and_distinct_byte_canonical_old_or_missing_views(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="card-state-name-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for owner, declaration, accepted, diagnostic in (
                (True, f"void (*checked)(DuelCardDisplayObject *) = {NAME};", True, ""),
                (True, f"void (*checked)(u8 *) = {NAME};", False, "incompatible pointer type"),
                (True, f"void (*checked)(DisplayObject *) = {NAME};", False, "incompatible pointer type"),
                (True, "__typeof__(&func_80017E3C) checked = func_80017E3C;", False, "undeclared"),
                (False, f"__typeof__(&{NAME}) checked = {NAME};", False, "undeclared"),
            ):
                with self.subTest(owner=owner, declaration=declaration):
                    source = directory / "probe.c"
                    include = "duel_card_display_state.h" if owner else "duel_card.h"
                    source.write_text(
                        f'#include "../../src/game/{include}"\n'
                        '#include "../../src/game/display_object.h"\n' + declaration + "\n"
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
                        self.assertIn(diagnostic, result.stderr)

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires Linux x86 compiler and ILP32 execution",
    )
    def test_actual_state_stores_all_flag_words_and_negatives(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_init_scene.c").read_text()
        match = re.search(r"void " + NAME + r"\([^)]*\)\n\{.*?\n\}", source, re.S)
        self.assertIsNotNone(match)
        helper = match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "angle-mapping"),
            ("-O2", "unconditional-angles"), ("-O2", "marker-gate"),
            ("-O2", "lost-dimming"), ("-O2", "lost-flags"),
            ("-O2", "wrong-record"), ("-O2", "guard-write"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                candidate = helper
                if mutation == "angle-mapping":
                    candidate = candidate.replace("object->field_21 = 0x80;", "object->field_22 = 0x80;")
                elif mutation == "unconditional-angles":
                    candidate = candidate.replace("u16 flags;", "u16 flags;\n    object->field_21 = 0;\n    object->field_22 = 0;")
                elif mutation == "marker-gate":
                    candidate = candidate.replace("if (!(card->flags & DUEL_CARD_FLAG_DISPLAY_MARKER))",
                                                  "if (card->flags & DUEL_CARD_FLAG_DISPLAY_MARKER)")
                elif mutation == "lost-dimming":
                    candidate = candidate.replace("if (card->flags & DUEL_CARD_FLAG_USED_THIS_TURN)", "if (0)")
                elif mutation == "lost-flags":
                    candidate = candidate.replace("object->flags & ~DISPLAY_OBJECT_FLAG_CLIP_TEST", "0")
                elif mutation == "wrong-record":
                    candidate = candidate.replace("[object->card_index]", "[(object->card_index + 1) % DUEL_CARD_RECORD_COUNT]")
                elif mutation == "guard-write":
                    candidate = candidate[:-1] + "    ((u8 *)object)[sizeof(DisplayObject)] ^= 0x80;\n}"
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(candidate, helper)
                with tempfile.TemporaryDirectory(prefix="card-state-native-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "state.c"
                    operations.write_text(HEADERS + candidate + "\n")
                    witness = directory / "witness.c"
                    witness.write_text(HEADERS + f"\n#define STATE_HELPER {NAME}\n" + WITNESS)
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
