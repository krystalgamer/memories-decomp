import csv
import hashlib
import importlib.util
import struct
import unittest

from tools.project.tests import test_spanish_model_variant446 as family
from tools.project.tests import test_spanish_model_variant460 as lifetimes

ROOT = family.ROOT
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52), ("sizeof(PSXLONG)", 4),
    ("sizeof(Strip446)", 0xD0), ("O(Strip446,center)", 0x28),
    ("O(Strip446,lower)", 0x50), ("O(Strip446,projected)", 0x78),
    ("O(Strip446,projected[1])", 0x8C), ("O(Strip446,projected[2])", 0xA0),
    ("O(Strip446,depth)", 0xBC), ("sizeof(Strip446Timing)", 0x40),
    ("O(Strip446Timing,iterations)", 0xC), ("O(Strip446Timing,width)", 0x10),
    ("O(Strip446Timing,start)", 0x30), ("O(Strip446Timing,expanded)", 0x34),
    ("O(Strip446Timing,fade)", 0x38), ("O(Strip446Timing,end)", 0x3C),
    ("O(Strip446State,strips)", 0x1060), ("O(Strip446State,polygon)", 0x19B0),
    ("O(Strip446State,origin)", 0x1B30), ("O(Strip446State,direction)", 0x1B64),
    ("O(Strip446State,projected)", 0x1B74), ("O(Strip446State,frame)", 0x1B90),
    ("O(Strip446State,time)", 0x1B94), ("O(Strip446State,timing)", 0x1BA4),
    ("O(Strip446State,iteration)", 0x1BB8), ("O(Strip446State,radius)", 0x1BCC),
    ("O(Strip446State,progress)", 0x1BCE), ("O(Strip446State,phase)", 0x1BDC),
    ("sizeof(Strip446State)", 0x1BE0), ("O(POLY_GT4,x0)", 8),
    ("O(POLY_GT4,x1)", 20), ("O(POLY_GT4,x2)", 32), ("O(POLY_GT4,x3)", 44),
    ("O(POLY_GT4,r0)", 4), ("O(POLY_GT4,r1)", 16), ("O(POLY_GT4,r2)", 28),
    ("O(POLY_GT4,r3)", 40),
)


