import csv
import hashlib
import json
from pathlib import Path
import re
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_extract import OverlayError, read_module


class DuplicateOverlayTests(unittest.TestCase):
    def setUp(self) -> None:
        temporary = tempfile.TemporaryDirectory(prefix="duplicate-overlay-", dir=ROOT / "tmp")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.archive = self.root / "archive.bin"
        self.archive.write_bytes(b"----abcd----abcd----abcd")
        self.module = {
            "name": "example",
            "archive": "archive.bin",
            "archive_sha256": hashlib.sha256(self.archive.read_bytes()).hexdigest(),
            "sector_offset": 1,
            "sector_count": 1,
            "duplicate_sector_offsets": [3, 5],
            "sha256": hashlib.sha256(b"abcd").hexdigest(),
            "output": "tmp/module.bin",
        }

    def test_all_copies_are_checked_without_changing_primary_output(self) -> None:
        output, payload = read_module(self.root, 4, self.module)
        self.assertEqual(output, self.root / "tmp/module.bin")
        self.assertEqual(payload, b"abcd")

    def test_existing_single_instance_manifest_still_works(self) -> None:
        del self.module["duplicate_sector_offsets"]
        self.assertEqual(read_module(self.root, 4, self.module)[1], b"abcd")

    def test_changed_last_copy_is_not_hidden_by_correct_primary(self) -> None:
        self.archive.write_bytes(b"----abcd----abcd----abce")
        self.module["archive_sha256"] = hashlib.sha256(self.archive.read_bytes()).hexdigest()
        with self.assertRaisesRegex(OverlayError, "sector 5 differs"):
            read_module(self.root, 4, self.module)

    def test_invalid_duplicate_locations_are_rejected(self) -> None:
        for offsets in (None, "3", [True], [-1], [1.5], [1], [3, 3], [6]):
            with self.subTest(offsets=offsets):
                self.module["duplicate_sector_offsets"] = offsets
                with self.assertRaises(OverlayError):
                    read_module(self.root, 4, self.module)


