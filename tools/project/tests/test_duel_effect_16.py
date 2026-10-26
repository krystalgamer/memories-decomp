import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect16Tests(unittest.TestCase):
    def test_complete_extent_and_canonical_header(self) -> None:
        manifest = json.loads(
            (ROOT / "config/sles_03951/overlays/duel_effects_matching_c.json").read_text()
        )
        entries = [row for row in manifest["functions"] if row["address"] == "0x80151558"]
        self.assertEqual(entries, [{
            "address": "0x80151558", "size": "0xAF0",
            "source": "src/overlays/duel_effects/row_attack_effects.c",
            "profile": "gcc_2_8_1_g0_split",
        }])




    def test_vector_and_config_have_real_nonoverlapping_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_80146198", "0x10"), ("D_8015AEF4", "0x24")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)
        self.assertEqual(0x80146198 + 16, 0x801461A8)
        self.assertEqual(0x8015AEF4 + 36, 0x8015AF18)


if __name__ == "__main__":
    unittest.main()
