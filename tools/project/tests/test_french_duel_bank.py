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
        self.assertEqual(len(matched), 64)
        self.assertEqual(sum(int(row["size"], 0) for row in matched), 28044)
        self.assertEqual(sum(row["status"] == "unmatched_asm" for row in rows), 21)
        for address in ("0x8014FABC",):
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
            ("color_test", 0x8014D378, 0x8014D3E8, 2),
            ("cross_lines", 0x8014E35C, 0x8014E3EC, 1),
            ("ring_vertices", 0x8014EA7C, 0x8014EC8C, 2),
            ("height_ring", 0x8014EC8C, 0x8014EE0C, 1),
            ("radial_random_vectors", 0x8014EE0C, 0x8014F010, 2),
            ("rect_vertices", 0x8014F490, 0x8014F524, 1),
            ("quad_helpers", 0x8014FE00, 0x8014FF40, 2),
            ("projected_wrappers", 0x80151218, 0x801513F4, 2),
            ("matrix_setup", 0x801513F4, 0x801514BC, 1),
            ("matrix_helpers", 0x801514BC, 0x80151558, 2),
            ("packet_helpers", 0x80152EC4, 0x80153200, 4),
            ("color_transition", 0x80153F28, 0x80154084, 3),
            ("screen_draw", 0x801556F4, 0x801558F4, 3),
            ("layered_drawing", 0x801558F4, 0x80155F94, 3),
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
                functions = [row for row in manifest["functions"] if row["source"] == source]
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
                            ("screen_draw", 3), ("layered_drawing", 3),
                            ("drawing_tail", 3)):
            source = f"src/overlays/duel_effects/{name}.c"
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
            ("effect_18", 0x80154084, 1540, False),
            ("effect_17", 0x80148BA4, 5100, False),
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
            (0x80146034, 16), (0x8015A60C, 32), (0x8015B7A0, 84),
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
                              ("Model_GetLightSourceMatrix", 0x8005C328),
                              ("Duel_CollectMatchingFieldCardObjects", 0x8002CB88)):
            self.assertIn(f"{name} = 0x{address:X};", aliases)
            self.assertEqual(int(resident[name]["address"], 0), address)
            self.assertEqual(resident[name]["status"], "matching_c")

    def test_line_projection_bindings_keep_resident_sdk_ownership(self) -> None:
        region = ROOT / "config/sles_03948"
        with (region / "functions.csv").open() as handle:
            functions = {row["address"]: row for row in csv.DictReader(handle)}
        bindings = (region / "overlays/duel_effects_linker_symbols.txt").read_text()
        for name, address, size in (("SetGeomOffset", "0x80087838", 24),
                                    ("RotTransPers", "0x80087868", 44),
                                    ("GsSortGLine", "0x800840B8", 264),
                                    ("ratan2", "0x80089928", 372)):
            with self.subTest(symbol=name):
                self.assertIn(f"{name} = {address};", bindings)
                self.assertEqual(functions[address]["status"], "sdk_asm")
                self.assertEqual(int(functions[address]["size"], 0), size)

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

    def test_effect_seventeen_keeps_canonical_contracts_and_complete_pools(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        header = (directory / "effect_17.h").read_text()
        self.assertIn('#include "../../game/duel_card.h"', header)
        self.assertIn('#include "../../game/display_object.h"', header)
        self.assertIn('#include "../../game/screen_projection.h"', header)
        for declaration in (
            "SVECTOR card_particles[20][16];", "SVECTOR particles[64];",
            "SVECTOR paths[48][8];", "CVECTOR path_colors[48];",
            "extern DuelEffect17Config D_8015A60C[2];",
            "extern u32 D_8015B7A0[21];",
        ):
            self.assertIn(declaration, header)
        dispatch = (directory / "dispatch.h").read_text()
        self.assertIn('#include "effect_17.h"', dispatch)
        self.assertNotIn("void func_80148BA4(", dispatch)

    def test_effect_seventeen_preserves_empty_collection_guards(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/effect_17.c").read_text()
        self.assertIn("Duel_CollectMatchingFieldCardObjects(D_8015B7A0, -1);", source)
        self.assertIn("Duel_CollectMatchingFieldCardObjects(D_8015B7A0, 0);", source)
        self.assertIn("for (i = 0; D_8015B7A0[i] != 0; i++)", source)
        self.assertIn("if (work->card_count && work->stage < 2) {\n"
                      "            work->active_particles++;", source)
        self.assertIn("if (work->card_count == 0 && work->stage == 1)", source)
        self.assertIn("work->card_particles[work->completed - 1]", source)
        self.assertIn("if (work->config->mode && work->card_count)", source)
        self.assertIn("if (work->active_cards > work->card_count)", source)
        self.assertIn("if (work->active_particles > 64)", source)
        self.assertIn("if (work->active_paths > 48)", source)
        self.assertIn("work->path_ages[i] = 7;", source)

    def test_brightness_helper_accepts_halfwords_but_packs_only_low_bytes(self) -> None:
        directory = ROOT / "src/overlays/duel_effects"
        signature = "void func_8015405C(u16 high, u16 middle, u16 low)"
        self.assertIn(signature + ";", (directory / "color_helpers.h").read_text())
        source = (directory / "color_transition.c").read_text()
        self.assertIn(signature, source)
        self.assertIn("((u8)high << 16) | ((u8)middle << 8) | (u8)low", source)


if __name__ == "__main__":
    unittest.main()
