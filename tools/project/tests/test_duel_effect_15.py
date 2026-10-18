from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect15Tests(unittest.TestCase):

    def test_collection_and_object_coordinates_use_canonical_owners(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        resident = (ROOT / "config/sles_03951/symbols.txt").read_text()
        for binding in (
            "Duel_CollectFieldRowCardObjects = 0x8002CB0C;",
            "Duel_CollectMatchingFieldCardObjects = 0x8002CB88;",
            "Model_GetLightSourceMatrix = 0x8005C328;",
        ):
            self.assertIn(binding, symbols)
            self.assertIn(binding, aliases)
            self.assertTrue(binding in resident)

    def test_tables_and_shared_output_retain_real_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (
            ("D_80146168", "0x10"), ("D_8015AC08", "0x40"),
            ("D_8015B7A0", "0x54"),
        ):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
