from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect8Tests(unittest.TestCase):

    def test_caller_evidence_refines_shared_halfword_contracts(self) -> None:
        for name in ("utility_helpers.h", "radial_random_vectors.c"):
            self.assertIn(
                "func_8014EE0C(u16 width, u16 depth, s16 height,",
                (DIRECTORY / name).read_text(),
            )
        for name in ("drawing_helpers.h", "primitive_draw.c"):
            self.assertIn(
                "func_80156E58(u8 *color, u16 width,",
                (DIRECTORY / name).read_text(),
            )

    def test_configuration_keeps_real_storage_and_canonical_matrix_owner(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_80146014", "0x10"), ("D_8015A514", "0xE4")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)
        resident = (ROOT / "config/sles_03951/symbols.txt").read_text()
        binding = "Model_GetLightSourceMatrix = 0x8005C328;"
        self.assertIn(binding, symbols)
        self.assertIn(binding, aliases)
        self.assertTrue(binding in resident)


if __name__ == "__main__":
    unittest.main()
