from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect12Tests(unittest.TestCase):
    def test_lifecycle_uses_one_configuration_and_the_canonical_request(self) -> None:
        source = (DIRECTORY / "effect_12.c").read_text()
        for expression in (
            "if (phase >= 0)", "work->config = &D_8015AEE4;",
            "scale = D_80146188;",
            "world = *(MATRIX *)Model_GetLightSourceMatrix();",
            "work->frame += frame_step;", "work->scale < 0x4000",
            "work->scale += 0x1000;", "D_8009B264->field_1D = 1;",
            "func_80153F28((u8 *)&work->color, 15);", "D_8009B261 = 1;",
        ):
            self.assertIn(expression, source)
        self.assertNotIn("phase >= 6", source)
        self.assertNotIn("asm", source)

    def test_fixed_geometry_and_original_transform_sequence_are_preserved(self) -> None:
        source = (DIRECTORY / "effect_12.c").read_text()
        header = (DIRECTORY / "effect_12.h").read_text()
        for declaration in (
            "SVECTOR rings[3][32];", "SVECTOR positions[32];",
            "SVECTOR velocities[32];",
        ):
            self.assertIn(declaration, header)
        self.assertIn("for (i = 0; i < 3; i++)", source)
        self.assertIn("for (i = 0; i < 32; i++)", source)
        self.assertEqual(source.count("func_801514BC(&saved, &scale);"), 2)
        self.assertIn(
            "scale.vx = work->scale >> 1;\n"
            "            scale.vy = work->scale >> 1;\n"
            "            scale.vz = work->scale >> 1;\n"
            "            func_80155BC0(",
            source,
        )

    def test_configuration_and_scale_keep_real_object_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name in ("D_80146188", "D_8015AEE4"):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:0x10", symbols)
            self.assertNotIn(name + " =", aliases)
        self.assertIn(
            "extern DuelEffect12Config D_8015AEE4;",
            (DIRECTORY / "effect_12.h").read_text(),
        )


if __name__ == "__main__":
    unittest.main()
