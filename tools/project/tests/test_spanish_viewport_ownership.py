import json
from pathlib import Path
import re
import unittest

import yaml


ROOT = Path(__file__).resolve().parents[3]
CONFIG = ROOT / "config/sles_03951"


class SpanishViewportOwnershipTests(unittest.TestCase):
    def test_viewport_bytes_come_from_the_complete_graphics_object(self) -> None:
        segments = yaml.safe_load((CONFIG / "split.yaml").read_text())["segments"]
        index = next(
            i for i, segment in enumerate(segments)
            if isinstance(segment, dict) and segment.get("name") == "viewport_state"
        )
        viewport = segments[index]
        following = segments[index + 1]
        self.assertEqual(viewport["type"], "code")
        self.assertEqual(viewport["start"], 0x8CCC0)
        self.assertEqual(viewport["vram"], 0x8009C4C0)
        self.assertEqual(viewport["align"], 2)
        self.assertEqual(
            viewport["subsegments"],
            [[0x8CCC0, ".sbss", "game/graphics_frame"]],
        )
        self.assertEqual(following["type"], "bin")
        self.assertEqual(following["start"], 0x8CCC4)
        self.assertEqual(following["vram"], 0x8009C4C4)

    def test_viewport_definitions_are_not_overridden_by_absolute_aliases(self) -> None:
        symbols = (CONFIG / "symbols.txt").read_text()
        linker = (CONFIG / "link_symbols.ld").read_text()
        for name, address in (
            ("gGraphics_sViewportX", 0x8009C4C0),
            ("gGraphics_sViewportY", 0x8009C4C2),
        ):
            with self.subTest(symbol=name):
                self.assertIn(f"{name} = 0x{address:08X}; // defined:True", symbols)
                self.assertIsNone(re.search(rf"^\s*{name}\s*=", linker, re.M))

    def test_graphics_functions_keep_their_complete_contiguous_group(self) -> None:
        manifest = json.loads((CONFIG / "matching_c.json").read_text())
        functions = [
            entry for entry in manifest["functions"]
            if entry["source"] == "src/game/graphics_frame.c"
        ]
        self.assertEqual(
            [(int(entry["address"], 0), int(entry["size"], 0)) for entry in functions],
            [(0x80012CB8, 0xA8), (0x80012D60, 0x210)],
        )
        self.assertEqual(
            {entry["profile"] for entry in functions},
            {"gcc_2_8_1_g8_split_comm"},
        )


if __name__ == "__main__":
    unittest.main()
