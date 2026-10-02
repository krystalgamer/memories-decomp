import csv
import hashlib
import json
from pathlib import Path
import struct
import sys
import unittest

from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_french_overlay_inventories
from verify_inputs import load_checksum_manifest


class FrenchOptionsTests(unittest.TestCase):
    starts = (4, 0x48, 0xAC, 0x100, 0x3E0, 0x6A4, 0x6AC, 0xA4C,
              0xBE8, 0xD34, 0xD68, 0xE1C, 0xF70, 0x1030, 0x1040)
    helpers = ((0xAC, 84, "position_easing"), (0x6A4, 8, "language_hook"))

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
        self.assertNotIn("linker_symbols", module)

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
            self.assertEqual(row["status"], "matching_c" if start in (0xAC, 0x6A4) else "unmatched_asm")
            if row["status"] == "unmatched_asm":
                self.assertIn(f"asm, overlays/french_options/{row['name']}", layout.read_text())
        counts = load_french_overlay_inventories(ROOT)["options"]
        self.assertEqual((counts["matching_c_function_count"], counts["matching_c_bytes"]), (2, 92))
        self.assertEqual(sum(int(r["size"], 0) for r in rows if r["status"] == "unmatched_asm"), 4064)

    def test_headers_and_unclassified_suffix_remain_owned(self):
        layout = ROOT / self.module()["layout"]
        symbols = layout.with_name("options_symbols.txt").read_text()
        self.assertIn("D_80168000 = 0x80168000; // type:u8 size:0x4 defined:true", symbols)
        self.assertIn("D_80169040 = 0x80169040; // type:u8 size:0x1FC0 defined:true", symbols)
        self.assertIn("data, overlays/french_options/unclassified_tail", layout.read_text())
        self.assertIn("[0x3000]", layout.read_text())
        for interior in (0xE8, 0xF4, 0x66C, 0xD3C, 0xFCC, 0x1024):
            self.assertNotIn(f"func_{0x80168000 + interior:X} =", symbols)

    def test_sources_and_attempts_preserve_exact_contracts(self):
        directory = ROOT / "src/overlays/pal_options"
        for stem in ("position_easing", "language_hook"):
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
        self.assertEqual(len([r for r in rows if r["result"] == "matched"]), 2)

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
                               (0x7A4, 0x801686A4)):
            word, = struct.unpack_from("<I", image, offset)
            self.assertEqual(word >> 26, 3)
            self.assertEqual(0x80000000 | ((word & 0x3FFFFFF) << 2), target)
        self.assertEqual(struct.unpack_from("<I", image, 0xAC)[0], 0x2882003C)
        self.assertEqual(struct.unpack_from("<I", image, 0xFC)[0], 0x00821023)
        self.assertEqual(struct.unpack_from("<I", image, 0x100)[0], 0x27BDFFC8)

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

    def test_production_image_and_real_c_owners_when_built(self):
        build = ROOT / "tmp/overlays/french_options/build"
        linked = build / "french_options.elf"
        if not linked.is_file():
            self.skipTest("Build french-match-overlays before checking production ELF ownership")
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
                    section = source.get_section(definition["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    self.assertEqual(section.data(), image[offset:offset + size])
                    self.assertFalse(any(s.name and s["st_shndx"] == "SHN_UNDEF"
                                         for s in source.get_section_by_name(".symtab").iter_symbols()))
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
            for symbol, offset, size in (("D_80168000", 0, 4), ("D_80169040", 0x1040, 0x1FC0)):
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
