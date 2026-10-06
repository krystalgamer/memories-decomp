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

BOUNDARIES = (4, 0x1204, 0x1778, 0x1E60, 0x2480, 0x2ACC, 0x30E4, 0x3750, 0x43A0)
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(GsGLINE)", 20),
    ("sizeof(Lines417)", 0x1A0), ("sizeof(((Lines417 *)0)->points)", 0x180),
    ("O(Lines417,points[1])", 0xC0), ("O(Lines417,points[0][1])", 0x30),
    ("O(Lines417,color)", 0x180), ("O(Lines417,size)", 0x194),
    ("sizeof(Companion417)", 0x8C), ("O(Companion417,size)", 0x88),
    ("O(Lines417State,groups)", 0x54C), ("O(Lines417State,companion)", 0xA98),
    ("O(Lines417State,line)", 0x20BC), ("O(Lines417State,origin)", 0x20E4),
    ("O(Lines417State,target)", 0x20F0), ("O(Lines417State,direction_a)", 0x20FC),
    ("O(Lines417State,projected)", 0x2108), ("O(Lines417State,direction)", 0x210C),
    ("O(Lines417State,step)", 0x2130), ("O(Lines417State,phase)", 0x2170),
    ("sizeof(Lines417State)", 0x2174), ("O(GsGLINE,attribute)", 0),
    ("O(GsGLINE,x0)", 4), ("O(GsGLINE,x1)", 8), ("O(GsGLINE,r0)", 12),
    ("O(GsGLINE,g0)", 13), ("O(GsGLINE,b0)", 14), ("O(GsGLINE,r1)", 15),
    ("O(GsGLINE,g1)", 16), ("O(GsGLINE,b1)", 17),
)
ANCHORS = {
    0xC: 0x00809021, 0x18: 0x26D8054C, 0x20: 0x26D80A98,
    0x468: 0xA6380002, 0x478: 0xA6220000, 0x490: 0xA6220004,
    0x4A0: 0x263000C0, 0x4AC: 0xA62200C0, 0x4B8: 0xA6180002,
    0x4DC: 0x26310008, 0x4F4: 0xA6020004, 0x4F8: 0x2A620006,
    0x520: 0x27180030, 0x534: 0x2AE20004,
    0x564: 0xA302FFE4, 0x588: 0xA303FFE5, 0x594: 0x271801A0,
    0x5BC: 0xA303FFE6, 0x5CC: 0x24421000, 0x5D0: 0xAF02FFF8,
    0x5D4: 0x271801A0, 0x5E4: 0x2B020003,
    0x7CC: 0x27030090, 0xA2C: 0xAC60FFF8, 0xA40: 0x27180098,
    0xA50: 0x2B020002, 0x1018: 0x8EC42138, 0x1024: 0x8C83001C,
    0x1028: 0x8EC22128, 0x1030: 0x0043102A, 0x1034: 0x14400005,
    0x1204: 0x27BDFED8, 0x120C: 0x00809821, 0x1210: 0x26680A98,
    0x1214: 0x2669054C, 0x123C: 0xAFA800D8, 0x124C: 0xAFAA00F0,
    0x125C: 0x267220BC, 0x1270: 0x266B20C0, 0x1284: 0x266820C4,
    0x129C: 0x267106E0, 0x12B8: 0x28621000, 0x12D4: 0x28820801,
    0x1320: 0x28821801, 0x1394: 0xA7A00028, 0x13A8: 0x28420002,
    0x13B4: 0x8E6220E4, 0x13D8: 0x866220F0, 0x1400: 0x27B00040,
    0x1408: 0xAFA60030, 0x1424: 0x27A40080, 0x1428: 0x27B00060,
    0x14A4: 0x26C200C0, 0x14C0: 0x3C025000, 0x14C4: 0xAE420000,
    0x14EC: 0x00803021, 0x14F0: 0x00A03821, 0x14F4: 0xAFAB0010,
    0x14F8: 0xAFA80014, 0x14FC: 0xAFAB0018, 0x1504: 0xAFA8001C,
    0x1550: 0x04C0000A, 0x1560: 0x04400006, 0x1574: 0x30C6FFFF,
    0x1588: 0x28420006, 0x15A4: 0x28420004, 0x15F8: 0x8D220088,
    0x1620: 0x8D420088, 0x1664: 0x000210C3, 0x1678: 0x00021023,
    0x16A0: 0x00021200, 0x16BC: 0x28420005, 0x16C8: 0x24022000,
    0x16E4: 0x1443000D, 0x1704: 0x14620005, 0x1710: 0xAE622170,
    0x1718: 0xAFA000F0, 0x171C: 0x263101A0, 0x172C: 0x252901A0,
    0x173C: 0x28420003,
}


