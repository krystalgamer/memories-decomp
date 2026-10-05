import csv
import hashlib
import importlib.util
import struct
import unittest

from tools.project.tests import test_spanish_model_variant385 as family
from tools.project.tests import test_spanish_model_variant460 as lifetimes

ROOT = family.ROOT
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52), ("sizeof(PSXLONG)", 4),
    ("sizeof(Strip385)", 0x8C), ("O(Strip385,center)", 0x18),
    ("O(Strip385,lower)", 0x30), ("O(Strip385,projected)", 0x48),
    ("O(Strip385,projected[1])", 0x54), ("O(Strip385,projected[2])", 0x60),
    ("O(Strip385,flags)", 0x74), ("O(Strip385,depth)", 0x80),
    ("sizeof(Strip385Timing)", 0x40), ("O(Strip385Timing,iterations)", 0xC),
    ("O(Strip385Timing,width)", 0x10), ("O(Strip385Timing,start)", 0x30),
    ("O(Strip385Timing,expanded)", 0x34), ("O(Strip385Timing,fade)", 0x38),
    ("O(Strip385Timing,end)", 0x3C), ("O(Strip385State,strips)", 0x4E0),
    ("O(Strip385State,polygon)", 0xE2C), ("O(Strip385State,origin)", 0xFAC),
    ("O(Strip385State,direction)", 0xFC0), ("O(Strip385State,projected)", 0xFD0),
    ("O(Strip385State,frame)", 0xFEC), ("O(Strip385State,time)", 0xFF0),
    ("O(Strip385State,timing)", 0x1000), ("O(Strip385State,iteration)", 0x1014),
    ("O(Strip385State,radius)", 0x1028), ("O(Strip385State,progress)", 0x102A),
    ("O(Strip385State,phase)", 0x1030), ("sizeof(Strip385State)", 0x1034),
    ("O(POLY_GT4,x0)", 8), ("O(POLY_GT4,x1)", 20), ("O(POLY_GT4,x2)", 32),
    ("O(POLY_GT4,x3)", 44), ("O(POLY_GT4,r0)", 4), ("O(POLY_GT4,r1)", 16),
    ("O(POLY_GT4,r2)", 28), ("O(POLY_GT4,r3)", 40),
)

DESCRIPTORS = {170: (32, (58, 64, 100, 180)), 406: (48, (68, 80, 330, 370)),
               407: (32, (56, 64, 160, 180)), 513: (32, (48, 56, 280, 320))}

ANCHORS = {
    0x24: 0x27D70E2C, 0x2E8: 0x0C020BBA, 0x2EC: 0x02E02021,
    0x2F0: 0x02E02021, 0x2F4: 0x24050001, 0x324: 0x0C020B6A,
    0x32C: 0x02E02021, 0x330: 0x0C020B76, 0x334: 0x00002821,
    0x9B4: 0xA7C21028, 0x9D4: 0xA7C0102A,
    0xD78: 0x8FC21030, 0xD80: 0x18400005, 0xD94: 0x02402021,
    0xD98: 0x8FC21014, 0xDA0: 0x24420001, 0xDA4: 0xAFC21014,
    0xDA8: 0x9463000C, 0xDB0: 0x0043102A,
    0x14BC: 0x26B10E2C, 0x14C8: 0x18600181,
    0x17CC: 0x28630003, 0x17D4: 0xAE020080, 0x17F0: 0x2694008C,
    0x18B0: 0x04410003, 0x18B8: 0xAE000080, 0x18C4: 0x04400005,
    0x18C8: 0xAE000074, 0x18CC: 0x96460080,
    0x1974: 0x04410002, 0x197C: 0xAC800080, 0x1988: 0x04400005,
    0x198C: 0xAE000074, 0x1990: 0x96460080,
    0x19B0: 0x28420002, 0x19D4: 0x2694008C,
}

