import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import unittest

from tools.project.tests import test_spanish_model_variant441 as lines

ROOT = lines.ROOT
BOUNDARIES = lines.BOUNDARIES
c_segments = lines.c_segments
lifetimes = lines.lifetimes

LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_G4)", 36), ("sizeof(CVECTOR)", 4), ("sizeof(DVECTOR)", 4),
    ("sizeof(Tube441)", 0x54C), ("sizeof(((Tube441 *)0)->points)", 0x288),
    ("O(Tube441,points[1])", 0x48), ("O(Tube441,colors)", 0x510),
    ("sizeof(Tube441Pulse)", 0x8C), ("O(Tube441Pulse,size)", 0x88),
    ("sizeof(Tube441Timing)", 0x30), ("O(Tube441Timing,expansion_start)", 0x20),
    ("O(Tube441Timing,expansion_end)", 0x24), ("O(Tube441Timing,fade_start)", 0x28),
    ("O(Tube441Timing,fade_end)", 0x2C), ("O(Tube441State,tubes)", 0x5AC),
    ("O(Tube441State,pulse)", 0x10DC), ("O(Tube441State,quad)", 0x2548),
    ("O(Tube441State,origin)", 0x2690), ("O(Tube441State,direction)", 0x26A4),
    ("O(Tube441State,projected)", 0x26B4), ("O(Tube441State,direction_b)", 0x26B8),
    ("O(Tube441State,time)", 0x26D4), ("O(Tube441State,step)", 0x26DC),
    ("O(Tube441State,timing)", 0x26E4), ("O(Tube441State,width)", 0x2704),
    ("O(Tube441State,progress)", 0x2708), ("O(Tube441State,angle)", 0x270C),
    ("O(Tube441State,phase)", 0x271C), ("sizeof(Tube441State)", 0x2720),
    ("O(POLY_G4,x0)", 8), ("O(POLY_G4,x1)", 16), ("O(POLY_G4,x2)", 24), ("O(POLY_G4,x3)", 32),
    ("O(POLY_G4,r0)", 4), ("O(POLY_G4,r1)", 12), ("O(POLY_G4,r2)", 20), ("O(POLY_G4,r3)", 28),
)


def reaching_context(image, base, call_offset=0x1014):
    writes = set(lifetimes.SpanishModelVariant460Tests.register_writes(image, 4, 0x11BC, 18))
    pending, visited, reaching = [(4, None)], set(), set()
    while pending:
        pc, definition = pending.pop()
        assert 4 <= pc < 0x11BC and pc % 4 == 0
        if (pc, definition) in visited:
            continue
        visited.add((pc, definition))
        word, = struct.unpack_from("<I", image, pc)
        op = word >> 26
        if pc in writes:
            definition = pc
        if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
            if pc+4 in writes:
                definition = pc+4
            if pc == call_offset:
                reaching.add(definition)
            if word == 0x03E00008:
                continue
            if op == 2:
                targets = [((base+pc+4) & 0xF0000000 | ((word & 0x3FFFFFF) << 2))-base]
            elif op == 3:
                targets = [pc+8]
            else:
                displacement = (word & 65535)-(65536 if word & 32768 else 0)
                targets = [pc+8, pc+4+displacement*4]
            pending.extend((target, definition) for target in targets)
        else:
            pending.append((pc+4, definition))
    return reaching


