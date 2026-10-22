from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect19Tests(unittest.TestCase):
    def test_resident_state_has_canonical_declaration_and_binding_owners(self) -> None:
        directory = ROOT / "config/sles_03951"
        resident = (directory / "symbols.txt").read_text() + (
            directory / "link_symbols.ld"
        ).read_text()
        aliases = (directory / "overlays/duel_effects_linker_symbols.txt").read_text()
        for name, address in (
            ("Model_GetFrameStep", "0x8005BF24"),
            ("Model_SetFrameStepOverride", "0x8005CBF4"),
            ("PushMatrix", "0x80087158"), ("PopMatrix", "0x800871FC"),
            ("memset", "0x8008F548"),
            ("gDuelEffect_pActiveRequest", "0x8009C5FC"), ("gDuelEffect_bRequestFinished", "0x8009C600"),
        ):
            binding = f"{name} = {address};"
            self.assertIn(binding, aliases)
            self.assertIn(binding, resident)
        self.assertNotIn("D_801461C8 =", aliases)
        symbols = (directory / "overlays/duel_effects_symbols.txt").read_text()
        self.assertIn("D_801461C8 = 0x801461C8; // size:0x10", symbols)



if __name__ == "__main__":
    unittest.main()
