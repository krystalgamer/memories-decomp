import csv
import hashlib
import importlib.util
import json
import struct
import unittest

from tools.project.tests import test_spanish_model_variant441 as lines
from tools.project.tests import test_spanish_model_variant441_tube as tube

ROOT = lines.ROOT
BOUNDARIES = lines.BOUNDARIES
c_segments = lines.c_segments

LAYOUT = (
    ("sizeof(SVECTOR)",8), ("sizeof(VECTOR)",16), ("sizeof(MATRIX)",32),
    ("sizeof(GsCOORDINATE2)",80), ("sizeof(POLY_GT4)",52),
    ("sizeof(FramebufferRings441)",0x1A8), ("sizeof(((FramebufferRings441 *)0)->points)",0x198),
    ("O(FramebufferRings441,points[1])",0x88), ("O(FramebufferRings441,points[2])",0x110),
    ("O(FramebufferRings441,progress)",0x1A0), ("O(FramebufferRings441State,groups)",0x14D4),
    ("O(FramebufferRings441State,quad)",0x25D4), ("O(FramebufferRings441State,target)",0x269C),
    ("O(FramebufferRings441State,direction)",0x26A4), ("O(FramebufferRings441State,frame)",0x26D0),
    ("O(FramebufferRings441State,step)",0x26DC), ("O(FramebufferRings441State,phase)",0x271C),
    ("sizeof(FramebufferRings441State)",0x2720),
    ("O(POLY_GT4,x0)",8), ("O(POLY_GT4,x1)",20), ("O(POLY_GT4,x2)",32), ("O(POLY_GT4,x3)",44),
    ("O(POLY_GT4,u0)",12), ("O(POLY_GT4,v0)",13), ("O(POLY_GT4,u1)",24), ("O(POLY_GT4,v1)",25),
    ("O(POLY_GT4,u2)",36), ("O(POLY_GT4,v2)",37), ("O(POLY_GT4,u3)",48), ("O(POLY_GT4,v3)",49),
    ("O(POLY_GT4,r0)",4), ("O(POLY_GT4,r1)",16), ("O(POLY_GT4,r2)",28), ("O(POLY_GT4,r3)",40),
    ("O(POLY_GT4,tpage)",26),
)
ANCHORS = {
    0x70:0x26D825D4,
    0x1040:0x8EC2271C, 0x1048:0x28420002, 0x104C:0x14400003,
    0x3060:0x27BDFEE8, 0x3068:0x00809821, 0x3094:0x240A0001, 0x3098:0xAFAA00E0,
    0x309C:0x0C0214AA, 0x30AC:0x267414D4, 0x30B4:0xAFA200E4,
    0x30E4:0x267125D4, 0x3104:0x269501A0,
    0x326C:0x26460110, 0x3274:0x26470118, 0x32A8:0x0C021E56,
    0x32B0:0x86220008, 0x32B8:0x284200A0,
    0x32E0:0x24060140, 0x32F4:0x3050FFFF, 0x32F8:0x0C020BBA,
    0x3354:0x24060080, 0x3364:0x240601C0, 0x3378:0x3050FFFF, 0x337C:0x0C020BBA,
    0x33E0:0x26460088, 0x33E8:0x26470090, 0x341C:0x0C021E56,
    0x342C:0x0C020B6A, 0x3438:0x0C020B76, 0x343C:0x00002821,
    0x3448:0x30820007, 0x346C:0x000210C3,
    0x3530:0x1A000005, 0x3540:0x0C0210AA, 0x3544:0x3206FFFF, 0x3554:0x2BC20010,
    0x3578:0xAFA000E0, 0x357C:0x00021140, 0x358C:0xAEA30000,
    0x35A8:0xAEA20000, 0x35E0:0xAE62271C, 0x35E4:0x26B501A8, 0x35F4:0x29420005,
}

