import csv
import hashlib
import importlib.util
import json
import struct
import unittest

from tools.project.tests import test_spanish_model_variant441 as lines

ROOT = lines.ROOT
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52), ("sizeof(PSXLONG)", 4),
    ("sizeof(Ribbon441)", 0x6C), ("O(Ribbon441,points[1])", 0x10),
    ("O(Ribbon441,points[2])", 0x20), ("O(Ribbon441,projected)", 0x30),
    ("O(Ribbon441,projected[1])", 0x38), ("O(Ribbon441,projected[2])", 0x40),
    ("O(Ribbon441,depth)", 0x64), ("sizeof(Ribbon441Timing)", 0x30),
    ("O(Ribbon441Timing,count)", 0x18), ("O(Ribbon441Timing,expansion_start)", 0x20),
    ("O(Ribbon441Timing,expansion_end)", 0x24), ("O(Ribbon441Timing,fade_start)", 0x28),
    ("O(Ribbon441Timing,fade_end)", 0x2C), ("O(Ribbon441State,ribbons)", 0xFD8),
    ("O(Ribbon441State,quad)", 0x256C), ("O(Ribbon441State,origin)", 0x2690),
    ("O(Ribbon441State,direction)", 0x26A4), ("O(Ribbon441State,projected)", 0x26B4),
    ("O(Ribbon441State,time)", 0x26D4), ("O(Ribbon441State,timing)", 0x26E4),
    ("O(Ribbon441State,index)", 0x26F8), ("O(Ribbon441State,progress)", 0x2714),
    ("O(Ribbon441State,width)", 0x2716), ("O(Ribbon441State,phase)", 0x271C),
    ("sizeof(Ribbon441State)", 0x2720),
    ("O(POLY_GT4,x0)", 8), ("O(POLY_GT4,x1)", 20),
    ("O(POLY_GT4,x2)", 32), ("O(POLY_GT4,x3)", 44),
    ("O(POLY_GT4,r0)", 4), ("O(POLY_GT4,r1)", 16),
    ("O(POLY_GT4,r2)", 28), ("O(POLY_GT4,r3)", 40),
)
CALLS = (
    (0x2C, 0x8005C018), (0x40, 0x80089928), (0x7C, 0x800866F8),
    (0xA4, 0x80086628), (0xD0, 0x800866F8), (0xE8, 0x80086628),
    (0x1E4, 0x80087CB8), (0x238, 0x80086258), (0x240, 0x80085558),
    (0x248, 0x800872A8), (0x254, 0x80087CB8), (0x260, 0x800875F8),
    (0x268, 0x80087738), (0x2C0, 0x80087898),
    (0x3C8, 0x800842A8), (0x474, 0x800842A8),
)


