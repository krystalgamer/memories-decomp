import csv
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect9Tests(unittest.TestCase):

    def test_shared_number_renderer_contract_and_real_owner(self) -> None:
        declaration = "void func_801566D4(s32 value, u8 *color, SVECTOR *offset, u16 mode,"
        directory = ROOT / "config/sles_03951/overlays"
        with (directory / "duel_effects_functions.csv").open() as handle:
            helper = next(row for row in csv.DictReader(handle)
                          if row["address"] == "0x801566D4")
        self.assertEqual(helper["status"], "matching_c")
        binding = "SD_SEPlayFull = 0x80040204;"
        self.assertIn(binding, (directory / "duel_effects_symbols.txt").read_text())
        self.assertIn(binding, (directory / "duel_effects_linker_symbols.txt").read_text())
        self.assertTrue(binding in (ROOT / "config/sles_03951/symbols.txt").read_text())

    def test_local_views_and_generated_storage_keep_the_observed_extents(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_80146238", "0x10"), ("D_8015B650", "0xB4")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
