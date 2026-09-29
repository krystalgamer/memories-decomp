from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffects45Tests(unittest.TestCase):
    def test_effect_four_preserves_card_transition_and_completion(self) -> None:
        source = (DIRECTORY / "effect_4.c").read_text()
        for expression in (
            "if (phase >= 2)", "work->config = &D_8015B704[phase];",
            "work->cross_frame > 180", "work->frame >= work->config->duration",
            "work->stage = 1;", "D_8009B264->field_1D = 1;",
            "(*(u32 *)&work->card_color & 0xFFFFFF) == 0x808080",
            "work->frame += frame_step;",
            "if ((u16)func_8014D378((u8 *)&work->color))",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("func_801514BC(&saved, &scale);"), 2)
        self.assertEqual(source.count("D_8009B261 = 1;"), 2)
        self.assertLess(source.index("packet = &polygon;"), source.index("scale = D_80146248;"))

    def test_effect_four_reuses_texture_and_request_owners(self) -> None:
        source = (DIRECTORY / "effect_4.c").read_text()
        header = (DIRECTORY / "effect_4.h").read_text()
        self.assertIn('#include "../../game/duel_effect_request.h"', header)
        self.assertIn("D_8015B748.pairs[work->config->texture][0]", source)
        self.assertIn("D_8015B748.pairs[work->config->texture][1]", source)
        self.assertIn("extern DuelEffect4Config D_8015B704[2];", header)
        for member in ("rings[3][32]", "rotations[16]", "positions[24]",
                       "velocities[24]", "card_rings[2][4]"):
            self.assertIn(f"SVECTOR {member};", header)

    def test_effect_five_preserves_distinct_variant_paths(self) -> None:
        source = (DIRECTORY / "effect_5.c").read_text()
        for expression in (
            "if (phase >= 10)", "work->config = &D_8015B450[phase];",
            "setVector(&work->rising_positions[i],",
            "-work->config->bounce_speed / work->bounces",
            "work->spawned = work->config->strip_count;",
            "work->variant >= 5 && work->hold_frames >= 24",
            "work->frame += frame_step;", "work->tick++;",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("(rand() - rand()) % 4096 * 70 / 4096"), 2)
        self.assertEqual(source.count("matrix = saved;"), 2)
        self.assertEqual(source.count("GsSetLsMatrix(&matrix);"), 2)
        self.assertEqual(source.count("work->variant < 5 || work->hold_frames == 0"), 2)

    def test_effect_five_keeps_existing_number_contract_and_array_bounds(self) -> None:
        header = (DIRECTORY / "effect_5.h").read_text()
        self.assertIn('#include "effect_6.h"', header)
        self.assertNotIn("void func_801566D4(", header)
        self.assertIn("extern DuelEffect5Config D_8015B450[10];", header)
        for member in ("rising_positions[64]", "positions[64]", "velocities[64]",
                       "rotations[64]", "rings[3][32]"):
            self.assertIn(f"SVECTOR {member};", header)
        self.assertIn("CVECTOR rising_colors[64];", header)

    def test_spanish_data_retains_real_generated_owners(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (
            ("D_80146248", "0x10"), ("D_8015B704", "0x38"),
            ("D_80146218", "0x10"), ("D_8015B450", "0x168"),
        ):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
