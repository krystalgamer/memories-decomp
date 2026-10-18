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

LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52),
    ("sizeof(Mesh427)", 0x4F0), ("O(Mesh427,colors)", 0x4C8),
    ("O(Mesh427,size)", 0x4EC), ("sizeof(Mesh427Companion)", 0x3A8),
    ("O(Mesh427Companion,progress)", 0x240), ("O(Mesh427State,meshes)", 0),
    ("O(Mesh427State,companions)", 0xED0), ("O(Mesh427State,polygon)", 0x3450),
    ("O(Mesh427State,origins)", 0x3690), ("O(Mesh427State,frame)", 0x373C),
    ("O(Mesh427State,step)", 0x3748), ("O(Mesh427State,size)", 0x3770),
    ("O(Mesh427State,intensity)", 0x3780), ("O(Mesh427State,phase)", 0x37C4),
    ("sizeof(Mesh427State)", 0x37C8), ("O(POLY_GT4,x0)", 8),
    ("O(POLY_GT4,x1)", 20), ("O(POLY_GT4,x2)", 32), ("O(POLY_GT4,x3)", 44),
)

ANCHORS = {
    0xC: 0x0080A821, 0x14: 0x02A0F021, 0x4C: 0x27D83450,
    0x58: 0xAFB80098, 0x90: 0xAFBE0084,
    0x3E4: 0x8FA40098, 0x3E8: 0x0C020BBA, 0x3FC: 0xA498001A,
    0x430: 0x0C020B6A, 0x42C: 0x24050001,
    0x43C: 0x0C020B76, 0x440: 0x00002821,
    0x508: 0x8FB30084, 0x5B4: 0x2A420011, 0x5BC: 0x26100008,
    0x5C8: 0xA26004C8, 0x5CC: 0xA26204C9, 0x5D0: 0xA26004CA,
    0x5D8: 0x26D60088, 0x5FC: 0x2B020009, 0x610: 0xAF2004EC,
    0x618: 0x273904F0, 0x624: 0x2B020003,
    0x44: 0x27D90ED0, 0x6CC: 0x27300240, 0x784: 0x261003A8,
    0xC7C: 0xAE433690, 0xC84: 0x26503690,
    0xDF4: 0x2442FFFD, 0xDF8: 0x2C420003, 0xE10: 0x02A02021,
    0xFB4: 0x27BDFEA0, 0xFBC: 0x0080B021, 0xFC0: 0x26C80ED0,
    0xFC8: 0x26D33450, 0x1004: 0x26C804EC,
    0x1344: 0x8D430240, 0x134C: 0x28630400, 0x1374: 0x30C6FFFF,
    0x137C: 0x24070001, 0x1478: 0x250804F0, 0x1498: 0x254A03A8,
    0x14D4: 0x00021200, 0x1518: 0x2442F000, 0x1530: 0x24020400,
    0x1544: 0xAEC03780, 0x1548: 0xAEC237C4,
}


