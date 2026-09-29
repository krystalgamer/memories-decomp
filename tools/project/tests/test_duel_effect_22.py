import json
from pathlib import Path
import re
import sys
import unittest


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments


DIRECTORY = ROOT / "src/overlays/duel_effects"
CONFIG = ROOT / "config/sles_03951/overlays"


class DuelEffect22Tests(unittest.TestCase):
    def test_all_variants_and_phase_670_keep_distinct_initialization(self) -> None:
        source = (DIRECTORY / "effect_22.c").read_text()
        expected = {670: 0, 671: 1, 721: 20, 665: 21, 666: 22, 667: 23}
        expected.update({phase: phase - 671 for phase in range(673, 681)})
        expected.update({phase: phase - 681 for phase in range(691, 701)})
        actual = {int(phase): int(index) for phase, index in re.findall(
            r"case (\d+): work->config = &D_8015A658\[(\d+)\]; break;", source)}
        self.assertEqual(actual, expected)
        self.assertIn("default: work->cross_frame = 1;\n        case 670:", source)
        self.assertIn("Duel_CheckRitual((DuelRitualResult *)D_8015B7A0, phase);", source)

    def test_particle_initialization_and_update_extents_are_not_normalized(self) -> None:
        source = (DIRECTORY / "effect_22.c").read_text()
        header = (DIRECTORY / "effect_22.h").read_text()
        self.assertIn("func_8014F608(work->particles, 392, 392, 392, 32);", source)
        for declaration in (
            "SVECTOR particles[64];", "SVECTOR particle_velocities[64];",
            "SVECTOR line_inner[64];", "SVECTOR line_outer[64];",
            "SVECTOR line_velocities[64];", "SVECTOR rotations[32];",
            "extern DuelEffect22Config D_8015A658[24];",
        ):
            self.assertIn(declaration, header)
        self.assertIn("for (i = 0; i < 64; i++)", source)
        self.assertNotIn("memset(work", source)

    def test_flame_gate_preserves_matching_pointer_allocation(self) -> None:
        source = (DIRECTORY / "effect_22.c").read_text()
        self.assertIn("vertices[j].vy -= 48;", source)
        self.assertIn("""if (work->stage >= 2) {
                    work->texture_frames[i] = (work->texture_frames[i] + 1) % 4;
                } else if ((u16)(work->frame % 3) == 0) {
                    work->texture_frames[i] = (work->texture_frames[i] + 1) % 4;""", source)
        self.assertNotIn("work->stage >= 2 ||", source)

    def test_completion_and_outer_strip_retain_original_color_choices(self) -> None:
        source = (DIRECTORY / "effect_22.c").read_text()
        self.assertIn("""if ((u16)func_8014D378((u8 *)&work->flash_color) &&
            (u16)func_8014D378((u8 *)&work->flash_color) &&
            work->stage == 5)""", source)
        self.assertIn("func_80155D90((u8 *)&work->particle_color,", source)
        self.assertIn("color.r /= 2;", source)

    def test_switch_table_uses_the_function_object_not_an_absolute_alias(self) -> None:
        layout = CONFIG / "duel_effects.yaml"
        text = layout.read_text()
        for segment in (
            "[0x54, .rodata, overlays/duel_effects/effect_22]",
            "[0x138, data, overlays/spanish_duel_effects/header_after_effect_22_table]",
            "[0x48E4, c, overlays/duel_effects/effect_22]",
        ):
            self.assertIn(segment, text)
        self.assertNotIn("text_after_tile", text)
        source = "src/overlays/duel_effects/effect_22.c"
        units = [unit for unit in c_segments(ROOT, layout) if unit["source"] == source]
        self.assertEqual(len(units), 1)
        self.assertEqual(units[0]["kind"], "text")
        self.assertEqual(units[0]["profile"], "gcc_2_8_1_g0_split")
        aliases = (CONFIG / "duel_effects_linker_symbols.txt").read_text()
        for name in ("D_80146044", "D_8015A658", "jtbl_80146054", "D_80146054"):
            self.assertNotIn(name + " =", aliases)
        manifest = json.loads((CONFIG / "duel_effects_matching_c.json").read_text())
        self.assertEqual(
            [entry for entry in manifest["functions"] if entry["source"] == source],
            [{"address": "0x8014A8E4", "size": "0x2018", "source": source,
              "profile": "gcc_2_8_1_g0_split"}],
        )


if __name__ == "__main__":
    unittest.main()