class SpanishModelVariant441RibbonTests(unittest.TestCase):
    setUp = lines.SpanishModelVariant441Tests.setUp
    legal_images = lines.SpanishModelVariant441Tests.legal_images

    def test_terminal_attempts_and_existing_aliases(self):
        self.assertEqual(len(self.modules), 10)
        self.assertEqual((len(self.bindings), len(set(self.bindings.values()))), (49, 37))
        for name, address in (("ReadRotMatrix", 0x800872A8), ("SetRotMatrix", 0x80087738),
                              ("RotTransPers3", 0x80087898)):
            self.assertEqual(self.bindings[name], address)
            self.assertEqual(self.bindings[f"func_spanish_{address:X}"], address)
        with (ROOT / "notes/overlays/spanish-model-variant441-ribbon-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 14)
        self.assertEqual([int(r["different_words"]) for r in attempts], [357, 10] + [0]*12)
        self.assertEqual([int(r["instruction_bytes"]) for r in attempts], [1564] + [1568]*13)
        self.assertEqual([r["slot"] for r in attempts[:4]], ["0", "0", "0", "1"])
        body = ROOT / "src/overlays/spanish_model_variant/variant441_ribbon.c"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes() +
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        for module in self.modules:
            selected, = [s for s in lines.c_segments(ROOT, ROOT / module["layout"])
                         if "/variant441_ribbon" in s["source"]]
            terminal, = [r for r in attempts[4:] if r["module"] == module["name"]]
            self.assertEqual(terminal["result"], "matched")
            self.assertEqual(terminal["fingerprint"],
                             hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)

    def test_retained_context_projection_packet_and_timing(self):
        decoder = lines.lifetimes.SpanishModelVariant460Tests
        anchors = {
            0x48: 0x26D7256C, 0x58: 0x26D80FD8,
            0x264: 0x0C020BBA, 0x268: 0x02E02021,
            0x2A0: 0x0C020B6A, 0x2AC: 0x0C020B76, 0x2B0: 0x00002821,
            0xD64: 0x24020400, 0xD6C: 0xA6C22716, 0xD8C: 0xA6C02714,
            0x1DDC: 0x27BDFF00, 0x1DE4: 0x0080A021, 0x1E18: 0x26950FD8,
            0x1E24: 0x2692256C, 0x1E30: 0x18600166, 0x1E4C: 0x0002B943,
            0x2094: 0x26070030, 0x209C: 0x0C021E26, 0x20C0: 0xAE020064,
            0x210C: 0x86020032, 0x2194: 0x18400005, 0x219C: 0x96060064,
            0x21B8: 0x86020042, 0x2240: 0x18400006, 0x2248: 0x96060064,
            0x2288: 0x94A30018, 0x228C: 0x24420001, 0x2290: 0x1443004E,
            0x22B0: 0x28420401, 0x2310: 0xA6832714, 0x231C: 0x28420400,
            0x2328: 0xA6822714, 0x2330: 0xAE82271C, 0x23A0: 0xA6822716,
            0x23C0: 0xA6802716, 0x23C8: 0xAE82271C,
        }
        expected_stores = ([(o, 2) for o in (8, 10, 20, 22, 32, 34, 44)] +
                           [(o, 1) for o in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)] +
                           [(46, 2)]) * 2
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            for offset in range(0, len(image), 4):
                word, = struct.unpack_from("<I", image, offset)
                self.assertNotEqual(word, base + 0x1DDC)
                if word >> 26 in (2, 3):
                    destination = ((base+offset+4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    self.assertNotEqual(destination, base + 0x1DDC)
            self.assertEqual(decoder.register_writes(image, 0x1DDC, 0x23FC, 20), [0x1DE4, 0x23E0])
            self.assertEqual(decoder.register_writes(image, 0x1DDC, 0x23FC, 18), [0x1E24, 0x23E8])
            self.assertEqual(decoder.register_writes(image, 4, 0x2B4, 23), [0x48])
            self.assertEqual([(o, s) for _, o, s in decoder.direct_stores(image, 0x1DDC, 0x23FC, 18)],
                             expected_stores)
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertFalse(any(w >> 26 == 35 and (w >> 21 & 31) == 29 and (w & 65535) == 0xCC
                                 for w, in struct.iter_unpack("<I", image[0x1DDC:0x23FC])))
            row = self.instances[module["name"]]
            descriptor = 0x4850 + int(row["command_word"]) % 1000 * 48
            self.assertEqual(struct.unpack_from("<H", image, descriptor + 0x18), (1,))
            start, end, fade_start, fade_end = struct.unpack_from("<iiii", image, descriptor + 0x20)
            self.assertLess(start, end)
            self.assertLess(fade_start, fade_end)
        self.assertEqual(0xFD8 + 0x6C, 0x1044)
        self.assertEqual(0x256C + 52, 0x25A0)

    def test_target_layout_and_coexisting_headers(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model441-ribbon-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' + "".join(
            f'#include "../../src/overlays/spanish_model_variant/variant441_{role}.h"\n' * 2
            for role in ("lines", "tube", "framebuffer_rings", "ribbon")) +
            '#define O(t,f) ((u32)&((t *)0)->f)\nu32 layout[] = {' +
            ",".join(e for e, _ in LAYOUT) + "};\n")
        obj = compile_c(ROOT, tool(ROOT, "as"),
                        dict(source=str(source.relative_to(ROOT)), profile="gcc_2_8_1_g0_split",
                             object="layout.o"), load_compiler_profiles(ROOT),
                        object_directory=str(directory.relative_to(ROOT)),
                        asm_directory=str(directory.relative_to(ROOT)))
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
            data = elf.get_section(symbol["st_shndx"]).data()
            expected = struct.pack("<" + "I"*len(LAYOUT), *(v for _, v in LAYOUT))
            self.assertEqual(symbol["st_value"], 0)
            self.assertEqual(data[:len(expected)], expected)
            self.assertFalse(any(data[len(expected):]))

    def test_selected_compiler_owner_and_every_relocation_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / "tmp/overlays" / module["name"]
            build = directory / "build"
            linked_path = build / (module["name"] + ".elf")
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL441 images before checking owners")
            self.assertEqual((build / (module["name"] + ".bin")).read_bytes(), image)
            selected, = [s for s in lines.c_segments(ROOT, ROOT / module["layout"])
                         if "/variant441_ribbon" in s["source"]]
            obj = build / selected["object"]
            self.assertIn(f"{obj.relative_to(ROOT)}(.text);", (directory / (module["name"] + ".ld")).read_text())
            with obj.open("rb") as handle, linked_path.open("rb") as final_handle:
                compiled, linked = ELFFile(handle), ELFFile(final_handle)
                symbols, final_symbols = compiled.get_section_by_name(".symtab"), linked.get_section_by_name(".symtab")
                own, = symbols.get_symbol_by_name(f"func_{base+0x1DDC:X}")
                final, = final_symbols.get_symbol_by_name(own.name)
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]), (0, 1568, "STT_FUNC"))
                self.assertEqual((final["st_value"], final["st_size"], final["st_info"]["type"]),
                                 (base+0x1DDC, 1568, "STT_FUNC"))
                section = compiled.get_section(own["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                text = bytearray(section.data()[:1568])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1568)
                    symbol = symbols.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", text, offset)
                    addend = (word & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        self.assertEqual(word >> 26, 3)
                        resolved, = final_symbols.get_symbol_by_name(symbol.name)
                        self.assertEqual(resolved["st_value"], self.bindings[symbol.name])
                        address = resolved["st_value"] + addend
                        calls.append((offset, address))
                    else:
                        self.assertEqual(word >> 26, 2)
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        address = base + 0x1DDC + symbol["st_value"] + addend
                        jumps.append((offset, address-base-0x1DDC))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | (address >> 2 & 0x3FFFFFF))
                self.assertEqual(calls, list(CALLS))
                self.assertEqual(jumps, [(0x138, 0x1D0)])
                self.assertEqual(bytes(text), image[0x1DDC:0x23FC])
