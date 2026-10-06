import csv
import hashlib
import importlib.util
import struct
import unittest

from tools.project.tests import test_spanish_model_variant385 as lines

ROOT = lines.ROOT
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52),
    ("sizeof(Pulse385)", 0x90), ("sizeof(((Pulse385 *)0)->points)", 0x80),
    ("O(Pulse385,points[1])", 0x20), ("O(Pulse385,points[2])", 0x40),
    ("O(Pulse385,points[3])", 0x60), ("O(Pulse385,size)", 0x80),
    ("sizeof(Pulse385Timing)", 0x40), ("O(Pulse385Timing,count)", 0xC),
    ("O(Pulse385Timing,start)", 0x2C), ("O(Pulse385Timing,end)", 0x30),
    ("O(Pulse385Timing,fade_start)", 0x38), ("O(Pulse385Timing,fade_end)", 0x3C),
    ("O(Pulse385State,groups)", 0x56C), ("O(Pulse385State,quad)", 0xE60),
    ("O(Pulse385State,origin)", 0xFAC), ("O(Pulse385State,direction)", 0xFC0),
    ("O(Pulse385State,frame)", 0xFEC), ("O(Pulse385State,time)", 0xFF0),
    ("O(Pulse385State,step)", 0xFF8), ("O(Pulse385State,timing)", 0x1000),
    ("O(Pulse385State,index)", 0x1014), ("O(Pulse385State,progress)", 0x102A),
    ("O(Pulse385State,phase)", 0x1030), ("sizeof(Pulse385State)", 0x1034),
    ("O(POLY_GT4,x0)", 8), ("O(POLY_GT4,x1)", 20),
    ("O(POLY_GT4,x2)", 32), ("O(POLY_GT4,x3)", 44),
    ("O(POLY_GT4,r0)", 4), ("O(POLY_GT4,r1)", 16),
    ("O(POLY_GT4,r2)", 28), ("O(POLY_GT4,r3)", 40),
)
CALLS = (
    (0x30, 0x8005C018), (0x174, 0x80087CB8), (0x1C8, 0x80086258),
    (0x1D4, 0x80085558), (0x1E0, 0x800872A8), (0x1EC, 0x80087CB8),
    (0x1F8, 0x800875F8), (0x204, 0x80087738), (0x248, 0x80087958),
    (0x2D0, 0x800842A8),
)
JUMPS = ((0xAC, 0x144), (0x384, 0x4AC), (0x3F8, 0x4B0),
         (0x410, 0x4B0), (0x424, 0x4B0), (0x46C, 0x4AC))


