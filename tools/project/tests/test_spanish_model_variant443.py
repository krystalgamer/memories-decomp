import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_spanish_overlay_inventories
from verify_inputs import load_checksum_manifest
from tools.project.tests import test_spanish_model_variant460 as lifetimes

BOUNDARIES = (4, 0x1ADC, 0x23AC, 0x298C, 0x2F2C, 0x3668, 0x3EF0, 0x45A8)
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_G4)", 36), ("sizeof(CVECTOR)", 4),
    ("sizeof(Mesh443)", 0x54C), ("sizeof(((Mesh443 *)0)->points)", 0x288),
    ("O(Mesh443,points[1])", 0x48), ("O(Mesh443,colors)", 0x510),
    ("sizeof(Companion443)", 0x8C), ("O(Companion443,size)", 0x88),
    ("O(Timing443,grow_start)", 0x48), ("O(Timing443,grow_end)", 0x4C),
    ("O(Mesh443State,mesh)", 0xC80), ("O(Mesh443State,companion)", 0x2A68),
    ("O(Mesh443State,quad)", 0x33AC), ("O(Mesh443State,origin)", 0x35A0),
    ("O(Mesh443State,path)", 0x35B8), ("O(Mesh443State,projected)", 0x35F8),
    ("O(Mesh443State,direction)", 0x3608), ("O(Mesh443State,time)", 0x3624),
    ("O(Mesh443State,step)", 0x362C), ("O(Mesh443State,timing)", 0x3634),
    ("O(Mesh443State,progress)", 0x3654), ("O(Mesh443State,angle)", 0x3658),
    ("O(Mesh443State,phase)", 0x3698), ("sizeof(Mesh443State)", 0x369C),
    ("O(POLY_G4,x0)", 8), ("O(POLY_G4,x1)", 16),
    ("O(POLY_G4,x2)", 24), ("O(POLY_G4,x3)", 32),
    ("O(POLY_G4,r0)", 4), ("O(POLY_G4,r1)", 12),
    ("O(POLY_G4,r2)", 20), ("O(POLY_G4,r3)", 28),
)
ANCHORS = {
    0xC: 0x00809821, 0x1C: 0x26CC0C80, 0x28: 0xAFAC0094,
    0x3C: 0x26CC2A68, 0x94: 0x26CB33AC,
    0x9F4: 0xA0A40510, 0x9F8: 0xA0A30511, 0x9FC: 0xA0A20512,
    0xA04: 0x2A620009, 0xA0C: 0x24A50004, 0xA24: 0x24C6054C,
    0xA28: 0x258C054C, 0xA2C: 0x1AE0FFC3,
    0x1944: 0x8EC23698, 0x194C: 0x28420004, 0x1950: 0x14400003,
    0x298C: 0x27BDFEE0, 0x2994: 0x0080A821, 0x29DC: 0x26B133AC,
    0x29E8: 0x26A80C80, 0x29FC: 0x24420C00, 0x2A08: 0x26A22A68,
    0x2A0C: 0x8C420088, 0x2A1C: 0x2442003F, 0x2A24: 0x00021280,
    0x2A28: 0x00021403, 0x2A34: 0x96BE3658,
    0x2A48: 0x001310C0, 0x2A4C: 0x00531021, 0x2A54: 0x000210C0,
    0x2A58: 0x00491021, 0x2A5C: 0xAFA200EC, 0x2A98: 0x0048A021,
    0x2AA0: 0x00041283, 0x2AA4: 0x00530018, 0x2AC8: 0x000218C3,
    0x2ADC: 0xA6830000, 0x2B58: 0xA6830002, 0x2BEC: 0xA6830004,
    0x2BD8: 0x28840009, 0x2C00: 0x28420009, 0x2C18: 0x254A054C,
    0x2C24: 0x1840FF82, 0x2C64: 0xAFA20030, 0x2C68: 0xAFA20034,
    0x2C6C: 0xAFA20038, 0x2D64: 0x90830510, 0x2D88: 0x92030510,
    0x2DF0: 0x04C0000A, 0x2E00: 0x04400007, 0x2E10: 0x30C6FFFF,
    0x2E14: 0x0C01356E, 0x2E18: 0x24070001,
    0x2E2C: 0x28420008, 0x2E48: 0x28420008, 0x2E6C: 0x1840FF9A,
    0x2E74: 0x8EA33698, 0x2E8C: 0x28420400, 0x2EA0: 0x8C640048,
    0x2EA4: 0x8C63004C, 0x2EB4: 0x0043001B, 0x2EE4: 0xAEA23698,
    0x2EF0: 0x000211C0, 0x2EF8: 0xAEA33658,
}
GUEST_ANCHORS = {
    0x8004FD94: 0x12C00005, 0x8004FD9C: 0x3C028001,
    0x8004FDA0: 0x8C420028, 0x8004FDA8: 0xAE220DEC,
    0x8004FDAC: 0x3C028001, 0x8004FDB0: 0x8C420024,
    0x8004FDB8: 0xAE220DEC, 0x80058B9C: 0x8E140DEC,
    0x80058C70: 0x3C028001, 0x80058C74: 0x8C420018,
    0x80058C7C: 0x24530004, 0x80058C80: 0x3C028001,
    0x80058C84: 0x8C420014, 0x80058C8C: 0x24530004,
    0x80058CC4: 0x02802021, 0x80058CEC: 0x0260F809,
    0x80058E08: 0x02802021, 0x80058E0C: 0x0260F809,
}


