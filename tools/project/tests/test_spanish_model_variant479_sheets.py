import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from tools.project.tests import test_spanish_model_variant460 as lifetimes


class SpanishModelVariant479SheetsTests(unittest.TestCase):
    def setUp(self):
        manifest = json.loads((ROOT / "config/sles_03951/overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m["linker_symbols"].endswith("/model_variant479_linker_symbols.txt")]
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

    def test_entry_boundaries_dispatch_and_conditional_overlap(self):
        anchors = {
            0x1C: 0x27D918D8, 0x24: 0x27D82D98, 0x28: 0xAFB90078,
            0x34: 0x27D82468, 0x6BC: 0x27300188, 0x7B0: 0x26100250,
            0x7B8: 0x2A620005, 0x7BC: 0x27390250,
            0x904: 0x2A020004, 0x948: 0x24630098, 0x950: 0x2B220008,
            0x954: 0x27180098, 0xE3C: 0x8C430018, 0xE48: 0x0043102B,
            0xE5C: 0x28420005, 0x2B90: 0x27BDFEF8, 0x2B98: 0x00809021,
            0x2BA0: 0x26562D98, 0x2BC8: 0x26513F7C, 0x2BD8: 0x26571A60,
            0x2C30: 0x86E4FEB8, 0x2C48: 0x00031940, 0x2C50: 0x8C624024,
            0x2C60: 0x86E4FEBA, 0x2C74: 0x86E4FEBC,
            0x2D78: 0x24E50020, 0x2D80: 0x24E60040, 0x2DB4: 0x24E70060,
            0x2E10: 0x000210C0, 0x2E5C: 0x2A820005, 0x2E68: 0x8EE20000,
            0x2E78: 0x04600008, 0x2E88: 0x04400004, 0x2E98: 0x3066FFFF,
            0x2EA0: 0x2A620004, 0x2EF8: 0x00021240, 0x2F18: 0xAE4241EC,
            0x2F24: 0x8CA40024, 0x2F48: 0x8CA20028, 0x2F54: 0x0062001B,
            0x2F80: 0x26F70250, 0x2FB0: 0x8C640018, 0x2FB4: 0x8C63001C,
            0x2FC4: 0x0043001B, 0x2FF4: 0xAE4241EC, 0x3030: 0x0062001B,
            0x305C: 0x26100098, 0x3060: 0x2A820008, 0x3068: 0x26D60098,
        }
        decoder = lifetimes.SpanishModelVariant460Tests
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            self.assertEqual(struct.unpack_from("<II", image, 0xE68),
                             (0x0C000000 | ((base + 0x2B90) >> 2 & 0x3FFFFFF), 0x02C02021))
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            for register, expected in ((18, [0x2B98, 0x3088]), (17, [0x2BC8, 0x308C]),
                                       (29, [0x2B90, 0x3098])):
                self.assertEqual(decoder.register_writes(image, 0x2B90, 0x309C, register), expected)
            self.assertEqual(sorted((off, size) for _, off, size in
                                    decoder.direct_stores(image, 0x2B90, 0x309C, 17)),
                             [(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
            self.assertEqual(tuple(struct.unpack_from("<I", image, 0x3E1C + o)[0]
                                   for o in (0x18, 0x1C, 0x24, 0x28)), (20, 100, 360, 440))
            context = base - 0x5000
            self.assertEqual(context + 0x41F0 - (base - 0x1000), 496)
            self.assertLess(context + 0x41F0, base)
        self.assertEqual(0x18D8 + 5 * 0x250, 0x2468)
        self.assertEqual(0x2D98 + 8 * 0x98, 0x3258)
        self.assertEqual(0x3F7C + 52, 0x3FB0)
        self.assertEqual(0x4024 + 3 * 32, 0x4084)
        self.assertEqual([i % 3 for i in range(5)], [0, 1, 2, 0, 1])
        self.assertEqual([i - 5 for i in range(5, 8)], [0, 1, 2])

    def test_target_compiled_layout(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() \
                or importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        constants = (
            ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
            ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52),
            ("sizeof(Sheets479Primary)", 0x250), ("O(Sheets479Primary,position)", 0x40),
            ("O(Sheets479Primary,active)", 0x188), ("sizeof(Sheets479Position)", 0x20),
            ("sizeof(ModelVariantSheet)", 0x98), ("O(ModelVariantSheet,v1)", 0x20),
            ("O(ModelVariantSheet,v2)", 0x40), ("O(ModelVariantSheet,v3)", 0x60),
            ("O(ModelVariantSheet,outer)", 0x80), ("O(ModelVariantSheet,inner)", 0x84),
            ("O(ModelVariantSheet,size)", 0x88), ("sizeof(Sheets479Timing)", 0x2C),
            ("O(Sheets479Timing,grow_begin)", 0x18), ("O(Sheets479Timing,grow_end)", 0x1C),
            ("O(Sheets479Timing,fade_begin)", 0x24), ("O(Sheets479Timing,fade_end)", 0x28),
            ("O(Sheets479State,primary)", 0x18D8), ("O(Sheets479State,sheets)", 0x2D98),
            ("O(Sheets479State,polygon)", 0x3F7C), ("O(Sheets479State,positions)", 0x4024),
            ("O(Sheets479State,frame)", 0x4164), ("O(Sheets479State,time)", 0x4168),
            ("O(Sheets479State,step)", 0x4170), ("O(Sheets479State,timing)", 0x4178),
            ("O(Sheets479State,phase)", 0x41EC), ("sizeof(Sheets479State)", 0x41F0),
            ("sizeof(((Sheets479State *)0)->timing)", 4),
            ("O(POLY_GT4,x0)", 8), ("O(POLY_GT4,x1)", 20), ("O(POLY_GT4,x2)", 32),
            ("O(POLY_GT4,x3)", 44), ("O(ModelSlot,field_CF8)", 0xCF8),
            ("O(ModelSlot,field_DE8)", 0xDE8), ("O(ModelSlot,field_DEC)", 0xDEC),
            ("O(ModelControlCommandView,commands)", 0xD08), ("O(ModelSlot,field_CF8.field_18)", 0xD10),
            ("O(ModelTransferMetadata,field_CF8)", 0x100), ("sizeof(ModelSlotCF8BlockWords)", 0x1C),
        )
        directory = ROOT / "tmp/test-spanish-model479-sheets-layout"
        directory.mkdir(parents=True, exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/overlays/spanish_model_variant/variant479_sheets.h"\n'
                          '#include "../../src/game/model_control.h"\n'
                          '#define O(t,f) ((u32)&((t *)0)->f)\n'
                          'const u32 layout[] = {' + ",".join(c for c, _ in constants) + '};\n')
        obj = compile_c(ROOT, tool(ROOT, "as"),
                        {"source": str(source.relative_to(ROOT)), "profile": "gcc_2_8_1_g0_split", "object": "layout.o"},
                        load_compiler_profiles(ROOT), object_directory=str(directory.relative_to(ROOT)),
                        asm_directory=str(directory.relative_to(ROOT)))
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
            section = elf.get_section(symbol["st_shndx"])
            self.assertEqual((symbol["st_value"], section["sh_size"]), (0, len(constants) * 4))
            self.assertEqual(section.data(), struct.pack("<" + "I" * len(constants), *(v for _, v in constants)))

    def test_complete_images_selected_c_owners_and_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL479 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selections = c_segments(ROOT, ROOT / module["layout"])
            self.assertEqual(len(selections), 2)
            selected, = [s for s in selections if "variant479_sheets" in s["source"]]
            path = directory / "build" / selected["object"]
            script = (directory / f"{module['name']}.ld").read_text()
            self.assertIn(f"{path.relative_to(ROOT)}(.text);", script)
            bindings = dict((n, int(a, 0)) for n, a in re.findall(
                r"^(\w+) = (0x[0-9A-F]+);", (ROOT / module["linker_symbols"]).read_text(), re.M))
            with linked_path.open("rb") as linked_handle, path.open("rb") as source_handle:
                linked, compiled = ELFFile(linked_handle), ELFFile(source_handle)
                symbols, originals = linked.get_section_by_name(".symtab"), compiled.get_section_by_name(".symtab")
                name = f"func_{base+0x2B90:X}"
                own, = originals.get_symbol_by_name(name)
                final, = symbols.get_symbol_by_name(name)
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1292, "STT_FUNC"))
                self.assertEqual((final["st_value"], final["st_size"], final["st_info"]["type"]),
                                 (base + 0x2B90, 1292, "STT_FUNC"))
                section, text = linked.get_section(final["st_shndx"]), compiled.get_section(own["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4 and text["sh_flags"] & 4)
                start = final["st_value"] - section["sh_addr"]
                self.assertEqual(section.data()[start:start+1292], image[0x2B90:0x309C])
                definitions = [s.name for s in originals.iter_symbols()
                               if isinstance(s["st_shndx"], int) and s["st_info"]["type"] == "STT_FUNC"]
                self.assertEqual(definitions, [name])
                count, local_count, callees = 0, 0, set()
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    if relocation["r_info_type"] != 4:
                        continue
                    symbol = originals.get_symbol(relocation["r_info_sym"])
                    site = relocation["r_offset"]
                    word, = struct.unpack_from("<I", image, 0x2B90 + site)
                    addend = (struct.unpack_from("<I", text.data(), site)[0] & 0x3FFFFFF) << 2
                    address = ((base + 0x2B90 + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    if isinstance(symbol["st_shndx"], int):
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        self.assertEqual(word >> 26, 2)
                        self.assertEqual(address, base + 0x2B90 + symbol["st_value"] + addend)
                        self.assertTrue(base + 0x2B90 <= address < base + 0x309C)
                        local_count += 1
                        continue
                    self.assertEqual(symbol["st_shndx"], "SHN_UNDEF")
                    self.assertEqual(word >> 26, 3)
                    resolved, = symbols.get_symbol_by_name(symbol.name)
                    self.assertEqual(resolved["st_value"], bindings[symbol.name])
                    self.assertEqual(address, resolved["st_value"] + addend)
                    callees.add(address)
                    count += 1
                self.assertEqual(count, 10)
                self.assertEqual(local_count, 3)
                self.assertEqual(callees, {0x8005C018, 0x800842A8, 0x80085558, 0x80086258,
                                          0x800872A8, 0x800875F8, 0x80087738, 0x80087958, 0x80087CB8})

    def test_terminal_records_match_selected_sources(self):
        with (ROOT / "notes/overlays/spanish-model-variant479-sheets-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 5)
        self.assertEqual([int(r["different_words"]) for r in rows], [316, 0, 0, 0, 0])
        for module in self.modules:
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant479_sheets" in s["source"]]
            terminal = [r for r in rows if r["module"] == module["name"]][-1]
            self.assertEqual((terminal["function_offset"], terminal["result"]), ("0x2B90", "matched"))
            self.assertEqual((terminal["instruction_bytes"], terminal["different_words"]), ("1292", "0"))
            self.assertEqual(terminal["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], hashlib.sha256(
                (ROOT / "src/overlays/spanish_model_variant/variant479_sheets.c").read_bytes() +
                (ROOT / "src/overlays/spanish_model_variant/variant479_sheets.h").read_bytes()).hexdigest())


if __name__ == "__main__":
    unittest.main()