class SpanishModelVariant446StripTests(unittest.TestCase):
    def setUp(self):
        self.family = family.SpanishModelVariant446Tests()
        self.family.setUp()

    def test_attempt_fingerprints_and_existing_address_aliases(self):
        with (ROOT / "notes/overlays/spanish-model-variant446-strip-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([int(row["different_words"]) for row in rows], [383, 332, 332, 10, 2, 0, 0, 0, 0])
        self.assertEqual([row["function_offset"] for row in rows], ["0x12F8"]*9)
        self.assertEqual([row["profile"] for row in rows], ["gcc_2_8_1_g0_split"]*9)
        body = ROOT / "src/overlays/spanish_model_variant/variant446_strip.c"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes() +
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        for slot, module in enumerate(self.family.modules):
            row, = [row for row in rows[7:] if row["module"] == module["name"]]
            source = body.with_name("variant446_strip_slot1.c") if slot else body
            self.assertEqual((row["result"], row["instruction_bytes"], row["dependency_fingerprint"]),
                             ("matched", "1636", dependency))
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

        directory = ROOT / "tmp/model446-strip-layout-test"
        directory.mkdir(exist_ok=True)
        for order in (("lines", "strip"), ("strip", "lines")):
            source = directory / ("layout_" + order[0] + ".c")
            source.write_text('#include "../../src/types.h"\n' + "".join(
                f'#include "../../src/overlays/spanish_model_variant/variant446_{name}.h"\n'
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
        anchors = {
            0x2C: 0x27D719B0, 0x3C: 0x27D81060, 0x50: 0xAFB80088,
            0x25C: 0x0C020BBA, 0x260: 0x02E02021, 0x264: 0x02E02021,
            0x268: 0x24050001, 0x274: 0xA6F2001A, 0x298: 0x0C020B6A,
            0x29C: 0xA6F3000E, 0x2A0: 0x02E02021, 0x2A4: 0x0C020B76, 0x2A8: 0x00002821,
            0x9B8: 0x24020400, 0x9BC: 0xA7C21BCC, 0x9DC: 0xA7C01BCE,
            0xBA4: 0x9462000C, 0xBAC: 0x10400042,
            0xC74: 0x8FC21BDC, 0xC7C: 0x18400005,
            0xC9C: 0x24420001, 0xCA0: 0xAFC21BB8, 0xCA4: 0x9463000C, 0xCAC: 0x0043102A,
            0x133C: 0x269119B0, 0x1348: 0x18600178, 0x13C8: 0x26951060,
            0x1654: 0x28630005, 0x165C: 0xAE0200BC,
            0x1674: 0x1840FF59, 0x1678: 0x26B500D0,
            0x1738: 0x18400005, 0x1740: 0x960600BC,
            0x17E4: 0x18400006, 0x17EC: 0x960600BC,
            0x180C: 0x28420004, 0x182C: 0x1840FF97, 0x1830: 0x26B500D0,
        }
        for module, image in self.family.legal_images():
            base = int(module["load_address"], 0)
            self.assertEqual(family.reaching_context(image, base, 0xC8C), {0xC})
            self.assertEqual(struct.unpack_from("<II", image, 0xC8C),
                             (0x0C000000 | ((base+0x12F8) >> 2 & 0x3FFFFFF), 0x02402021))
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(decoder.register_writes(image, 0x2C, 0x2AC, 23), [0x2C])
            self.assertEqual(decoder.register_writes(image, 0x12F8, 0x195C, 20), [0x1300, 0x1940])
            self.assertEqual(decoder.register_writes(image, 0x12F8, 0x195C, 17), [0x133C, 0x194C])
            stores = decoder.direct_stores(image, 0x12F8, 0x195C, 17)
            offsets = [(8, 2), (10, 2), (20, 2), (22, 2), (32, 2), (34, 2), (44, 2)]
            offsets += [(start+channel, 1) for start in (4, 16, 28, 40) for channel in range(3)]
            offsets += [(46, 2)]
            self.assertEqual([(offset, size) for _, offset, size in stores], offsets*2)
            self.assertTrue(all(0 <= offset < offset+size <= 52 for _, offset, size in stores))
            self.assertEqual(struct.unpack_from("<H", image, 0x37D0+0xC), (1,))
            self.assertEqual(struct.unpack_from("<i", image, 0x37D0+0x10), (32,))
            self.assertEqual(struct.unpack_from("<4I", image, 0x37D0+0x30), (184, 192, 340, 400))
        self.assertEqual(0x1060+0xD0, 0x1130)
        self.assertEqual(0xBC+5*4, 0xD0)

    def test_selected_c_owner_and_every_relocation_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        calls_expected = [(0x2C, "func_80058F10"), (0x3C, "ratan2"), (0xE8, "rcos"),
                          (0x110, "rsin"), (0x13C, "rcos"), (0x154, "rsin"),
                          (0x270, "RotMatrix"), (0x2C4, "GsGetLs"), (0x2CC, "GsSetLsMatrix"),
                          (0x2D4, "ReadRotMatrix"), (0x2E0, "RotMatrix"), (0x2EC, "ScaleMatrix"),
                          (0x2F4, "SetRotMatrix"), (0x340, "RotTransPers3"),
                          (0x450, "GsSortPoly"), (0x4FC, "GsSortPoly")]
        for module, image in self.family.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / "tmp/overlays" / module["name"]
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL446 images before checking owners")
            selected, = [segment for segment in family.c_segments(ROOT, ROOT / module["layout"])
                         if "/variant446_strip" in segment["source"]]
            path = directory / "build" / selected["object"]
            self.assertIn(f"{path.relative_to(ROOT)}(.text);",
                          (directory / f"{module['name']}.ld").read_text())
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            with path.open("rb") as handle, linked_path.open("rb") as final_handle:
                compiled, linked = ELFFile(handle), ELFFile(final_handle)
                original, final = compiled.get_section_by_name(".symtab"), linked.get_section_by_name(".symtab")
                function, = original.get_symbol_by_name(f"func_{base+0x12F8:X}")
                symbol, = final.get_symbol_by_name(function.name)
                self.assertEqual((function["st_value"], function["st_size"], function["st_info"]["type"]),
                                 (0, 1636, "STT_FUNC"))
                self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                 (base+0x12F8, 1636, "STT_FUNC"))
                section = linked.get_section(symbol["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                offset = symbol["st_value"]-section["sh_addr"]
                self.assertEqual(section.data()[offset:offset+1636], image[0x12F8:0x195C])
                section = compiled.get_section(function["st_shndx"])
                self.assertEqual(section.name, ".text")
                self.assertTrue(section["sh_flags"] & 4)
                text = bytearray(section.data()[:1636])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1636)
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
                        address = base+0x12F8+target["st_value"]+addend
                        jumps.append((offset, address-base-0x12F8))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(calls, calls_expected)
                self.assertEqual(jumps, [(0x90, 0xD0), (0x5C8, 0x634)])
                self.assertEqual(bytes(text), image[0x12F8:0x195C])
