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


    def test_japanese_alias_and_confirmed_ledger_row(self):
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entry, = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("function", "0x80019CC8", "confirmed"))


if __name__ == "__main__":
    unittest.main()