class SpanishModelVariant427MeshTests(unittest.TestCase):
    setUp = family.SpanishModelVariant427Tests.setUp
    legal_images = family.SpanishModelVariant427Tests.legal_images

    def test_entry_context_initialized_mesh_packet_and_companion_bounds(self):
        decoder = lifetimes.SpanishModelVariant460Tests
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            for offset, word in ANCHORS.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(family.reaching_context(image, base, 0xE0C), {0xC})
            self.assertEqual(struct.unpack_from("<I", image, 0xE0C)[0],
                             0x0C000000 | ((base + 0xFB4) >> 2 & 0x3FFFFFF))
            stores = decoder.direct_stores(image, 4, 0x3EC, 29)
            self.assertEqual([(site, size) for site, offset, size in stores if offset == 0x98],
                             [(0x58, 4)])
            self.assertEqual(decoder.register_writes(image, 0xFB4, 0x157C, 22), [0xFBC, 0x1558])
            self.assertEqual(decoder.register_writes(image, 0xFB4, 0x157C, 19), [0xFC8, 0x1564])
            stores = decoder.direct_stores(image, 0xFB4, 0x157C, 19)
            self.assertEqual([(offset, size) for _, offset, size in stores],
                             [(offset, 1) for offset in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
        self.assertEqual(9 * 17 * 8, 0x4C8)
        self.assertEqual(0x4C8 + 9 * 4, 0x4EC)
        self.assertEqual(3 * 0x4F0, 0xED0)
        self.assertEqual(0xED0 + 3 * 0x3A8, 0x19C8)
        self.assertEqual(0x3450 + 52, 0x3484)
        self.assertEqual(0x3690 + 3 * 16, 0x36C0)

    def test_target_layout_header_isolation_and_terminal_fingerprints(self):
        compiler = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
        if not compiler.is_file() or importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model427-mesh-layout-test"
        directory.mkdir(exist_ok=True)
        headers = ("variant427_mesh.h", "variant427_panels.h", "variant427_curtains.h")
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
                self.assertEqual(symbol["st_value"], 0)
                self.assertEqual(data, struct.pack("<" + "I" * len(LAYOUT),
                                                   *(value for _, value in LAYOUT)))
        body = ROOT / "src/overlays/spanish_model_variant/variant427_mesh.c"
        shared = ROOT / "src/overlays/model_variant/model_variant.h"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes()
                                    + shared.read_bytes()).hexdigest()
        with (ROOT / "notes/overlays/spanish-model-variant427-mesh-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual([int(row["different_words"]) for row in attempts], [271, 0, 0, 0, 0])
        self.assertEqual({row["module"] for row in attempts[3:]}, {m["name"] for m in self.modules})
        for row in attempts[3:]:
            source = body.with_name(body.stem + ("_slot1" if int(row["slot"]) else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["dependency_fingerprint"], dependency)
            self.assertEqual((row["result"], row["instruction_bytes"], row["different_words"]),
                             ("matched", "1480", "0"))

    def test_all_compiled_mesh_owners_and_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            directory = ROOT / "tmp/overlays" / module["name"]
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL427 images before checking owners")
            base = int(module["load_address"], 0)
            selected, = [segment for segment in c_segments(ROOT, ROOT / module["layout"])
                         if "variant427_mesh" in segment["source"]]
            self.assertEqual(selected["source"], "src/overlays/spanish_model_variant/variant427_mesh"
                             + ("_slot1" if base == 0x8017B000 else "") + ".c")
            obj_path = directory / "build" / selected["object"]
            self.assertIn(f"{obj_path.relative_to(ROOT)}(.text);",
                          (directory / f"{module['name']}.ld").read_text())
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            with obj_path.open("rb") as handle, linked_path.open("rb") as linked_handle:
                obj, linked = ELFFile(handle), ELFFile(linked_handle)
                symbols = obj.get_section_by_name(".symtab")
                final_symbols = linked.get_section_by_name(".symtab")
                own, = symbols.get_symbol_by_name(f"func_{base+0xFB4:X}")
                final, = final_symbols.get_symbol_by_name(own.name)
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1480, "STT_FUNC"))
                self.assertEqual((final["st_value"], final["st_size"]), (base+0xFB4, 1480))
                self.assertTrue(obj.get_section(own["st_shndx"])["sh_flags"] & 4)
                final_section = linked.get_section(final["st_shndx"])
                start = final["st_value"] - final_section["sh_addr"]
                self.assertEqual(final_section.data()[start:start+1480], image[0xFB4:0x157C])
                text = bytearray(obj.get_section(own["st_shndx"]).data()[:1480])
                calls, jumps = [], []
                for relocation in obj.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1480)
                    symbol = symbols.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", text, offset)
                    addend = (word & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        self.assertEqual(word >> 26, 3)
                        resolved, = final_symbols.get_symbol_by_name(symbol.name)
                        address = resolved["st_value"] + addend
                        calls.append(address)
                    else:
                        self.assertEqual(word >> 26, 2)
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        address = base + 0xFB4 + symbol["st_value"] + addend
                        jumps.append((offset, address - base - 0xFB4))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0xFB4:0x157C])
                self.assertEqual(len(calls), 7)
                self.assertEqual(set(calls), {0x8005C018, 0x80087CB8, 0x800875F8, 0x80086258,
                                             0x80085558, 0x80087958, 0x8004D5B8})
                self.assertEqual(jumps, [(0xA8, 0xBC), (0x21C, 0x270), (0x464, 0x4AC)])
