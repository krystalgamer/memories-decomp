from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect21Tests(unittest.TestCase):
    def test_slots_reuse_the_canonical_resident_payload_pointer(self) -> None:
        source = (DIRECTORY / "effect_21.c").read_text()
        header = (DIRECTORY / "effect_21.h").read_text()
        self.assertIn('#include "../../game/high_memory_addresses.h"', header)
        self.assertIn('#include "../../game/screen_projection.h"', header)
        self.assertIn("D_80010000 + 0x2800", source)
        self.assertIn("D_80010000 + 0x2880", source)
        self.assertIn("world = *(MATRIX *)Model_GetLightSourceMatrix();", source)
        config = ROOT / "config/sles_03951"
        aliases = (config / "overlays/duel_effects_linker_symbols.txt").read_text()
        resident = (config / "symbols.txt").read_text() + (
            config / "link_symbols.ld"
        ).read_text()
        for name, address in (
            ("D_80010000", "0x80010000"),
            ("Model_GetLightSourceMatrix", "0x8005C328"),
        ):
            self.assertIn(f"{name} = {address};", aliases)
            self.assertTrue(f"{name} = {address};" in resident, name)

    def test_original_partial_initialization_and_halfword_increment_survive(self) -> None:
        source = (DIRECTORY / "effect_21.c").read_text()
        self.assertIn("for (i = 0; i < 8; i++)", source)
        self.assertEqual(source.count("->tiles[i] = rand() % 3;"), 1)
        self.assertIn("if (++slots[__builtin_abs(phase + 1) % 2]->count > 10)", source)
        self.assertIn("D_8009B264->field_1A = ~(__builtin_abs(phase) % 2);", source)
        self.assertIn("D_8009B264->field_1D = 2;", source)
        self.assertNotIn("asm", source)

    def test_real_overlay_data_extents_remain_generated_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_80146158", "0x10"), ("D_8015AB68", "0xA0")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
