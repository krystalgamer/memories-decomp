from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect19Tests(unittest.TestCase):
    def test_resident_state_has_canonical_declaration_and_binding_owners(self) -> None:
        header = (DIRECTORY / "effect_19.h").read_text()
        self.assertIn('#include "../../game/duel_effect_request.h"', header)
        self.assertIn("#define D_8009B264_VISIBLE", header)
        self.assertIn('#include "../../unmatched.h"', header)
        self.assertNotIn("extern DuelEffectRequest", header)
        directory = ROOT / "config/sles_03951"
        resident = (directory / "symbols.txt").read_text() + (
            directory / "link_symbols.ld"
        ).read_text()
        aliases = (directory / "overlays/duel_effects_linker_symbols.txt").read_text()
        for name, address in (
            ("Model_GetFrameStep", "0x8005BF24"),
            ("Model_SetFrameStepOverride", "0x8005CBF4"),
            ("PushMatrix", "0x80087158"), ("PopMatrix", "0x800871FC"),
            ("memset", "0x8008F548"),
            ("D_8009B264", "0x8009C5FC"), ("D_8009B261", "0x8009C600"),
        ):
            binding = f"{name} = {address};"
            self.assertIn(binding, aliases)
            self.assertIn(binding, resident)
        self.assertNotIn("D_801461C8 =", aliases)
        symbols = (directory / "overlays/duel_effects_symbols.txt").read_text()
        self.assertIn("D_801461C8 = 0x801461C8; // size:0x10", symbols)

    def test_update_preserves_observed_side_effects_and_state_recheck(self) -> None:
        source = (DIRECTORY / "effect_19.c").read_text()
        self.assertIn("scale = D_801461C8;", source)
        self.assertEqual(source.count("if (work->fade_started == 0)"), 2)
        self.assertIn("(u16)func_8014D3AC((u8 *)&work->color)", source)
        self.assertIn("(u16)func_8014D378((u8 *)&work->color)", source)
        self.assertIn("color_copy = work->color;", source)
        for channel in "rgb":
            self.assertIn(f"color_copy.{channel} >>= 2;", source)
        self.assertIn(
            "func_80155D90((u8 *)&work->color, work->widths, 48, 0);", source
        )
        self.assertIn("D_8009B264->field_1D = 1;", source)
        self.assertIn("D_8009B261 = 1;", source)
        self.assertIn("work->spawned > 64", source)
        self.assertIn("work->scale < 0x5000", source)
        self.assertNotIn("asm", source)


if __name__ == "__main__":
    unittest.main()