class SpanishModelVariant441FramebufferRingsTests(unittest.TestCase):
    setUp = lines.SpanishModelVariant441Tests.setUp
    legal_images = lines.SpanishModelVariant441Tests.legal_images

    def test_aliases_and_terminal_attempts(self):
        self.assertEqual(len(self.modules), 10)
        self.assertEqual(len(self.bindings), 49)
        self.assertEqual(len(set(self.bindings.values())), 37)
        aliases = dict(GsGetActiveBuff=0x800852A8, GetTPage=0x80082CE8, SetPolyGT4=0x80082EE8,
                       SetSemiTrans=0x80082DA8, SetShadeTex=0x80082DD8, GsSortPoly=0x800842A8)
        for name, address in aliases.items():
            self.assertEqual(self.bindings[name], address)
            self.assertEqual(self.bindings[f"func_spanish_{address:X}"], address)
        with (ROOT / "notes/overlays/spanish-model-variant441-framebuffer-rings-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 13)
        self.assertEqual([int(r["different_words"]) for r in attempts], [2]+[0]*12)
        self.assertEqual({r["instruction_bytes"] for r in attempts}, {"1488"})
        self.assertEqual((attempts[1]["slot"], attempts[2]["slot"]), ("0", "1"))
        self.assertNotEqual(attempts[1]["fingerprint"], attempts[2]["fingerprint"])
        body = ROOT / "src/overlays/spanish_model_variant/variant441_framebuffer_rings.c"
        dependency = hashlib.sha256(body.read_bytes()+body.with_suffix(".h").read_bytes()+
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        for module in self.modules:
            layout = ROOT / module["layout"]
            entries = json.loads(layout.with_name(layout.stem+"_matching_c.json").read_text())["functions"]
            selected, = [e for e in entries if "variant441_framebuffer_rings" in e["source"]]
            terminal, = [r for r in attempts[3:] if r["module"] == module["name"]]
            self.assertEqual((terminal["result"], terminal["instruction_bytes"]), ("matched", "1488"))
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)

    def test_original_context_sampling_passes_packet_bounds_and_completion(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        decoder = lines.lifetimes.SpanishModelVariant460Tests
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            self.assertEqual(tube.reaching_context(image, base, 0x1054), {0xC})
            self.assertEqual(struct.unpack_from("<II",image,0x1054),
                             (0x0C000000|((base+0x3060)>>2&0x3FFFFFF),0x02402021))
            self.assertEqual(decoder.register_writes(image,0x3060,0x3630,19), [0x3068,0x3618])
            self.assertEqual(decoder.register_writes(image,0x3060,0x3630,17), [0x30E4,0x3620])
            expected = [(26,2),*((o,1) for o in (12,13,24,25,36,37,48,49)),
                        (12,1),(13,1),(24,1),(25,1),(36,1),(26,2),(37,1),(48,1),(49,1),
                        *((o,1) for o in (4,5,6,16,17,18,28,29,30,40,41,42))]
            self.assertEqual([(o,s) for _,o,s in decoder.direct_stores(image,0x3060,0x3630,17)], expected)
            for offset, word in ANCHORS.items():
                self.assertEqual(struct.unpack_from("<I",image,offset)[0], word, hex(offset))
            self.assertEqual([(a,o,s) for a,o,s in decoder.direct_stores(image,0x3060,0x3630,29) if o==0xE0],
                             [(0x3098,0xE0,4),(0x3578,0xE0,4)])
            self.assertFalse(any(w>>26==35 and (w>>21&31)==29 and (w&65535)==0xD4
                                 for w, in struct.iter_unpack("<I",image[0x3060:0x3630])))
        self.assertEqual(0x14D4+5*0x1A8, 0x1D1C)
        self.assertEqual(0x25D4+52, 0x2608)
        self.assertEqual(3*17*8, 0x198)

    def test_target_layout_and_repeated_header_inclusion(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model441-framebuffer-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' +
                          '#include "../../src/overlays/spanish_model_variant/variant441_lines.h"\n' * 2 +
                          '#include "../../src/overlays/spanish_model_variant/variant441_tube.h"\n' * 2 +
                          '#include "../../src/overlays/spanish_model_variant/variant441_framebuffer_rings.h"\n' * 2 +
                          '#define O(t,f) ((u32)&((t *)0)->f)\n'
                          'u32 layout[] = {' + ",".join(expression for expression, _ in LAYOUT) + "};\n")
        obj = compile_c(ROOT, tool(ROOT, "as"),
                        dict(source=str(source.relative_to(ROOT)), profile="gcc_2_8_1_g0_split",
                             object="layout.o"), load_compiler_profiles(ROOT),
                        object_directory=str(directory.relative_to(ROOT)),
                        asm_directory=str(directory.relative_to(ROOT)))
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
            self.assertEqual(symbol["st_value"], 0)
            data = elf.get_section(symbol["st_shndx"]).data()
            expected = struct.pack("<" + "I"*len(LAYOUT), *(value for _, value in LAYOUT))
            self.assertEqual(data[:len(expected)], expected)
            self.assertFalse(any(data[len(expected):]))

    def test_all_input_final_owners_and_every_selected_relocation_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / "tmp/overlays" / module["name"]
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL441 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [segment for segment in c_segments(ROOT, ROOT / module["layout"])
                         if "/variant441_framebuffer_rings" in segment["source"]]
            selected_path = directory / "build" / selected["object"]
            owners = {}
            for path in (directory / "build").rglob("*.o"):
                with path.open("rb") as handle:
                    elf = ELFFile(handle)
                    for symbol in elf.get_section_by_name(".symtab").iter_symbols():
                        if isinstance(symbol["st_shndx"], int) and symbol.name.startswith(("func_", "D_")):
                            section = elf.get_section(symbol["st_shndx"])
                            if f"{path.relative_to(ROOT)}({section.name});" not in script:
                                continue
                            owners.setdefault(symbol.name, []).append(
                                (path, section.name, section["sh_flags"], symbol["st_size"],
                                 section.data()[symbol["st_value"]:]))
            with linked_path.open("rb") as handle, selected_path.open("rb") as obj_handle:
                linked, compiled = ELFFile(handle), ELFFile(obj_handle)
                symbols, original = linked.get_section_by_name(".symtab"), compiled.get_section_by_name(".symtab")
                own, = original.get_symbol_by_name(f"func_{base+0x3060:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1488, "STT_FUNC"))
                for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                    symbol, = symbols.get_symbol_by_name(f"func_{base+start:X}")
                    owner, = owners[symbol.name]
                    self.assertEqual(owner[0] == selected_path, start == 0x3060)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"]-section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x4754, 0x8AC)):
                    symbol, = symbols.get_symbol_by_name(f"D_{base+start:X}")
                    owner, = owners[symbol.name]
                    self.assertFalse(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual(owner[4], image[start:start+size])
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertEqual(symbol["st_value"], base+start)
                    self.assertFalse(section["sh_flags"] & 4)
                    offset = symbol["st_value"]-section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+size], image[start:start+size])
                text = bytearray(compiled.get_section(own["st_shndx"]).data()[:1488])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1488)
                    symbol = original.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", text, offset)
                    addend = (word & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        self.assertEqual(word >> 26, 3)
                        resolved, = symbols.get_symbol_by_name(symbol.name)
                        self.assertEqual(resolved["st_value"], self.bindings[symbol.name])
                        address = resolved["st_value"] + addend
                        calls.append(address)
                    else:
                        self.assertEqual(word >> 26, 2)
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        address = base+0x3060+symbol["st_value"]+addend
                        jumps.append((offset, address-base-0x3060))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0x3060:0x3630])
                self.assertEqual(len(calls), 18)
                self.assertEqual(set(calls), {0x8005C018,0x800852A8,0x80089928,0x80086628,0x80087CB8,
                                             0x800875F8,0x80086258,0x80085558,0x80087958,0x80082CE8,
                                             0x80082EE8,0x80082DA8,0x80082DD8,0x800842A8})
                self.assertEqual(jumps, [(0x90,0xA0),(0x27C,0x28C),(0x2E0,0x378),(0x300,0x310),
                                         (0x3F8,0x4A0),(0x42C,0x4A0),(0x43C,0x450),(0x454,0x4A0),
                                         (0x46C,0x4A0),(0x47C,0x48C),(0x490,0x4A0)])
