import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class DuelSharedEffectsTests(unittest.TestCase):
    def test_pal_wrappers_reuse_complete_shared_sources_and_named_profile(self) -> None:
        path = ROOT / "config/sles_03951/overlays/duel_effects_matching_c.json"
        entries = {row["address"]: row for row in json.loads(path.read_text())["functions"]}
        for address, size, name in (
            ("0x801481A8", "0x9FC", "gather_effect"),
            ("0x80149F90", "0x954", "tile_effect"),
        ):
            entry = entries[address]
            self.assertEqual(entry["size"], size)
            self.assertEqual(entry["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(entry["source"], f"src/overlays/european/duel_effects/{name}.c")
            wrapper = (ROOT / entry["source"]).read_text()
            self.assertIn("#define VERSION_EUROPE", wrapper)
            self.assertIn(f'#include "../../duel_effects/{name}.c"', wrapper)

    def test_only_verified_regional_geometry_is_selected(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/tile_effect.c").read_text()
        self.assertIn("#define TILE_ROW_STEP 30", source)
        self.assertIn("#define TILE_ROW_STEP 28", source)
        self.assertIn("#define TILE_Y_ORIGIN 106", source)
        self.assertIn("#define TILE_Y_ORIGIN 98", source)
        self.assertIn("#define TILE_V_BOTTOM -120", source)
        self.assertIn("#define TILE_V_BOTTOM 126", source)
        self.assertIn("tile->x0 = k * 28 - 70;", source)
        self.assertIn("tile->x1 = k * 28 - 42;", source)
        source = (ROOT / "src/overlays/duel_effects/gather_effect.c").read_text()
        self.assertIn("#define GATHER_CURVE_HEIGHT 106", source)
        self.assertIn("#define GATHER_CURVE_HEIGHT 98", source)

    def test_descriptor_owners_are_generated_storage_not_absolute_aliases(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        for name, size in (
            ("D_80146024", "0x10"), ("D_8015A5F8", "0x14"),
            ("D_8015A62C", "0x1C"), ("D_8015A648", "0x10"),
        ):
            self.assertNotIn(name + " =", aliases)
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)


if __name__ == "__main__":
    unittest.main()
