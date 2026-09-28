import csv
import json
from pathlib import Path
import sys
import unittest


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import progress


class JapaneseDuelBankTests(unittest.TestCase):
    def test_all_seven_terrain_copies_are_registered(self) -> None:
        manifest = json.loads((ROOT / "config/slpm_86398/overlays.json").read_text())
        bank = next(m for m in manifest["modules"] if m["name"] == "japanese_duel_effects")
        self.assertEqual(
            [bank["sector_offset"], *bank["duplicate_sector_offsets"]],
            [5953 + terrain * 239 for terrain in range(7)],
        )
        self.assertEqual(bank["sector_count"], 48)
        self.assertEqual(bank["load_address"], "0x80154000")
        self.assertEqual(bank["archive"], "game/japanese/DATA/WA_MRG.MRG")
        self.assertEqual(
            bank["sha256"],
            "17784f0d718e5218e8770eb9bc98d566060a133f9cdcc59133dc295823080561",
        )

    def test_full_inventory_preserves_unmatched_boundaries(self) -> None:
        directory = ROOT / "config/slpm_86398/overlays"
        with (directory / "duel_effects_functions.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 85)
        cursor = 0x801542B0
        for row in rows:
            self.assertEqual(int(row["address"], 0), cursor)
            cursor += int(row["size"], 0)
        self.assertEqual(cursor, 0x80168270)
        matched = [r for r in rows if r["status"] == "matching_c"]
        self.assertEqual(len(matched), 15)
        self.assertEqual(sum(int(r["size"], 0) for r in matched), 2584)
        self.assertEqual(sum(r["status"] == "unmatched_asm" for r in rows), 70)
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        self.assertEqual(
            {(r["address"], r["size"]) for r in matched},
            {(r["address"], r["size"]) for r in manifest["functions"]},
        )

    def test_shared_units_keep_the_french_profile_at_a_fixed_offset(self) -> None:
        north_american = json.loads(
            (ROOT / "config/slpm_86398/overlays/duel_effects_matching_c.json").read_text()
        )["functions"]
        french = json.loads(
            (ROOT / "config/sles_03948/overlays/duel_effects_matching_c.json").read_text()
        )["functions"]
        sources = {row["source"] for row in north_american}
        french = [row for row in french if row["source"] in sources]
        self.assertEqual(len(north_american), len(french))
        for ours, theirs in zip(north_american, french):
            self.assertEqual(int(ours["address"], 16) - int(theirs["address"], 16), 0xE094)
            self.assertEqual(
                {k: v for k, v in ours.items() if k != "address"},
                {k: v for k, v in theirs.items() if k != "address"},
            )

    def test_reporting_counts_the_bank(self) -> None:
        modules = progress.load_japanese_overlay_inventories(ROOT)
        self.assertEqual(modules["duel_effects"]["function_count"], 85)
        self.assertEqual(modules["duel_effects"]["matching_c_function_count"], 15)
        self.assertEqual(modules["duel_effects"]["matching_c_bytes"], 2584)


if __name__ == "__main__":
    unittest.main()