class FrenchDuelBankTests(unittest.TestCase):
    def test_loader_locations_cover_every_terrain_copy(self) -> None:
        modules = json.loads((ROOT / "config/sles_03948/overlays.json").read_text())["modules"]
        bank = next(module for module in modules if module["name"] == "french_duel_effects")
        phase_order = [64, 5, 4, 5, 32, 1, 2, 32]
        expected = [0x1B88 + sum(phase_order) + terrain * 0xF0 for terrain in range(7)]
        self.assertEqual([bank["sector_offset"], *bank["duplicate_sector_offsets"]], expected)
        self.assertEqual(bank["sector_count"], 44)
        self.assertEqual(bank["load_address"], "0x80146000")
        self.assertEqual(
            bank["sha256"],
            "a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3",
        )

    def test_inventory_keeps_complete_text_and_unmatched_boundaries(self) -> None:
        directory = ROOT / "config/sles_03948/overlays"
        with (directory / "duel_effects_functions.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        cursor = 0x80146258
        for row in rows:
            self.assertEqual(int(row["address"], 0), cursor)
            cursor += int(row["size"], 0)
        self.assertEqual(cursor, 0x8015A1E4)
        self.assertEqual(len(rows), 85)
        matched = [row for row in rows if row["status"] == "matching_c"]
        self.assertEqual(len(matched), 79)
        self.assertEqual(sum(int(row["size"], 0) for row in matched), 58780)
        self.assertEqual(sum(row["status"] == "unmatched_asm" for row in rows), 6)
        for address in ("0x8014FABC", "0x801566D4"):
            deferred = next(row for row in rows if row["address"] == address)
            self.assertEqual(deferred["status"], "unmatched_asm")
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        self.assertEqual(
            {(row["address"], row["size"]) for row in matched},
            {(row["address"], row["size"]) for row in manifest["functions"]},
        )
        self.assertEqual({row["profile"] for row in manifest["functions"]}, {"gcc_2_8_1_g0_split"})

    def test_vector_group_preserves_complete_definition_order(self) -> None:
        directory = ROOT / "config/sles_03948/overlays"
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        source = "src/overlays/duel_effects/vector_init.c"
        functions = [row for row in manifest["functions"] if row["source"] == source]
        cursor = 0x8014F608
        names = []
        for row in functions:
            self.assertEqual(int(row["address"], 0), cursor)
            names.append(f"func_{cursor:08X}")
            cursor += int(row["size"], 0)
        self.assertEqual(len(functions), 5)
        self.assertEqual(cursor, 0x8014FABC)
        text = (ROOT / source).read_text()
        self.assertEqual(re.findall(r"^void (func_[0-9A-F]+)\(", text, re.MULTILINE), names)
        self.assertIn('#include "utility_helpers.h"', text)
        self.assertNotRegex(text, r"\b(?:extern|asm|__asm__)\b")

    def test_small_helper_groups_have_exact_extents_and_definition_order(self) -> None:
        manifest = json.loads(
            (ROOT / "config/sles_03948/overlays/duel_effects_matching_c.json").read_text()
        )
        for name, start, end, count in (
            ("dispatch", 0x80146258, 0x80146760, 1),
            ("effect_8", 0x80147B18, 0x801481A8, 1),
            ("color_test", 0x8014D378, 0x8014D3E8, 2),
            ("cross_lines", 0x8014E35C, 0x8014E3EC, 1),
            ("effect_21", 0x8014E3EC, 0x8014EA7C, 1),
            ("ring_vertices", 0x8014EA7C, 0x8014EC8C, 2),
            ("height_ring", 0x8014EC8C, 0x8014EE0C, 1),
            ("radial_random_vectors", 0x8014EE0C, 0x8014F010, 2),
            ("rect_vertices", 0x8014F490, 0x8014F524, 1),
            ("quad_helpers", 0x8014FE00, 0x8014FF40, 2),
            ("effect_12", 0x80150E00, 0x80151218, 1),
            ("projected_wrappers", 0x80151218, 0x801513F4, 2),
            ("matrix_setup", 0x801513F4, 0x801514BC, 1),
            ("matrix_helpers", 0x801514BC, 0x80151558, 2),
            ("packet_helpers", 0x80152EC4, 0x80153200, 4),
            ("effect_2", 0x80153200, 0x80153ADC, 1),
            ("color_transition", 0x80153F28, 0x80154084, 3),
            ("effect_0", 0x80154688, 0x80154B30, 1),
            ("effect_6", 0x80154B30, 0x801556F4, 1),
            ("screen_draw", 0x801556F4, 0x801558F4, 3),
            ("contour_quads", 0x801558F4, 0x80155BC0, 1),
            ("layered_drawing", 0x80155BC0, 0x80155F94, 2),
            ("drawing_tail", 0x80155F94, 0x80156448, 3),
            ("gradient_strip", 0x80156448, 0x801566D4, 1),
            ("number_helpers", 0x80156AD4, 0x80156C40, 2),
            ("textured_quads", 0x80156C40, 0x80156E58, 2),
            ("primitive_draw", 0x80156E58, 0x801570B0, 2),
            ("gradient_lines", 0x801570B0, 0x801573A8, 1),
            ("display_quads", 0x801573A8, 0x80157794, 3),
        ):
            with self.subTest(source=name):
                source = f"src/overlays/duel_effects/{name}.c"
                # These are registered through their PAL wrappers.
                registered = (source.replace("/duel_effects/", "/european/duel_effects/")
                              if name in ("screen_draw", "cross_lines", "effect_6") else source)
                functions = [row for row in manifest["functions"] if row["source"] == registered]
                names = []
                cursor = start
                for row in functions:
                    self.assertEqual(int(row["address"], 0), cursor)
                    names.append(f"func_{cursor:08X}")
                    cursor += int(row["size"], 0)
                self.assertEqual(len(functions), count)
                self.assertEqual(cursor, end)
                text = (ROOT / source).read_text()
                self.assertEqual(
                    re.findall(r"^(?:void|s32|u16) (func_[0-9A-F]+)\(", text, re.MULTILINE), names
                )
                self.assertNotRegex(text, r"\b(?:extern|asm|__asm__)\b")

    def test_spanish_drawing_groups_are_reused_without_regional_variants(self) -> None:
        manifests = [
            json.loads((ROOT / f"config/{region}/overlays/duel_effects_matching_c.json").read_text())
            for region in ("sles_03948", "sles_03951")
        ]
        for name, count in (("dispatch", 1), ("effect_18", 1), ("effect_19", 1),
                            ("number_helpers", 2), ("primitive_draw", 2),
                            ("textured_quads", 2), ("packet_helpers", 4),
                            ("gradient_strip", 1), ("gradient_lines", 1),
                            ("display_quads", 3),
                            ("color_transition", 3),
                            ("screen_draw", 3), ("contour_quads", 1), ("layered_drawing", 2),
                            ("drawing_tail", 3), ("effect_0", 1), ("effect_6", 1),
                            ("effect_9", 1)):
            prefix = "european/" if name in ("effect_6", "effect_18", "screen_draw") else ""
            source = f"src/overlays/{prefix}duel_effects/{name}.c"
            groups = [[row for row in manifest["functions"] if row["source"] == source]
                      for manifest in manifests]
            self.assertEqual(len(groups[0]), count)
            self.assertEqual(groups[0], groups[1])

    def test_effect_groups_keep_whole_sources_and_existing_profile(self) -> None:
        entries = json.loads(
            (ROOT / "config/sles_03948/overlays/duel_effects_matching_c.json").read_text()
        )["functions"]
        for name, address, size, pal in (
            ("gather_effect", 0x801481A8, 2556, True),
            ("tile_effect", 0x80149F90, 2388, True),
            ("effect_19", 0x80153ADC, 1100, False),
            ("effect_18", 0x80154084, 1540, True),
            ("effect_24", 0x8014D3E8, 3956, False),
            ("effect_23", 0x80152048, 3708, False),
            ("effect_13", 0x801503F8, 2568, False),
            ("effect_7", 0x801587D8, 3024, True),
            ("effect_10", 0x8014C8FC, 2684, False),
            ("effect_5", 0x80157E10, 2504, True),
            ("effect_4", 0x80159AAC, 1848, False),
            ("effect_15", 0x8014FF40, 1208, False),
            ("effect_1", 0x80157794, 1660, False),
            ("effect_9", 0x801593A8, 1796, False),
        ):
            with self.subTest(source=name):
                prefix = "european/" if pal else ""
                source = f"src/overlays/{prefix}duel_effects/{name}.c"
                self.assertEqual([row for row in entries if row["source"] == source], [{
                    "address": f"0x{address:X}", "size": f"0x{size:X}",
                    "source": source, "profile": "gcc_2_8_1_g0_split",
                }])
                if pal:
                    self.assertEqual((ROOT / source).read_text().splitlines(), [
                        '#include "../../../types.h"', "#define VERSION_EUROPE",
                        f'#include "../../duel_effects/{name}.c"',
                    ])
                body = (ROOT / f"src/overlays/duel_effects/{name}.c").read_text()
                self.assertEqual(re.findall(r"^void (func_[0-9A-F]+)\(", body, re.MULTILINE),
                                 [f"func_{address:X}"])

    def test_effect_data_and_resident_bindings_keep_real_owners(self) -> None:
        directory = ROOT / "config/sles_03948"
        aliases = (directory / "overlays/duel_effects_linker_symbols.txt").read_text()
        symbols = (directory / "overlays/duel_effects_symbols.txt").read_text()
        resident_aliases = (directory / "link_symbols.ld").read_text()
        for address, size in (
            (0x80146024, 16), (0x801461C8, 16), (0x801461D8, 16),
            (0x8015A5F8, 20), (0x8015A62C, 28), (0x8015A648, 16),
            (0x8015B078, 60),
            (0x80146148, 16), (0x8015B7F4, 4), (0x8015B800, 2),
            (0x801461A8, 16),
            (0x80146178, 16), (0x8015AC48, 588), (0x8015AE94, 80),
            (0x8015B748, 84), (0x8015B7F8, 8),
            (0x80146228, 16), (0x8015B5B8, 150),
            (0x80146138, 16), (0x8015A8C8, 588), (0x8015AB14, 84),
            (0x80146218, 16), (0x8015B450, 360),
            (0x80146248, 16), (0x8015B704, 56),
            (0x80146168, 16), (0x80146208, 16), (0x8015AC08, 64),
            (0x8015B420, 48), (0x8015B7A0, 84),
            (0x80146238, 16), (0x8015B650, 180),
        ):
            name = f"D_{address:X}"
            self.assertNotIn(name + " =", aliases)
            self.assertIn(f"{name} = 0x{address:X}; // size:0x{size:X}", symbols)
        for name, address in (
            ("D_8009B261", 0x8009C600), ("D_8009B264", 0x8009C5FC),
            ("PushMatrix", 0x80087158), ("PopMatrix", 0x800871FC),
            ("memset", 0x8008F548),
        ):
            binding = f"{name} = 0x{address:X};"
            self.assertIn(binding, aliases)
            self.assertIn(binding, resident_aliases)
        with (directory / "functions.csv").open() as handle:
            resident = {row["name"]: row for row in csv.DictReader(handle)}
        for name, address in (("Model_GetFrameStep", 0x8005BF24),
                              ("Model_SetFrameStepOverride", 0x8005CBF4),
                              ("DisplayObject_CopyWorkSlots", 0x8002CD24),
                              ("Model_GetLightSourceMatrix", 0x8005C328),
                              ("Duel_CollectFieldRowCardObjects", 0x8002CB0C),
                              ("SD_SEPlayFull", 0x80040204),
                              ("Duel_CollectMatchingFieldCardObjects", 0x8002CB88)):
            self.assertIn(f"{name} = 0x{address:X};", aliases)
            self.assertEqual(int(resident[name]["address"], 0), address)
            self.assertEqual(resident[name]["status"], "matching_c")

    def test_effect_twentyfour_uses_fixed_canonical_work_slots(self) -> None:
        header = (ROOT / "src/overlays/duel_effects/effect_24.h").read_text()
        source = (ROOT / "src/overlays/duel_effects/effect_24.c").read_text()
        self.assertIn('#include "../../game/display_object_work_slots.h"', header)
        self.assertIn("extern u32 D_8015B7A0[21];", header)
        self.assertNotIn("typedef struct", source)
        self.assertIn("DisplayObject_CopyWorkSlots((s32 *)D_8015B7A0);", source)
        for declaration in ("SVECTOR targets[5];", "SVECTOR points[5];",
                            "SVECTOR point_steps[5];", "SVECTOR heads[64];",
                            "SVECTOR tails[64];", "SVECTOR velocities[64];",
                            "SVECTOR rotations[32];", "u32 ray_scales[32];"):
            self.assertIn(declaration, header)
        self.assertIn("work->line_count = 64;", source)
        self.assertIn("work->ray_count = 32;", source)
        self.assertIn("D_8009B264->field_1D = 1;", source)
        self.assertIn("if (work->stage == 3) {\n            D_8009B261 = 1;", source)

    def test_effect_twentyfour_keeps_intentional_motion_and_projection(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/effect_24.c").read_text()
        self.assertIn("(s16)(work->center.vx - 24)", source)
        self.assertIn("(s16)(work->center.vy - 28)", source)
        self.assertIn("if (flag >= 0)", source)
        self.assertNotIn("= RotTransPers(", source)
        self.assertIn("Model_GetFrameStep();\n        Model_SetFrameStepOverride(1);", source)
        self.assertNotIn("work->frame", source)
        self.assertIn("work->ray_scales[i] += 0x400;", source)
        self.assertIn("work->scale += 128;", source)
        self.assertIn("work->stage < 2", source)

    def test_effect_twentythree_preserves_canonical_cards_and_empty_row(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/effect_23.c").read_text()
        header = (ROOT / "src/overlays/duel_effects/effect_23.h").read_text()
        self.assertIn('#include "../../game/display_object.h"', header)
        self.assertIn('#include "../../game/duel_card.h"', header)
        self.assertIn("extern u32 D_8015B7A0[21];", header)
        for declaration in (
            "SVECTOR slots[5];", "SVECTOR card_velocities[5];",
            "SVECTOR card_rotations[5];", "SVECTOR particles[5][3];",
            "SVECTOR velocities[5][3];", "u16 states[5];", "u16 ages[5];",
        ):
            self.assertIn(declaration, header)
        self.assertIn("Duel_CollectFieldRowCardObjects(D_8015B7A0, 1);", source)
        self.assertIn("while (D_8015B7A0[work->count] != 0)", source)
        for field in ("field_30.h.field_30", "field_30.h.field_32", "field_34.h.field_34"):
            self.assertIn(f"->{field} += work->card_velocities[i]", source)
        for field in ("field_20", "field_21", "field_22"):
            self.assertIn(f"->field_20.b.{field} += work->card_rotations[i].vx;", source)
        self.assertIn("work->ages[i] < 24 && work->states[i] >= 2", source)
        self.assertIn("i * 70 * (D_8015B7F8.vz >= 0 ? 1 : -1)", source)
        self.assertEqual(source.count("SD_SEPlayFull(36);"), 2)
        self.assertNotIn("cross_frame", source)
        completion = source.rsplit("PopMatrix();", 1)[1]
        self.assertLess(completion.index("if (work->count != 0)"),
                        completion.index("D_8015B7A0[work->count - 1]"))
        self.assertEqual(completion.count("func_8014D378((u8 *)&work->color)"), 2)
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")

    def test_effect_thirteen_preserves_caller_contract_and_retail_lifecycle(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/effect_13.c").read_text()
        header = (ROOT / "src/overlays/duel_effects/effect_13.h").read_text()
        dispatch = (ROOT / "src/overlays/duel_effects/dispatch.h").read_text()
        self.assertIn("void func_801503F8(void *work, s32 phase, s16 variant);", dispatch)
        self.assertIn("void func_801503F8(void *buffer, s32 phase, s16 number)", source)
        self.assertIn('#include "effect_6.h"', header)
        self.assertNotIn("void func_801566D4(", header)
        for declaration in (
            "SVECTOR paths[48][8];", "SVECTOR endpoints[48];",
            "u16 angles[48];", "u16 states[48];", "u16 ages[48];",
            "CVECTOR colors[48];", "extern GsIMAGE D_8015AC48[21];",
            "extern DuelEffect13Config D_8015AE94[4];",
        ):
            self.assertIn(declaration, header)
        self.assertEqual(source.count("color.r /= 4;"), 3)
        self.assertNotIn("color.g /= 4;", source)
        self.assertNotIn("color.b /= 4;", source)
        self.assertLess(source.index("scale_pointer = &scale;"),
                        source.index("position_pointer = &position;"))
        for operation in (
            "if (phase >= 4)", "work->number = number;",
            "func_801566D4(-__builtin_abs(work->number)",
            "work->number_velocity.vy += 3;", "work->number_velocity.vy = -12 / work->bounces;",
            "work->active += 2;", "work->active = 48;", "work->ages[i] = 7;",
        ):
            self.assertIn(operation, source)
        self.assertNotRegex(source, r"work->config->number\b")
        completion = source.rsplit("PopMatrix();", 1)[1]
        for field in ("colors[47]", "background_color", "number_color"):
            self.assertIn(f"func_8014D378((u8 *)&work->{field})", completion)
        self.assertNotIn("glow_color", completion)
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")

    def test_effect_seven_keeps_distinct_particle_and_number_lifecycle(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/effect_7.c").read_text()
        header = (ROOT / "src/overlays/duel_effects/effect_7.h").read_text()
        self.assertIn('#include "effect_6.h"', header)
        self.assertNotIn("void func_801566D4(", header)
        for declaration in (
            "SVECTOR trails[16][4];", "SVECTOR particles[16][32];",
            "SVECTOR particle_velocities[16][32];", "u16 ages[16];",
            "CVECTOR colors[16];", "s16 start_y;", "s16 number;",
            "extern DuelEffect7Config D_8015B5B8[5];",
        ):
            self.assertIn(declaration, header)
        for operation in (
            "if (phase >= 5)", "work->sizes[i] += work->config->growth;",
            "matrix = saved;", "ScaleMatrix(&matrix, &scale);",
            "GsSetLsMatrix(&matrix);", "work->number_velocity.vy += 4;",
            "work->number_velocity.vy = -16 / work->bounces;",
            "work->cross_frame > 180",
        ):
            self.assertIn(operation, source)
        self.assertEqual(source.count("setVector(&work->number_position, 0, 0, 0);"), 2)
        self.assertNotIn("work->velocities[i].vz =", source)
        self.assertNotIn("func_801514BC(", source)
        completion = source.rsplit("PopMatrix();", 1)[1]
        for field in ("colors[work->config->count - 1]", "background_color", "number_color"):
            self.assertIn(f"func_8014D378((u8 *)&work->{field})", completion)
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")

    def test_effect_ten_preserves_real_image_table_and_dissolve_contract(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        header = (directory / "effect_10.h").read_text()
        source = (directory / "effect_10.c").read_text()
        self.assertIn("extern GsIMAGE D_8015A8C8[21];", header)
        self.assertIn("extern DuelEffect10Config D_8015AB14[6];", header)
        for field in ("particles[64]", "columns[64]", "rings[2][4]", "plane[4]"):
            self.assertIn(f"SVECTOR {field};", header)
        for field in ("speeds[64]", "ages[64]"):
            self.assertIn(f"u16 {field};", header)
        self.assertIn("CVECTOR column_colors[64];", header)
        self.assertIn("if (phase >= 6)", source)
        self.assertIn("D_8015B748.pairs[12][0] = getTPage(D_8015A8C8[12].pmode,", source)
        self.assertIn("func_8014EC8C(175, 194, 16, (u16)(i * 8), work->rings[i], 4);", source)
        self.assertIn("func_8014F490(quad, (s16)(12 - j), (s16)(24 - j));", source)
        self.assertIn("for (j = 0; j < 4; j++)", source)
        self.assertIn("for (k = 0; k < 4; k++)", source)
        self.assertIn("if (work->ages[i] > 160)", source)
        self.assertIn("work->ages[i] = 0;", source)
        self.assertIn("if (!(work->tick & 3))", source)
        self.assertIn("work->spawned = 64;", source)
        self.assertIn("if (*(u32 *)&work->color_ready == 0x10001)", source)
        self.assertIn("D_8009B264->field_1D = 1;", source)
        self.assertIn("if (work->stage == 0 && phase < -1)", source)
        self.assertIn("func_8014F010((u8 *)&work->color, 64);", source)
        self.assertLess(source.index("packet = &polygon;"), source.index("scale = D_80146138;"))
        self.assertIn("work->frame += frame_step;", source)
        self.assertIn("work->tick++;", source)

    def test_effect_five_preserves_variants_and_number_renderer_contract(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        header = (directory / "effect_5.h").read_text()
        source = (directory / "effect_5.c").read_text()
        self.assertIn('#include "effect_6.h"', header)
        self.assertNotIn("void func_801566D4(", header)
        self.assertIn("extern DuelEffect5Config D_8015B450[10];", header)
        for member in ("rising_positions[64]", "positions[64]", "velocities[64]",
                       "rotations[64]", "rings[3][32]"):
            self.assertIn(f"SVECTOR {member};", header)
        self.assertIn("CVECTOR rising_colors[64];", header)
        self.assertIn("if (phase >= 10)", source)
        self.assertIn("setVector(&work->rising_positions[i],", source)
        self.assertEqual(source.count("(rand() - rand()) % 4096 * 70 / 4096"), 2)
        self.assertEqual(source.count("matrix = saved;"), 2)
        self.assertEqual(source.count("GsSetLsMatrix(&matrix);"), 2)
        self.assertEqual(source.count("work->variant < 5 || work->hold_frames == 0"), 2)
        self.assertIn("-work->config->bounce_speed / work->bounces", source)
        self.assertIn("work->variant >= 5 && work->hold_frames >= 24", source)
        self.assertIn("work->spawned = work->config->strip_count;", source)
        self.assertIn("work->frame += frame_step;", source)
        self.assertIn("work->tick++;", source)
        entries = json.loads(
            (ROOT / "config/sles_03948/overlays/duel_effects_matching_c.json").read_text()
        )["functions"]
        self.assertNotIn("0x801566D4", {entry["address"] for entry in entries})

    def test_effect_four_preserves_lifecycle_and_array_bounds(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        header = (directory / "effect_4.h").read_text()
        source = (directory / "effect_4.c").read_text()
        self.assertIn('#include "../../game/duel_effect_request.h"', header)
        self.assertIn("extern DuelEffect4Config D_8015B704[2];", header)
        for member in ("rings[3][32]", "rotations[16]", "positions[24]",
                       "velocities[24]", "card_rings[2][4]"):
            self.assertIn(f"SVECTOR {member};", header)
        self.assertIn("if (phase >= 2)", source)
        self.assertIn("if (work->frame >= work->config->duration)", source)
        self.assertIn("D_8009B264->field_1D = 1;", source)
        self.assertIn("work->frame += frame_step;", source)
        self.assertIn("if ((*(u32 *)&work->card_color & 0xFFFFFF) == 0x808080)", source)
        self.assertEqual(source.count("func_801514BC(&saved, &scale);"), 2)
        self.assertIn("for (i = 0; i < 24; i++)", source)
        self.assertIn("for (i = 0; i < 16; i++)", source)
        self.assertIn("if (work->scale < 0x7000)", source)
        self.assertIn("work->scale += 0x1000;", source)
        self.assertLess(source.index("packet = &polygon;"), source.index("scale = D_80146248;"))
        self.assertEqual(source.count("func_8014D378("), 1)
        self.assertIn("func_8014D378((u8 *)&work->color)", source)

    def test_card_and_particle_effects_preserve_shared_contracts(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        card_header = (directory / "effect_15.h").read_text()
        card = (directory / "effect_15.c").read_text()
        for include in ("duel_card.h", "display_object.h"):
            self.assertIn(f'#include "../../game/{include}"', card_header)
        self.assertIn("extern u32 D_8015B7A0[21];", card_header)
        self.assertIn("Duel_CollectFieldRowCardObjects(D_8015B7A0, 1);", card)
        self.assertIn("Duel_CollectMatchingFieldCardObjects(D_8015B7A0,", card)
        for field in ("field_30.h.field_30", "field_30.h.field_32", "field_34.h.field_34"):
            self.assertIn(f"((DisplayObject *)D_8015B7A0[i])->{field}", card)
        self.assertEqual(card.count("func_80151218(packet, quad, 32, 1);"), 2)
        self.assertIn("else if (work->timer >= 16)", card)
        header = (directory / "effect_1.h").read_text()
        source = (directory / "effect_1.c").read_text()
        for member in ("rotations[12]", "positions[64]", "velocities[64]"):
            self.assertIn(f"SVECTOR {member};", header)
        self.assertIn("work->rotation_step * (work->tick << 1)", source)
        self.assertIn("work->frame += frame_step;", source)
        self.assertIn("work->tick++;", source)
        self.assertIn("D_8009B264->field_1D = 1;", source)
        self.assertEqual(source.count("color_copy = work->beam_color;"), 2)
        for color in ("base_color", "beam_color", "screen_color"):
            self.assertIn(f"func_8014D378((u8 *)&work->{color})", source)

    def test_line_projection_bindings_keep_resident_sdk_ownership(self) -> None:
        region = ROOT / "config/sles_03948"
        with (region / "functions.csv").open() as handle:
            functions = {row["address"]: row for row in csv.DictReader(handle)}
        bindings = (region / "overlays/duel_effects_linker_symbols.txt").read_text()
        for name, address, size in (("SetGeomOffset", "0x80087838", 24),
                                    ("RotTransPers", "0x80087868", 44),
                                    ("GsSortGLine", "0x800840B8", 264)):
            with self.subTest(symbol=name):
                self.assertIn(f"{name} = {address};", bindings)
                self.assertEqual(functions[address]["status"], "sdk_asm")
                self.assertEqual(int(functions[address]["size"], 0), size)

    def test_effect_eight_preserves_six_variants_and_zero_acceleration_read(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        source = (directory / "effect_8.c").read_text()
        self.assertIn("DuelEffect8Config D_8015A514[6];", (directory / "effect_8.h").read_text())
        for expression in ("if (phase >= 6)", "work->config = &D_8015A514[phase];",
                           "work->velocities[i].vy -= 0;",
                           "work->width = work->config->minimum_width;",
                           "work->height = work->config->maximum_height;",
                           "work->cross_frame > 180", "work->frame += frame_step;"):
            self.assertIn(expression, source)

    def test_effect_twelve_preserves_single_configuration_and_two_scale_updates(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/effect_12.c").read_text()
        self.assertIn("work->config = &D_8015AEE4;", source)
        self.assertIn("if (phase >= 0)", source)
        self.assertNotIn("phase >= 6", source)
        self.assertEqual(source.count("func_801514BC("), 2)
        self.assertIn("scale.vy = work->scale >> 1;", source)
        self.assertIn("for (i = 0; i < 32; i++)", source)
        self.assertIn("D_8009B264->field_1D = 1;", source)
        self.assertIn("D_8009B261 = 1;", source)

    def test_effect_caller_contracts_agree_with_shared_definitions(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        for files, declaration in (
            (("utility_helpers.h", "radial_random_vectors.c"),
             "void func_8014EE0C(u16 width, u16 depth, s16 height,"),
            (("drawing_helpers.h", "primitive_draw.c"),
             "void func_80156E58(u8 *color, u16 width,"),
        ):
            for filename in files:
                with self.subTest(source=filename):
                    self.assertIn(declaration, (directory / filename).read_text())

    def test_effect_eight_and_twelve_keep_real_data_and_matrix_getter(self) -> None:
        region = ROOT / "config/sles_03948"
        symbols = (region / "overlays/duel_effects_symbols.txt").read_text()
        aliases = (region / "overlays/duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_80146014", "0x10"), ("D_80146188", "0x10"),
                           ("D_8015A514", "0xE4"), ("D_8015AEE4", "0x10")):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)
        with (region / "functions.csv").open() as handle:
            getter = next(row for row in csv.DictReader(handle)
                          if row["name"] == "Model_GetLightSourceMatrix")
        self.assertEqual(getter["address"], "0x8005C328")
        self.assertEqual(getter["status"], "matching_c")
        self.assertEqual(int(getter["size"], 0), 12)
        self.assertIn("Model_GetLightSourceMatrix = 0x8005C328;", aliases)

    def test_lifecycle_data_and_number_renderer_keep_real_overlay_storage(self) -> None:
        directory = ROOT / "config/sles_03948/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_801461E8", "0x10"), ("D_801461F8", "0x10"),
                           ("D_8015B0B4", "0x258"), ("D_8015B30C", "0xB4")):
            with self.subTest(symbol=name):
                self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
                self.assertNotIn(name + " =", aliases)
        self.assertNotIn("func_801566D4 =", aliases)

    def test_lifecycle_bindings_follow_verified_french_resident_owners(self) -> None:
        region = ROOT / "config/sles_03948"
        resident = (region / "link_symbols.ld").read_text()
        aliases = (region / "overlays/duel_effects_linker_symbols.txt").read_text()
        for name, address in (("D_8009B261", "0x8009C600"), ("D_8009B264", "0x8009C5FC"),
                              ("PushMatrix", "0x80087158"), ("PopMatrix", "0x800871FC"),
                              ("memset", "0x8008F548")):
            with self.subTest(symbol=name):
                binding = f"{name} = {address};"
                self.assertIn(binding, resident)
                self.assertIn(binding, aliases)
        with (region / "functions.csv").open() as handle:
            functions = {row["name"]: row for row in csv.DictReader(handle)}
        for name, address, size in (("Model_GetFrameStep", "0x8005BF24", 32),
                                    ("Model_SetFrameStepOverride", "0x8005CBF4", 12)):
            with self.subTest(symbol=name):
                self.assertIn(f"{name} = {address};", aliases)
                self.assertEqual(functions[name]["address"], address)
                self.assertEqual(int(functions[name]["size"], 0), size)
                self.assertEqual(functions[name]["status"], "matching_c")

    def test_effect_two_preserves_seventh_record_and_signed_number(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        source = (directory / "effect_2.c").read_text()
        header = (directory / "effect_2.h").read_text()
        self.assertIn("DuelEffect2Config D_8015AF18[7];", header)
        self.assertIn("work->config = &D_8015AF18[6];", source)
        self.assertIn("s16 number", source)
        self.assertIn("func_801566D4(-__builtin_abs(work->number)", source)
        self.assertIn("work->frame += frame_step;", source)
        self.assertIn("work->cross_frame > 180", source)
        self.assertIn("for (i = 0; i < 64; i++)", source)
        for name in ("color", "background_color", "number_color"):
            self.assertIn(f"(u16)func_8014D378((u8 *)&work->{name})", source)

    def test_effect_twentyone_preserves_canonical_slots_and_single_tile_write(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        source = (directory / "effect_21.c").read_text()
        header = (directory / "effect_21.h").read_text()
        self.assertIn('#include "../../game/high_memory_addresses.h"', header)
        self.assertIn('#include "../../game/screen_projection.h"', header)
        for offset in ("0x2800", "0x2880"):
            self.assertIn(f"(D_80010000 + {offset})", source)
        self.assertIn("world = *(MATRIX *)Model_GetLightSourceMatrix();", source)
        self.assertIn("for (i = 0; i < 8; i++)", source)
        self.assertEqual(source.count("tiles[i] = rand() % 3;"), 1)
        self.assertIn("if (++slots[__builtin_abs(phase + 1) % 2]->count > 10)", source)
        self.assertEqual(len(re.findall(r"\bbuffer\b", source)), 1)

    def test_effect_two_and_twentyone_retain_real_data_and_resident_owners(self) -> None:
        region = ROOT / "config/sles_03948"
        directory = region / "overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (("D_80146158", "0x10"), ("D_801461B8", "0x10"),
                           ("D_8015AB68", "0xA0"), ("D_8015AF18", "0x15E")):
            with self.subTest(symbol=name):
                self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
                self.assertNotIn(name + " =", aliases)
        self.assertNotIn("D_8015B044 =", aliases)
        self.assertNotIn("func_801566D4 =", aliases)
        self.assertIn("D_80010000 = 0x80010000;", aliases)
        self.assertIn("D_80010000 = 0x80010000;", (region / "link_symbols.ld").read_text())
        with (region / "functions.csv").open() as handle:
            getter = next(row for row in csv.DictReader(handle)
                          if row["name"] == "Model_GetLightSourceMatrix")
        self.assertEqual(getter["address"], "0x8005C328")
        self.assertEqual(getter["status"], "matching_c")
        self.assertEqual(int(getter["size"], 0), 12)
        self.assertIn("Model_GetLightSourceMatrix = 0x8005C328;", aliases)

    def test_height_ring_keeps_count_promotion_inside_nonzero_guard(self) -> None:
        header = (ROOT / "src/overlays/duel_effects/utility_helpers.h").read_text()
        self.assertEqual(header.count("void func_8014EC8C("), 1)
        source = (ROOT / "src/overlays/duel_effects/height_ring.c").read_text()
        prefix, guarded = source.split("if (count != 0) {", 1)
        self.assertEqual(prefix.count("csin(512) << 1"), 2)
        self.assertNotIn("4096 /", prefix)
        self.assertNotIn("vertices->", prefix)
        self.assertLess(guarded.index("length = count;"), guarded.index("4096 / length"))
        self.assertIn("bottom = top - bottom;", guarded)
        self.assertIn("vertices->vy = bottom;", guarded)
        self.assertIn("while (i < length);", guarded)
        self.assertNotIn("->pad", source)

    def test_color_word_binding_preserves_resident_ownership(self) -> None:
        region = ROOT / "config/sles_03948"
        binding = "D_8009B300 = 0x8009C688;"
        for path in ("link_symbols.ld", "overlays/duel_effects_linker_symbols.txt",
                     "overlays/duel_effects_symbols.txt"):
            with self.subTest(path=path):
                self.assertIn(binding, (region / path).read_text())
        self.assertIn(
            '#include "../../game/sorted_entry.h"',
            (ROOT / "src/overlays/duel_effects/color_helpers.h").read_text(),
        )

    def test_packet_declarations_have_one_shared_owner(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        headers = {name: (directory / name).read_text()
                   for name in ("packet_helpers.h", "drawing_helpers.h", "utility_helpers.h")}
        for symbol in ("D_8015B7F8", "D_8015B800", "func_80152EC4",
                       "func_80152F9C", "func_801530B0", "func_801531C4"):
            with self.subTest(symbol=symbol):
                owner = "packet_helpers.h" if symbol == "func_801531C4" else "drawing_helpers.h"
                for name, text in headers.items():
                    self.assertEqual(len(re.findall(rf"\b{symbol}\b", text)),
                                     1 if name == owner else 0)


if __name__ == "__main__":
    unittest.main()
