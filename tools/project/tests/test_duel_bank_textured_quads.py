import json
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]


class DuelBankTexturedQuadTests(unittest.TestCase):
    def test_complete_pair_preserves_extent_profile_and_definition_order(self) -> None:
        source = "src/overlays/duel_effects/textured_quads.c"
        manifest = json.loads(
            (ROOT / "config/sles_03951/overlays/duel_effects_matching_c.json").read_text()
        )
        entries = [row for row in manifest["functions"] if row["source"] == source]
        self.assertEqual(
            [(int(row["address"], 0), int(row["size"], 0)) for row in entries],
            [(0x80156C40, 0x110), (0x80156D50, 0x108)],
        )
        self.assertEqual({row["profile"] for row in entries}, {"gcc_2_8_1_g0_split"})
        text = (ROOT / source).read_text()
        self.assertEqual(
            re.findall(r"^void (func_[0-9A-F]+)\(", text, re.M),
            ["func_80156C40", "func_80156D50"],
        )
        self.assertEqual(text.count("addVector(&packet->vertices[i], offset)"), 2)

    def test_texture_words_remain_preserved_data_and_callee_is_not_aliased(self) -> None:
        header = (ROOT / "src/overlays/duel_effects/textured_quads.h").read_text()
        self.assertIn("extern DuelEffectQuadTextureWords D_8015B748;", header)
        self.assertIn("u8 unknown[0x18];", header)
        for field in (
            "page_at_00", "clut_at_02", "page_at_04", "clut_at_06",
            "page_at_08", "clut_at_0A", "page_at_0C", "clut_at_0E",
        ):
            self.assertIn(f"u16 {field};", header)
        self.assertIn("POLY_FT4 polygon;", header)
        self.assertIn("SVECTOR vertices[4];", header)
        aliases = (
            ROOT / "config/sles_03951/overlays/duel_effects_linker_symbols.txt"
        ).read_text()
        self.assertNotIn("D_8015B748 =", aliases)
        self.assertNotIn("func_80151218 =", aliases)


if __name__ == "__main__":
    unittest.main()
