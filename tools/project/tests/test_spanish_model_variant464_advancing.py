import csv
import hashlib
import importlib.util
import re
import struct
import unittest

from tools.project.tests import test_spanish_model_variant464 as family

ROOT = family.ROOT
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52),
    ("sizeof(Advancing464)", 0x84), ("O(Advancing464,points[2])", 0x10),
    ("O(Advancing464,points[4])", 0x20), ("O(Advancing464,projected)", 0x30),
    ("O(Advancing464,projected[1])", 0x38), ("O(Advancing464,projected[2])", 0x40),
    ("O(Advancing464,inner)", 0x48), ("O(Advancing464,outer)", 0x50),
    ("O(Advancing464,scale)", 0x58), ("O(Advancing464,rotation)", 0x68),
    ("O(Advancing464,depth)", 0x70), ("O(Advancing464,progress)", 0x78),
    ("O(Advancing464,active)", 0x80), ("sizeof(Advancing464Timing)", 0x48),
    ("O(Advancing464Timing,count)", 0x44),
    ("O(Advancing464State,groups)", 0x4E0), ("O(Advancing464State,quad)", 0x1A80),
    ("O(Advancing464State,origin)", 0x1BD8), ("O(Advancing464State,target)", 0x1BE4),
    ("O(Advancing464State,direction)", 0x1BEC), ("O(Advancing464State,projected)", 0x1BFC),
    ("O(Advancing464State,frame)", 0x1C18), ("O(Advancing464State,time)", 0x1C1C),
    ("O(Advancing464State,step)", 0x1C24), ("O(Advancing464State,timing)", 0x1C2C),
    ("O(Advancing464State,width)", 0x1C48), ("O(Advancing464State,phase)", 0x1C60),
    ("sizeof(Advancing464State)", 0x1C64), ("O(POLY_GT4,x0)", 8),
    ("O(POLY_GT4,x1)", 20), ("O(POLY_GT4,x2)", 32), ("O(POLY_GT4,x3)", 44),
)
CALLS = (
    (0x2C, 0x8005C018), (0x3C, 0x80089928), (0xF4, 0x800866F8),
    (0x120, 0x80086628), (0x154, 0x800866F8), (0x174, 0x80086628),
    (0x244, 0x80087CB8), (0x298, 0x80086258), (0x2A0, 0x80085558),
    (0x2A8, 0x800872A8), (0x2B4, 0x80087CB8), (0x2C0, 0x800875F8),
    (0x2C8, 0x80087738), (0x328, 0x80087898), (0x530, 0x800842A8),
)


