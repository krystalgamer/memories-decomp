import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/overlays/duel_effects/effect_23.c"


class DuelEffect23Tests(unittest.TestCase):
    def test_spanish_registration_reuses_the_complete_french_source(self) -> None:
        entries = []
        for region in ("sles_03948", "sles_03951"):
            manifest = json.loads(
                (ROOT / f"config/{region}/overlays/duel_effects_matching_c.json").read_text()
            )
            entries.append([row for row in manifest["functions"] if row["address"] == "0x80152048"])
        self.assertEqual(entries[0], entries[1])
        self.assertEqual(entries[1], [{
            "address": "0x80152048", "size": "0xE7C",
            "source": "src/overlays/duel_effects/effect_23.c",
            "profile": "gcc_2_8_1_g0_split",
        }])




    def test_initial_vector_has_exact_real_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        self.assertIn("D_801461A8 = 0x801461A8; // size:0x10", symbols)
        self.assertNotIn("D_801461A8 =", aliases)
        self.assertEqual(0x801461A8 + 16, 0x801461B8)


if __name__ == "__main__":
    unittest.main()
