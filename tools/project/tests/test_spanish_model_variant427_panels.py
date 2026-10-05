import csv
import hashlib
import importlib.util
from pathlib import Path
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from tools.project.tests import test_spanish_model_variant427 as family
from tools.project.tests import test_spanish_model_variant460 as lifetimes


class SpanishModelVariant427PanelsTests(unittest.TestCase):
    setUp = family.SpanishModelVariant427Tests.setUp
    legal_images = family.SpanishModelVariant427Tests.legal_images

    def test_original_context_records_packet_and_timing(self):
        anchors = {
            0xC: 0x0080A821, 0x1C: 0x27D91FF8, 0x44: 0x27D90ED0,
            0x34: 0x27D73560, 0x2D8: 0x26F70034, 0x2DC: 0x0C020BBA, 0x2E0: 0x02E02021,
            0x6CC: 0x27300240, 0x784: 0x261003A8, 0x8C4: 0x2A020004,
            0x908: 0x24630098, 0x910: 0x2B220006, 0x914: 0x27180098,
            0xDC4: 0x0043102B, 0xDD8: 0x28420005, 0xDE8: 0x02A02021,
            0x28A8: 0x00809021, 0x28AC: 0x26480ED0, 0x28B4: 0x26561FF8,
            0x28E0: 0x26513594, 0x28EC: 0x26502080,
            0x2DB8: 0x252903A8, 0x2DA8: 0x2AA20006,
            0x2B44: 0x3C046666, 0x2B50: 0x34846667,
            0x2B5C: 0x000210C0, 0x2B68: 0x00440018,
        }
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            self.assertEqual(struct.unpack_from("<I", image, 0xDE4)[0],
                             0x0C000000 | ((base + 0x28A0) >> 2 & 0x3FFFFFF))
            self.assertEqual(family.reaching_context(image, base, 0xDE4), {0xC})
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(lifetimes.SpanishModelVariant460Tests.register_writes(
                image, 0x28A0, 0x2DF8, 17), [0x28E0, 0x2DE8])
            stores = lifetimes.SpanishModelVariant460Tests.direct_stores(image, 0x28A0, 0x2DF8, 17)
            self.assertEqual(sorted((off, size) for _, off, size in stores),
                             [(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
            self.assertEqual(struct.unpack_from("<5I", image, 0x3BC8 + 0x18), (20, 100, 160, 360, 420))
            for offset in (0x2CA8, 0x2D04, 0x2D70):
                self.assertEqual(struct.unpack_from("<I", image, offset)[0] & 0x3F, 0x1B)
            for offset in (0x2CB4, 0x2D10, 0x2D7C):
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], 0x0007000D)
        self.assertEqual(0xED0 + 3 * 0x3A8, 0x19C8)
        self.assertEqual(0xED0 + 6 * 0x3A8, 0x24C0)
        self.assertLess(0x24C0, 0x37C8)
        self.assertEqual(0x1FF8 + 6 * 0x98, 0x2388)
        self.assertEqual(3 * 8 + 0x60 + 8, 0x80)
        self.assertEqual(0x3594 + 52, 0x35C8)

    def test_target_compiled_layout(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        expressions = """
sizeof(SVECTOR), sizeof(VECTOR), sizeof(MATRIX), sizeof(GsCOORDINATE2), sizeof(POLY_GT4),
sizeof(Panel427Primary), OFF(Panel427Primary, progress),
sizeof(ModelVariantSheet), OFF(ModelVariantSheet, v1), OFF(ModelVariantSheet, v2),
OFF(ModelVariantSheet, v3), OFF(ModelVariantSheet, outer), OFF(ModelVariantSheet, inner),
OFF(ModelVariantSheet, size), sizeof(Panel427Descriptor),
OFF(Panel427Descriptor, grow_start), OFF(Panel427Descriptor, grow_end),
OFF(Panel427Descriptor, shrink_start), OFF(Panel427Descriptor, shrink_end),
OFF(Panel427State, primary), OFF(Panel427State, sheets), OFF(Panel427State, polygon),
OFF(Panel427State, matrices), OFF(Panel427State, directions), OFF(Panel427State, frame),
OFF(Panel427State, time), OFF(Panel427State, step), OFF(Panel427State, descriptor),
OFF(Panel427State, phase), sizeof(Panel427State), sizeof(((Panel427State *)0)->descriptor),
OFF(POLY_GT4, x0), OFF(POLY_GT4, x1), OFF(POLY_GT4, x2), OFF(POLY_GT4, x3),
sizeof(((Panel427State *)0)->unknown_0000) + 6 * sizeof(Panel427Primary)
"""
        expected = (8, 16, 32, 80, 52, 0x3A8, 0x240, 0x98, 0x20, 0x40, 0x60, 0x80, 0x84, 0x88,
                    0x2C, 0x18, 0x1C, 0x24, 0x28, 0xED0, 0x1FF8, 0x3594, 0x3628, 0x36F0,
                    0x373C, 0x3740, 0x3748, 0x3750, 0x37C4, 0x37C8, 4, 8, 20, 32, 44, 0x24C0)
        directory = ROOT / "tmp/model427-panels-layout-tests"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n'
                          '#include "../../src/overlays/spanish_model_variant/variant427_panels.h"\n'
                          '#define OFF(t, f) ((u32)&((t *)0)->f)\n'
                          'u32 layout[] = {' + expressions + "};\n")
        obj = compile_c(ROOT, tool(ROOT, "as"),
                        {"source": str(source.relative_to(ROOT)), "profile": "gcc_2_8_1_g0_split", "object": "layout.o"},
                        load_compiler_profiles(ROOT), object_directory=str(directory.relative_to(ROOT)),
                        asm_directory=str(directory.relative_to(ROOT)))
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
            section = elf.get_section(symbol["st_shndx"])
            self.assertEqual((symbol["st_value"], section["sh_size"]), (0, len(expected) * 4))
            self.assertEqual(section.data(), struct.pack("<" + "I" * len(expected), *expected))

    def test_selected_object_and_all_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build MODEL427 overlays before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"]) if "variant427_panels" in s["source"]]
            path = directory / "build" / selected["object"]
            self.assertIn(f"{path.relative_to(ROOT)}(.text);", (directory / f"{module['name']}.ld").read_text())
            with linked_path.open("rb") as final_handle, path.open("rb") as obj_handle:
                linked, compiled = ELFFile(final_handle), ELFFile(obj_handle)
                symbols = compiled.get_section_by_name(".symtab")
                own, = symbols.get_symbol_by_name(f"func_{base+0x28A0:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]), (0, 1368, "STT_FUNC"))
                section = compiled.get_section(own["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                text = section.data()
                callees, local = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    symbol = symbols.get_symbol(relocation["r_info_sym"])
                    site = relocation["r_offset"]
                    self.assertLess(site, 1368)
                    word, = struct.unpack_from("<I", image, 0x28A0 + site)
                    address = ((base+0x28A0+site+4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        resolved, = linked.get_section_by_name(".symtab").get_symbol_by_name(symbol.name)
                        self.assertEqual((word >> 26, address), (3, resolved["st_value"]+addend))
                        callees.append(address)
                    else:
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        self.assertEqual((word >> 26, address),
                                         (2, base+0x28A0+symbol["st_value"]+addend))
                        local.append((site, address-base-0x28A0))
                self.assertEqual(local, [(0x128, 0x158), (0x380, 0x3D0), (0x41C, 0x4E8)])
                self.assertEqual(len(callees), 10)
                self.assertEqual(set(callees), {0x8005C018, 0x800842A8, 0x80085558, 0x80086258,
                                               0x800872A8, 0x800875F8, 0x80087738, 0x80087958, 0x80087CB8})

    def test_terminal_transitive_fingerprints(self):
        with (ROOT / "notes/overlays/spanish-model-variant427-panels-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([int(r["different_words"]) for r in rows], [264, 0, 0, 0, 0])
        body = ROOT / "src/overlays/spanish_model_variant/variant427_panels.c"
        shared = ROOT / "src/overlays/model_variant/model_variant.h"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes() +
                                    shared.read_bytes()).hexdigest()
        for module in self.modules:
            terminal = [r for r in rows if r["module"] == module["name"]][-1]
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"]) if "variant427_panels" in s["source"]]
            self.assertEqual((terminal["result"], terminal["instruction_bytes"], terminal["different_words"]),
                             ("matched", "1368", "0"))
            self.assertEqual(terminal["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)


if __name__ == "__main__":
    unittest.main()
