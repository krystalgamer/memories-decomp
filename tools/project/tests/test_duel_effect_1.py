from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect1Tests(unittest.TestCase):
    def test_lifecycle_keeps_independent_frames_and_three_completion_colors(self) -> None:
        source = (DIRECTORY / "effect_1.c").read_text()
        for expression in (
            "if (phase >= 2)", "work->config = &D_8015B420[phase];",
            "work->cross_frame > 180", "work->frame += frame_step;",
            "work->tick++;", "work->frame < work->config->first_end",
            "work->frame < work->config->second_end", "work->stage = 1;",
            "work->growth < 0x7000", "work->growth += 0x400;",
            "D_8009B264->field_1D = 1;",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("Model_SetFrameStepOverride(1);"), 2)
        self.assertEqual(source.count("func_8014D378("), 3)
        self.assertEqual(source.count("D_8009B261 = 1;"), 2)
        self.assertNotIn("asm", source)

    def test_geometry_retains_both_fixed_counts_and_original_strip_color(self) -> None:
        source = (DIRECTORY / "effect_1.c").read_text()
        header = (DIRECTORY / "effect_1.h").read_text()
        for declaration in (
            "SVECTOR rotations[12];", "SVECTOR positions[64];",
            "SVECTOR velocities[64];", "DuelEffect1Config D_8015B420[2];",
        ):
            self.assertIn(declaration, header)
        self.assertIn("for (i = 0; i < 12; i++)", source)
        self.assertIn("for (i = 0; i < 64; i++)", source)
        self.assertIn("(work->rotation_step * (work->tick << 1)) & 0xFFF", source)
        self.assertEqual(source.count("color_copy = work->beam_color;"), 2)
        self.assertIn("func_80155D90((u8 *)&work->beam_color,", source)

    def test_initial_scale_and_two_configurations_keep_real_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_80146208", "0x10"), ("D_8015B420", "0x30")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
