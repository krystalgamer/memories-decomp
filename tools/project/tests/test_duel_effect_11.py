import csv
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect11Tests(unittest.TestCase):


    def test_caller_contracts_are_paired_and_sdk_ownership_is_preserved(self) -> None:
        for declaration, files in (
            ("void func_8014EE0C(u16 width, u16 depth, s16 height,",
             ("utility_helpers.h", "radial_random_vectors.c")),
            ("void func_80156E58(u8 *color, u16 width,",
             ("drawing_helpers.h", "primitive_draw.c")),
        ):
            for filename in files:
                self.assertIn(declaration, (DIRECTORY / filename).read_text())
        region = ROOT / "config/sles_03951"
        for filename in ("link_symbols.ld", "overlays/duel_effects_linker_symbols.txt"):
            self.assertIn("ratan2 = 0x80089928;", (region / filename).read_text())
        with (region / "functions.csv").open() as handle:
            rows = {row["address"]: row for row in csv.DictReader(handle)}
        self.assertEqual(rows["0x80089928"]["status"], "sdk_asm")
        self.assertEqual(int(rows["0x80089928"]["size"], 0), 372)
        self.assertEqual(rows["0x8005C328"]["name"], "Model_GetLightSourceMatrix")
        self.assertEqual(rows["0x8005C328"]["status"], "matching_c")


if __name__ == "__main__":
    unittest.main()