def reaching_context(image, base):
    writes = set(lifetimes.SpanishModelVariant460Tests.register_writes(image, 4, 0x1204, 18))
    pending, visited, reaching = [(4, None)], set(), set()
    while pending:
        pc, definition = pending.pop()
        assert 4 <= pc < 0x1204 and pc % 4 == 0
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
            if pc == 0x103C:
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


class SpanishModelVariant417Tests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [module for module in manifest["modules"]
                        if module["linker_symbols"].endswith("/model_variant417_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant417-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.bindings = dict((name, int(address, 0)) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config / "overlays/model_variant417_linker_symbols.txt").read_text(), re.M))

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
        self.assertEqual(len(self.modules), 8)
        self.assertEqual(len(self.instances), 8)
        self.assertEqual(len({m["sha256"] for m in self.modules}), 8)
        self.assertEqual(len(self.bindings), 37)
        self.assertEqual(len(set(self.bindings.values())), 37)
        with (ROOT / "notes/overlays/spanish-model-variant417-lines-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 12)
        self.assertEqual((attempts[0]["result"], attempts[0]["instruction_bytes"],
                          attempts[0]["different_words"]), ("compile_error", "", ""))
        self.assertEqual([r["different_words"] for r in attempts[1:]], ["0"]*11)
        self.assertEqual({r["instruction_bytes"] for r in attempts[1:]}, {"1396"})
        self.assertEqual([r["module"] for r in attempts[:4]],
                         ["spanish_model_variant_134_stage9_slot0"]*3 +
                         ["spanish_model_variant_134_stage10_slot1"])
        body = ROOT / "src/overlays/spanish_model_variant/variant417_lines.c"
        dependency = hashlib.sha256(body.read_bytes()+body.with_suffix(".h").read_bytes()+
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        checksums = load_checksum_manifest(self.config / "files.sha256")
        totals = load_spanish_overlay_inventories(ROOT)
        expected = {134:(134,9,37184,583003), 232:(232,9,64232,583002),
                    354:(304,7,84084,583000), 535:(485,9,134060,583004)}
        for module in self.modules:
            row = self.instances[module["name"]]
            slot, model = int(row["slot"]), int(row["model"])
            record, stage, sector, command = expected[model]
            base = 0x8013B000 + slot*0x40000
            self.assertEqual((int(row["record"]),int(row["stage"]),int(row["sector"]),
                              int(row["header"]),int(row["command_word"])),
                             (record,stage+slot,sector+slot*10,417+150*slot,command))
            self.assertEqual((module["sector_offset"],module["sector_count"]), (sector+slot*10,10))
            self.assertEqual(module["sha256"],row["sha256"])
            self.assertEqual(module["archive_sha256"],checksums[module["archive"]])
            self.assertEqual(int(module["load_address"],0),base)
            self.assertNotIn("duplicate_sector_offsets",module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant417_lines" + ("_slot1" if slot else "") + ".c"
            self.assertEqual(json.loads(layout.with_name(layout.stem+"_matching_c.json").read_text()),
                             {"schema":1,"functions":[{"address":f"0x{base+0x1204:X}","size":"0x574",
                              "profile":"gcc_2_8_1_g0_split","source":source}]})
            self.assertEqual([s["source"] for s in c_segments(ROOT,layout)],[source])
            with layout.with_name(layout.stem+"_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"],0)-base,int(r["size"],0)) for r in rows],
                             [(a,b-a) for a,b in zip(BOUNDARIES,BOUNDARIES[1:])])
            self.assertEqual([r["status"] for r in rows],["unmatched_asm","matching_c"]+["unmatched_asm"]*6)
            self.assertEqual((totals[layout.stem]["function_count"],
                              totals[layout.stem]["matching_c_function_count"],
                              totals[layout.stem]["matching_c_bytes"]),(8,1,1396))
            terminal, = [r for r in attempts[4:] if r["module"]==module["name"]]
            self.assertEqual((terminal["result"],terminal["profile"]),("matched","gcc_2_8_1_g0_split"))
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
                    if header in (417,567):
                        observed.add((model,record,stage,sector,header))
                        row, = [r for r in self.instances.values() if int(r["sector"])==sector]
                        handle.seek((record*276+275)*2048+0x110+(stage-7)//2*4)
                        self.assertEqual(struct.unpack("<i",handle.read(4)),(int(row["command_word"]),))
        self.assertEqual(observed,{(int(r["model"]),int(r["record"]),int(r["stage"]),
                                    int(r["sector"]),int(r["header"])) for r in self.instances.values()})
        for module,image in images:
            row = self.instances[module["name"]]
            offset = 0x449C+int(row["command_word"])%1000*68
            self.assertEqual(offset,int(row["descriptor_offset"]))
            self.assertEqual(hashlib.sha256(image[offset:offset+68]).hexdigest(),row["descriptor_sha256"])
            base = int(module["load_address"],0)
            self.assertEqual(struct.unpack_from("<II",image,0xA0),
                             (0x3C020000|((base+0x449C+0x8000)>>16),0x2442F49C))
            self.assertEqual(struct.unpack_from("<IIII",image,0xB4),
                             (0x00181900,0x00781821,0x00031880,0x00621821))

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
                self.assertEqual(flow["calls"],{base+o for o in (0x1204,0x1778,0x2ACC,0x3750)}
                                 if start==4 else set())
                self.assertLessEqual(flow["external"],set(self.bindings.values()))
            self.assertEqual(reaching_context(image,base),{0xC})
            self.assertEqual(struct.unpack_from("<II",image,0x103C),
                             (0x0C000000|((base+0x1204)>>2&0x3FFFFFF),0x02402021))
            for offset,word in ANCHORS.items():
                self.assertEqual(struct.unpack_from("<I",image,offset)[0],word,hex(offset))
            self.assertEqual(decoder.register_writes(image,0x1204,0x1778,19),[0x120C,0x1760])
            self.assertEqual(decoder.register_writes(image,0x1204,0x1778,18),[0x125C,0x1764])
            self.assertEqual([(o,s) for _,o,s in decoder.direct_stores(image,0x1204,0x1778,18)],
                             [(0,4)]+[(o,1) for o in range(12,18)]*2)
            self.assertEqual([(a,o,s) for a,o,s in decoder.direct_stores(image,0x1204,0x1778,29)
                              if o in (0xD8,0xF0)],
                             [(0x123C,0xD8,4),(0x124C,0xF0,4),(0x1718,0xF0,4)])
        self.assertEqual(0x54C+3*0x1A0,0xA2C)
        self.assertEqual(0x20BC+20,0x20D0)
        self.assertEqual(2*4*6*8,0x180)

    def test_target_layout_and_repeated_header_inclusion(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model417-lines-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' +
                          '#include "../../src/overlays/spanish_model_variant/variant417_lines.h"\n' * 2 +
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
                self.skipTest("Build Spanish MODEL417 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [segment for segment in c_segments(ROOT, ROOT / module["layout"])
                         if "/variant417_lines" in segment["source"]]
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
                own, = original.get_symbol_by_name(f"func_{base+0x1204:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1396, "STT_FUNC"))
                for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                    symbol, = symbols.get_symbol_by_name(f"func_{base+start:X}")
                    owner, = owners[symbol.name]
                    self.assertEqual(owner[0] == selected_path, start == 0x1204)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"]-section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x43A0, 0xC60)):
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
                text = bytearray(compiled.get_section(own["st_shndx"]).data()[:1396])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1396)
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
                        address = base+0x1204+symbol["st_value"]+addend
                        jumps.append((offset, address-base-0x1204))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0x1204:0x1778])
                self.assertEqual(len(calls), 11)
                self.assertEqual(set(calls), {2148029784, 2148039000, 2148024504, 2148047144, 2148038136, 2148033112, 2147860504, 2148039864})
                self.assertEqual(jumps, [(192, 208), (220, 296), (244, 400), (268, 284), (380, 400), (460, 504), (812, 844), (1032, 1044), (1092, 1304), (1144, 1304), (1288, 1304)])

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
                self.assertTrue(context+0x2174 <= base or base+size <= context)
