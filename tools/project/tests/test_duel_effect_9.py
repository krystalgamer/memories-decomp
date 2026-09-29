import csv
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect9Tests(unittest.TestCase):
    def test_lifecycle_preserves_signed_bounces_and_all_four_completion_colors(self) -> None:
        source = (DIRECTORY / "effect_9.c").read_text()
        for expression in (
            "if (phase >= 5)", "work->config = &D_8015B650[phase];",
            "work->cross_frame > 180", "work->negative_step += 4;",
            "work->positive_step = work->negative_step;",
            "work->negative_step = -work->config->bounce_speed;",
            "work->bounce_count < 5", "work->bounce_count++;",
            "work->negative_step = -work->config->bounce_speed /",
            "work->positive.vy < 200", "} else if (work->positive.vy >= 200)",
            "work->stage >= 2", "if (work->stage == 1)",
            "SD_SEPlayFull(26);", "D_8009B264->field_1D = 1;",
            "work->stage = 2;", "work->frame += frame_step;", "work->tick++;",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("func_8014D378("), 4)
        self.assertEqual(source.count("D_8009B261 = 1;"), 2)
        self.assertNotIn("asm", source)

    def test_shared_number_renderer_contract_does_not_claim_the_helper_as_c(self) -> None:
        declaration = "void func_801566D4(s32 value, u8 *color, SVECTOR *offset, u16 mode,"
        self.assertIn(declaration, (DIRECTORY / "drawing_helpers.h").read_text())
        self.assertNotIn(declaration, (DIRECTORY / "effect_6.h").read_text())
        source = (DIRECTORY / "effect_9.c").read_text()
        self.assertIn("func_801566D4(-work->config->value,", source)
        self.assertIn("func_801566D4(work->config->value,", source)
        directory = ROOT / "config/sles_03951/overlays"
        with (directory / "duel_effects_functions.csv").open() as handle:
            helper = next(row for row in csv.DictReader(handle)
                          if row["address"] == "0x801566D4")
        self.assertEqual(helper["status"], "unmatched_asm")
        binding = "SD_SEPlayFull = 0x80040204;"
        self.assertIn(binding, (directory / "duel_effects_symbols.txt").read_text())
        self.assertIn(binding, (directory / "duel_effects_linker_symbols.txt").read_text())
        self.assertTrue(binding in (ROOT / "config/sles_03951/symbols.txt").read_text())

    def test_local_views_and_generated_storage_keep_the_observed_extents(self) -> None:
        header = (DIRECTORY / "effect_9.h").read_text()
        for declaration in (
            "SVECTOR positions[32];", "SVECTOR velocities[32];",
            "SVECTOR rings[3][32];", "SVECTOR rotations[64];",
            "s16 positive_step;", "s16 negative_step;",
            "DuelEffect9Config D_8015B650[5];",
        ):
            self.assertIn(declaration, header)
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_80146238", "0x10"), ("D_8015B650", "0xB4")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
