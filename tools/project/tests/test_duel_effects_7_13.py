import csv
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffects7And13Tests(unittest.TestCase):


    def test_effect_thirteen_keeps_complete_paths_and_distinct_images(self) -> None:
        aliases = (ROOT / "config/sles_03951/overlays/duel_effects_linker_symbols.txt").read_text()
        self.assertNotIn("D_8015AC48 =", aliases)
        self.assertNotIn("D_8015AE94 =", aliases)

    def test_helpers_preserve_status_and_exact_extent(self) -> None:
        with (ROOT / "config/sles_03951/overlays/duel_effects_functions.csv").open() as handle:
            rows = {row["address"]: row for row in csv.DictReader(handle)}
        for address, size, status in (
            ("0x8014FABC", "0x344", "matching_c"),
            ("0x801566D4", "0x400", "matching_c"),
        ):
            self.assertEqual(rows[address]["status"], status)
            self.assertEqual(rows[address]["size"], size)


if __name__ == "__main__":
    unittest.main()