class SpanishModelVariant385StripTests(unittest.TestCase):
    def setUp(self):
        self.family = family.SpanishModelVariant385Tests()
        self.family.setUp()

    def test_attempt_fingerprints_and_existing_address_aliases(self):
        with (ROOT / "notes/overlays/spanish-model-variant385-strip-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([int(row["different_words"]) for row in rows], [277, 138, 138] + [0]*10)
        self.assertEqual([row["function_offset"] for row in rows], ["0x1478"]*13)
        self.assertEqual([row["profile"] for row in rows], ["gcc_2_8_1_g0_split"]*13)
        body = ROOT / "src/overlays/spanish_model_variant/variant385_strip.c"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes() +
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        for module in self.family.modules:
            slot = (int(module["load_address"], 0)-0x8013B000)//0x40000
            row, = [row for row in rows[5:] if row["module"] == module["name"]]
            source = body.with_name("variant385_strip_slot1.c") if slot else body
            self.assertEqual((row["result"], row["instruction_bytes"], row["dependency_fingerprint"]),
                             ("matched", "1672", dependency))
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
        for name, address in dict(GsSortPoly=0x800842A8, ReadRotMatrix=0x800872A8,
                                  SetRotMatrix=0x80087738, RotTransPers3=0x80087898,
                                  rcos=0x800866F8, rsin=0x80086628).items():
            self.assertEqual(self.family.bindings[name], address)
            self.assertEqual(self.family.bindings[f"func_spanish_{address:X}"], address)

    def test_target_layout_and_header_coexistence(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model385-strip-layout-test"
        directory.mkdir(exist_ok=True)
        for order in (("lines", "strip"), ("strip", "lines")):
            source = directory / ("layout_" + order[0] + ".c")
            source.write_text('#include "../../src/types.h"\n' + "".join(
                f'#include "../../src/overlays/spanish_model_variant/variant385_{name}.h"\n'
                for name in order*2) + '#define O(t,f) ((u32)&((t *)0)->f)\n'
                'u32 layout[] = {' + ",".join(expression for expression, _ in LAYOUT) + "};\n")
            obj = compile_c(ROOT, tool(ROOT, "as"),
                            dict(source=str(source.relative_to(ROOT)), profile="gcc_2_8_1_g0_split",
                                 object=source.with_suffix(".o").name), load_compiler_profiles(ROOT),
                            object_directory=str(directory.relative_to(ROOT)),
                            asm_directory=str(directory.relative_to(ROOT)))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
                self.assertEqual(symbol["st_value"], 0)
                data = elf.get_section(symbol["st_shndx"]).data()
                self.assertEqual(data[:4*len(LAYOUT)],
                                 struct.pack("<" + "I"*len(LAYOUT), *(value for _, value in LAYOUT)))
                self.assertEqual(data[4*len(LAYOUT):], bytes(len(data)-4*len(LAYOUT)))

    def test_original_context_initializer_descriptor_and_packet_bounds(self):
        decoder = lifetimes.SpanishModelVariant460Tests
        for module, image in self.family.legal_images():
            base = int(module["load_address"], 0)
            self.assertEqual(family.reaching_context(image, base, 0xD90), {0xC})
            self.assertEqual(struct.unpack_from("<II", image, 0xD90),
                             (0x0C000000 | ((base+0x1478) >> 2 & 0x3FFFFFF), 0x02402021))
            for offset, word in ANCHORS.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(decoder.register_writes(image, 0x24, 0x338, 23), [0x24])
            self.assertEqual(decoder.register_writes(image, 0x1478, 0x1B00, 21), [0x1480, 0x1AE0])
            self.assertEqual(decoder.register_writes(image, 0x1478, 0x1B00, 17), [0x14BC, 0x1AF0])
            stores = decoder.direct_stores(image, 0x1478, 0x1B00, 17)
            offsets = [(8, 2), (10, 2), (20, 2), (22, 2), (32, 2), (34, 2), (44, 2)]
            offsets += [(s+c, 1) for s in (4, 16, 28, 40) for c in range(3)]
            offsets += [(46, 2)]
            self.assertEqual([(o, s) for _, o, s in stores], offsets*2)
            self.assertTrue(all(0 <= o < o+s <= 52 for _, o, s in stores))
            row = self.family.instances[module["name"]]
            descriptor = 0x2FB0 + int(row["command_word"]) % 1000 * 76
            width, times = DESCRIPTORS[int(row["model"])]
            self.assertEqual(struct.unpack_from("<H", image, descriptor+12), (1,))
            self.assertEqual(struct.unpack_from("<i", image, descriptor+16), (width,))
            self.assertEqual(struct.unpack_from("<4I", image, descriptor+48), times)
            self.assertGreater(times[1], times[0])
            self.assertGreater(times[3], times[2])
        self.assertEqual(0x4E0+0x8C, 0x56C)
        self.assertEqual(0x74+3*4, 0x80)
        self.assertEqual(0x80+3*4, 0x8C)

    def test_selected_c_owner_and_every_relocation_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        calls_expected = [(44, 'func_80058F10'), (60, 'ratan2'), (232, 'rcos'), (272, 'rsin'), (316, 'rcos'), (340, 'rsin'), (612, 'RotMatrix'), (696, 'GsGetLs'), (704, 'GsSetLsMatrix'), (712, 'ReadRotMatrix'), (724, 'RotMatrix'), (736, 'ScaleMatrix'), (744, 'SetRotMatrix'), (824, 'RotTransPers3'), (1116, 'GsSortPoly'), (1312, 'GsSortPoly')]
        for module, image in self.family.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / "tmp/overlays" / module["name"]
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL385 images before checking owners")
            selected, = [segment for segment in family.c_segments(ROOT, ROOT / module["layout"])
                         if "/variant385_strip" in segment["source"]]
            path = directory / "build" / selected["object"]
            self.assertIn(f"{path.relative_to(ROOT)}(.text);",
                          (directory / f"{module['name']}.ld").read_text())
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            with path.open("rb") as handle, linked_path.open("rb") as final_handle:
                compiled, linked = ELFFile(handle), ELFFile(final_handle)
                original, final = compiled.get_section_by_name(".symtab"), linked.get_section_by_name(".symtab")
                function, = original.get_symbol_by_name(f"func_{base+0x1478:X}")
                symbol, = final.get_symbol_by_name(function.name)
                self.assertEqual((function["st_value"], function["st_size"], function["st_info"]["type"]),
                                 (0, 1672, "STT_FUNC"))
                self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                 (base+0x1478, 1672, "STT_FUNC"))
                section = linked.get_section(symbol["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                offset = symbol["st_value"]-section["sh_addr"]
                self.assertEqual(section.data()[offset:offset+1672], image[0x1478:0x1B00])
                section = compiled.get_section(function["st_shndx"])
                self.assertEqual(section.name, ".text")
                self.assertTrue(section["sh_flags"] & 4)
                text = bytearray(section.data()[:1672])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1672)
                    target = original.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", text, offset)
                    addend = (word & 0x3FFFFFF) << 2
                    if target["st_shndx"] == "SHN_UNDEF":
                        self.assertEqual(word >> 26, 3)
                        resolved, = final.get_symbol_by_name(target.name)
                        self.assertEqual(resolved["st_value"], self.family.bindings[target.name])
                        address = resolved["st_value"]+addend
                        calls.append((offset, target.name))
                    else:
                        self.assertEqual(word >> 26, 2)
                        self.assertEqual(target["st_shndx"], function["st_shndx"])
                        address = base+0x1478+target["st_value"]+addend
                        jumps.append((offset, address-base-0x1478))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(calls, calls_expected)
                self.assertEqual(jumps, [(144, 208), (1516, 1624)])
                self.assertEqual(bytes(text), image[0x1478:0x1B00])
