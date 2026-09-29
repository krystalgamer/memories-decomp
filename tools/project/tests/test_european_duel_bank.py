import csv
import json
from pathlib import Path
import sys
import unittest


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import progress


class EuropeanDuelBankTests(unittest.TestCase):
    def test_all_seven_terrain_copies_are_registered(self) -> None:
        manifest = json.loads((ROOT / "config/sles_03947/overlays.json").read_text())
        bank = next(m for m in manifest["modules"] if m["name"] == "european_duel_effects")
        self.assertEqual(
            [bank["sector_offset"], *bank["duplicate_sector_offsets"]],
            [7193 + terrain * 240 for terrain in range(7)],
        )
        self.assertEqual(bank["sector_count"], 44)
        self.assertEqual(bank["load_address"], "0x80146000")
        self.assertEqual(bank["archive"], "game/europe/DATA/WA_MRG.MRG")
        self.assertEqual(
            bank["archive_sha256"],
            "b0b4ddca3a872e7579343eb18874bc22db0d9a3adf43ca440ab7d8e337a81d7c",
        )
        self.assertEqual(
            bank["sha256"],
            "e0863650755d5de5d4abfdcfaead643e5b2b2463b0019c790df881e1dadb6075",
        )

    def test_full_inventory_preserves_unmatched_boundaries(self) -> None:
        directory = ROOT / "config/sles_03947/overlays"
        with (directory / "duel_effects_functions.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 85)
        cursor = 0x80146258
        for row in rows:
            self.assertEqual(int(row["address"], 0), cursor)
            cursor += int(row["size"], 0)
        self.assertEqual(cursor, 0x8015A1E4)
        matched = [r for r in rows if r["status"] == "matching_c"]
        self.assertEqual(len(matched), 58)
        self.assertEqual(sum(int(r["size"], 0) for r in matched), 15804)
        self.assertEqual(sum(r["status"] == "unmatched_asm" for r in rows), 27)
        self.assertFalse(any(r["status"] in ("handwritten_asm", "sdk_asm") for r in rows))
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        self.assertEqual(
            {(r["address"], r["size"]) for r in matched},
            {(r["address"], r["size"]) for r in manifest["functions"]},
        )
        accepted = {}
        for config in ("config/sles_03948", "config/sles_03951"):
            path = ROOT / config / "overlays/duel_effects_matching_c.json"
            for row in json.loads(path.read_text())["functions"]:
                accepted.setdefault(row["address"], []).append(row)
        for row in manifest["functions"]:
            self.assertIn(row, accepted[row["address"]])

    def test_reporting_does_not_hide_the_new_unmatched_bank(self) -> None:
        modules = progress.load_european_overlay_inventories(ROOT)
        self.assertEqual(len(modules), 7)
        self.assertEqual(modules["duel_effects"]["function_count"], 85)
        self.assertEqual(modules["duel_effects"]["matching_c_function_count"], 58)
        self.assertEqual(sum(m["function_count"] for m in modules.values()), 209)
        self.assertEqual(sum(m["matching_c_function_count"] for m in modules.values()), 182)
        self.assertEqual(sum(m["matching_c_bytes"] for m in modules.values()), 71656)


if __name__ == "__main__":
    unittest.main()
