import csv
import json
from pathlib import Path
import re
import shutil
import subprocess
import unittest


ROOT = Path(__file__).resolve().parents[3]


class DuelResultLocalizedSpritesTests(unittest.TestCase):
    def preprocess(self, source: str) -> str:
        compiler = shutil.which("cc")
        if compiler is None:
            self.skipTest("a C preprocessor is required")
        result = subprocess.run(
            [compiler, "-E", "-P", str(ROOT / source)],
            cwd=ROOT, check=True, capture_output=True, text=True,
        )
        return re.sub(r"\s+", " ", result.stdout)

    def test_existing_regions_keep_seven_slots_without_private_tables(self) -> None:
        for source in (
            "src/game/duel_result_runtime.c",
            "src/game/european/duel_result_runtime.c",
            "src/game/japanese/duel_result_runtime.c",
        ):
            with self.subTest(source=source):
                text = self.preprocess(source)
                self.assertEqual(text.count("for (i = 0; i < 7; i++)"), 2)
                self.assertNotIn("gDuelResult_nOutroSpriteCount", text)
                self.assertNotIn("gDuelResult_aFrenchSpriteSpecsOpponent", text)

    def test_localized_tables_and_active_count_preserve_binary_order(self) -> None:
        text = self.preprocess("src/game/spanish/duel_result_runtime.c")
        self.assertIn("for (i = 0; i < 10; i++)", text)
        self.assertIn("for (i = 0; i < gDuelResult_nOutroSpriteCount; i++)", text)
        for symbol in (
            "gDuelResult_aFrenchSpriteSpecsOpponent", "gDuelResult_aFrenchSpriteSpecsNoOpponent", "gDuelResult_aGermanSpriteSpecsOpponent", "gDuelResult_aGermanSpriteSpecsNoOpponent",
            "gDuelResult_aItalianSpriteSpecsOpponent", "gDuelResult_aItalianSpriteSpecsNoOpponent", "gDuelResult_aSpanishSpriteSpecsOpponent", "gDuelResult_aSpanishSpriteSpecsNoOpponent",
        ):
            self.assertIn(f"extern DuelResultSpriteSpec {symbol}[2][10];", text)
            self.assertIn(f"spec = &{symbol}[gDuel_bWinnerSide][i];", text)
        self.assertIn(
            "if (spec->x == 0) { continue; } gDuelResult_nOutroSpriteCount = i + 1; "
            "slots[i].object = 0; if (spec->kind != 0)", text,
        )

    def test_spanish_resident_and_original_six_overlays_remain_complete(self) -> None:
        config = ROOT / "config/sles_03951"
        with (config / "functions.csv").open() as handle:
            resident = list(csv.DictReader(handle))
        self.assertEqual(sum(r["status"] == "matching_c" for r in resident), 1140)
        self.assertFalse(any(r["status"] == "unmatched_asm" for r in resident))
        inventories = [
            config / "overlays" / f"{name}_functions.csv"
            for name in (
                "free_duel", "main_menu", "overworld_after_coup",
                "overworld_before_coup", "password_a", "password_b",
            )
        ]
        count = 0
        for inventory in inventories:
            with inventory.open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertTrue(rows)
            self.assertTrue(all(r["status"] == "matching_c" for r in rows))
            count += len(rows)
        self.assertEqual(count, 124)


if __name__ == "__main__":
    unittest.main()
