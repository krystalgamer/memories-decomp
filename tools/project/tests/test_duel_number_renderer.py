import csv
import json
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"
CONFIG = ROOT / "config/sles_03951/overlays"


class DuelNumberRendererTests(unittest.TestCase):
    def test_renderer_and_curve_keep_separate_matching_c_extents(self) -> None:
        manifest = json.loads((CONFIG / "duel_effects_matching_c.json").read_text())
        entries = [entry for entry in manifest["functions"]
                   if entry["source"] == "src/overlays/duel_effects/number_renderer.c"]
        # The renderer opens the unit; its two digit helpers follow it.
        self.assertEqual([(e["address"], e["size"]) for e in entries],
                         [("0x801566D4", "0x400"), ("0x80156AD4", "0x6C"),
                          ("0x80156B40", "0x100")])
        self.assertEqual({e["profile"] for e in entries}, {"gcc_2_8_1_g0_split"})
        with (CONFIG / "duel_effects_functions.csv").open() as handle:
            rows = {row["address"]: row for row in csv.DictReader(handle)}
        self.assertEqual(rows["0x801566D4"]["status"], "matching_c")
        self.assertEqual((rows["0x8014FABC"]["status"], rows["0x8014FABC"]["size"]),
                         ("matching_c", "0x344"))
        self.assertIn("[0x106D4, c, overlays/duel_effects/number_renderer]",
                      (CONFIG / "duel_effects.yaml").read_text())

    def test_glyphs_and_texture_words_keep_real_separate_storage(self) -> None:
        symbols = (CONFIG / "duel_effects_symbols.txt").read_text()
        aliases = (CONFIG / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_8015B3C0", "0x60"),
                           ("D_8015B748", "0x54"), ("D_8015B7F4", "0x4")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)
        self.assertNotIn("func_801566D4 =", aliases)



if __name__ == "__main__":
    unittest.main()
