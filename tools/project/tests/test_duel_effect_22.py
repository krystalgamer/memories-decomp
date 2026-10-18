import json
from pathlib import Path
import re
import sys
import unittest


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments


DIRECTORY = ROOT / "src/overlays/duel_effects"
CONFIG = ROOT / "config/sles_03951/overlays"


class DuelEffect22Tests(unittest.TestCase):




    def test_switch_table_uses_the_function_object_not_an_absolute_alias(self) -> None:
        layout = CONFIG / "duel_effects.yaml"
        text = layout.read_text()
        for segment in (
            "[0x54, .rodata, overlays/duel_effects/effect_22]",
            "[0x138, data, overlays/spanish_duel_effects/header_after_effect_22_table]",
            "[0x48E4, c, overlays/duel_effects/effect_22]",
        ):
            self.assertIn(segment, text)
        self.assertNotIn("text_after_tile", text)
        source = "src/overlays/duel_effects/effect_22.c"
        units = [unit for unit in c_segments(ROOT, layout) if unit["source"] == source]
        self.assertEqual(len(units), 1)
        self.assertEqual(units[0]["kind"], "text")
        self.assertEqual(units[0]["profile"], "gcc_2_8_1_g0_split")
        aliases = (CONFIG / "duel_effects_linker_symbols.txt").read_text()
        for name in ("D_80146044", "D_8015A658", "jtbl_80146054", "D_80146054"):
            self.assertNotIn(name + " =", aliases)
        manifest = json.loads((CONFIG / "duel_effects_matching_c.json").read_text())
        self.assertEqual(
            [entry for entry in manifest["functions"] if entry["source"] == source],
            [{"address": "0x8014A8E4", "size": "0x2018", "source": source,
              "profile": "gcc_2_8_1_g0_split"}],
        )


if __name__ == "__main__":
    unittest.main()