class SpanishModelVariant385PulseTests(unittest.TestCase):
    setUp = lines.SpanishModelVariant385Tests.setUp
    legal_images = lines.SpanishModelVariant385Tests.legal_images

    def test_terminal_fingerprints_and_unchanged_bindings(self):
        self.assertEqual(len(self.modules), 8)
        self.assertEqual((len(self.bindings), len(set(self.bindings.values()))), (42, 36))
        with (ROOT / "notes/overlays/spanish-model-variant385-pulse-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 11)
        self.assertEqual([int(r["different_words"]) for r in attempts], [12] + [0]*10)
        self.assertEqual({r["instruction_bytes"] for r in attempts}, {"1276"})
        self.assertEqual([r["slot"] for r in attempts[:3]], ["0", "0", "1"])
        body = ROOT / "src/overlays/spanish_model_variant/variant385_pulse.c"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes() +
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        for module in self.modules:
            selected, = [s for s in lines.c_segments(ROOT, ROOT / module["layout"])
                         if "/variant385_pulse" in s["source"]]
            terminal, = [r for r in attempts[3:] if r["module"] == module["name"]]
            self.assertEqual(terminal["result"], "matched")
            self.assertEqual(terminal["fingerprint"],
                             hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)

    def test_original_context_initialization_and_unsigned_timing(self):
        decoder = lines.lifetimes.SpanishModelVariant460Tests
        anchors = {
            0x18: 0x27D8056C, 0x24: 0x27D70E2C, 0x338: 0x26F70034,
            0x33C: 0x0C020BBA, 0x340: 0x02E02021, 0x374: 0x0C020B6A,
            0x380: 0x0C020B76, 0x384: 0x00002821,
            0x6EC: 0x27240088, 0x730: 0x8C430028, 0x830: 0x28A20004,
            0x84C: 0xAC80FFF8, 0x858: 0x24840090, 0x860: 0x2B020002,
            0xD58: 0x8C83002C, 0xD64: 0x0043102B, 0xD68: 0x14400003,
            0x21B8: 0x27BDFEF0, 0x21C0: 0x00808021, 0x21C8: 0x261E056C,
            0x21F0: 0x26110E60, 0x21F8: 0x261205EC,
            0x2210: 0x30420001, 0x2318: 0x00461021,
            0x2400: 0x0C021E56, 0x2410: 0x000210C0, 0x2430: 0x00430018,
            0x246C: 0x04600008, 0x247C: 0x04400004, 0x248C: 0x3066FFFF,
            0x24A0: 0x2AE20004, 0x24B4: 0x94A3000C,
            0x24DC: 0x14400019, 0x2510: 0x0043001B,
            0x2550: 0x0064102B, 0x2578: 0x0062001B,
            0x25AC: 0x24020004, 0x2608: 0x00021280,
            0x264C: 0x00021180, 0x265C: 0x24020005,
            0x2668: 0x26520090, 0x2670: 0x27DE0090, 0x2678: 0x29020002,
        }
        timing = {170: (128, 0, 58, 100, 180), 406: (192, 20, 68, 330, 370),
                  407: (128, 0, 56, 160, 180), 513: (128, 0, 48, 280, 320)}
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            self.assertEqual(lines.reaching_context(image, base, 0xD70), {0xC})
            self.assertEqual(struct.unpack_from("<II", image, 0xD70),
                             (0x0C000000 | ((base+0x21B8) >> 2 & 0x3FFFFFF), 0x02402021))
            self.assertEqual(decoder.register_writes(image, 4, 0x388, 23), [0x24, 0x338])
            self.assertEqual(decoder.register_writes(image, 0x21B8, 0x26B4, 16), [0x21C0, 0x26A8])
            self.assertEqual(decoder.register_writes(image, 0x21B8, 0x26B4, 17), [0x21F0, 0x26A4])
            self.assertEqual([(o, s) for _, o, s in decoder.direct_stores(image, 0x21B8, 0x26B4, 17)],
                             [(o, 1) for o in (17, 18, 5, 29, 4, 6, 16, 28, 30, 40, 41, 42)])
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            row = self.instances[module["name"]]
            descriptor = 0x2FB0 + int(row["command_word"]) % 1000 * 76
            self.assertEqual(struct.unpack_from("<H", image, descriptor+0xC), (1,))
            values = struct.unpack_from("<III", image, descriptor+0x28) + \
                struct.unpack_from("<II", image, descriptor+0x38)
            self.assertEqual(values, timing[int(row["model"])])
            self.assertLess(values[1], values[2])
            self.assertLess(values[3], values[4])
        self.assertEqual(0x56C + 2*0x90, 0x68C)
        self.assertEqual(0xE60 + 52, 0xE94)

    def test_target_layout_and_coexisting_headers(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model385-pulse-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' + "".join(
            f'#include "../../src/overlays/spanish_model_variant/variant385_{role}.h"\n' * 2
            for role in ("lines", "strip", "pulse")) +
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
                self.skipTest("Build Spanish MODEL385 images before checking owners")
            self.assertEqual((build / (module["name"] + ".bin")).read_bytes(), image)
            selected, = [s for s in lines.c_segments(ROOT, ROOT / module["layout"])
                         if "/variant385_pulse" in s["source"]]
            obj = build / selected["object"]
            self.assertIn(f"{obj.relative_to(ROOT)}(.text);", (directory / (module["name"] + ".ld")).read_text())
            with obj.open("rb") as handle, linked_path.open("rb") as final_handle:
                compiled, linked = ELFFile(handle), ELFFile(final_handle)
                symbols, final_symbols = compiled.get_section_by_name(".symtab"), linked.get_section_by_name(".symtab")
                own, = symbols.get_symbol_by_name(f"func_{base+0x21B8:X}")
                final, = final_symbols.get_symbol_by_name(own.name)
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]), (0, 1276, "STT_FUNC"))
                self.assertEqual((final["st_value"], final["st_size"], final["st_info"]["type"]),
                                 (base+0x21B8, 1276, "STT_FUNC"))
                section = compiled.get_section(own["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                text = bytearray(section.data()[:1276])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1276)
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
                        address = base + 0x21B8 + symbol["st_value"] + addend
                        jumps.append((offset, address-base-0x21B8))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | (address >> 2 & 0x3FFFFFF))
                self.assertEqual(calls, list(CALLS))
                self.assertEqual(jumps, list(JUMPS))
                self.assertEqual(bytes(text), image[0x21B8:0x26B4])