def reaching_context(image, base):
    writes = set(lifetimes.SpanishModelVariant460Tests.register_writes(image, 4, 0x1ADC, 19))
    pending, visited, reaching = [(4, None)], set(), set()
    while pending:
        pc, definition = pending.pop()
        assert 4 <= pc < 0x1ADC and pc % 4 == 0
        if (pc, definition) in visited:
            continue
        visited.add((pc, definition))
        word, = struct.unpack_from("<I", image, pc)
        op = word >> 26
        if pc in writes:
            definition = pc
        if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
            if pc + 4 in writes:
                definition = pc + 4
            if pc == 0x1958:
                reaching.add(definition)
            if word == 0x03E00008:
                continue
            if op == 2:
                targets = [((base + pc + 4) & 0xF0000000 | ((word & 0x3FFFFFF) << 2)) - base]
            elif op == 3:
                targets = [pc + 8]
            else:
                displacement = (word & 65535) - (65536 if word & 32768 else 0)
                targets = [pc + 8, pc + 4 + displacement * 4]
            pending.extend((target, definition) for target in targets)
        else:
            pending.append((pc + 4, definition))
    return reaching


def verify_guest_contexts(retail):
    for address, word in GUEST_ANCHORS.items():
        assert struct.unpack_from("<I", retail, address-0x8000F800)[0] == word, hex(address)
    assert lifetimes.SpanishModelVariant460Tests.register_writes(retail, 0x80058B4C-0x8000F800,
                                   0x80058E14-0x8000F800, 20) == [0x80058B9C-0x8000F800]
    assert lifetimes.SpanishModelVariant460Tests.register_writes(retail, 0x80058CC4-0x8000F800,
                                   0x80058CF4-0x8000F800, 4) == [0x80058CC4-0x8000F800]
    for slot in (0, 1):
        context, = struct.unpack_from("<I", retail, 0x824+4*slot)
        entry, = struct.unpack_from("<I", retail, 0x814+4*slot)
        assert context == 0x80136000+slot*0x40000 and entry == 0x8013B000+slot*0x40000
        assert context + 0x369C <= 0x8013A000+slot*0x40000
        for row in range(9):
            address = context + 0xC80 + row*0x48
            assert 0 <= address < address+9*8 <= 0x100000000
            assert address == ((context+0xC80) & 0xFFFFFFFF) + row*0x48
            assert address + 9*8 <= context+0xC80+0x288