class SpanishModelVariant441TubeTests(unittest.TestCase):
    setUp = lines.SpanishModelVariant441Tests.setUp
    legal_images = lines.SpanishModelVariant441Tests.legal_images

    def test_aliases_and_terminal_attempts(self):
        self.assertEqual(len(self.modules), 10)
        self.assertEqual(len(self.bindings), 49)
        self.assertEqual(len(set(self.bindings.values())), 37)
        for name, address in (("rsin", 0x80086628), ("rcos", 0x800866F8)):
            self.assertEqual(self.bindings[name], address)
            self.assertEqual(self.bindings[f"func_spanish_{address:X}"], address)
        with (ROOT / "notes/overlays/spanish-model-variant441-tube-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 17)
        self.assertEqual([int(r["different_words"]) for r in attempts], [125, 121, 20, 390] + [0]*13)
        self.assertEqual([int(r["instruction_bytes"]) for r in attempts], [1696]*3 + [1700] + [1696]*13)
        self.assertEqual((attempts[5]["slot"], attempts[6]["slot"]), ("0", "1"))
        self.assertEqual(attempts[4]["fingerprint"], attempts[5]["fingerprint"])
        self.assertNotEqual(attempts[5]["fingerprint"], attempts[6]["fingerprint"])
        body = ROOT / "src/overlays/spanish_model_variant/variant441_tube.c"
        dependency = hashlib.sha256(body.read_bytes()+body.with_suffix(".h").read_bytes()+
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        for module in self.modules:
            layout = ROOT / module["layout"]
            entries = json.loads(layout.with_name(layout.stem+"_matching_c.json").read_text())["functions"]
            selected, = [e for e in entries if "variant441_tube" in e["source"]]
            terminal, = [r for r in attempts[7:] if r["module"] == module["name"]]
            self.assertEqual((terminal["result"], terminal["instruction_bytes"]), ("matched", "1696"))
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)

    def test_original_context_packet_initialization_and_signed_descriptors(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        anchors = {
            0x68:0x26D82548, 0x6C:0xAFB800A0, 0x24C:0x8FA400A0,
            0x250:0x0C020BB2, 0x258:0x8FA400A0, 0x25C:0x0C020B6A, 0x260:0x24050001,
            0x1004:0x8EC2271C, 0x100C:0x18400003, 0x173C:0x27BDFED8,
            0x1744:0x0080A021, 0x178C:0x26912548, 0x17D4:0x268310DC,
            0x17DC:0x8C630088, 0x1818:0x269705AC, 0x1858:0x00571021,
            0x19DC:0x28840009, 0x1A08:0x28420009, 0x1A2C:0x26F7054C,
            0x1A60:0x24021000, 0x1A64:0xAFA20030, 0x1A68:0xAFA20034, 0x1A6C:0xAFA20038,
            0x1BE8:0x04C0000A, 0x1BF0:0x8FA200D4, 0x1BF8:0x04400007,
            0x1C08:0x30C6FFFF, 0x1C10:0x24070001, 0x1C24:0x28420008, 0x1C40:0x28420008,
            0x1CB4:0x0043001A, 0x1D38:0x0062001A, 0x1DA0:0x000211C0, 0x1DA8:0xAE83270C,
        }
        expected = {166:(76,88,180,200), 360:(168,180,360,400), 487:(128,140,360,430),
                    590:(76,88,180,200), 709:(84,96,400,460)}
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            self.assertEqual(reaching_context(image, base), {0xC})
            self.assertEqual(struct.unpack_from("<II", image, 0x1014),
                             (0x0C000000 | ((base+0x173C) >> 2 & 0x3FFFFFF), 0x02402021))
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            decoder = lifetimes.SpanishModelVariant460Tests
            self.assertEqual(decoder.register_writes(image,0x173C,0x1DDC,20), [0x1744,0x1DC0])
            self.assertEqual(decoder.register_writes(image,0x173C,0x1DDC,17), [0x178C,0x1DCC])
            self.assertEqual([(o,s) for _,o,s in decoder.direct_stores(image,0x173C,0x1DDC,17)],
                             [(o,1) for start in (4,12,20,28) for o in range(start,start+3)])
            self.assertEqual([(a,o,s) for a,o,s in decoder.direct_stores(image,4,0x11BC,29) if o == 0xA0],
                             [(0x6C,0xA0,4)])
            row = self.instances[module["name"]]
            descriptor = 0x4850 + int(row["command_word"]) % 1000 * 48
            self.assertEqual(struct.unpack_from("<H",image,descriptor+0x18), (1,))
            times = struct.unpack_from("<iiii",image,descriptor+0x20)
            self.assertEqual(times, expected[int(row["model"])])
            self.assertGreater(times[1],times[0])
            self.assertGreater(times[3],times[2])
        self.assertEqual(0x5AC+0x54C, 0xAF8)
        self.assertEqual(0x2548+36, 0x256C)
        self.assertEqual(9*9*8, 0x288)

    def test_target_layout_and_repeated_header_inclusion(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model441-tube-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' +
                          '#include "../../src/overlays/spanish_model_variant/variant441_lines.h"\n' * 2 +
                          '#include "../../src/overlays/spanish_model_variant/variant441_tube.h"\n' * 2 +
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
                         if "/variant441_tube" in segment["source"]]
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
                own, = original.get_symbol_by_name(f"func_{base+0x173C:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1696, "STT_FUNC"))
                for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                    symbol, = symbols.get_symbol_by_name(f"func_{base+start:X}")
                    owner, = owners[symbol.name]
                    self.assertEqual(owner[0] == selected_path, start == 0x173C)
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
                text = bytearray(compiled.get_section(own["st_shndx"]).data()[:1696])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1696)
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
                        address = base+0x173C+symbol["st_value"]+addend
                        jumps.append((offset, address-base-0x173C))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0x173C:0x1DDC])
                self.assertEqual(len(calls), 15)
                self.assertEqual(set(calls), {0x8005C018, 0x80089928, 0x80086628, 0x800866F8,
                                             0x80087CB8, 0x80086258, 0x80085558, 0x80087958, 0x8004D5B8})
                self.assertEqual(jumps, [(0xBC, 0xDC)])
