"""Guard the paired constructor's semantic bindings without running the pool."""

import csv
import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
NAME = "CardPreview_CreateResourceObjects"
REGIONS = (
    ("slus_01411", 0x80029164, 0x7C, 0x348, ""),
    ("slpm_86398", 0x80028F44, 0x7C, 0x348, "japanese/"),
    ("sles_03947", 0x80029198, 0x78, 0x36C, "european/"),
    ("sles_03948", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03949", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03950", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03951", 0x800292DC, 0x78, 0x36C, "european/"),
)


class CardPreviewObjectConstructorNameTests(unittest.TestCase):
    def test_seven_region_constructor_bindings_preserve_complete_group(self):
        for config, start, request_size, size, tier in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
                address = start + request_size
                with (directory / "functions.csv").open(newline="") as handle:
                    rows = list(csv.DictReader(handle))
                entry, = [row for row in rows if int(row["address"], 0) == address]
                self.assertEqual((entry["name"], entry["status"], entry["module"]),
                                 (NAME, "matching_c", "game"))
                self.assertEqual(int(entry["size"], 0), size)
                matching = json.loads((directory / "matching_c.json").read_text())["functions"]
                owner = "src/game/" + tier + "duel_effect_resource_setup.c"
                group = [row for row in matching if row["source"] == owner]
                self.assertEqual(
                    [(int(row["address"], 0), int(row["size"], 0), row["profile"]) for row in group],
                    [(start, request_size, "gcc_2_8_1_g8_split"),
                     (address, size, "gcc_2_8_1_g8_split")],
                )
                self.assertRegex((directory / "symbols.txt").read_text(),
                                 rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func")
                self.assertNotRegex((directory / "symbols.txt").read_text(),
                                    r"(?m)^func_800291E0\s*=")

    def test_paired_allocations_callbacks_publication_and_primary_return(self):
        source = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        body = source[source.index(f"DisplayObject *{NAME}("):]
        self.assertEqual(body.count("DisplayObject_AcquireSlot("), 2)
        statements = (
            "DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);",
            "object->field_4C = (s32)CardPreview_DrawDetailPanel;",
            "entry->secondary_object = object;",
            "DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);",
            "object->field_10 = (u32)CardPreview_UpdateVariant;",
            "entry->primary_object = primary;",
            "secondary->field_54 = primary;",
            "return object;",
        )
        positions = [body.index(statement) for statement in statements]
        self.assertEqual(positions, sorted(positions))
        self.assertIn("setup = attack;", body)
        self.assertIn("card_id = (s16)entry->card_id;", body)
        self.assertIn("s16 attack_value = setup;", body)
        self.assertIn("s16 defense_value = defense;", body)

    def test_canonical_objects_header_and_confirmed_evidence_row(self):
        header = (ROOT / "src/game/card_preview_objects.h").read_text()
        self.assertIn("#ifndef MEMORIES_DECOMP_CARD_PREVIEW_OBJECTS_H", header)
        self.assertIn(f"struct DisplayObject *{NAME}(s32 index, s32 attack, s32 defense);", header)
        self.assertIn("void DuelEffect_ReleaseResourceObjects(s32 index);", header)
        self.assertFalse((ROOT / "src/game/func_800291E0.h").exists())
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entry, = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("function", "0x800291E0", "confirmed"))


if __name__ == "__main__":
    unittest.main()
