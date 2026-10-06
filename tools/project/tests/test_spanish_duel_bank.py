import csv
import hashlib
import json
from pathlib import Path
import re
import struct
import sys
import unittest


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import progress


class SpanishDuelBankTests(unittest.TestCase):
    def test_curve_selects_shared_c_without_an_absolute_function_alias(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        layout = (directory / "duel_effects.yaml").read_text()
        self.assertIn("[0x9ABC, c, overlays/duel_effects/bolt_vertices]", layout)
        self.assertNotIn("curve_preserved", layout)
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        self.assertIn("csin = 0x80086B38;", aliases)
        self.assertIn("rand = 0x8008F708;", aliases)
        self.assertNotRegex(aliases, r"func_8014FABC\s*=")
        with (ROOT / "notes/overlays/spanish-duel-curve-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 1)
        attempt = attempts[0]
        source = ROOT / "src/overlays/duel_effects/bolt_vertices.c"
        header = ROOT / "src/overlays/duel_effects/utility_helpers.h"
        self.assertEqual(attempt["fingerprint"], hashlib.sha256(source.read_bytes() + header.read_bytes()).hexdigest())
        self.assertEqual((attempt["result"], attempt["instruction_bytes"], attempt["different_words"]),
                         ("matched", "836", "0"))
        self.assertEqual(attempt["profile"], "gcc_2_8_1_g0_split")

    def test_legal_curve_frames_calls_and_all_seven_full_copies(self) -> None:
        path = ROOT / "game/spain/DATA/WA_MRG.MRG"
        if not path.exists():
            self.skipTest("legal Spanish WA input required")
        manifest = json.loads((ROOT / "config/sles_03951/overlays.json").read_text())
        module = next(m for m in manifest["modules"] if m["name"] == "spanish_duel_effects")
        with path.open("rb") as archive:
            for sector in [module["sector_offset"], *module["duplicate_sector_offsets"]]:
                archive.seek(sector * 2048)
                data = archive.read(module["sector_count"] * 2048)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<I", data, 0x9ABC)[0], 0x27BDFFB0)
                self.assertEqual(struct.unpack_from("<II", data, 0x9DF8), (0x03E00008, 0x27BD0050))
                words = struct.unpack("<209I", data[0x9ABC:0x9E00])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(calls.count(0x8008F708), 8)
                self.assertEqual(calls.count(0x80086B38), 2)
                self.assertEqual(len(calls), 10)

    def test_curve_callers_keep_eight_point_paths(self) -> None:
        for stem in ("effect_13", "effect_17"):
            directory = ROOT / "src/overlays/duel_effects"
            self.assertIn("SVECTOR paths[48][8];", (directory / f"{stem}.h").read_text())
            body = (directory / f"{stem}.c").read_text()
            self.assertRegex(body, r"func_8014FABC\([^;]*,\s*8,\s*work->paths\[i\]\)")

    def test_all_seven_terrain_copies_are_registered(self) -> None:
        manifest = json.loads((ROOT / "config/sles_03951/overlays.json").read_text())
        bank = next(m for m in manifest["modules"] if m["name"] == "spanish_duel_effects")
        self.assertEqual(
            [bank["sector_offset"], *bank["duplicate_sector_offsets"]],
            [7193 + terrain * 240 for terrain in range(7)],
        )
        self.assertEqual(bank["sector_count"], 44)
        self.assertEqual(bank["load_address"], "0x80146000")
        self.assertEqual(bank["archive"], "game/spain/DATA/WA_MRG.MRG")
        self.assertEqual(
            bank["archive_sha256"],
            "00d79fecc9aadd11914dc9d4b79d24619f4ced86049670a3fc23f417ef6ecd8f",
        )
        self.assertEqual(
            bank["sha256"],
            "a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3",
        )

    def test_full_inventory_preserves_all_function_boundaries(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        with (directory / "duel_effects_functions.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 85)
        cursor = 0x80146258
        for row in rows:
            self.assertEqual(int(row["address"], 0), cursor)
            cursor += int(row["size"], 0)
        self.assertEqual(cursor, 0x8015A1E4)
        matched = [r for r in rows if r["status"] == "matching_c"]
        self.assertEqual(len(matched), 85)
        self.assertEqual(sum(int(r["size"], 0) for r in matched), 81804)
        self.assertEqual(sum(r["status"] == "unmatched_asm" for r in rows), 0)
        self.assertFalse(any(r["status"] in ("handwritten_asm", "sdk_asm") for r in rows))
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        self.assertEqual(
            {(r["address"], r["size"]) for r in matched},
            {(r["address"], r["size"]) for r in manifest["functions"]},
        )
        french = ROOT / "config/sles_03948/overlays/duel_effects_matching_c.json"
        accepted = {
            row["address"]: row for row in json.loads(french.read_text())["functions"]
        }
        shared_sources = {
            f"src/overlays/duel_effects/{name}.c"
            for name in (
                "utility_helpers", "texture_words", "vector_init",
                "color_test", "quad_helpers", "matrix_helpers",
                "projected_wrappers", "matrix_setup", "rect_vertices",
                "ring_vertices", "radial_random_vectors", "bolt_vertices", "effect_23", "effect_24",
            )
        }
        for row in manifest["functions"]:
            if row["source"] in shared_sources:
                self.assertEqual(row, accepted[row["address"]])

    def test_effect_24_keeps_real_storage_and_canonical_slot_copy(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        symbols = (directory / "duel_effects_symbols.txt").read_text()
        aliases = (directory / "duel_effects_linker_symbols.txt").read_text()
        for name, size in (
            ("D_80146148", "0x10"), ("D_8015B748", "0x54"),
            ("D_8015B7A0", "0x54"), ("D_8015B7F4", "0x4"),
            ("D_8015B7F8", "0x8"), ("D_8015B800", "0x2"),
        ):
            self.assertRegex(symbols, rf"{name} = 0x{name[2:]}; //[^\n]*size:{size}\b")
            self.assertNotIn(name + " =", aliases)
        self.assertIn("DisplayObject_CopyWorkSlots = 0x8002CD24;", aliases)
        with (ROOT / "config/sles_03951/functions.csv").open() as handle:
            resident = {row["name"]: row for row in csv.DictReader(handle)}
        slot_copy = resident["DisplayObject_CopyWorkSlots"]
        self.assertEqual(
            (slot_copy["address"], slot_copy["size"], slot_copy["status"]),
            ("0x8002CD24", "0x30", "matching_c"),
        )
        header = (ROOT / "src/overlays/duel_effects/effect_24.h").read_text()
        self.assertIn('#include "../../game/display_object_work_slots.h"', header)
        self.assertIn("extern u32 D_8015B7A0[21];", header)

    def test_reporting_keeps_complete_bank_and_partial_runtime_scope_separate(self) -> None:
        modules = progress.load_spanish_overlay_inventories(ROOT)
        self.assertEqual(len(modules), 3596)
        self.assertEqual(modules["duel_effects"]["function_count"], 85)
        self.assertEqual(modules["duel_effects"]["matching_c_function_count"], 85)
        self.assertEqual(sum(m["function_count"] for m in modules.values()), 2631)
        self.assertEqual(sum(m["matching_c_function_count"] for m in modules.values()), 1799)
        self.assertEqual(sum(m["matching_c_bytes"] for m in modules.values()), 2398448)

    def test_new_groups_preserve_exact_extents_and_definition_order(self) -> None:
        directory = ROOT / "config/sles_03951/overlays"
        manifest = json.loads((directory / "duel_effects_matching_c.json").read_text())
        expected = {
            "bolt_vertices": [(0x8014FABC, 0x344)],
            "effect_22": [(0x8014A8E4, 0x2018)],
            "effect_16": [(0x80151558, 0xAF0)],
            "number_renderer": [(0x801566D4, 0x400)],
            "effect_4": [(0x80159AAC, 0x738)],
            "effect_5": [(0x80157E10, 0x9C8)],
            "effect_10": [(0x8014C8FC, 0xA7C)],
            "effect_21": [(0x8014E3EC, 0x690)],
            "effect_2": [(0x80153200, 0x8DC)],
            "effect_11": [(0x80146760, 0x13B8)],
            "effect_17": [(0x80148BA4, 0x13EC)],
            "effect_7": [(0x801587D8, 0xBD0)],
            "effect_13": [(0x801503F8, 0xA08)],
            "effect_23": [(0x80152048, 0xE7C)],
            "effect_24": [(0x8014D3E8, 0xF74)],
            "effect_0": [(0x80154688, 0x4A8)],
            "effect_1": [(0x80157794, 0x67C)],
            "effect_6": [(0x80154B30, 0xBC4)],
            "effect_8": [(0x80147B18, 0x690)],
            "effect_12": [(0x80150E00, 0x418)],
            "effect_9": [(0x801593A8, 0x704)],
            "effect_15": [(0x8014FF40, 0x4B8)],
            "effect_18": [(0x80154084, 0x604)],
            "effect_19": [(0x80153ADC, 0x44C)],
            "polygon_vertices": [(0x8014EC8C, 0x180)],
            "dispatch": [(0x80146258, 0x508)],
            "rect_vertices": [(0x8014F490, 0x94)],
            "cross_lines": [(0x8014E35C, 0x90)],
            "ring_vertices": [(0x8014EA7C, 0xA0), (0x8014EB1C, 0x170)],
            "radial_random_vectors": [(0x8014EE0C, 0x120), (0x8014EF2C, 0xE4)],
            "gradient_lines": [(0x801570B0, 0x2F8)],
            "display_quads": [
                (0x801573A8, 0xEC), (0x80157494, 0x138), (0x801575CC, 0x1C8),
            ],
            "gradient_strip": [(0x80156448, 0x28C)],
            "projected_wrappers": [(0x80151218, 0x104), (0x8015131C, 0xD8)],
            "matrix_setup": [(0x801513F4, 0xC8)],
            "drawing_tail": [
                (0x80155F94, 0xD0), (0x80156064, 0x108), (0x8015616C, 0x2DC),
            ],
            "contour_quads": [(0x801558F4, 0x2CC)],
            "layered_drawing": [(0x80155BC0, 0x1D0), (0x80155D90, 0x204)],
            "color_transition": [
                (0x80153F28, 0x70), (0x80153F98, 0xC4), (0x8015405C, 0x28),
            ],
            "screen_draw": [
                (0x801556F4, 0xA4), (0x80155798, 0xCC), (0x80155864, 0x90),
            ],
            "packet_helpers": [
                (0x80152EC4, 0xD8), (0x80152F9C, 0x114),
                (0x801530B0, 0x114), (0x801531C4, 0x3C),
            ],
            "color_test": [(0x8014D378, 0x34), (0x8014D3AC, 0x3C)],
            "quad_helpers": [(0x8014FE00, 0xD4), (0x8014FED4, 0x6C)],
            "matrix_helpers": [(0x801514BC, 0x3C), (0x801514F8, 0x60)],
            "number_helpers": [(0x80156AD4, 0x6C), (0x80156B40, 0x100)],
            "primitive_draw": [(0x80156E58, 0x14C), (0x80156FA4, 0x10C)],
        }
        # The PAL arms of these shared sources are registered through wrappers.
        pal_wrapped = {
            "cross_lines", "effect_5", "effect_6", "effect_7", "effect_18", "screen_draw",
        }
        for unit, extents in expected.items():
            source = f"src/overlays/duel_effects/{unit}.c"
            registered = (f"src/overlays/european/duel_effects/{unit}.c"
                          if unit in pal_wrapped else source)
            functions = [row for row in manifest["functions"] if row["source"] == registered]
            self.assertEqual(
                [(int(row["address"], 0), int(row["size"], 0)) for row in functions],
                extents,
            )
            self.assertEqual({row["profile"] for row in functions}, {"gcc_2_8_1_g0_split"})
            definitions = re.findall(
                r"^(?:s32|u16|void) (func_[0-9A-F]+)\(", (ROOT / source).read_text(), re.M
            )
            self.assertEqual(definitions, [f"func_{address:X}" for address, _ in extents])
        with (directory / "duel_effects_functions.csv").open() as handle:
            rows = {row["address"]: row for row in csv.DictReader(handle)}
        self.assertEqual(rows["0x8014FABC"]["status"], "matching_c")

    def test_effect_ten_keeps_separate_texture_storage_and_canonical_binding(self) -> None:
        directory = ROOT / "config/sles_03951"
        symbols = (directory / "overlays/duel_effects_symbols.txt").read_text()
        aliases = (directory / "overlays/duel_effects_linker_symbols.txt").read_text()
        for name, size in (
            ("D_80146138", "0x10"), ("D_8015A8C8", "0x24C"),
            ("D_8015AB14", "0x54"),
        ):
            self.assertIn(f"{name} = 0x{name[2:]}; // size:{size}", symbols)
            self.assertNotIn(name + " =", aliases)
        with (directory / "functions.csv").open() as handle:
            resident = {row["name"]: row for row in csv.DictReader(handle)}
        getter = resident["Model_GetLightSourceMatrix"]
        self.assertEqual((getter["address"], getter["size"]), ("0x8005C328", "0xC"))
        self.assertEqual(getter["status"], "matching_c")
        self.assertIn("Model_GetLightSourceMatrix = 0x8005C328;", aliases)

    def test_effect_ten_preserves_phase_and_bounded_column_lifecycle(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/effect_10.c").read_text()
        header = (ROOT / "src/overlays/duel_effects/effect_10.h").read_text()
        for expression in (
            "if (phase >= 6)", "work->config = &D_8015AB14[phase];",
            "if (work->spawned > 64)", "work->spawned = 64;",
            "work->stage == 0 && phase < -1",
            "*(u32 *)&work->color_ready == 0x10001",
            "D_8015B748.pairs[12][0] = getTPage(D_8015A8C8[12].pmode,",
            "work->frame += frame_step;", "work->tick++;",
        ):
            self.assertIn(expression, source)
        self.assertIn("extern GsIMAGE D_8015A8C8[21];", header)
        self.assertIn("extern DuelEffect10Config D_8015AB14[6];", header)
        self.assertEqual(source.count("D_8009B261 = 1;"), 2)

    def test_drawing_bindings_reuse_resident_owners_without_data_aliases(self) -> None:
        directory = ROOT / "config/sles_03951"
        aliases = (directory / "overlays/duel_effects_linker_symbols.txt").read_text()
        resident = (directory / "symbols.txt").read_text() + (
            directory / "link_symbols.ld"
        ).read_text()
        for name, address in (
            ("D_8009B300", "0x8009C688"),
            ("ScaleMatrix", "0x800875F8"), ("GsSetLsMatrix", "0x80085558"),
            ("MulMatrix2", "0x80087408"), ("RotTrans", "0x800878F8"),
            ("RotMatrix", "0x80087CB8"),
            ("RotAverage3", "0x800879D8"), ("RotAverage4", "0x80087A38"),
            ("GsSortPoly", "0x800842A8"), ("func_8005B260", "0x8004D5B8"),
        ):
            binding = f"{name} = {address};"
            self.assertIn(binding, aliases)
            self.assertIn(binding, resident)
        for name in ("D_8015B7F4", "D_8015B7F8", "D_8015B800"):
            self.assertNotIn(f"{name} =", aliases)

    def test_packet_helpers_preserve_distinct_priority_and_mode_paths(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/packet_helpers.c").read_text()
        self.assertIn("void func_80152EC4(void *primitive, u16 flags)", source)
        bodies = dict(re.findall(
            r"void (func_[0-9A-F]+)\([^\n]+\)\n\{(.*?)\n\}", source, re.S
        ))
        for name in ("func_80152F9C", "func_801530B0"):
            self.assertIn("if (mode == 1)", bodies[name])
            self.assertIn("setSemiTrans(packet, 0);", bodies[name])
            self.assertIn(
                "func_8005B260((u32 *)packet, D_8015B7F4, D_8015B800, 1);",
                bodies[name],
            )
        self.assertIn(
            "GsSortPoly(packet, D_8015B7F4, D_8015B800);",
            bodies["func_80152F9C"],
        )
        self.assertIn(
            "GsSortPoly(packet, D_8015B7F4, (u16)(D_8015B800 + 1));",
            bodies["func_801530B0"],
        )
        self.assertIn(
            "func_8005B260((u32 *)packet, D_8015B7F4, (u16)(D_8015B800 + 1), flags);",
            bodies["func_80152EC4"],
        )

    def test_regional_packet_declarations_share_one_compatible_owner(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        declarations = (
            (directory / "drawing_helpers.h").read_text()
            + (directory / "packet_helpers.h").read_text()
        )
        self.assertEqual(declarations.count("extern SVECTOR D_8015B7F8;"), 1)
        self.assertNotIn("D_8015B7F8[", declarations)
        self.assertEqual(declarations.count("void func_80152EC4("), 1)
        self.assertIn("void func_80152EC4(void *primitive, u16 flags);", declarations)
        self.assertIn(
            "void func_80152EC4(void *primitive,",
            (directory / "packet_helpers.c").read_text(),
        )
        self.assertFalse((directory / "sorting_helpers.c").exists())

    def test_gradient_strip_keeps_flag_only_rejection_and_clamped_bias(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/gradient_strip.c").read_text()
        self.assertEqual(source.count("if (flag >= 0)"), 2)
        self.assertNotIn("depth >= 0", source)
        self.assertEqual(source.count("depth -= bias;"), 2)
        self.assertEqual(source.count("if (depth < 0)"), 2)
        self.assertEqual(source.count("func_80152EC4(packet, 1);"), 2)
        self.assertEqual(source.count("(u16)(depth >> 2), 1);"), 2)

    def test_gradient_line_sdk_bindings_preserve_resident_ownership(self) -> None:
        directory = ROOT / "config/sles_03951"
        aliases = (directory / "overlays/duel_effects_linker_symbols.txt").read_text()
        with (directory / "functions.csv").open() as handle:
            resident = {int(row["address"], 0): row for row in csv.DictReader(handle)}
        for name, address, size in (
            ("GsSortGLine", 0x800840B8, 0x108),
            ("RotTransPers", 0x80087868, 0x2C),
            ("GsSortLine", 0x80083F38, 0xD8),
            ("ccos", 0x800868A8, 0xC4),
            ("csin", 0x80086B38, 0x138),
            ("SetGeomOffset", 0x80087838, 0x18),
        ):
            self.assertIn(f"{name} = 0x{address:X};", aliases)
            self.assertEqual(resident[address]["status"], "sdk_asm")
            self.assertEqual(int(resident[address]["size"], 0), size)
        self.assertNotIn("D_8015B7F4 =", aliases)

    def test_generators_preserve_signed_arithmetic_and_padding(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        circle = (directory / "ring_vertices.c").read_text()
        random = (directory / "radial_random_vectors.c").read_text()
        lines = (directory / "cross_lines.c").read_text()
        self.assertIn("i < 32", circle)
        self.assertIn("radius * ccos(i * 128) / 4096", circle)
        self.assertIn("radius * csin(i * 128) / 4096", circle)
        self.assertEqual(random.count("(rand() - rand()) % 4096"), 3)
        self.assertNotIn(".pad", circle + random)
        self.assertEqual(lines.count("GsSortLine(&line, D_8015B7F4, 0);"), 2)


if __name__ == "__main__":
    unittest.main()
