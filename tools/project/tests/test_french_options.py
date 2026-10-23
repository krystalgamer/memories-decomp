import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_french_overlay_inventories
from verify_inputs import load_checksum_manifest


class FrenchOptionsTests(unittest.TestCase):
    region = "france"
    region_name = "French"
    module_name = "french_options"
    config_name = "sles_03948"
    executable_name = "SLES_039.48"
    boot_region = "french"
    build_target = "french-match"
    sdk_object = "asm/text_73c4c.o"
    sdk_prefix = "func_"
    unproven_helpers = ("PalOptions_SetTextColorSlots", "PalOptions_DrawScrollingStrips", "PalOptions_DrawWaveSprite",
                        "PalOptions_UpdateWaveTables", "PalOptions_StepStripScroll", "PalOptions_GetLanguage",
                        "func_80169D78", "func_80169F3C")
    load_inventories = staticmethod(load_french_overlay_inventories)
    resident_raw_owners = (
        ("resident_tail_8cb98", 0x8009C398, (
            ("D_8009C02B", 0x8009C44B, 1), ("gText_abColorSlots", 0x801BF98C, 1),
            ("D_800E9D70", 0x8009C838, 16), ("D_8009B118", 0x8009C4B0, 4),
            ("gLibrary_aCardArtRecord", 0x801DC000, 0x2600),
            ("D_801AF000", 0x801AF000, 1),
            ("gFade_State", 0x800EB248, 40), ("D_8009B0F4", 0x8009C460, 4),
            ("D_8009B134", 0x8009C484, 4),
            ("gInput_wPad1Pressed", 0x8009C72C, 2), ("gInput_wPad1Repeat", 0x8009C728, 2),
            ("gSD_bOutputType", 0x8009C784, 1),
        )),
    )
    profiles = {"suffix_signed_step": "gcc_2_8_1_o0_g0_split",
                "suffix_byte": "gcc_2_8_1_o0_g0_split"}
    helpers = ((4, 68, "color_slots"), (0x48, 100, "cursor_layout"),
               (0xAC, 84, "position_easing"), (0x100, 736, "textured_strips"),
               (0x3E0, 708, "grid"),
               (0x6A4, 8, "language_hook"), (0x6AC, 928, "initialize"),
               (0xA4C, 412, "input"), (0xBE8, 332, "wave_tables"),
               (0xD34, 52, "language_request"), (0xD68, 180, "language_image"),
               (0xE1C, 340, "update"), (0xF70, 192, "signed_step"),
               (0x1030, 16, "language_selection"),
               (0x1D78, 452, "suffix_signed_step"), (0x1F3C, 56, "suffix_byte"))
    data_owners = ((0, 4), (0x1040, 0xF), (0x104F, 1), (0x1050, 2), (0x1052, 1),
                   (0x1053, 0x1D),
                   (0x1070, 1), (0x1071, 1), (0x1072, 2), (0x1074, 4),
                   (0x1078, 4), (0x107C, 4), (0x1080, 0xB4), (0x1134, 1),
                   (0x1135, 3), (0x1138, 4), (0x113C, 4), (0x1140, 1),
                   (0x1141, 3), (0x1144, 4), (0x1148, 0xB4), (0x11FC, 1),
                   (0x11FD, 0xB7B), (0x1F74, 0x14), (0x1F88, 2),
                   (0x1F8A, 0x20), (0x1FAA, 2), (0x1FAC, 0xCC),
                   (0x2078, 1), (0x2079, 0xF87))
    dependencies = {
        "color_slots": {"gText_abColorSlots"},
        "cursor_layout": {"gPalOptions_bSoundOutputType", "gPalOptions_pSoundTypeSprite", "gPalOptions_pCursor",
                          "DisplayObject_UpdateResourceVariant"},
        "position_easing": set(),
        "textured_strips": {"gPalOptions_aStripTextures", "gPalOptions_wStripScroll", "PalOptions_EaseStripY", "GsSortPoly"},
        "grid": {"gPalOptions_aWaveOffsets", "gPalOptions_bLanguage", "gPalOptions_aWaveColors",
                 "SetPolyGT4", "GsSortPoly", "GsSortFastSprite"},
        "language_hook": set(),
        "initialize": {"gPalOptions_bMode", "gPalOptions_bSoundOutputType", "gPalOptions_pLanguageSprite", "gPalOptions_pSoundTypeSprite",
                       "gPalOptions_bSelectedRow", "gPalOptions_pLabelSprite", "gPalOptions_pCursor", "gPalOptions_bLanguage",
                       "gPalOptions_dwWavePhase", "gPalOptions_bState", "D_8009C02B", "D_8009B118",
                       "gGraphics_aImageTransferRects", "D_801AF000", "gSD_bOutputType", "StoreImage",
                       "DrawSync", "func_801686A4", "PalOptions_PositionCursor",
                       "DisplayObject_FindFreeGeneralSlot", "DisplayObject_AcquireSlot",
                       "DisplayObject_ConfigureSpriteAtPositionWithResource",
                       "DisplayObject_SetDepthOffset", "DisplayObject_ConfigureSpriteAtPosition",
                       "SD_BGMPlay"},
        "input": {"gPalOptions_bSoundOutputType", "gPalOptions_pLanguageSprite", "gPalOptions_pSoundTypeSprite", "gPalOptions_bSelectedRow",
                  "gPalOptions_pLabelSprite", "gPalOptions_bLanguage", "gPalOptions_bState", "gInput_wPad1Pressed",
                  "gInput_wPad1Repeat", "gSD_bOutputType", "SD_SetOutputType",
                  "SD_SEPlayFull", "DisplayObject_SetResourceVariant"},
        "wave_tables": {"gPalOptions_aWaveOffsets", "gPalOptions_dwWavePhase", "gPalOptions_aWaveColors", "rcos"},
        "language_request": {"D_8009C02B", "func_80043BC8"},
        "language_image": {"func_80043B7C", "gGraphics_aImageTransferRects", "D_8009B118",
                           "D_8009C02B", "StoreImage", "LoadImage", "DrawSync"},
        "update": {"gPalOptions_bState", "gPalOptions_bSelectedRow", "gPalOptions_bLanguage", "PalOptions_HandleInput",
                   "PalOptions_RequestLanguage", "PalOptions_StoreLanguageImage", "PalOptions_PositionCursor", "Fade_StartIn",
                   "Fade_StartOut", "SD_BGMFadeOut", "gFade_State", "D_8009B0F4", "D_8009B134"},
        "signed_step": {"gPalOptions_wStripScroll", "gPalOptions_wStripScrollTarget"},
        "language_selection": {"gPalOptions_bLanguage"},
        "suffix_signed_step": {"D_80169F88", "D_80169FAA"},
        "suffix_byte": {"D_8016A078"},
    }

    def module(self):
        modules = json.loads((ROOT / f"config/{self.config_name}/overlays.json").read_text())["modules"]
        selected = [m for m in modules if m["name"] == self.module_name]
        self.assertEqual(len(selected), 1)
        return selected[0]

    def test_runtime_slices_and_duplicate_policy(self):
        module = self.module()
        self.assertEqual(module["archive"], f"game/{self.region}/DATA/WA_MRG.MRG")
        self.assertEqual(module["archive_sha256"],
                         load_checksum_manifest(ROOT / f"config/{self.config_name}/files.sha256")[module["archive"]])
        self.assertEqual(module["sector_offset"], 10170)
        self.assertEqual(module["duplicate_sector_offsets"], [10211, 10252, 10293, 10334])
        self.assertEqual((module["sector_count"], int(module["load_address"], 0)), (6, 0x80168000))
        self.assertEqual(module["linker_symbols"], f"config/{self.config_name}/overlays/options_linker_symbols.txt")

    def test_only_verified_helpers_select_c(self):
        layout = ROOT / self.module()["layout"]
        entries = json.loads(layout.with_name("options_matching_c.json").read_text())["functions"]
        expected = [
            dict(address=f"0x{0x80168000 + offset:X}",
                 profile=self.profiles.get(stem, "gcc_2_8_1_g0_split"),
                 size=f"0x{size:X}", source=f"src/overlays/pal_options/{stem}.c")
            for offset, size, stem in self.helpers
        ]
        self.assertEqual(entries, expected)
        self.assertEqual([s["source"] for s in c_segments(ROOT, layout)],
                         [s["source"] for s in expected])
        with layout.with_name("options_functions.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), len(self.helpers))
        for row, (start, size, _) in zip(rows, self.helpers):
            self.assertEqual(int(row["address"], 0), 0x80168000 + start)
            self.assertEqual(int(row["size"], 0), size)
            self.assertEqual(row["status"], "matching_c")
        counts = self.load_inventories(ROOT)["options"]
        selected_bytes = sum(size for _, size, _ in self.helpers)
        self.assertEqual((counts["matching_c_function_count"], counts["matching_c_bytes"]),
                         (len(self.helpers), selected_bytes))
        self.assertEqual(sum(int(r["size"], 0) for r in rows if r["status"] == "unmatched_asm"),
                         0)
        intervals = sorted([(offset, size) for offset, size, _ in self.helpers] + list(self.data_owners))
        cursor = 0
        for offset, size in intervals:
            self.assertEqual(offset, cursor)
            cursor += size
        self.assertEqual(cursor, 0x3000)

    def test_headers_and_unclassified_suffix_remain_owned(self):
        layout = ROOT / self.module()["layout"]
        symbols = layout.with_name("options_symbols.txt").read_text()
        self.assertIn("D_80168000 = 0x80168000; // type:u8 size:0x4 defined:true", symbols)
        self.assertIn(f"gPalOptions_aStripTextures = 0x80169040; // type:u8 size:0x{dict(self.data_owners)[0x1040]:X} defined:true", symbols)
        for offset, size in self.data_owners:
            declaration = next(line for line in symbols.splitlines()
                               if f" = 0x{0x80168000 + offset:X};" in line)
            self.assertIn(f"size:0x{size:X} defined:true", declaration)
        bindings = (ROOT / self.module()["linker_symbols"]).read_text()
        for offset, _ in self.data_owners:
            name = next(line for line in symbols.splitlines()
                        if f" = 0x{0x80168000 + offset:X};" in line).split(" = ")[0]
            self.assertNotIn(f"{name} =", bindings)
        self.assertIn(f"data, overlays/{self.module_name}/unclassified_tail", layout.read_text())
        self.assertIn("[0x3000]", layout.read_text())
        for interior in (0xE8, 0xF4, 0x66C, 0xD3C, 0xFCC, 0x1024):
            self.assertNotIn(f"func_{0x80168000 + interior:X} =", symbols)


    def retail_image(self):
        module = self.module()
        archive = ROOT / module["archive"]
        if not archive.is_file():
            self.skipTest(f"Legally obtained {self.region_name} WA archive is unavailable")
        digest = hashlib.sha256()
        with archive.open("rb") as handle:
            for chunk in iter(lambda: handle.read(1024 * 1024), b""):
                digest.update(chunk)
            self.assertEqual(digest.hexdigest(), module["archive_sha256"])
            images = []
            for sector in [module["sector_offset"], *module["duplicate_sector_offsets"]]:
                handle.seek(sector * 2048)
                images.append(handle.read(6 * 2048))
        for image in images:
            self.assertEqual(len(image), 12288)
            self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
            self.assertEqual(image, images[0])
        return images[0]

    def test_retail_helper_boundaries_and_game_callers(self):
        image = self.retail_image()
        self.assertEqual(struct.unpack_from("<I", image)[0], 8)
        self.assertEqual(image[0x6A4:0x6AC], bytes.fromhex("0800e00300000000"))
        for offset, target in ((0x1D4, 0x801680AC), (0x1E8, 0x801680AC),
                               (0x2D0, 0x801680AC), (0x2E0, 0x801680AC),
                               (0x7A4, 0x801686A4), (0xA04, 0x80168048),
                               (0xF3C, 0x80168048), (0xED0, 0x80168D34),
                               (0xF28, 0x80168D68), (0xE6C, 0x80168A4C)):
            word, = struct.unpack_from("<I", image, offset)
            self.assertEqual(word >> 26, 3)
            self.assertEqual(0x80000000 | ((word & 0x3FFFFFF) << 2), target)
        self.assertEqual(struct.unpack_from("<I", image, 0xAC)[0], 0x2882003C)
        self.assertEqual(struct.unpack_from("<I", image, 0xFC)[0], 0x00821023)
        self.assertEqual(struct.unpack_from("<I", image, 0x100)[0], 0x27BDFFC8)

    def test_helpers_without_callers_do_not_claim_direct_reachability(self):
        with (ROOT / f"config/{self.config_name}/overlays/options_functions.csv").open() as handle:
            rows = {r["name"]: r for r in csv.DictReader(handle)}
        for name in self.unproven_helpers:
            self.assertIn("no direct caller found", rows[name]["notes"])

    def test_unoptimized_leaf_views_and_shared_bodies(self):
        image = self.retail_image()
        for offset, word in (
            (0x1D78, 0x27BDFFE8), (0x1D7C, 0xAFBE0010),
            (0x1DB8, 0xAFC20008), (0x1DC0, 0x84429F88),
            (0x1EF4, 0x97C20000), (0x1EFC, 0xA4229F88),
            (0x1F3C, 0x27BDFFF8), (0x1F4C, 0x8063A078),
        ):
            self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
        step = struct.unpack("<113I", image[0x1D78:0x1F3C])
        self.assertEqual([word for word in step
                          if word >> 26 == 0x2B and (word >> 21) & 31 == 30 and word & 0xFFFF == 8],
                         [0xAFC20008])
        self.assertFalse(any(word >> 26 in (0x20, 0x21, 0x23, 0x24, 0x25)
                             and (word >> 21) & 31 == 30 and word & 0xFFFF == 8 for word in step))
    def test_strip_packet_and_table_bounds(self):
        image = self.retail_image()
        for offset, word in (
            (0x118, 0x3C101F80), (0x11C, 0x36100344),
            (0x144, 0x24020009), (0x154, 0xA2020003),
            (0x158, 0x2402002C), (0x15C, 0xA2020007),
            (0x20C, 0x92220002), (0x298, 0x96C60014),
            (0x29C, 0x0C0210AA), (0x2A0, 0x26310003),
            (0x2A4, 0x2A620005), (0x2BC, 0x24130004),
            (0x2CC, 0x2691000C), (0x304, 0x92220002),
            (0x394, 0x0C0210AA), (0x398, 0x2631FFFD), (0x3A8, 0x24130004),
        ):
            self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
        last_row = struct.unpack_from("<I", image, 0x2CC)[0] & 0xFFFF
        last_field = struct.unpack_from("<I", image, 0x20C)[0] & 0xFFFF
        self.assertEqual(last_row + last_field + 1, 15)
        self.assertLessEqual(last_row + last_field + 1, dict(self.data_owners)[0x1040])
        packet_base = ((struct.unpack_from("<I", image, 0x118)[0] & 0xFFFF) << 16
                       | (struct.unpack_from("<I", image, 0x11C)[0] & 0xFFFF))
        packet_bytes = ((struct.unpack_from("<I", image, 0x144)[0] & 0xFFFF) + 1) * 4
        self.assertEqual(packet_bytes, 40)
        self.assertGreaterEqual(packet_base, 0x1F800000)
        self.assertLessEqual(packet_base + packet_bytes, 0x1F800400)

    def test_unproven_callers_remain_unproven_in_actual_images(self):
        image = self.retail_image()
        executable = ROOT / f"game/{self.region}/{self.executable_name}"
        if not executable.is_file():
            self.skipTest(f"Legally obtained {self.region_name} executable is unavailable")
        data = executable.read_bytes()
        expected = load_checksum_manifest(ROOT / f"config/{self.config_name}/files.sha256")
        self.assertEqual(hashlib.sha256(data).hexdigest(), expected[executable.relative_to(ROOT).as_posix()])
        for blob in (image, data):
            words = struct.unpack(f"<{len(blob) // 4}I", blob)
            jumps = {0x80000000 | ((word & 0x3FFFFFF) << 2)
                     for word in words if word >> 26 in (2, 3)}
            for target in (0x80168004, 0x80168100, 0x801683E0, 0x80169030):
                self.assertNotIn(target, jumps)
                self.assertNotIn(target, words)
        words = struct.unpack("<1039I", image[4:0x1040])
        for target in (0x80168BE8, 0x80168F70):
            self.assertFalse(any(word >> 26 in (2, 3) and
                                 (0x80000000 | ((word & 0x3FFFFFF) << 2)) == target
                                 for word in words))

    def test_renderer_resource_chunks_do_not_prove_reachability(self):
        self.retail_image()
        module = self.module()
        with (ROOT / module["archive"]).open("rb") as handle:
            for sector in (module["sector_offset"], *module["duplicate_sector_offsets"]):
                handle.seek((sector - 2) * 2048)
                resource = handle.read(4096)
                self.assertEqual(len(resource), 4096)
                self.assertNotIn(0x80168100, struct.unpack("<1024I", resource))
                words = struct.unpack("<1024I", resource)
                self.assertNotIn(0x801683E0, words)
                self.assertFalse(any(word >> 26 in (2, 3) and
                                     (0x80000000 | ((word & 0x3FFFFFF) << 2)) == 0x801683E0
                                     for word in words))

    def test_grid_packet_tables_and_signed_dispatch(self):
        image = self.retail_image()
        for offset, word in (
            (0x3EC, 0x3C041F80), (0x3F0, 0x34840344), (0x418, 0x0C020BBA),
            (0x420, 0x3C131F80), (0x428, 0x36730320),
            (0x48C, 0x80439140), (0x498, 0x9202006A), (0x4A0, 0x1462006F),
            (0x534, 0x26100006), (0x540, 0x26310001),
            (0x5B8, 0x24420008), (0x5BC, 0x02821021),
            (0x604, 0x00003021), (0x618, 0x0C0210AA),
            (0x630, 0x2A220008), (0x64C, 0x2BC20004), (0x654, 0x26F70008),
            (0x66C, 0x0C02125E), (0x670, 0x00003021),
        ):
            self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
        self.assertEqual(0x320 + 36, 0x344)
        self.assertLessEqual(0x344 + 52, 0x400)
        for offset in (0x1080, 0x1148):
            self.assertEqual(dict(self.data_owners)[offset], 5 * 9 * 4)
            self.assertEqual((4 * 9 + 8) * 4 + 4, dict(self.data_owners)[offset])

    def test_initializer_transfer_extent_and_comparison_bounds(self):
        image = self.retail_image()
        for offset, word in (
            (0x6D0, 0x8E25C4B0), (0x6F0, 0xA0559052),
            (0x6E4, 0x24140030), (0x6EC, 0x24120010),
            (0x71C, 0x24A52000), (0x720, 0x0C01FFDC),
            (0x728, 0x0C01FF19), (0x730, 0x2604FFF8),
            (0x748, 0x0C01FFDC), (0x750, 0x0C01FF19),
            (0x760, 0x90A30060), (0x764, 0x90A22060),
            (0x774, 0x24840001), (0x778, 0x288205A0),
            (0x784, 0x90430000), (0x788, 0x90422000),
        ):
            self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
        width = struct.unpack_from("<I", image, 0x6E4)[0] & 0xFFFF
        height = struct.unpack_from("<I", image, 0x6EC)[0] & 0xFFFF
        second = struct.unpack_from("<I", image, 0x71C)[0] & 0xFFFF
        limit = struct.unpack_from("<I", image, 0x778)[0] & 0xFFFF
        self.assertEqual(second + width * height * 2, 0x2600)
        self.assertLessEqual(limit, width * height * 2)
        self.assertLessEqual(1, dict(self.data_owners)[0x1052])
        self.assertLessEqual(0x1052 + dict(self.data_owners)[0x1052], 0x1070)

    def test_input_pointer_acquisition_and_measured_reloads(self):
        image = self.retail_image()
        for offset, word in (
            (0x84C, 0x0C0100D4), (0x854, 0x00402021), (0x858, 0x0C0100F4),
            (0x860, 0x00408021), (0x8A0, 0x3C028017), (0x8A4, 0xAC509074),
            (0x8B8, 0x0C0100D4), (0x8C0, 0x00402021), (0x8C4, 0x0C0100F4),
            (0x8CC, 0x00408021), (0x910, 0x3C038017), (0x928, 0xAC709138),
            (0xAF8, 0x9482C728), (0xB0C, 0x9482C728),
        ):
            self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))

    def test_resident_load_pointer_and_options_entrypoints(self):
        executable = ROOT / f"game/{self.region}/{self.executable_name}"
        if not executable.is_file():
            self.skipTest(f"Legally obtained {self.region_name} executable is unavailable")
        data = executable.read_bytes()
        expected = load_checksum_manifest(ROOT / f"config/{self.config_name}/files.sha256")[executable.relative_to(ROOT).as_posix()]
        self.assertEqual(hashlib.sha256(data).hexdigest(), expected)
        self.assertEqual(struct.unpack_from("<I", data, 0x18)[0], 0x80010000)
        self.assertEqual(struct.unpack_from("<I", data, 0x800 + 0x1D8)[0], 0x80168000)
        offset = 0x800 + 0x8002D89C - 0x80010000
        words = struct.unpack_from("<22I", data, offset)
        targets = {0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3}
        self.assertTrue({0x801686AC, 0x80168E1C} <= targets)

    def test_resident_dependency_owners_when_built(self):
        linked = ROOT / f"tmp/project-build/{self.executable_name}.elf"
        executable = ROOT / f"game/{self.region}/{self.executable_name}"
        if not linked.is_file() or not executable.is_file():
            self.skipTest(f"Build {self.build_target} with legal inputs before checking resident owners")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools is required for resident ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        retail = executable.read_bytes()
        expected = load_checksum_manifest(ROOT / f"config/{self.config_name}/files.sha256")[executable.relative_to(ROOT).as_posix()]
        self.assertEqual(hashlib.sha256(retail).hexdigest(), expected)
        self.assertEqual((linked.parent / self.executable_name).read_bytes(), retail)
        directory = ROOT / f"tmp/splat/{self.config_name}"
        script = (directory / f"{self.config_name}.ld").read_text()
        with linked.open("rb") as handle:
            final = ELFFile(handle)
            for stem, base, views in self.resident_raw_owners:
                raw = directory / f"build/tmp/splat/{self.config_name}/assets/{stem}.o"
                self.assertIn(raw.relative_to(ROOT).as_posix(), script)
                with raw.open("rb") as source_handle:
                    elf = ELFFile(source_handle)
                    owners = [s for s in elf.get_section_by_name(".symtab").iter_symbols()
                              if s.name.endswith(f"{stem}_bin_start")]
                    self.assertEqual(len(owners), 1)
                    owner = owners[0]
                    self.assertIsInstance(owner["st_shndx"], int)
                    owner_name = owner.name
                    raw_data = elf.get_section(owner["st_shndx"]).data()
                offset = 0x800 + base - 0x80010000
                self.assertEqual(raw_data, retail[offset:offset + len(raw_data)])
                owner, = final.get_section_by_name(".symtab").get_symbol_by_name(owner_name)
                self.assertIsInstance(owner["st_shndx"], int)
                self.assertEqual(owner["st_value"], base)
                section = final.get_section(owner["st_shndx"])
                start = base - section["sh_addr"]
                self.assertEqual(section.data()[start:start + len(raw_data)], raw_data)
                for symbol, address, size in views:
                    alias, = final.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                    self.assertEqual(alias["st_value"], address)
                    self.assertLessEqual(base, address)
                    self.assertLessEqual(address + size, base + len(raw_data))
            for symbol, address, size, source in (
                ("func_80043BC8", 0x80043DC8, 116, f"{self.boot_region}/main_run_boot_sequence"),
                ("func_80043B7C", 0x80043D7C, 76, f"{self.boot_region}/main_run_boot_sequence"),
                ("DisplayObject_UpdateResourceVariant", 0x80040748, 40, "european/display_object_core"),
                ("Fade_StartIn", 0x800156F8, 64, "european/fade_runtime"),
                ("Fade_StartOut", 0x80015820, 64, "european/fade_runtime"),
                ("SD_BGMFadeOut", 0x80040258, 36, "european/sound_frontend"),
                ("File_InitTransferState", 0x800137B4, 92, "file_stream"),
                ("File_SetPositionTable", 0x80013600, 256, "european/file_set_position_table"),
                ("SD_SetOutputType", 0x80047430, 104, "european/sound_output"),
                ("SD_SEPlayFull", 0x80040204, 40, "european/sound_frontend"),
                ("DisplayObject_SetResourceVariant", 0x80040734, 20, "european/display_object_core"),
                ("DisplayObject_FindFreeGeneralSlot", 0x80040350, 64, "european/display_object_core"),
                ("DisplayObject_AcquireSlot", 0x800403D0, 352, "european/display_object_core"),
                ("DisplayObject_ConfigureSpriteAtPosition", 0x80040800, 68, "european/display_object_core"),
                ("DisplayObject_ConfigureSpriteAtPositionWithResource", 0x80042BD8, 68, "european/display_object_helpers"),
                ("DisplayObject_SetDepthOffset", 0x80042C1C, 44, "european/display_object_helpers"),
                ("SD_BGMPlay", 0x8004022C, 44, "european/sound_frontend"),
            ):
                obj = directory / f"build/src/game/{source}.o"
                self.assertIn(obj.relative_to(ROOT).as_posix(), script)
                with obj.open("rb") as source_handle:
                    elf = ELFFile(source_handle)
                    definition, = elf.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                    self.assertIsInstance(definition["st_shndx"], int)
                    self.assertEqual((definition["st_info"]["type"], definition["st_size"]), ("STT_FUNC", size))
                definition, = final.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                self.assertIsInstance(definition["st_shndx"], int)
                self.assertEqual((definition["st_value"], definition["st_size"]), (address, size))
                section = final.get_section(definition["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                start = address - section["sh_addr"]
                offset = 0x800 + address - 0x80010000
                self.assertEqual(section.data()[start:start + size], retail[offset:offset + size])
            obj = directory / f"build/tmp/splat/{self.config_name}/{self.sdk_object}"
            self.assertIn(obj.relative_to(ROOT).as_posix(), script)
            with obj.open("rb") as source_handle:
                source = ELFFile(source_handle)
                for alias_name, symbol, address, size in (
                    ("DrawSync", f"{self.sdk_prefix}8007FC64", 0x8007FC64, 104),
                    ("LoadImage", f"{self.sdk_prefix}8007FF10", 0x8007FF10, 96),
                    ("StoreImage", f"{self.sdk_prefix}8007FF70", 0x8007FF70, 96),
                    ("rcos", f"{self.sdk_prefix}800866F8", 0x800866F8, 160),
                    ("GsSortPoly", f"{self.sdk_prefix}800842A8", 0x800842A8, 452),
                    ("SetPolyGT4", f"{self.sdk_prefix}80082EE8", 0x80082EE8, 20),
                    ("GsSortFastSprite", f"{self.sdk_prefix}80084978", 0x80084978, 380),
                ):
                    definition, = source.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                    self.assertIsInstance(definition["st_shndx"], int)
                    self.assertEqual((definition["st_info"]["type"], definition["st_size"]), ("STT_FUNC", size))
                    self.assertTrue(source.get_section(definition["st_shndx"])["sh_flags"] & 4)
                    alias, = final.get_section_by_name(".symtab").get_symbol_by_name(alias_name)
                    self.assertEqual(alias["st_value"], address)
                    definition, = final.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                    self.assertIsInstance(definition["st_shndx"], int)
                    self.assertEqual((definition["st_value"], definition["st_size"]), (address, size))
                    section = final.get_section(definition["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    start = address - section["sh_addr"]
                    offset = 0x800 + address - 0x80010000
                    self.assertEqual(section.data()[start:start + size], retail[offset:offset + size])

    def test_production_image_and_real_c_owners_when_built(self):
        build = ROOT / f"tmp/overlays/{self.module_name}/build"
        linked = build / f"{self.module_name}.elf"
        if not linked.is_file():
            self.skipTest(f"Build {self.build_target}-overlays before checking production ELF ownership")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools is required for production ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        image = self.retail_image()
        self.assertEqual((build / f"{self.module_name}.bin").read_bytes(), image)
        script = (build.parent / f"{self.module_name}.ld").read_text()
        with linked.open("rb") as handle:
            final = ELFFile(handle)
            for offset, size, stem in self.helpers:
                symbol = f"func_{0x80168000 + offset:X}"
                obj = build / f"src/overlays/pal_options/{stem}.o"
                self.assertIn(obj.relative_to(ROOT).as_posix(), script)
                with obj.open("rb") as source_handle:
                    source = ELFFile(source_handle)
                    definitions = source.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                    self.assertEqual(len(definitions), 1)
                    definition = definitions[0]
                    self.assertIsInstance(definition["st_shndx"], int)
                    self.assertEqual((definition["st_info"]["type"], definition["st_size"]), ("STT_FUNC", size))
                    self.assertEqual(definition["st_info"]["bind"], "STB_GLOBAL")
                    section = source.get_section(definition["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    self.assertEqual(len(section.data()), size)
                    unresolved = {s.name for s in source.get_section_by_name(".symtab").iter_symbols()
                                  if s.name and s["st_shndx"] == "SHN_UNDEF"}
                    self.assertEqual(unresolved, self.dependencies[stem])
                    if not unresolved:
                        self.assertEqual(section.data(), image[offset:offset + size])
                definitions = final.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                self.assertEqual(len(definitions), 1)
                definition = definitions[0]
                self.assertIsInstance(definition["st_shndx"], int)
                self.assertEqual(definition["st_info"]["type"], "STT_FUNC")
                self.assertEqual((definition["st_value"], definition["st_size"]), (0x80168000 + offset, size))
                section = final.get_section(definition["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                start = definition["st_value"] - section["sh_addr"]
                self.assertEqual(section.data()[start:start + size], image[offset:offset + size])
            for offset, size in self.data_owners:
                symbol = f"D_{0x80168000 + offset:X}"
                stem = ("module_header" if offset == 0 else
                        "unclassified_tail" if offset < 0x1F74 else "unclassified_suffix")
                obj = build / f"tmp/overlays/{self.module_name}/asm/data/overlays/{self.module_name}/{stem}.data.o"
                self.assertIn(obj.relative_to(ROOT).as_posix(), script)
                with obj.open("rb") as source_handle:
                    source = ELFFile(source_handle)
                    definition, = source.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                    self.assertIsInstance(definition["st_shndx"], int)
                    self.assertEqual((definition["st_info"]["type"], definition["st_size"]), ("STT_OBJECT", size))
                    section = source.get_section(definition["st_shndx"])
                    self.assertFalse(section["sh_flags"] & 4)
                    start = definition["st_value"]
                    self.assertEqual(section.data()[start:start + size], image[offset:offset + size])
                definitions = final.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                self.assertEqual(len(definitions), 1)
                definition = definitions[0]
                self.assertIsInstance(definition["st_shndx"], int)
                self.assertEqual(definition["st_value"], 0x80168000 + offset)
                self.assertEqual((definition["st_info"]["type"], definition["st_size"]), ("STT_OBJECT", size))
                section = final.get_section(definition["st_shndx"])
                self.assertFalse(section["sh_flags"] & 4)
                start = definition["st_value"] - section["sh_addr"]
                self.assertEqual(section.data()[start:start + size], image[offset:offset + size])


if __name__ == "__main__":
    unittest.main()
