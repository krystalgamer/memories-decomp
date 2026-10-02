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
    starts = (4, 0x48, 0xAC, 0x100, 0x3E0, 0x6A4, 0x6AC, 0xA4C,
              0xBE8, 0xD34, 0xD68, 0xE1C, 0xF70, 0x1030, 0x1040)
    helpers = ((4, 68, "color_slots"), (0x48, 100, "cursor_layout"),
               (0xAC, 84, "position_easing"), (0x6A4, 8, "language_hook"),
               (0xD34, 52, "language_request"), (0x1030, 16, "language_selection"))
    data_owners = ((0, 4), (0x1040, 0x30), (0x1070, 1), (0x1071, 7),
                   (0x1078, 4), (0x107C, 0xC0), (0x113C, 4), (0x1140, 1),
                   (0x1141, 0x1EBF))
    dependencies = {
        "color_slots": {"gText_abColorSlots"},
        "cursor_layout": {"D_80169070", "D_80169078", "D_8016913C",
                          "DisplayObject_UpdateResourceVariant"},
        "position_easing": set(),
        "language_hook": set(),
        "language_request": {"D_8009C02B", "func_80043BC8"},
        "language_selection": {"D_80169140"},
    }

    def module(self):
        modules = json.loads((ROOT / "config/sles_03948/overlays.json").read_text())["modules"]
        selected = [m for m in modules if m["name"] == "french_options"]
        self.assertEqual(len(selected), 1)
        return selected[0]

    def test_runtime_slices_and_duplicate_policy(self):
        module = self.module()
        self.assertEqual(module["archive"], "game/france/DATA/WA_MRG.MRG")
        self.assertEqual(module["archive_sha256"],
                         load_checksum_manifest(ROOT / "config/sles_03948/files.sha256")[module["archive"]])
        self.assertEqual(module["sector_offset"], 10170)
        self.assertEqual(module["duplicate_sector_offsets"], [10211, 10252, 10293, 10334])
        self.assertEqual((module["sector_count"], int(module["load_address"], 0)), (6, 0x80168000))
        self.assertEqual(module["linker_symbols"], "config/sles_03948/overlays/options_linker_symbols.txt")

    def test_only_verified_helpers_select_c(self):
        layout = ROOT / self.module()["layout"]
        entries = json.loads(layout.with_name("options_matching_c.json").read_text())["functions"]
        expected = [
            dict(address=f"0x{0x80168000 + offset:X}", profile="gcc_2_8_1_g0_split",
                 size=f"0x{size:X}", source=f"src/overlays/pal_options/{stem}.c")
            for offset, size, stem in self.helpers
        ]
        self.assertEqual(entries, expected)
        self.assertEqual([s["source"] for s in c_segments(ROOT, layout)],
                         [s["source"] for s in expected])
        with layout.with_name("options_functions.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 14)
        for row, start, end in zip(rows, self.starts, self.starts[1:]):
            self.assertEqual(int(row["address"], 0), 0x80168000 + start)
            self.assertEqual(int(row["size"], 0), end - start)
            self.assertEqual(row["status"], "matching_c" if start in {h[0] for h in self.helpers} else "unmatched_asm")
            if row["status"] == "unmatched_asm":
                self.assertIn(f"asm, overlays/french_options/{row['name']}", layout.read_text())
        counts = load_french_overlay_inventories(ROOT)["options"]
        self.assertEqual((counts["matching_c_function_count"], counts["matching_c_bytes"]), (6, 328))
        self.assertEqual(sum(int(r["size"], 0) for r in rows if r["status"] == "unmatched_asm"), 3828)

    def test_headers_and_unclassified_suffix_remain_owned(self):
        layout = ROOT / self.module()["layout"]
        symbols = layout.with_name("options_symbols.txt").read_text()
        self.assertIn("D_80168000 = 0x80168000; // type:u8 size:0x4 defined:true", symbols)
        self.assertIn("D_80169040 = 0x80169040; // type:u8 size:0x30 defined:true", symbols)
        for offset, size in self.data_owners:
            declaration = next(line for line in symbols.splitlines()
                               if line.startswith(f"D_{0x80168000 + offset:X} ="))
            self.assertIn(f"size:0x{size:X} defined:true", declaration)
        bindings = (ROOT / self.module()["linker_symbols"]).read_text()
        for symbol in ("D_80169070", "D_80169078", "D_8016913C", "D_80169140"):
            self.assertNotIn(symbol, bindings)
        self.assertIn("data, overlays/french_options/unclassified_tail", layout.read_text())
        self.assertIn("[0x3000]", layout.read_text())
        for interior in (0xE8, 0xF4, 0x66C, 0xD3C, 0xFCC, 0x1024):
            self.assertNotIn(f"func_{0x80168000 + interior:X} =", symbols)

    def test_sources_and_attempts_preserve_exact_contracts(self):
        directory = ROOT / "src/overlays/pal_options"
        for _, _, stem in self.helpers:
            source = (directory / (stem + ".c")).read_text()
            self.assertIn('#include "../../types.h"', source)
            self.assertIn('#include "helpers.h"', source)
            self.assertNotRegex(source, r"\b(?:asm|__asm__|extern)\b")
        source = (directory / "position_easing.c").read_text()
        self.assertIn("s32 result;", source)
        self.assertIn("distance *= distance;", source)
        self.assertIn("return result;", source)
        with (ROOT / "notes/overlays/french-options-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        easing = [r for r in rows if r["function"] == "func_801680AC"]
        self.assertEqual([r["attempt"] for r in easing], ["01", "02", "03", "04", "05"])
        self.assertEqual([r["result"] for r in easing], ["nonmatching"] * 4 + ["matched"])
        self.assertEqual((easing[-1]["instruction_bytes"], easing[-1]["different_words"]), ("84", "0"))
        self.assertEqual(len([r for r in rows if r["result"] == "matched"]), 6)
        header = (directory / "helpers.h").read_text()
        self.assertIn("extern DisplayObjectConfig *G32 D_80169078;", header)
        self.assertIn("extern DisplayObject *G32 D_8016913C;", header)
        self.assertIn("extern s8 D_80169070;", header)
        self.assertIn("extern s8 D_80169140;", header)

    def retail_image(self):
        module = self.module()
        archive = ROOT / module["archive"]
        if not archive.is_file():
            self.skipTest("Legally obtained French WA archive is unavailable")
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
                               (0xF3C, 0x80168048), (0xED0, 0x80168D34)):
            word, = struct.unpack_from("<I", image, offset)
            self.assertEqual(word >> 26, 3)
            self.assertEqual(0x80000000 | ((word & 0x3FFFFFF) << 2), target)
        self.assertEqual(struct.unpack_from("<I", image, 0xAC)[0], 0x2882003C)
        self.assertEqual(struct.unpack_from("<I", image, 0xFC)[0], 0x00821023)
        self.assertEqual(struct.unpack_from("<I", image, 0x100)[0], 0x27BDFFC8)

    def test_color_and_accessor_do_not_claim_direct_reachability(self):
        with (ROOT / "config/sles_03948/overlays/options_functions.csv").open() as handle:
            rows = {r["name"]: r for r in csv.DictReader(handle)}
        for name in ("func_80168004", "func_80169030"):
            self.assertIn("no direct caller found", rows[name]["notes"])

    def test_resident_load_pointer_and_options_entrypoints(self):
        executable = ROOT / "game/france/SLES_039.48"
        if not executable.is_file():
            self.skipTest("Legally obtained French executable is unavailable")
        data = executable.read_bytes()
        expected = load_checksum_manifest(ROOT / "config/sles_03948/files.sha256")["game/france/SLES_039.48"]
        self.assertEqual(hashlib.sha256(data).hexdigest(), expected)
        self.assertEqual(struct.unpack_from("<I", data, 0x18)[0], 0x80010000)
        self.assertEqual(struct.unpack_from("<I", data, 0x800 + 0x1D8)[0], 0x80168000)
        offset = 0x800 + 0x8002D89C - 0x80010000
        words = struct.unpack_from("<22I", data, offset)
        targets = {0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3}
        self.assertTrue({0x801686AC, 0x80168E1C} <= targets)

    def test_resident_dependency_owners_when_built(self):
        linked = ROOT / "tmp/project-build/SLES_039.48.elf"
        executable = ROOT / "game/france/SLES_039.48"
        if not linked.is_file() or not executable.is_file():
            self.skipTest("Build french-match with legal inputs before checking resident owners")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools is required for resident ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        retail = executable.read_bytes()
        expected = load_checksum_manifest(ROOT / "config/sles_03948/files.sha256")["game/france/SLES_039.48"]
        self.assertEqual(hashlib.sha256(retail).hexdigest(), expected)
        self.assertEqual((linked.parent / "SLES_039.48").read_bytes(), retail)
        directory = ROOT / "tmp/splat/sles_03948"
        script = (directory / "sles_03948.ld").read_text()
        raw = directory / "build/tmp/splat/sles_03948/assets/resident_tail_8cb98.o"
        self.assertIn(raw.relative_to(ROOT).as_posix(), script)
        with raw.open("rb") as handle:
            elf = ELFFile(handle)
            owners = [s for s in elf.get_section_by_name(".symtab").iter_symbols()
                      if s.name.endswith("resident_tail_8cb98_bin_start")]
            self.assertEqual(len(owners), 1)
            owner = owners[0]
            self.assertIsInstance(owner["st_shndx"], int)
            owner_name = owner.name
            raw_data = elf.get_section(owner["st_shndx"]).data()
        self.assertEqual(raw_data, retail[0x8CB98:0x8CB98 + len(raw_data)])
        with linked.open("rb") as handle:
            final = ELFFile(handle)
            owner, = final.get_section_by_name(".symtab").get_symbol_by_name(owner_name)
            self.assertIsInstance(owner["st_shndx"], int)
            self.assertEqual(owner["st_value"], 0x8009C398)
            section = final.get_section(owner["st_shndx"])
            start = owner["st_value"] - section["sh_addr"]
            self.assertEqual(section.data()[start:start + len(raw_data)], raw_data)
            for address in (0x8009C44B, 0x801BF98C):
                self.assertLessEqual(owner["st_value"], address)
                self.assertLess(address, owner["st_value"] + len(raw_data))
            for symbol, address, size, source in (
                ("func_80043BC8", 0x80043DC8, 116, "french/main_run_boot_sequence"),
                ("DisplayObject_UpdateResourceVariant", 0x80040748, 40, "european/display_object_core"),
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

    def test_production_image_and_real_c_owners_when_built(self):
        build = ROOT / "tmp/overlays/french_options/build"
        linked = build / "french_options.elf"
        if not linked.is_file():
            self.skipTest("Build french-match-overlays before checking production ELF ownership")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools is required for production ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        image = self.retail_image()
        self.assertEqual((build / "french_options.bin").read_bytes(), image)
        script = (build.parent / "french_options.ld").read_text()
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
                definitions = final.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                self.assertEqual(len(definitions), 1)
                definition = definitions[0]
                self.assertIsInstance(definition["st_shndx"], int)
                self.assertEqual(definition["st_value"], 0x80168000 + offset)
                section = final.get_section(definition["st_shndx"])
                self.assertFalse(section["sh_flags"] & 4)
                start = definition["st_value"] - section["sh_addr"]
                self.assertEqual(section.data()[start:start + size], image[offset:offset + size])


if __name__ == "__main__":
    unittest.main()
