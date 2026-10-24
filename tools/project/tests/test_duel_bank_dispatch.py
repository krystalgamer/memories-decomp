import csv
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelBankDispatchTests(unittest.TestCase):


    def test_callees_and_data_keep_real_overlay_owners(self) -> None:
        for region in ("sles_03951", "sles_03948"):
            with self.subTest(region=region):
                self._assert_real_owners(region)

    def _assert_real_owners(self, region: str) -> None:
        directory = ROOT / f"config/{region}/overlays"
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        source = (DIRECTORY / "dispatch.c").read_text()
        views = dict(re.findall(
            r"extern void (\w+)\([^)]*\) __asm__\(\"(func_[0-9A-F]+)\"\);",
            (DIRECTORY / "handler_semantic_views.h").read_text(),
        ))
        self.assertEqual(len(views), 6)
        with (directory / "duel_effects_functions.csv").open() as handle:
            inventory = {row["name"]: row for row in csv.DictReader(handle)}
        named_callees = {
            "DuelEffect_RunCardBreakup": ("func_80146760", 0x80146760, 0x13B8),
            "DuelEffect_RunRowStrike": ("func_80151558", 0x80151558, 0xAF0),
            "DuelEffect_PackTexturePageClut": ("func_8014F564", 0x8014F564, 0x6C),
            "DuelEffect_RunContourBurst": ("func_80154688", 0x80154688, 0x4A8),
            "DuelEffect_RunParticleContourBurst": ("func_80154084", 0x80154084, 0x604),
        }
        callees = set(re.findall(
            r"\b(func_[0-9A-F]+|"
            + "|".join((*named_callees, *views)) + r")\(", source
        )) - {"func_80146258"}
        self.assertEqual(len(callees), 25)
        for name in callees:
            self.assertNotIn(name + " =", aliases)
            if name in named_callees:
                native, address, size = named_callees[name]
                self.assertNotIn(native + " =", aliases)
                self.assertEqual(inventory[name]["module"], "overlay/duel_effects")
                self.assertEqual(int(inventory[name]["address"], 16), address)
                self.assertEqual(int(inventory[name]["size"], 16), size)
            if name in views:
                native = views[name]
                self.assertNotIn(native + " =", aliases)
                self.assertNotIn(native, inventory)
                self.assertEqual(inventory[name]["module"], "overlay/duel_effects")
                self.assertEqual(
                    int(inventory[name]["address"], 16), int(native[5:], 16)
                )
                self.assertIn(f"{native} = 0x{native[5:]}; // type:func", symbols)
            self.assertIn(inventory[name]["status"], ("matching_c", "unmatched_asm"))
        for address, size in (
            (0x8015A1E4, 0x24C), (0x8015A430, 0xB4),
            (0x8015B748, 0x54), (0x8015B7F4, 4),
            (0x8015B7F8, 8), (0x8015B800, 2),
        ):
            name = f"D_{address:X}"
            self.assertNotIn(name + " =", aliases)
            self.assertIn(f"{name} = 0x{address:X}; // size:0x{size:X}", symbols)



if __name__ == "__main__":
    unittest.main()
