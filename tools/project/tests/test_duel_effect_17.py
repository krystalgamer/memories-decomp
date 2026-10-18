import csv
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect17Tests(unittest.TestCase):


    def test_resident_bindings_and_curve_c_keep_real_owners(self) -> None:
        region = ROOT / "config/sles_03951"
        with (region / "functions.csv").open() as handle:
            resident = {row["address"]: row for row in csv.DictReader(handle)}
        aliases = (region / "overlays/duel_effects_linker_symbols.txt").read_text()
        for name, address in (
            ("Duel_CollectMatchingFieldCardObjects", "0x8002CB88"),
            ("Model_GetLightSourceMatrix", "0x8005C328"),
        ):
            self.assertEqual(resident[address]["name"], name)
            self.assertEqual(resident[address]["status"], "matching_c")
            self.assertIn(f"{name} = {address};", aliases)
        self.assertEqual(resident["0x80089928"]["status"], "sdk_asm")
        with (region / "overlays/duel_effects_functions.csv").open() as handle:
            curve = next(row for row in csv.DictReader(handle) if row["address"] == "0x8014FABC")
        self.assertEqual((curve["status"], curve["size"]), ("matching_c", "0x344"))
        for name in ("D_80146034", "D_8015A60C", "D_8015B7A0"):
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
