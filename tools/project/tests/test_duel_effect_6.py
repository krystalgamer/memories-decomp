from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect6Tests(unittest.TestCase):
    def test_lifecycle_preserves_independent_counters_and_variant_paths(self) -> None:
        source = (DIRECTORY / "effect_6.c").read_text()
        for expression in (
            "if (phase >= 6)", "work->config = &D_8015B30C[phase];",
            "work->cross_frame > 180", "work->frame += frame_step;",
            "work->tick++;", "if (!(work->tick & 1))",
            "work->active = work->config->count;",
            "work->states[i] = 1;", "work->states[i] = 2;",
            "if (work->variant >= 5)", "work->hold >= 32",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("D_8009B261 = 1;"), 3)
        self.assertNotIn("asm", source)

    def test_geometry_preserves_observed_dimensions_and_signed_division(self) -> None:
        source = (DIRECTORY / "effect_6.c").read_text()
        header = (DIRECTORY / "effect_6.h").read_text()
        self.assertIn("SVECTOR trails[16][4];", header)
        self.assertIn("SVECTOR particles[16][32];", header)
        self.assertIn("DuelEffect6Config D_8015B30C[6];", header)
        self.assertIn("x * 70 / 4096, y * EFFECT_6_START_HEIGHT / 4096", source)
        self.assertIn("#define EFFECT_6_START_HEIGHT 106", source)
        self.assertIn("work->velocities[i].vx / (j + 1)", source)
        self.assertIn("work->velocities[i].vy / (j + 1)", source)
        self.assertIn("work->scales[i] < 0x6000", source)
        self.assertIn("work->scales[i] += 0x1000;", source)
        self.assertIn("scale = D_801461F8;", source)

    def test_configuration_and_initial_scale_retain_generated_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_801461F8", "0x10"), ("D_8015B30C", "0xB4")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)


if __name__ == "__main__":
    unittest.main()
