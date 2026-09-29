import csv
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect11Tests(unittest.TestCase):
    def test_overlapping_modes_and_images_have_one_owner(self) -> None:
        header = (DIRECTORY / "image_inputs.h").read_text()
        for declaration in (
            "typedef union", "u16 modes[21];", "u16 leading_modes[20];",
            "GsIMAGE images[5];", "sizeof(DuelEffectImageInputs) == 0xB4",
            "->variant.images == 0x28",
            "extern DuelEffectImageInputs D_8015A430;",
        ):
            self.assertIn(declaration, header)
        self.assertIn('#include "image_inputs.h"', (DIRECTORY / "dispatch.h").read_text())
        source = (DIRECTORY / "effect_11.c").read_text()
        self.assertIn("image = &D_8015A430.variant.images[variant];", source)
        self.assertNotIn("D_8015A458", source + (DIRECTORY / "effect_11.h").read_text())

    def test_lifecycle_preserves_branch_local_motion_and_pool_bounds(self) -> None:
        header = (DIRECTORY / "effect_11.h").read_text()
        source = (DIRECTORY / "effect_11.c").read_text()
        for declaration in (
            "SVECTOR positions[4][7];", "SVECTOR particles[4][7][16];",
            "u16 sizes[4][7][16];", "u16 chances[4][7];",
            "CVECTOR particle_colors[4][7];", "SVECTOR rays[32];",
            "SVECTOR dust[64];", "SVECTOR dust_velocities[64];",
        ):
            self.assertIn(declaration, header)
        self.assertEqual(source.count("work->velocities[i][j].vy += 3;"), 2)
        for expression in (
            "work->dust_velocities[i].vy += 0;", "work->states[i][j] = 16;",
            "work->completed == 28", "work->cross > 180", "variant >= 5",
            "variant = phase & 15;", "if ((phase >> 7) == 1)",
            "work->sizes[i][j][k]--;", "work->chances[i][j]--;",
        ):
            self.assertIn(expression, source)

    def test_caller_contracts_are_paired_and_sdk_ownership_is_preserved(self) -> None:
        for declaration, files in (
            ("void func_8014EE0C(u16 width, u16 depth, s16 height,",
             ("utility_helpers.h", "radial_random_vectors.c")),
            ("void func_80156E58(u8 *color, u16 width,",
             ("drawing_helpers.h", "primitive_draw.c")),
        ):
            for filename in files:
                self.assertIn(declaration, (DIRECTORY / filename).read_text())
        region = ROOT / "config/sles_03951"
        for filename in ("link_symbols.ld", "overlays/duel_effects_linker_symbols.txt"):
            self.assertIn("ratan2 = 0x80089928;", (region / filename).read_text())
        with (region / "functions.csv").open() as handle:
            rows = {row["address"]: row for row in csv.DictReader(handle)}
        self.assertEqual(rows["0x80089928"]["status"], "sdk_asm")
        self.assertEqual(int(rows["0x80089928"]["size"], 0), 372)
        self.assertEqual(rows["0x8005C328"]["name"], "Model_GetLightSourceMatrix")
        self.assertEqual(rows["0x8005C328"]["status"], "matching_c")


if __name__ == "__main__":
    unittest.main()
