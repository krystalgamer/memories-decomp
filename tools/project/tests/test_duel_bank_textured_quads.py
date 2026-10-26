import json
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]


class DuelBankTexturedQuadTests(unittest.TestCase):
    def test_complete_pair_preserves_extent_profile_and_definition_order(self) -> None:
        # The textured pair opens the merged primitive-drawing unit.
        source = "src/overlays/duel_effects/primitive_draw.c"
        manifest = json.loads(
            (ROOT / "config/sles_03951/overlays/duel_effects_matching_c.json").read_text()
        )
        entries = [row for row in manifest["functions"] if row["source"] == source]
        self.assertEqual(
            [(int(row["address"], 0), int(row["size"], 0)) for row in entries[:2]],
            [(0x80156C40, 0x110), (0x80156D50, 0x108)],
        )
        self.assertEqual({row["profile"] for row in entries}, {"gcc_2_8_1_g0_split"})

    def test_texture_words_remain_preserved_data_and_callee_is_not_aliased(self) -> None:
        aliases = (
            ROOT / "config/sles_03951/overlays/duel_effects_linker_symbols.txt"
        ).read_text()
        self.assertNotIn("D_8015B748 =", aliases)
        self.assertNotIn("func_80151218 =", aliases)


if __name__ == "__main__":
    unittest.main()
