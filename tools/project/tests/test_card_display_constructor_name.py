"""Check the constructor's semantic name and both measured declaration arms."""

import csv
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
NAME = "Duel_CreateCardDisplayObject"
REGIONS = (
    ("slus_01411", 0x80017F04, "src/game/duel_init_scene.c",
     ((0x800179F4, 0x3C0), (0x80017DB4, 0x88), (0x80017E3C, 0xC8), (0x80017F04, 0x100))),
    ("slpm_86398", 0x80017D80, "src/game/japanese/duel_create_card_display_object.c",
     ((0x80017D80, 0x100),)),
    ("sles_03947", 0x80017E90, "src/game/european/duel_card_display_state.c",
     ((0x80017D40, 0x88), (0x80017DC8, 0xC8), (0x80017E90, 0x100))),
) + tuple(
    (region, 0x80017E94, "src/game/european/duel_card_display_state.c",
     ((0x80017D44, 0x88), (0x80017DCC, 0xC8), (0x80017E94, 0x100)))
    for region in ("sles_03948", "sles_03949", "sles_03950", "sles_03951")
)


class CardDisplayConstructorNameTests(unittest.TestCase):
    def test_seven_bindings_and_original_group_profiles_order_and_extents(self):
        for region, address, owner, expected_group in REGIONS:
            with self.subTest(region=region):
                directory = ROOT / "config" / region
                with (directory / "functions.csv").open(newline="") as handle:
                    entry, = [row for row in csv.DictReader(handle)
                              if int(row["address"], 0) == address]
                self.assertEqual(
                    (entry["name"], entry["status"], entry["module"], int(entry["size"], 0)),
                    (NAME, "matching_c", "game", 0x100),
                )
                rows = json.loads((directory / "matching_c.json").read_text())["functions"]
                group = [row for row in rows if row["source"] == owner]
                self.assertEqual(
                    [(int(row["address"], 0), int(row["size"], 0)) for row in group],
                    list(expected_group),
                )
                self.assertTrue(all(row["profile"] == "gcc_2_8_1_g8_split" for row in group))
                self.assertTrue((ROOT / owner).is_file())
                for left, right in zip(expected_group, expected_group[1:]):
                    self.assertEqual(sum(left), right[0])
                self.assertIsNotNone(
                    re.search(rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func",
                              (directory / "symbols.txt").read_text()), region,
                )
        wrapper = (ROOT / "src/game/japanese/duel_create_card_display_object.c").read_text()
        self.assertIn("#define VERSION_JAPAN_FUNC_80017F04", wrapper)
        self.assertIn('#include "../duel_init_scene.c"', wrapper)
        self.assertIn("game/japanese/duel_create_card_display_object",
                      (ROOT / "config/slpm_86398/split.yaml").read_text())
        self.assertFalse((ROOT / "src/game/japanese/func_80017f04.c").exists())

    def test_factory_evidence_and_unchanged_ambient_caller(self):
        source = (ROOT / "src/game/duel_init_scene.c").read_text()
        for statement in (
            f"DuelCardDisplayObject *{NAME}(DuelCardRecord *arg0, s32 arg1, s32 arg2)",
            "DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);",
            "k = arg0->card_id - 1;",
            "p->field_68 = (tbl[k] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;",
            "((u32)arg0 - (u32)gDuel_aCardRecords) / DUEL_CARD_RECORD_SIZE;",
            "p->out_x = arg1;", "p->out_y = arg2;",
            "p->field_10 = DisplayObject_SetResourceVariantFromSign;",
            "DisplayObject_SelectOrderingTable1((DisplayObject *)p);",
            "p->field_4C = Duel_DrawCardAtOutputPosition;",
        ):
            self.assertIn(statement, source)
        wrapper = (ROOT / "src/game/func_80018004.c").read_text()
        self.assertIn("#define FUNC_80017F04_AMBIENT_POSITION_ARGS", wrapper)
        self.assertIn(f"        {NAME}((u8 *)card);", wrapper)
        header = (ROOT / "src/game/duel_card_display_state.h").read_text()
        self.assertIn(f"DuelCardDisplayObject *{NAME}();", header)
        self.assertIn(f"DuelCardDisplayObject *{NAME}(DuelCardRecord *arg0, s32 arg1, s32 arg2);", header)
        for filename in (
            "duel_scene_hand_actions.c", "duel_scene_field_actions.c",
            "duel_scene_card_placement.c", "duel_ritual_effect.c", "debug_effect_screen.c",
        ):
            self.assertIn(NAME + "(", (ROOT / "src/game" / filename).read_text())
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entry, = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("function", "0x80017F04", "high"))

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_actual_owner_wide_ambient_arity_return_and_name_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="card-factory-name-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for ambient, owner, declaration, accepted, diagnostic in (
                (False, True, f"DuelCardDisplayObject *(*checked)(DuelCardRecord *, s32, s32) = {NAME};", True, ""),
                (True, True, f"DuelCardDisplayObject *use(DuelCardRecord *card) {{ return {NAME}((u8 *)card); }}", True, ""),
                (False, True, f"DuelCardDisplayObject *use(DuelCardRecord *card) {{ return {NAME}(card); }}", False,
                 "too few arguments"),
                (False, True, f"u8 *(*checked)(DuelCardRecord *, s32, s32) = {NAME};",
                 False, "incompatible pointer type"),
                (False, True, f"DuelCardDisplayObject *(*checked)(DuelCardRecord *, s16, s16) = {NAME};",
                 False, "incompatible pointer type"),
                (False, True, "__typeof__(&func_80017F04) checked = func_80017F04;", False, "undeclared"),
                (False, False, f"__typeof__(&{NAME}) checked = {NAME};", False, "undeclared"),
            ):
                with self.subTest(ambient=ambient, owner=owner, declaration=declaration):
                    source = directory / "probe.c"
                    prefix = "#define FUNC_80017F04_AMBIENT_POSITION_ARGS\n" if ambient else ""
                    include = "duel_card_display_state.h" if owner else "duel_card.h"
                    source.write_text(prefix + f'#include "../../src/game/{include}"\n' + declaration + "\n")
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


if __name__ == "__main__":
    unittest.main()
