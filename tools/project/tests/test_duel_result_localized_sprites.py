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
            "src/game/european/duel_result_orbit_sprites.c",
            "src/game/japanese/duel_result_runtime.c",
        ):
            with self.subTest(source=source):
                text = self.preprocess(source)
                self.assertEqual(text.count("for (i = 0; i < 7; i++)"), 2)
                self.assertNotIn("D_8009C2BC", text)
                self.assertNotIn("D_80091B30", text)

    def test_localized_tables_and_active_count_preserve_binary_order(self) -> None:
        text = self.preprocess("src/game/spanish/duel_result_orbit_sprites.c")
        self.assertIn("for (i = 0; i < 10; i++)", text)
        self.assertIn("for (i = 0; i < D_8009C2BC; i++)", text)
        for symbol in (
            "D_80091B30", "D_80091B80", "D_80091BD0", "D_80091C20",
            "D_80091C70", "D_80091CC0", "D_80091D10", "D_80091D60",
        ):
            self.assertIn(f"extern DuelResultSpriteSpec {symbol}[2][10];", text)
            self.assertIn(f"spec = &{symbol}[gDuel_bWinnerSide][i];", text)
        self.assertIn(
            "if (spec->x == 0) { continue; } D_8009C2BC = i + 1; "
            "slots[i].object = 0; if (spec->kind != 0)", text,
        )

    def test_all_four_functions_keep_complete_contiguous_group(self) -> None:
        manifest = json.loads((ROOT / "config/sles_03951/matching_c.json").read_text())
        group = [
            e for e in manifest["functions"]
            if e["source"] == "src/game/spanish/duel_result_orbit_sprites.c"
        ]
        self.assertEqual(
            [(int(e["address"], 0), int(e["size"], 0)) for e in group],
            [(0x80020CB0, 0x19C), (0x80020E4C, 0x64),
             (0x80020EB0, 0x688), (0x80021538, 0xDC)],
        )
        self.assertEqual({e["profile"] for e in group}, {"gcc_2_8_1_g8_split"})

    def test_spanish_resident_and_all_overlay_game_targets_are_complete(self) -> None:
        config = ROOT / "config/sles_03951"
        with (config / "functions.csv").open() as handle:
            resident = list(csv.DictReader(handle))
        self.assertEqual(sum(r["status"] == "matching_c" for r in resident), 1140)
        self.assertFalse(any(r["status"] == "unmatched_asm" for r in resident))
        inventories = sorted((config / "overlays").glob("*_functions.csv"))
        self.assertEqual(len(inventories), 6)
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
