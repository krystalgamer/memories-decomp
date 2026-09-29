import csv
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffects7And13Tests(unittest.TestCase):
    def test_effect_seven_preserves_pools_and_matrix_copy(self) -> None:
        header = (DIRECTORY / "effect_7.h").read_text()
        for declaration in (
            "SVECTOR trails[16][4];", "SVECTOR rings[3][32];",
            "SVECTOR particles[16][32];", "SVECTOR particle_velocities[16][32];",
            "extern DuelEffect7Config D_8015B5B8[5];",
        ):
            self.assertIn(declaration, header)
        source = (DIRECTORY / "effect_7.c").read_text()
        self.assertIn("matrix = saved;\n                    ScaleMatrix(&matrix, &scale);", source)
        self.assertIn("work->config->duration;", source)
        self.assertIn("func_801566D4(work->config->number,", source)
        self.assertIn("work->number_velocity.vy = -16 / work->bounces;", source)
        self.assertIn("work->active = work->config->count;", source)

    def test_effect_thirteen_keeps_caller_number_and_byte_truncations(self) -> None:
        source = (DIRECTORY / "effect_13.c").read_text()
        self.assertIn("void func_801503F8(void *buffer, s32 phase, s16 number)", source)
        self.assertIn("work->number = number;", source)
        self.assertNotRegex(source, r"work->config->number\b")
        self.assertIn("func_801566D4(-__builtin_abs(work->number),", source)
        self.assertIn("void func_801503F8(void *work, s32 phase, s16 variant);",
                      (DIRECTORY / "dispatch.h").read_text())
        self.assertEqual(source.count("color.r /= 4;"), 3)
        self.assertNotIn("color.g /= 4;", source)
        self.assertNotIn("color.b /= 4;", source)

    def test_effect_thirteen_keeps_complete_paths_and_distinct_images(self) -> None:
        header = (DIRECTORY / "effect_13.h").read_text()
        for declaration in (
            "SVECTOR paths[48][8];", "SVECTOR endpoints[48];",
            "CVECTOR colors[48];", "extern GsIMAGE D_8015AC48[21];",
            "extern DuelEffect13Config D_8015AE94[4];",
        ):
            self.assertIn(declaration, header)
        source = (DIRECTORY / "effect_13.c").read_text()
        self.assertIn("work->active += 2;", source)
        self.assertIn("work->active = 48;", source)
        self.assertIn("work->ages[i] = 7;", source)
        self.assertIn("work->number_velocity.vy = -12 / work->bounces;", source)
        aliases = (ROOT / "config/sles_03951/overlays/duel_effects_linker_symbols.txt").read_text()
        self.assertNotIn("D_8015AC48 =", aliases)
        self.assertNotIn("D_8015AE94 =", aliases)

    def test_unmatched_helpers_remain_visible_game_owned_assembly(self) -> None:
        with (ROOT / "config/sles_03951/overlays/duel_effects_functions.csv").open() as handle:
            rows = {row["address"]: row for row in csv.DictReader(handle)}
        for address, size in (("0x8014FABC", "0x344"), ("0x801566D4", "0x400")):
            self.assertEqual(rows[address]["status"], "unmatched_asm")
            self.assertEqual(rows[address]["size"], size)


if __name__ == "__main__":
    unittest.main()
