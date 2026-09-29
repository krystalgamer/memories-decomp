from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect0Tests(unittest.TestCase):
    def test_configuration_bounds_and_separate_crossed_line_path(self) -> None:
        source = (DIRECTORY / "effect_0.c").read_text()
        header = (DIRECTORY / "effect_0.h").read_text()
        self.assertIn("extern DuelEffect0Config D_8015B0B4[30];", header)
        self.assertIn("if (phase >= 0)", source)
        self.assertIn("if (phase >= 30)", source)
        self.assertIn("work->config = &D_8015B0B4[phase];", source)
        self.assertIn("work->cross_frame > 180", source)
        self.assertIn("func_8014E35C(1);", source)
        self.assertIn("func_8014E35C(0);", source)
        self.assertEqual(source.count("D_8009B261 = 1;"), 2)

    def test_drawing_retains_saved_step_and_unclamped_growth(self) -> None:
        source = (DIRECTORY / "effect_0.c").read_text()
        self.assertIn("work->frame >= work->config->delay", source)
        self.assertIn("work->frame += frame_step;", source)
        self.assertIn("D_8009B264->field_1D = 1;", source)
        self.assertEqual(source.count("working = saved;"), 2)
        self.assertIn("applyVector(&work->rotations[i], 128, 128, 128, +=);", source)
        self.assertIn("if (work->scale < 0x7000)", source)
        self.assertIn("work->scale += 0x1000;", source)
        self.assertNotIn("work->scale = 0x7000", source)
        self.assertNotIn("asm", source)

    def test_configuration_and_scale_have_generated_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_801461E8", "0x10"), ("D_8015B0B4", "0x258")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
