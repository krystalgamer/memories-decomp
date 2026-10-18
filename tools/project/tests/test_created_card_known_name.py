"""Guard the confirmed Library-known marker and its seven isolated bindings."""

import csv
import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
NAME = "Duel_MarkCreatedCardKnown"
STEM = "duel_mark_created_card_known"
REGIONS = (
    ("slus_01411", 0x80019CC8, ""),
    ("slpm_86398", 0x80019B44, "japanese/"),
    ("sles_03947", 0x80019C54, ""),
    ("sles_03948", 0x80019C58, ""),
    ("sles_03949", 0x80019C58, ""),
    ("sles_03950", 0x80019C58, ""),
    ("sles_03951", 0x80019C58, ""),
)


class CreatedCardKnownNameTests(unittest.TestCase):
    def test_seven_region_names_owners_profiles_and_sizes(self):
        for config, address, tier in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
                with (directory / "functions.csv").open(newline="") as handle:
                    entry, = [row for row in csv.DictReader(handle) if int(row["address"], 0) == address]
                self.assertEqual((entry["name"], entry["status"], entry["module"], int(entry["size"], 0)),
                                 (NAME, "matching_c", "game", 0x50))
                rows = json.loads((directory / "matching_c.json").read_text())["functions"]
                entry, = [row for row in rows if int(row["address"], 0) == address]
                owner = "src/game/" + tier + STEM + ".c"
                self.assertEqual((entry["source"], entry["profile"], int(entry["size"], 0)),
                                 (owner, "gcc_2_8_1_g8", 0x50))
                self.assertEqual(sum(row["source"] == owner for row in rows), 1)
                self.assertTrue((ROOT / owner).is_file())
                self.assertRegex((directory / "symbols.txt").read_text(),
                                 rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func")
                if config != "slus_01411":
                    self.assertIn(owner.removeprefix("src/").removesuffix(".c"),
                                  (directory / "split.yaml").read_text())
        self.assertFalse((ROOT / "src/game/func_80019CC8.c").exists())
        self.assertFalse((ROOT / "src/game/japanese/func_80019CC8.c").exists())

    def test_result_callers_and_known_not_owned_consumer(self):
        source = (ROOT / "src/game" / (STEM + ".c")).read_text()
        self.assertIn(f"void {NAME}(s32 card_id)", source)
        self.assertIn(
            "Library_UpdateCardUsedFlag((s32)((u32)card_id + CAMPAIGN_FLAG_LIBRARY_CARD_BASE));", source,
        )
        self.assertIn("D_8009B1D5 == 0 && D_8009B360 < 0 && gDuel_bOpponentID[0] >= 0", source)
        ritual = (ROOT / "src/game/duel_ritual_effect.c").read_text()
        placement = (ROOT / "src/game/duel_scene_card_placement.c").read_text()
        self.assertIn(f"{NAME}((s32)D_8009B1A0);", ritual)
        self.assertIn(f"{NAME}((u32)(D_8009B150 & 0xFFF));", placement)
        library = (ROOT / "src/game/library_runtime.c").read_text()
        statements = (
            "Campaign_TestStoryFlag(n + CAMPAIGN_FLAG_LIBRARY_CARD_BASE) != 0",
            "r->cards[n].flags = LIBRARY_CARD_FLAG_KNOWN;",
            "Library_CheckCardOwned(n) < 0",
            "r->cards[n].flags |= LIBRARY_CARD_FLAG_UNOWNED;",
        )
        positions = [library.index(statement) for statement in statements]
        self.assertEqual(positions, sorted(positions))
        grid = (ROOT / "src/game/library_grid_cursor.c").read_text()
        self.assertIn("LIBRARY_CARD_FLAG_KNOWN) != 0", grid)
        self.assertIn("state->motion.selected_card_id = card_id;", grid)

    def test_japanese_alias_and_confirmed_ledger_row(self):
        wrapper = (ROOT / "src/game/japanese" / (STEM + ".c")).read_text()
        self.assertIn("#define D_8009B360 gJapanese_DuelPlayerState", wrapper)
        self.assertIn(f'#include "../{STEM}.c"', wrapper)
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entry, = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("function", "0x80019CC8", "confirmed"))


if __name__ == "__main__":
    unittest.main()
