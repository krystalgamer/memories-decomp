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
from tools.project.tests import test_spanish_model_variant474 as sheets

LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_G4)", 36),
    ("sizeof(Mesh474Rows)", 0x4EC), ("O(Mesh474Rows,color)", 0x4C8),
    ("O(Mesh474State,mesh)", 0x6DC), ("O(Mesh474State,polygon)", 0x1A50),
    ("O(Mesh474State,target)", 0x1B5C), ("O(Mesh474State,direction)", 0x1B64),
    ("O(Mesh474State,frame)", 0x1B90), ("O(Mesh474State,step)", 0x1B9C),
    ("O(Mesh474State,size)", 0x1BC4), ("O(Mesh474State,intensity)", 0x1BD4),
    ("O(Mesh474State,phase)", 0x1C1C), ("sizeof(Mesh474State)", 0x1C20),
    ("O(POLY_G4,x0)", 8), ("O(POLY_G4,x1)", 16),
    ("O(POLY_G4,x2)", 24), ("O(POLY_G4,x3)", 32),
)

ANCHORS = {
    0xC: 0x00809021, 0x14: 0x0240F021, 0x4C: 0x27D006DC,
    0x64: 0x27D91A50, 0x88: 0xAFB90098, 0x118: 0xAFB000B8,
    0x120: 0x0200C021, 0x244: 0x8FA40098, 0x248: 0x0C020BB2,
    0x250: 0x8FA40098, 0x254: 0x0C020B6A, 0x258: 0x24050001,
    0x4D4: 0x8FB000B8, 0x518: 0x2A220011, 0x520: 0x26100008,
    0x54C: 0xA32204C8, 0x550: 0xA32204C9, 0x530: 0xA33804CA,
    0x564: 0x27180088, 0x570: 0x2B220009, 0x574: 0xAFB800B8,
    0x1DC0: 0x27BDFEB0, 0x1DC8: 0x0080B021, 0x1E10: 0x26D21A50,
    0x1E14: 0x26C906DC, 0x1E18: 0xAFA90108, 0x1E60: 0x86C21B5C,
    0x1E6C: 0x86C21B5E, 0x1E78: 0x86C21B60, 0x2134: 0x18400005,
    0x2140: 0x3046FFFF, 0x2158: 0x2AA20010, 0x2170: 0x2A020008,
    0x2188: 0x28420003, 0x219C: 0x28621000, 0x21B0: 0x00021200,
    0x21C8: 0xAEC21BC4, 0x21D0: 0x24020005, 0x21F4: 0x00021140,
    0x2208: 0xAEC01BD4, 0x220C: 0xAEC21C1C,
}


