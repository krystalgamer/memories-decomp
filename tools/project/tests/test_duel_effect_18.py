from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect18Tests(unittest.TestCase):
    def test_helper_contracts_preserve_observed_argument_values(self) -> None:
        drawing = (DIRECTORY / "drawing_helpers.h").read_text()
        lines = (DIRECTORY / "cross_lines.c").read_text()
        layered = (DIRECTORY / "layered_drawing.h").read_text()
        self.assertIn("void func_8014E35C(s32 mode)", drawing)
        self.assertIn("void func_8014E35C(s32 mode)", lines)
        self.assertIn("u16 *widths, u16 height, s16 depth", layered)
        source = (DIRECTORY / "effect_18.c").read_text()
        self.assertIn("func_8014E35C(1);", source)
        self.assertIn("func_8014E35C(0);", source)
        self.assertIn("applyVector(&work->rotations[i], 128, 128, 128, +=);", source)

    def test_lifecycle_keeps_timed_and_particle_paths_distinct(self) -> None:
        source = (DIRECTORY / "effect_18.c").read_text()
        self.assertIn("if (phase >= 2)", source)
        self.assertIn("work->config = &D_8015B078[phase];", source)
        self.assertIn("work->cross_frame > 180", source)
        self.assertIn("work->frame >= work->config->delay", source)
        self.assertIn("work->frame += frame_step;", source)
        self.assertIn("D_8009B264->field_1D = 1;", source)
        self.assertEqual(source.count("D_8009B261 = 1;"), 2)
        self.assertEqual(source.count("working = saved;"), 2)
        self.assertIn("scale = D_801461D8;", source)
        self.assertNotIn("asm", source)

    def test_configuration_and_scale_remain_real_generated_data(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_801461D8", "0x10"), ("D_8015B078", "0x3C")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
