from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect2Tests(unittest.TestCase):
    def test_zero_value_uses_the_last_real_configuration_record(self) -> None:
        source = (DIRECTORY / "effect_2.c").read_text()
        header = (DIRECTORY / "effect_2.h").read_text()
        self.assertIn("if (phase >= 7)", source)
        self.assertIn("work->config = &D_8015AF18[6];", source)
        self.assertIn("work->config = &D_8015AF18[phase];", source)
        self.assertIn("DuelEffect2Config D_8015AF18[7];", header)
        self.assertNotIn("D_8015B044", source + header)

    def test_lifecycle_preserves_signed_value_and_separate_completion_colors(self) -> None:
        source = (DIRECTORY / "effect_2.c").read_text()
        for expression in (
            "void func_80153200(void *buffer, s32 phase, s16 number)",
            "work->number = value;", "-__builtin_abs(work->number)",
            "work->cross_frame > 180", "work->frame += frame_step;",
            "work->stage < 3", "work->scale < 0x6000",
            "work->scale += 0x800;", "scale = D_801461B8;",
            "func_8014D378((u8 *)&work->color)",
            "func_8014D378((u8 *)&work->background_color)",
            "func_8014D378((u8 *)&work->number_color)",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("D_8009B261 = 1;"), 2)
        self.assertNotIn("asm", source)

    def test_table_and_initial_vector_keep_real_generated_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_801461B8", "0x10"), ("D_8015AF18", "0x15E")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)
        self.assertNotIn("D_8015B044 =", aliases)


if __name__ == "__main__":
    unittest.main()