class SpanishModelVariant464AdvancingTests(unittest.TestCase):
    setUp = family.SpanishModelVariant464Tests.setUp
    legal_images = family.SpanishModelVariant464Tests.legal_images

    def test_terminal_fingerprints_and_existing_resident_aliases(self):
        with (ROOT / "notes/overlays/spanish-model-variant464-advancing-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual([int(r["different_words"]) for r in attempts], [272, 334, 328, 335, 1] + [0]*8)
        self.assertEqual([int(r["instruction_bytes"]) for r in attempts],
                         [1456, 1484, 1456, 1444] + [1448]*9)
        self.assertEqual([r["slot"] for r in attempts[:7]], ["0"]*6 + ["1"])
        self.assertEqual({r["profile"] for r in attempts}, {"gcc_2_8_1_g0_split"})
        self.assertEqual({r["function_offset"] for r in attempts}, {"0x18EC"})
        bindings = dict((n, int(a, 0)) for n, a in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config / "overlays/model_variant464_linker_symbols.txt").read_text(), re.M))
        self.assertEqual((len(bindings), len(set(bindings.values()))), (40, 36))
        for name in ("rcos", "rsin", "ratan2", "RotTransPers3"):
            self.assertEqual(bindings[name], bindings[f"func_spanish_{bindings[name]:X}"])
        body = ROOT / "src/overlays/spanish_model_variant/variant464_advancing.c"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes() +
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        for module in self.modules:
            selected, = [s for s in family.c_segments(ROOT, ROOT / module["layout"])
                         if "/variant464_advancing" in s["source"]]
            terminal, = [r for r in attempts[7:] if r["module"] == module["name"]]
            self.assertEqual(terminal["result"], "matched")
            self.assertEqual(terminal["fingerprint"],
                             hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)

    def test_original_packet_groups_and_progress_bounds(self):
        decoder = family.SpanishModelVariant464Tests
        anchors = {
            0x4C: 0x27D71A80, 0x26C: 0x0C020BBA, 0x270: 0x02E02021,
            0x278: 0x24050001, 0x2BC: 0x0C020B6A, 0x2C8: 0x0C020B76,
            0x2CC: 0x00002821, 0x450: 0x27240080, 0x460: 0xAC88FFD8,
            0x464: 0xAC88FFDC, 0x468: 0xAC88FFE0, 0x47C: 0xA0620048,
            0x4B8: 0xA0620050, 0x4E0: 0xAC650078, 0x4E4: 0x24A5FE00,
            0x4EC: 0x2A620002, 0x4F8: 0x24E7F800, 0x500: 0xA480FFE8,
            0x508: 0xA480FFEA, 0x50C: 0xAC800000, 0x518: 0x28C20005,
            0x18EC: 0x27BDFEC8, 0x18F4: 0x0080A021, 0x1930: 0x26911A80,
            0x1994: 0x269304E0, 0x19B0: 0x27A800C8, 0x19D4: 0x06410002,
            0x19DC: 0x00009021, 0x1C20: 0xAEC20070, 0x1C24: 0x28620400,
            0x1C38: 0x00021180, 0x1C44: 0x28420400, 0x1C58: 0xAEC20078,
            0x1C5C: 0xAE630080, 0x1C68: 0x14430004, 0x1C74: 0xAE821C60,
            0x1C88: 0x28420002, 0x1CBC: 0x26730084,
            0x1CCC: 0x269304E0, 0x1DF4: 0x0440000C,
            0x1E04: 0x8C4200C8, 0x1E0C: 0x04400006, 0x1E14: 0x94A60070,
            0x1E30: 0x1840FFAE, 0x1E60: 0x26730084,
        }
        for module, image in self.legal_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(decoder.register_writes(image, 4, 0x2D0, 23), [0x4C])
            self.assertEqual(decoder.register_writes(image, 0x18EC, 0x1E94, 20), [0x18F4, 0x1E78])
            self.assertEqual(decoder.register_writes(image, 0x18EC, 0x1E94, 17), [0x1930, 0x1E84])
            self.assertEqual([(o, s) for _, o, s in decoder.direct_stores(image, 0x18EC, 0x1E94, 17)],
                             [(o, 2) for o in (8, 10, 20, 22, 32, 34, 44, 46)] +
                             [(o, 1) for o in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
            row = self.instances[module["name"]]
            descriptor = 0x2EA8 + int(row["command_word"]) % 1000 * 72
            count, = struct.unpack_from("<i", image, descriptor+0x44)
            self.assertEqual(count, int(row["primary_count"]))
            self.assertTrue(0 < count <= 5)
            self.assertLessEqual(0x4E0 + count*0x84, 0x774)
            self.assertLessEqual(0xC8 + count*2*4, 0xF0)
        self.assertEqual(0x1A80+52, 0x1AB4)

    def test_target_layout_and_coexisting_headers(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model464-advancing-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' + "".join(
            f'#include "../../src/overlays/spanish_model_variant/variant464_{role}.h"\n' * 2
            for role in ("sheets", "advancing")) +
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

    def test_selected_compiler_owner_and_all_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / "tmp/overlays" / module["name"]
            build = directory / "build"
            linked_path = build / (module["name"] + ".elf")
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL464 images before checking owners")
            self.assertEqual((build / (module["name"] + ".bin")).read_bytes(), image)
            selected, = [s for s in family.c_segments(ROOT, ROOT / module["layout"])
                         if "/variant464_advancing" in s["source"]]
            obj = build / selected["object"]
            self.assertIn(f"{obj.relative_to(ROOT)}(.text);", (directory / (module["name"] + ".ld")).read_text())
            bindings = dict((n, int(a, 0)) for n, a in re.findall(
                r"^(\w+) = (0x[0-9A-F]+);", (ROOT / module["linker_symbols"]).read_text(), re.M))
            with obj.open("rb") as handle, linked_path.open("rb") as final_handle:
                compiled, linked = ELFFile(handle), ELFFile(final_handle)
                symbols, final_symbols = compiled.get_section_by_name(".symtab"), linked.get_section_by_name(".symtab")
                own, = symbols.get_symbol_by_name(f"func_{base+0x18EC:X}")
                final, = final_symbols.get_symbol_by_name(own.name)
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]), (0, 1448, "STT_FUNC"))
                self.assertEqual((final["st_value"], final["st_size"], final["st_info"]["type"]),
                                 (base+0x18EC, 1448, "STT_FUNC"))
                section = compiled.get_section(own["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                text = bytearray(section.data()[:1448])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1448)
                    symbol = symbols.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", text, offset)
                    addend = (word & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        self.assertEqual(word >> 26, 3)
                        resolved, = final_symbols.get_symbol_by_name(symbol.name)
                        self.assertEqual(resolved["st_value"], bindings[symbol.name])
                        address = resolved["st_value"] + addend
                        calls.append((offset, address))
                    else:
                        self.assertEqual(word >> 26, 2)
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        address = base + 0x18EC + symbol["st_value"] + addend
                        jumps.append((offset, address-base-0x18EC))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | (address >> 2 & 0x3FFFFFF))
                self.assertEqual(calls, list(CALLS))
                self.assertEqual(jumps, [(0x70, 0x9C)])
                self.assertEqual(bytes(text), image[0x18EC:0x1E94])
