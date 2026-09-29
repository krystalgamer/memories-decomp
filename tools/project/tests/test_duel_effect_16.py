import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = ROOT / "src/overlays/duel_effects"


class DuelEffect16Tests(unittest.TestCase):
    def test_complete_extent_and_canonical_header(self) -> None:
        source = (DIRECTORY / "effect_16.c").read_text()
        header = (DIRECTORY / "effect_16.h").read_text()
        manifest = json.loads(
            (ROOT / "config/sles_03951/overlays/duel_effects_matching_c.json").read_text()
        )
        entries = [row for row in manifest["functions"] if row["address"] == "0x80151558"]
        self.assertEqual(entries, [{
            "address": "0x80151558", "size": "0xAF0",
            "source": "src/overlays/duel_effects/effect_16.c",
            "profile": "gcc_2_8_1_g0_split",
        }])
        self.assertIn('#include "../../types.h"', source)
        self.assertNotIn("extern ", source)
        self.assertNotIn("asm", source)
        self.assertIn("DuelEffect16Config D_8015AEF4;", header)
        self.assertIn("VECTOR D_80146198;", header)
        self.assertIn("u16 ring_height;", header)
        self.assertIn('game/duel_effect_request.h"', header)

    def test_overlapping_particle_windows_are_not_transposed(self) -> None:
        source = (DIRECTORY / "effect_16.c").read_text()
        header = (DIRECTORY / "effect_16.h").read_text()
        self.assertIn("SVECTOR positions[160];", header)
        self.assertIn("SVECTOR velocities[160];", header)
        self.assertIn("func_8014F180(8, &work->positions[i * 5], &work->velocities[i * 5], 32);", source)
        self.assertIn("func_8014F2D4(quad, &(&work->positions[j * 5])[k]);", source)
        self.assertIn("addVector(&(&work->positions[j * 5])[k],", source)
        self.assertIn("&(&work->velocities[j * 5])[k]);", source)
        self.assertNotIn("positions[5][32]", header)

    def test_path_and_animation_contracts(self) -> None:
        source = (DIRECTORY / "effect_16.c").read_text()
        header = (DIRECTORY / "effect_16.h").read_text()
        self.assertIn("SVECTOR paths[5][4][5];", header)
        for expression in (
            "if (k == 4)", "work->paths[k][l][i].vx = 0;",
            "work->paths[k][l][i].vz = 0;",
            "copyVector(&quad[l]", "copyVector(&translation",
            "D_8015B748.pairs[14][0]", "D_8015B748.pairs[14][1]",
            "D_8015B7F8.vz >= 0 ? 95 : -95",
            "work->ages[j]++;", "work->ages[j] = 4;",
            "D_8009B264->field_1D = work->hits;",
            "work->scales[j] < 16384", "work->scales[j] += 4096;",
            "if (!(work->tick & 7))", "work->count > 5",
        ):
            self.assertIn(expression, source)

    def test_both_lifecycles_and_completion_colors_remain(self) -> None:
        source = (DIRECTORY / "effect_16.c").read_text()
        for expression in (
            "if (phase >= 2)", "work->cross_frame = 1;",
            "work->cross_frame > 180",
            "Model_SetFrameStepOverride(1);",
            "func_80153F28((u8 *)&work->primary[j], 31);",
            "func_80153F28((u8 *)&work->secondary[j], 15);",
            "func_8014D378((u8 *)&work->primary[4])",
            "func_8014D378((u8 *)&work->secondary[4])",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("D_8009B261 = 1;"), 2)

    def test_vector_and_config_have_real_nonoverlapping_storage(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_80146198", "0x10"), ("D_8015AEF4", "0x24")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)
        self.assertEqual(0x80146198 + 16, 0x801461A8)
        self.assertEqual(0x8015AEF4 + 36, 0x8015AF18)


if __name__ == "__main__":
    unittest.main()
