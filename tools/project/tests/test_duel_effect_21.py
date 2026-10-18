from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect21Tests(unittest.TestCase):
    def test_slots_reuse_the_canonical_resident_payload_pointer(self) -> None:
        config = ROOT / "config/sles_03951"
        aliases = (config / "overlays/duel_effects_linker_symbols.txt").read_text()
        resident = (config / "symbols.txt").read_text() + (
            config / "link_symbols.ld"
        ).read_text()
        for name, address in (
            ("D_80010000", "0x80010000"),
            ("Model_GetLightSourceMatrix", "0x8005C328"),
        ):
            self.assertIn(f"{name} = {address};", aliases)
            self.assertTrue(f"{name} = {address};" in resident, name)


    def test_real_overlay_data_extents_remain_generated_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_80146158", "0x10"), ("D_8015AB68", "0xA0")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