class SpanishModelVariant443Tests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [module for module in manifest["modules"]
                        if module["linker_symbols"].endswith("/model_variant443_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant443-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.bindings = dict((name, int(address, 0)) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config / "overlays/model_variant443_linker_symbols.txt").read_text(), re.M))

    def legal_images(self):
        archive = ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive.is_file():
            self.skipTest("Legal Spanish MODEL input required")
        with archive.open("rb") as handle:
            for module in self.modules:
                handle.seek(module["sector_offset"] * 2048)
                image = handle.read(20480)
                self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
                yield module, image

    def test_metadata_and_terminal_fingerprints(self):
        self.assertEqual(len(self.modules), 2)
        self.assertEqual(len(self.instances), 2)
        self.assertEqual(len({m["sha256"] for m in self.modules}), 2)
        self.assertEqual(len(self.bindings), 42)
        self.assertEqual(len(set(self.bindings.values())), 35)
        with (ROOT / "notes/overlays/spanish-model-variant443-mesh-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 17)
        self.assertEqual([(int(r["instruction_bytes"]),int(r["different_words"])) for r in attempts[:15]],
                         [(1420,345),(1424,324),(1444,334),(1444,337),(1440,27),(1428,238),
                          (1440,10),(1440,8),(1440,8),(1440,208),(1440,1),(1428,253),(1440,1),
                          (1440,0),(1440,0)])
        self.assertEqual([r["result"] for r in attempts], ["mismatch"]*13+["matched"]*4)
        self.assertEqual([r["module"] for r in attempts[:15]],
                         ["spanish_model_variant_62_stage9_slot0"]*14 +
                         ["spanish_model_variant_62_stage10_slot1"])
        body = ROOT / "src/overlays/spanish_model_variant/variant443_mesh.c"
        dependency = hashlib.sha256(body.read_bytes()+body.with_suffix(".h").read_bytes()+
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()+
                                    (ROOT / "src/game/gpu_packets.h").read_bytes()).hexdigest()
        checksums = load_checksum_manifest(self.config / "files.sha256")
        totals = load_spanish_overlay_inventories(ROOT)
        for module in self.modules:
            row = self.instances[module["name"]]
            slot = int(row["slot"])
            base = 0x8013B000+slot*0x40000
            self.assertEqual((int(row["model"]),int(row["record"]),int(row["stage"]),int(row["sector"]),
                              int(row["header"]),int(row["command_word"])),
                             (62,62,9+slot,17312+slot*10,443+150*slot,609000))
            self.assertEqual((module["sector_offset"],module["sector_count"]), (17312+slot*10,10))
            self.assertEqual(module["sha256"],row["sha256"])
            self.assertEqual(module["archive_sha256"],checksums[module["archive"]])
            self.assertEqual(int(module["load_address"],0),base)
            self.assertNotIn("duplicate_sector_offsets",module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant443_mesh" + ("_slot1" if slot else "") + ".c"
            self.assertEqual(json.loads(layout.with_name(layout.stem+"_matching_c.json").read_text()),
                             {"schema":1,"functions":[{"address":f"0x{base+0x23AC:X}","size":"0x5E0",
                              "profile":"gcc_2_8_1_g0_split",
                              "source":source.replace("variant443_mesh","variant443_rings")},
                              {"address":f"0x{base+0x298C:X}","size":"0x5A0",
                              "profile":"gcc_2_8_1_g0_split","source":source}]})
            self.assertEqual([s["source"] for s in c_segments(ROOT,layout)],
                             [source.replace("variant443_mesh","variant443_rings"),source])
            with layout.with_name(layout.stem+"_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"],0)-base,int(r["size"],0)) for r in rows],
                             [(a,b-a) for a,b in zip(BOUNDARIES,BOUNDARIES[1:])])
            self.assertEqual([r["status"] for r in rows],["unmatched_asm"]*2+["matching_c"]*2+["unmatched_asm"]*3)
            self.assertEqual((totals[layout.stem]["function_count"],
                              totals[layout.stem]["matching_c_function_count"],
                              totals[layout.stem]["matching_c_bytes"]),(7,2,2944))
            terminal, = [r for r in attempts[15:] if r["module"]==module["name"]]
            self.assertEqual((terminal["result"],terminal["profile"],terminal["instruction_bytes"],
                              terminal["different_words"]),("matched","gcc_2_8_1_g0_split","1440","0"))
            self.assertEqual(terminal["fingerprint"],hashlib.sha256((ROOT/source).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"],dependency)

    def test_exhaustive_loads_and_descriptors(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import model_records
        images = list(self.legal_images())
        observed = set()
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as handle:
            for record, model in enumerate(model_records()):
                for stage in range(7,11):
                    sector = record*276+180+(stage-7)*10
                    handle.seek(sector*2048)
                    header, = struct.unpack("<I",handle.read(4))
                    if header in (443,593):
                        observed.add((model,record,stage,sector,header))
                        row, = [r for r in self.instances.values() if int(r["sector"])==sector]
                        handle.seek((record*276+275)*2048+0x110+(stage-7)//2*4)
                        self.assertEqual(struct.unpack("<i",handle.read(4)),(int(row["command_word"]),))
        self.assertEqual(observed,{(int(r["model"]),int(r["record"]),int(r["stage"]),
                                    int(r["sector"]),int(r["header"])) for r in self.instances.values()})
        for module,image in images:
            row = self.instances[module["name"]]
            offset = 0x4BF8+int(row["command_word"])%1000*88
            self.assertEqual(offset,int(row["descriptor_offset"]))
            self.assertEqual(hashlib.sha256(image[offset:offset+88]).hexdigest(),row["descriptor_sha256"])
            base = int(module["load_address"],0)
            self.assertEqual(struct.unpack_from("<II",image,0xD4),
                             (0x3C030000|((base+0x4BF8+0x8000)>>16),0x2463FBF8))
            self.assertEqual(struct.unpack_from("<IIIIII",image,0xE8),
                             (0x000B1040,0x004B1021,0x00021080,0x004B1023,0x000210C0,0x00431021))

    def test_original_context_cfg_packet_and_initialization(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import walk_function
        decoder = lifetimes.SpanishModelVariant460Tests
        for module,image in self.legal_images():
            base = int(module["load_address"],0)
            for start,end in zip(BOUNDARIES,BOUNDARIES[1:]):
                flow = walk_function(image,base,start,end-start)
                self.assertTrue(flow["closed"])
                self.assertEqual(flow["visited"],set(range(start,end,4)))
                self.assertEqual((flow["returns"],flow["indirect_calls"]),(1,0))
                self.assertEqual(flow["calls"],{base+o for o in BOUNDARIES[1:-1]} if start==4 else set())
                self.assertLessEqual(flow["external"],set(self.bindings.values()))
                if start==0x298C:
                    self.assertNotIn(0x800875F8,flow["external"])
            self.assertEqual(reaching_context(image,base),{0xC})
            self.assertEqual(struct.unpack_from("<II",image,0x1958),
                             (0x0C000000|((base+0x298C)>>2&0x3FFFFFF),0x02602021))
            for offset,word in ANCHORS.items():
                self.assertEqual(struct.unpack_from("<I",image,offset)[0],word,hex(offset))
            self.assertEqual(decoder.register_writes(image,0x298C,0x2F2C,21),[0x2994,0x2F0C])
            self.assertEqual(decoder.register_writes(image,0x298C,0x2F2C,17),[0x29DC,0x2F1C])
            self.assertEqual([(o,s) for _,o,s in decoder.direct_stores(image,0x298C,0x2F2C,17)],
                             [(o,1) for o in (4,5,6,12,13,14,20,21,22,28,29,30)])
            self.assertEqual([(o,s) for _,o,s in decoder.direct_stores(image,0x298C,0x2F2C,20)],
                             [(0,2),(2,2),(4,2)])
            self.assertEqual([(a,o,s) for a,o,s in decoder.direct_stores(image,0x298C,0x2F2C,29)
                              if 0x30<=o<0x40],
                             [(0x2C64,0x30,4),(0x2C68,0x34,4),(0x2C6C,0x38,4)])
        self.assertEqual(0xC80+0x54C,0x11CC)
        self.assertEqual(0x33AC+36,0x33D0)
        self.assertEqual(9*9*8,0x288)

    def test_fixed_guest_context_chain_and_row_bounds(self):
        retail = ROOT / "game/spain/SLES_039.51"
        if not retail.is_file():
            self.skipTest("Legal Spanish resident required")
        checksums = load_checksum_manifest(self.config / "files.sha256")
        data = retail.read_bytes()
        self.assertEqual(hashlib.sha256(data).hexdigest(),checksums["game/spain/SLES_039.51"])
        verify_guest_contexts(data)

    def test_target_layout_and_repeated_header_inclusion(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model443-mesh-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' +
                          '#include "../../src/overlays/spanish_model_variant/variant443_mesh.h"\n' * 2 +
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
            expected = struct.pack("<" + "I"*len(LAYOUT), *(value for _, value in LAYOUT))
            data = elf.get_section(symbol["st_shndx"]).data()
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
                self.skipTest("Build Spanish MODEL443 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [segment for segment in c_segments(ROOT, ROOT / module["layout"])
                         if "/variant443_mesh" in segment["source"]]
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
                own, = original.get_symbol_by_name(f"func_{base+0x298C:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1440, "STT_FUNC"))
                for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                    symbol, = symbols.get_symbol_by_name(f"func_{base+start:X}")
                    owner, = owners[symbol.name]
                    self.assertEqual(owner[0] == selected_path, start == 0x298C)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"]-section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x45A8, 0xA58)):
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
                text = bytearray(compiled.get_section(own["st_shndx"]).data()[:1440])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1440)
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
                        address = base+0x298C+symbol["st_value"]+addend
                        jumps.append((offset, address-base-0x298C))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0x298C:0x2F2C])
                self.assertEqual(len(calls), 15)
                self.assertEqual(set(calls), {2148029784, 2148039000, 2147800504, 2148047144, 2148034088, 2148033112, 2148034296, 2147860504, 2148039864})
                self.assertEqual(jumps, [])

    def test_resident_callee_loader_and_context_owners_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        elf_path = ROOT / "tmp/project-build/SLES_039.51.elf"
        retail_path = ROOT / "game/spain/SLES_039.51"
        if not elf_path.is_file() or not retail_path.is_file():
            self.skipTest("Build Spanish resident with legal inputs before checking owners")
        retail = retail_path.read_bytes()
        self.assertEqual((ROOT / "tmp/project-build/SLES_039.51").read_bytes(), retail)
        selections = [(kind, int(address, 16), int(size, 16), path)
                      for kind, address, size, path in re.findall(
                          r"^\s+\.(text|data|rodata|sdata)\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+\.o)\s*$",
                          (ROOT / "tmp/project-build/SLES_039.51.map").read_text(), re.M)]
        script = (ROOT / "tmp/splat/sles_03951/sles_03951.ld").read_text()
        with (self.config / "functions.csv").open() as handle:
            inventory = {int(row["address"], 0): row for row in csv.DictReader(handle)}
        addresses = set(self.bindings.values())
        for name in ("Model_LoadMonsterMerge", "func_80056D7C", "func_8004CB0C", "func_800559D4"):
            address, = [address for address, row in inventory.items() if row["name"] == name]
            self.assertEqual(inventory[address]["status"], "matching_c")
            addresses.add(address)
        pointers = {0x80010000: 0x80100000, 0x80010004: 0x80140000,
                    0x8001000C: 0x8013A000, 0x80010010: 0x8017A000,
                    0x80010014: 0x8013B000, 0x80010018: 0x8017B000,
                    0x80010024: 0x80136000, 0x80010028: 0x80176000}
        with elf_path.open("rb") as handle:
            linked = ELFFile(handle)
            symbols = linked.get_section_by_name(".symtab")
            for address in sorted(addresses | pointers.keys()):
                executable = address in addresses
                size = int(inventory[address]["size"], 0) if executable else 4
                selection, = [item for item in selections if (item[0] == "text") == executable
                              and item[1] <= address and address+size <= item[1]+item[2]]
                kind, start, extent, path = selection
                self.assertIn(f"{path}(.{kind})", script)
                with (ROOT / path).open("rb") as source_handle:
                    source = ELFFile(source_handle)
                    section = source.get_section_by_name("." + kind)
                    self.assertEqual(section["sh_size"], extent)
                    self.assertEqual(bool(section["sh_flags"] & 4), executable)
                    if executable:
                        name = inventory[address]["name"]
                        definition, = source.get_section_by_name(".symtab").get_symbol_by_name(name)
                        final, = symbols.get_symbol_by_name(name)
                        self.assertEqual((definition["st_value"]+start, definition["st_size"],
                                          definition["st_info"]["type"]), (address, size, "STT_FUNC"))
                        self.assertEqual((final["st_value"], final["st_size"], final["st_info"]["type"]),
                                         (address, size, "STT_FUNC"))
                        self.assertEqual(source.get_section(definition["st_shndx"]).name, "." + kind)
                        final_section = linked.get_section(final["st_shndx"])
                        self.assertTrue(final_section["sh_flags"] & 4)
                    else:
                        self.assertEqual(section.data()[address-start:address-start+size],
                                         struct.pack("<I", pointers[address]))
                        final_section, = [item for item in linked.iter_sections() if item["sh_type"] == "SHT_PROGBITS"
                                          and item["sh_addr"] <= address
                                          and address+size <= item["sh_addr"]+item["sh_size"]]
                    data = final_section.data()[address-final_section["sh_addr"]:address-final_section["sh_addr"]+size]
                    self.assertEqual(data, retail[address-0x8000F800:address-0x8000F800+size])
        for slot in (0, 1):
            context = pointers[0x80010024+slot*4]
            for base, size in ((0x80100000, 96*2048), (0x8013A000, 4096), (0x8013B000, 20480)):
                base += slot*0x40000
                self.assertTrue(context+0x369C <= base or base+size <= context)
