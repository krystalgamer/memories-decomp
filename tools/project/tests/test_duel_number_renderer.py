import csv
import json
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"
CONFIG = ROOT / "config/sles_03951/overlays"


class DuelNumberRendererTests(unittest.TestCase):
    def test_complete_renderer_has_its_own_c_extent(self) -> None:
        manifest = json.loads((CONFIG / "duel_effects_matching_c.json").read_text())
        entries = [entry for entry in manifest["functions"]
                   if entry["source"] == "src/overlays/duel_effects/number_renderer.c"]
        self.assertEqual(entries, [{
            "address": "0x801566D4", "size": "0x400",
            "source": "src/overlays/duel_effects/number_renderer.c",
            "profile": "gcc_2_8_1_g0_split",
        }])
        with (CONFIG / "duel_effects_functions.csv").open() as handle:
            rows = {row["address"]: row for row in csv.DictReader(handle)}
        self.assertEqual(rows["0x801566D4"]["status"], "matching_c")
        self.assertEqual(rows["0x8014FABC"]["status"], "unmatched_asm")
        self.assertIn("[0x106D4, c, overlays/duel_effects/number_renderer]",
                      (CONFIG / "duel_effects.yaml").read_text())

    def test_glyphs_and_texture_words_keep_real_separate_storage(self) -> None:
        source = (DIRECTORY / "number_renderer.c").read_text()
        self.assertIn("extern RECT D_8015B3C0[12];",
                      (DIRECTORY / "drawing_helpers.h").read_text())
        self.assertNotRegex(source, r"\bextern\b")
        self.assertIn('#include "textured_quads.h"', source)
        self.assertIn("D_8015B748.pairs[9][0]", source)
        self.assertIn("D_8015B748.pairs[9][1]", source)
        self.assertNotRegex(source, r"\b(?:typedef|asm|__asm__)\b")
        self.assertNotRegex(source, r"D_8015B3C0\s*\[[^\]]*\]\s*=")
        symbols = (CONFIG / "duel_effects_symbols.txt").read_text()
        aliases = (CONFIG / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_8015B3C0", "0x60"),
                           ("D_8015B748", "0x54"), ("D_8015B7F4", "0x4")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)
        self.assertNotIn("func_801566D4 =", aliases)

    def test_projection_and_submission_keep_the_retail_control_flow(self) -> None:
        source = (DIRECTORY / "number_renderer.c").read_text()
        for expression in (
            "SVECTOR vertices[20];", "u16 digits[8];",
            "count = func_80156AD4(value);",
            "func_80156B40(__builtin_abs(value), digits);",
            "i < count * 2 + 4", "i < count + 1",
            "if (value > 0)", "tile = 10;", "tile = 11;",
            "copy = polygon;", "if (bias == 0)", "if (flag >= 0)",
            "depth = adjusted - bias;", "if (depth >= 0 && flag >= 0)",
        ):
            self.assertIn(expression, source)
        for mode in (1, 0):
            self.assertRegex(source, re.compile(
                rf"if \(mode == {mode}\) \{{\s*draw_packet = packet;\s*draw_mode = {mode};"
            ))
        self.assertEqual(source.count("func_80152F9C(draw_packet, draw_mode);"), 1)
        self.assertRegex(source, r"else \{\s*vertex \+= 2;\s*continue;\s*\}")
        for mode in (1, 0):
            self.assertIn(
                f"func_8005B260((u32 *)packet, D_8015B7F4, (u16)(depth >> 2), {mode});",
                source,
            )


if __name__ == "__main__":
    unittest.main()