class SpanishModelVariant474MeshTests(unittest.TestCase):
    legal_images = sheets.SpanishModelVariant474Tests.legal_images

    def setUp(self):
        sheets.SpanishModelVariant474Tests.setUp(self)

    def test_target_layout_header_isolation_and_terminal_fingerprints(self):
        compiler = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
        if not compiler.is_file() or importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model474-mesh-layout-test"
        directory.mkdir(exist_ok=True)
        headers = ("variant474_mesh.h", "variant474_sheets.h")
        for index, order in enumerate((headers, headers[::-1])):
            source = directory / f"layout{index}.c"
            source.write_text('#include "../../src/types.h"\n' + "".join(
                f'#include "../../src/overlays/spanish_model_variant/{header}"\n'
                for header in order * 2) + '#define O(t,f) ((u32)&((t *)0)->f)\n'
                'u32 layout[] = {' + ",".join(expression for expression, _ in LAYOUT) + "};\n")
            obj = compile_c(ROOT, tool(ROOT, "as"),
                            dict(source=str(source.relative_to(ROOT)), profile="gcc_2_8_1_g0_split",
                                 object=f"layout{index}.o"), load_compiler_profiles(ROOT),
                            object_directory=str(directory.relative_to(ROOT)),
                            asm_directory=str(directory.relative_to(ROOT)))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
                data = elf.get_section(symbol["st_shndx"]).data()
                expected = struct.pack("<" + "I" * len(LAYOUT), *(value for _, value in LAYOUT))
                self.assertEqual(symbol["st_value"], 0)
                self.assertEqual(data, expected)
        body = ROOT / "src/overlays/spanish_model_variant/variant474_mesh.c"
        shared = ROOT / "src/overlays/model_variant/model_variant.h"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes()
                                    + shared.read_bytes()).hexdigest()
        with (ROOT / "notes/overlays/spanish-model-variant474-mesh-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 6)
        self.assertEqual({row["module"] for row in attempts[2:]}, {m["name"] for m in self.modules})
        for row in attempts[2:]:
            source = body.with_name(body.stem + ("_slot1" if int(row["slot"]) else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["dependency_fingerprint"], dependency)
            self.assertEqual((row["result"], row["instruction_bytes"], row["different_words"]),
                             ("matched", "1152", "0"))

    def test_initialized_mesh_packet_and_internal_parameter_without_invented_caller(self):
        decoder = sheets.SpanishModelVariant474Tests
        for module, image in self.legal_images():
            for offset, word in ANCHORS.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            stores = decoder.direct_stores(image, 4, 0x24C, 29)
            self.assertEqual([(site, size) for site, offset, size in stores if offset == 0x98],
                             [(0x88, 4)])
            self.assertEqual(decoder.register_writes(image, 0x1DC0, 0x2240, 22), [0x1DC8, 0x221C])
            self.assertEqual(decoder.register_writes(image, 0x1DC0, 0x2240, 18), [0x1E10, 0x222C])
            self.assertEqual([(offset, size) for _, offset, size in
                              decoder.direct_stores(image, 0x1DC0, 0x2240, 18)],
                             [(offset, 1) for offset in (4, 5, 6, 12, 13, 14, 20, 21, 22, 28, 29, 30)])
            self.assertNotIn(struct.pack("<I", int(module["load_address"], 0) + 0x1DC0), image)
        self.assertEqual(0x6DC + 9 * 17 * 8, 0xBA4)
        self.assertEqual(0xBA4 + 9 * 4, 0xBC8)
        self.assertEqual(0x1A50 + 36, 0x1A74)

    def test_all_compiled_mesh_owners_and_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            directory = ROOT / "tmp/overlays" / module["name"]
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL474 images before checking owners")
            base = int(module["load_address"], 0)
            selected, = [segment for segment in c_segments(ROOT, ROOT / module["layout"])
                         if "variant474_mesh" in segment["source"]]
            source = "src/overlays/spanish_model_variant/variant474_mesh" + (
                "_slot1" if base == 0x8017B000 else "") + ".c"
            self.assertEqual(selected["source"], source)
            obj_path = directory / "build" / selected["object"]
            self.assertIn(f"{obj_path.relative_to(ROOT)}(.text);",
                          (directory / f"{module['name']}.ld").read_text())
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            with obj_path.open("rb") as handle, linked_path.open("rb") as linked_handle:
                obj, linked = ELFFile(handle), ELFFile(linked_handle)
                symbols = obj.get_section_by_name(".symtab")
                final_symbols = linked.get_section_by_name(".symtab")
                own, = symbols.get_symbol_by_name(f"func_{base+0x1DC0:X}")
                final, = final_symbols.get_symbol_by_name(own.name)
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1152, "STT_FUNC"))
                self.assertEqual((final["st_value"], final["st_size"]), (base+0x1DC0, 1152))
                self.assertTrue(obj.get_section(own["st_shndx"])["sh_flags"] & 4)
                final_section = linked.get_section(final["st_shndx"])
                start = final["st_value"] - final_section["sh_addr"]
                self.assertEqual(final_section.data()[start:start+1152], image[0x1DC0:0x2240])
                text = bytearray(obj.get_section(own["st_shndx"]).data()[:1152])
                calls, jumps = [], []
                for relocation in obj.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    symbol = symbols.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", text, offset)
                    addend = (word & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        resolved, = final_symbols.get_symbol_by_name(symbol.name)
                        address = resolved["st_value"] + addend
                        calls.append(address)
                    else:
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        address = base + 0x1DC0 + symbol["st_value"] + addend
                        jumps.append((offset, address - base - 0x1DC0))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0x1DC0:0x2240])
                self.assertEqual(len(calls), 9)
                self.assertEqual(set(calls), {0x8005C018, 0x80089928, 0x80087CB8, 0x800875F8,
                                             0x80086258, 0x80085558, 0x80087958, 0x8004D5B8})
                self.assertEqual(jumps, [(0x88, 0x94), (0x1F0, 0x244)])
