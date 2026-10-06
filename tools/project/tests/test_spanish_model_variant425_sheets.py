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
from tools.project.tests import test_spanish_model_variant460 as lifetimes


class SpanishModelVariant425SheetsTests(unittest.TestCase):
    def setUp(self):
        manifest = json.loads((ROOT / "config/sles_03951/overlays.json").read_text())
        self.modules = [module for module in manifest["modules"]
                        if module.get("linker_symbols", "").endswith("/model_variant425_linker_symbols.txt")]
        self.assertEqual(len(self.modules), 2)

    def legal_images(self):
        path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.is_file():
            self.skipTest("Legal Spanish MODEL input required")
        with path.open("rb") as handle:
            for module in self.modules:
                handle.seek(module["sector_offset"] * 2048)
                image = handle.read(20480)
                self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
                yield module, image

    def test_initialized_packet_sheet_bounds_and_unsigned_timings(self):
        decoder = lifetimes.SpanishModelVariant460Tests
        anchors = {
            0xC: 0x00809021,
            0x14: 0x0240F021,
            0x1C: 0x27D90E1C,
            0x28: 0xAFB90088,
            0x34: 0x27D7166C,
            0x29C: 0x26F70034,
            0x2A0: 0x0C020BBA,
            0x2A4: 0x02E02021,
            0x2B4: 0xA6F1001A,
            0x2B8: 0xA2F3000C,
            0x2D0: 0xA2F20030,
            0x2DC: 0xA6F4000E,
            0x888: 0x24840098,
            0x890: 0x2A820002,
            0x894: 0x27180098,
            0xCA4: 0x8C630018,
            0xCA8: 0x8FC2178C,
            0xCB0: 0x0043102B,
            0xCB4: 0x14400003,
            0xCC0: 0x02402021,
            0x26E4: 0x00809021,
            0x26EC: 0x26550E1C,
            0x2718: 0x265116A0,
            0x2724: 0x26500EA4,
            0x278C: 0x8E4317D0,
            0x2A34: 0x0043001B,
            0x2A40: 0x0007000D,
            0x2AA0: 0x0043001B,
            0x2AAC: 0x0007000D,
            0x2B5C: 0x0062001B,
            0x2B68: 0x0007000D,
            0x2AEC: 0x28624000,
            0x2B00: 0x000212C0,
            0x2B54: 0x00031B80,
            0x2B8C: 0x2AE20002,
            0x2B94: 0x26B50098,
        }
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            self.assertEqual(struct.unpack_from("<II", image, 0xCBC),
                             (0x0C000000 | ((base + 0x26DC) >> 2 & 0x3FFFFFF), 0x02402021))
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(decoder.register_writes(image, 0x34, 0x2A8, 23), [0x34, 0x29C])
            self.assertEqual(decoder.register_writes(image, 0x26DC, 0x2BC8, 17), [0x2718, 0x2BB8])
            self.assertEqual(sorted((offset, size) for _, offset, size in
                                    decoder.direct_stores(image, 0x26DC, 0x2BC8, 17)),
                             [(offset, 1) for offset in
                              (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
            timings = struct.unpack_from("<6I", image, 0x3A04 + 0x18)
            self.assertEqual(timings, (120, 220, 240, 360, 540, 580))
            self.assertEqual((timings[1] - timings[0], timings[5] - timings[4]), (100, 40))
            self.assertLess(base - 0x5000 + 0x1810, base - 0x1000)
        self.assertEqual(0xE1C + 2 * 152, 0xF4C)
        self.assertEqual(0x60 + 3 * 8 + 8, 0x80)
        self.assertEqual(0x166C + 52, 0x16A0)
        self.assertEqual(0x16A0 + 52, 0x16D4)

    def test_target_compiled_layout_and_terminal_fingerprints(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        constants = (
            ("sizeof(SVECTOR)", 8),
            ("sizeof(VECTOR)", 16),
            ("sizeof(MATRIX)", 32),
            ("sizeof(GsCOORDINATE2)", 80),
            ("sizeof(POLY_GT4)", 52),
            ("sizeof(ModelVariantSheet)", 152),
            ("O(ModelVariantSheet,v1)", 0x20),
            ("O(ModelVariantSheet,v2)", 0x40),
            ("O(ModelVariantSheet,v3)", 0x60),
            ("O(ModelVariantSheet,outer)", 0x80),
            ("O(ModelVariantSheet,inner)", 0x84),
            ("O(ModelVariantSheet,size)", 0x88),
            ("sizeof(Sheet425Descriptor)", 48),
            ("O(Sheet425Descriptor,grow_start)", 0x18),
            ("O(Sheet425Descriptor,grow_end)", 0x1C),
            ("O(Sheet425Descriptor,shrink_start)", 0x28),
            ("O(Sheet425Descriptor,shrink_end)", 0x2C),
            ("O(Sheet425State,sheets)", 0xE1C),
            ("O(Sheet425State,polygon)", 0x16A0),
            ("O(Sheet425State,origin)", 0x1748),
            ("O(Sheet425State,direction)", 0x175C),
            ("O(Sheet425State,frame)", 0x1788),
            ("O(Sheet425State,time)", 0x178C),
            ("O(Sheet425State,step)", 0x1794),
            ("O(Sheet425State,descriptor)", 0x179C),
            ("O(Sheet425State,progress)", 0x17D0),
            ("O(Sheet425State,phase)", 0x180C),
            ("sizeof(Sheet425State)", 0x1810),
            ("O(POLY_GT4,x0)", 8),
            ("O(POLY_GT4,x1)", 20),
            ("O(POLY_GT4,x2)", 32),
            ("O(POLY_GT4,x3)", 44),
        )
        directory = ROOT / "tmp/model425-sheets-layout-tests"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n'
                          '#include "../../src/overlays/spanish_model_variant/variant425_sheets.h"\n'
                          '#define O(t,f) ((u32)&((t *)0)->f)\n'
                          'u32 layout[] = {' + ",".join(expression for expression, _ in constants) + "};\n")
        obj = compile_c(ROOT, tool(ROOT, "as"),
                        {"source": str(source.relative_to(ROOT)), "profile": "gcc_2_8_1_g0_split",
                         "object": "layout.o"}, load_compiler_profiles(ROOT),
                        object_directory=str(directory.relative_to(ROOT)),
                        asm_directory=str(directory.relative_to(ROOT)))
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
            section = elf.get_section(symbol["st_shndx"])
            self.assertEqual((symbol["st_value"], section["sh_size"]), (0, len(constants) * 4))
            self.assertEqual(section.data(),
                             struct.pack("<" + "I" * len(constants), *(value for _, value in constants)))
        with (ROOT / "notes/overlays/spanish-model-variant425-sheets-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([row["different_words"] for row in rows], ["0"] * 6)
        body = ROOT / "src/overlays/spanish_model_variant/variant425_sheets.c"
        shared = ROOT / "src/overlays/model_variant/model_variant.h"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes()
                                    + shared.read_bytes()).hexdigest()
        for module in self.modules:
            selected, = [segment for segment in c_segments(ROOT, ROOT / module["layout"])
                         if "variant425_sheets" in segment["source"]]
            terminal = [row for row in rows if row["module"] == module["name"]][-1]
            self.assertEqual((terminal["function_offset"], terminal["result"], terminal["instruction_bytes"],
                              terminal["different_words"], terminal["profile"]),
                             ("0x26DC", "matched", "1260", "0", "gcc_2_8_1_g0_split"))
            self.assertEqual(terminal["fingerprint"],
                             hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)

    def test_sheet_headers_coexist_in_both_include_orders(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model425-sheets-header-tests"
        directory.mkdir(exist_ok=True)
        for order in ((423, 425), (425, 423)):
            with self.subTest(order=order):
                source = directory / f"headers_{order[0]}.c"
                source.write_text(
                    '#include "../../src/types.h"\n'
                    + "".join(
                        f'#include "../../src/overlays/spanish_model_variant/variant{family}_sheets.h"\n'
                        for family in (*order, *order))
                    + "u32 sizes[] = {sizeof(Sheet423State), sizeof(Sheet425State)};\n")
                obj = compile_c(
                    ROOT, tool(ROOT, "as"),
                    {"source": str(source.relative_to(ROOT)), "profile": "gcc_2_8_1_g0_split",
                     "object": f"headers_{order[0]}.o"}, load_compiler_profiles(ROOT),
                    object_directory=str(directory.relative_to(ROOT)),
                    asm_directory=str(directory.relative_to(ROOT)))
                with obj.open("rb") as handle:
                    elf = ELFFile(handle)
                    symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("sizes")
                    section = elf.get_section(symbol["st_shndx"])
                    self.assertEqual((symbol["st_value"], section["sh_size"]), (0, 8))
                    self.assertEqual(section.data(), struct.pack("<II", 0x1E10, 0x1810))

    def test_sheet_input_final_owner_and_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build MODEL425 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selections = c_segments(ROOT, ROOT / module["layout"])
            self.assertEqual(len(selections), 2)
            selected, = [segment for segment in selections if "variant425_sheets" in segment["source"]]
            path = directory / "build" / selected["object"]
            self.assertIn(f"{path.relative_to(ROOT)}(.text);",
                          (directory / f"{module['name']}.ld").read_text())
            with linked_path.open("rb") as linked_handle, path.open("rb") as source_handle:
                linked, compiled = ELFFile(linked_handle), ELFFile(source_handle)
                symbols = linked.get_section_by_name(".symtab")
                original = compiled.get_section_by_name(".symtab")
                name = f"func_{base+0x26DC:X}"
                own, = original.get_symbol_by_name(name)
                final, = symbols.get_symbol_by_name(name)
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1260, "STT_FUNC"))
                self.assertEqual((final["st_value"], final["st_size"], final["st_info"]["type"]),
                                 (base + 0x26DC, 1260, "STT_FUNC"))
                definitions = [symbol.name for symbol in original.iter_symbols()
                               if isinstance(symbol["st_shndx"], int)
                               and symbol["st_info"]["type"] == "STT_FUNC"]
                self.assertEqual(definitions, [name])
                section, text = linked.get_section(final["st_shndx"]), compiled.get_section(own["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4 and text["sh_flags"] & 4)
                start = final["st_value"] - section["sh_addr"]
                self.assertEqual(section.data()[start:start+1260], image[0x26DC:0x2BC8])
                callees, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    site = relocation["r_offset"]
                    self.assertLess(site, 1260)
                    symbol = original.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", image, 0x26DC + site)
                    address = ((base + 0x26DC + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    addend = (struct.unpack_from("<I", text.data(), site)[0] & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        resolved, = symbols.get_symbol_by_name(symbol.name)
                        self.assertEqual((word >> 26, address), (3, resolved["st_value"] + addend))
                        callees.append(address)
                    else:
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        self.assertEqual((word >> 26, address),
                                         (2, base + 0x26DC + symbol["st_value"] + addend))
                        jumps.append((site, address - base - 0x26DC))
                self.assertEqual(len(callees), 10)
                self.assertEqual(set(callees), {0x8005C018, 0x800842A8, 0x80085558, 0x80086258,
                                               0x800872A8, 0x800875F8, 0x80087738, 0x80087958,
                                               0x80087CB8})
                self.assertEqual(callees.count(0x80087CB8), 2)
                self.assertEqual(jumps, [(0xA4, 0x134), (0x3DC, 0x49C), (0x3F8, 0x448)])


if __name__ == "__main__":
    unittest.main()
