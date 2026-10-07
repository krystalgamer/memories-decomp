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

BOUNDARIES = (4, 0xE60, 0x157C, 0x1E58, 0x24A4, 0x29B4)
LOADS = {
    (364,9): (314,86864,411,577002), (364,10): (314,86874,561,577002),
    (452,9): (402,111152,411,577001), (452,10): (402,111162,561,577001),
}
LAYOUT = (
    ("sizeof(SVECTOR)",8), ("sizeof(VECTOR)",16), ("sizeof(MATRIX)",32),
    ("sizeof(GsCOORDINATE2)",80), ("sizeof(POLY_GT4)",52),
    ("sizeof(ModelVariantSheet)",0x98), ("O(ModelVariantSheet,v1)",0x20),
    ("O(ModelVariantSheet,v2)",0x40), ("O(ModelVariantSheet,v3)",0x60),
    ("O(ModelVariantSheet,outer)",0x80), ("O(ModelVariantSheet,inner)",0x84),
    ("O(ModelVariantSheet,size)",0x88), ("O(Timing411,grow_start)",0xC),
    ("O(Timing411,grow_end)",0x10), ("O(Timing411,fade_start)",0x18),
    ("O(Timing411,fade_end)",0x1C), ("O(Sheet411State,sheets)",0x54C),
    ("sizeof(((Sheet411State *)0)->sheets)",0x130), ("O(Sheet411State,quad)",0x1478),
    ("O(Sheet411State,origin)",0x15F8), ("O(Sheet411State,path)",0x160C),
    ("O(Sheet411State,frame)",0x1638), ("O(Sheet411State,time)",0x163C),
    ("O(Sheet411State,step)",0x1644), ("O(Sheet411State,timing)",0x164C),
    ("O(Sheet411State,progress)",0x1658), ("O(Sheet411State,fade)",0x166C),
    ("O(Sheet411State,phase)",0x167C), ("sizeof(Sheet411State)",0x1680),
    ("O(POLY_GT4,x0)",8), ("O(POLY_GT4,x1)",20), ("O(POLY_GT4,x2)",32), ("O(POLY_GT4,x3)",44),
    ("O(POLY_GT4,r0)",4), ("O(POLY_GT4,r1)",16), ("O(POLY_GT4,r2)",28), ("O(POLY_GT4,r3)",40),
)
ANCHORS = {
    0xC:0x00809021, 0x14:0x0240F021, 0xCE0:0x02402021,
    0x1E58:0x27BDFF00, 0x1E60:0x00809021, 0x1E68:0x2655054C, 0x1E94:0x26511478, 0x1EA0:0x265005D4,
}


def reaching_context(image, base):
    writes = set(lifetimes.SpanishModelVariant460Tests.register_writes(image,4,0xE60,18))
    pending, visited, reaching = [(4,None)], set(), set()
    while pending:
        pc, definition = pending.pop()
        assert 4 <= pc < 0xE60 and pc % 4 == 0
        if (pc,definition) in visited:
            continue
        visited.add((pc,definition))
        word, = struct.unpack_from("<I",image,pc)
        op = word >> 26
        if pc in writes:
            definition = pc
        if op in (1,2,3,4,5,6,7) or word == 0x03E00008:
            if pc+4 in writes:
                definition = pc+4
            if pc == 0xCDC:
                reaching.add(definition)
            if word == 0x03E00008:
                continue
            if op == 2:
                targets = [((base+pc+4)&0xF0000000|((word&0x3FFFFFF)<<2))-base]
            elif op == 3:
                targets = [pc+8]
            else:
                displacement = (word&65535)-(65536 if word&32768 else 0)
                targets = [pc+8,pc+4+displacement*4]
            pending.extend((target,definition) for target in targets)
        else:
            pending.append((pc+4,definition))
    return reaching


class SpanishModelVariant411Tests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [module for module in manifest["modules"]
                        if module.get("linker_symbols", "").endswith("/model_variant411_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant411-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.bindings = dict((name, int(address, 0)) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config / "overlays/model_variant411_linker_symbols.txt").read_text(), re.M))

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
        self.assertEqual(len(self.modules), 4)
        self.assertEqual(len(self.instances), 4)
        self.assertEqual(len({m["sha256"] for m in self.modules}), 4)
        self.assertEqual(len(self.bindings), 35)
        self.assertEqual(len(set(self.bindings.values())), 35)
        with (ROOT / "notes/overlays/spanish-model-variant411-sheets-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts),6)
        self.assertEqual([(int(r["instruction_bytes"]),int(r["different_words"])) for r in attempts],
                         [(1612,0)]*6)
        self.assertEqual([r["result"] for r in attempts],["matched"]*6)
        self.assertEqual([r["module"] for r in attempts[0:2]],
                         ["spanish_model_variant_364_stage9_slot0","spanish_model_variant_364_stage10_slot1"])
        body = ROOT / "src/overlays/spanish_model_variant/variant411_sheets.c"
        dependency = hashlib.sha256(body.read_bytes()+body.with_suffix(".h").read_bytes()+
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()+
                                    (ROOT / "src/game/gpu_packets.h").read_bytes()).hexdigest()
        checksums = load_checksum_manifest(self.config / "files.sha256")
        totals = load_spanish_overlay_inventories(ROOT)
        for module in self.modules:
            row = self.instances[module["name"]]
            slot = int(row["slot"])
            base = 0x8013B000+slot*0x40000
            key = (int(row["model"]),int(row["stage"]))
            self.assertEqual(slot,(key[1]-9)%2)
            self.assertEqual((int(row["record"]),int(row["sector"]),int(row["header"]),int(row["command_word"])),
                             LOADS[key])
            self.assertEqual((module["sector_offset"],module["sector_count"]), (LOADS[key][1],10))
            self.assertEqual(module["sha256"],row["sha256"])
            self.assertEqual(module["archive_sha256"],checksums[module["archive"]])
            self.assertEqual(int(module["load_address"],0),base)
            self.assertNotIn("duplicate_sector_offsets",module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant411_sheets" + ("_slot1" if slot else "") + ".c"
            self.assertEqual(json.loads(layout.with_name(layout.stem+"_matching_c.json").read_text()),
                             {"schema":1,"functions":[{"address":f"0x{base+0x1E58:X}","size":"0x64C",
                              "profile":"gcc_2_8_1_g0_split","source":source},
                             {"address":f"0x{base+0x24A4:X}","size":"0x510","profile":"gcc_2_8_1_g0_split",
                              "source":source.replace("_sheets","_funnels")}]})
            self.assertEqual([s["source"] for s in c_segments(ROOT,layout)],[source,source.replace("_sheets","_funnels")])
            with layout.with_name(layout.stem+"_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"],0)-base,int(r["size"],0)) for r in rows],
                             [(a,b-a) for a,b in zip(BOUNDARIES,BOUNDARIES[1:])])
            self.assertEqual([r["status"] for r in rows],["unmatched_asm"]*3+["matching_c","matching_c"])
            self.assertEqual((totals[layout.stem]["function_count"],
                              totals[layout.stem]["matching_c_function_count"],
                              totals[layout.stem]["matching_c_bytes"]),(5,2,1612+1296))
            terminal, = [r for r in attempts[2:] if r["module"]==module["name"]]
            self.assertEqual((terminal["result"],terminal["profile"],terminal["instruction_bytes"],
                              terminal["different_words"]),("matched","gcc_2_8_1_g0_split","1612","0"))
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
                    if header in (411,561):
                        observed.add((model,record,stage,sector,header))
                        row, = [r for r in self.instances.values() if int(r["sector"])==sector]
                        handle.seek((record*276+275)*2048+0x110+(stage-7)//2*4)
                        self.assertEqual(struct.unpack("<i",handle.read(4)),(int(row["command_word"]),))
        self.assertEqual(observed,{(int(r["model"]),int(r["record"]),int(r["stage"]),
                                    int(r["sector"]),int(r["header"])) for r in self.instances.values()})
        for module,image in images:
            row = self.instances[module["name"]]
            offset = 0x2AB0+int(row["command_word"])%1000*48
            self.assertEqual(offset,int(row["descriptor_offset"]))
            self.assertEqual(hashlib.sha256(image[offset:offset+48]).hexdigest(),row["descriptor_sha256"])
            base = int(module["load_address"],0)
            self.assertEqual(struct.unpack_from("<II",image,0xA4),
                             (0x3C030000|((base+0x2AB0+0x8000)>>16),0x2463DAB0))
            self.assertEqual(struct.unpack_from("<IIII",image,0xBC),
                             (0x00191040,0x00591021,0x00021100,0x00431021))

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
                self.assertEqual(flow["calls"],{base+o for o in (0xE60,0x1E58,0x24A4)} if start==4 else set())
                self.assertLessEqual(flow["external"],set(self.bindings.values()))
            self.assertEqual(reaching_context(image,base),{0xC})
            self.assertEqual(struct.unpack_from("<II",image,0xCDC),
                             (0x0C000000|((base+0x1E58)>>2&0x3FFFFFF),0x02402021))
            for offset,word in ANCHORS.items():
                self.assertEqual(struct.unpack_from("<I",image,offset)[0],word,hex(offset))
            self.assertEqual(decoder.register_writes(image,0x1E58,0x24A4,18),[0x1E60,0x2490])
            self.assertEqual(decoder.register_writes(image,0x1E58,0x24A4,17),[0x1E94,0x2494])
        self.assertEqual(0x54C+2*0x98,0x67C)
        self.assertEqual(0x1444+52,0x1478)
        for offset in (8,20,32,44):
            self.assertLessEqual(offset+4,52)

    def test_target_layout_and_repeated_header_inclusion(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model411-sheets-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' +
                          '#include "../../src/overlays/spanish_model_variant/variant411_sheets.h"\n' * 2 +
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
                self.skipTest("Build Spanish MODEL411 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [segment for segment in c_segments(ROOT, ROOT / module["layout"])
                         if "/variant411_sheets" in segment["source"]]
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
                own, = original.get_symbol_by_name(f"func_{base+0x1E58:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1612, "STT_FUNC"))
                for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                    symbol, = symbols.get_symbol_by_name(f"func_{base+start:X}")
                    owner, = owners[symbol.name]
                    self.assertEqual(owner[0] == selected_path, start == 0x1E58)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"]-section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x29B4, 0x264C)):
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
                text = bytearray(compiled.get_section(own["st_shndx"]).data()[:1612])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1612)
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
                        address = base+0x1E58+symbol["st_value"]+addend
                        jumps.append((offset, address-base-0x1E58))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0x1E58:0x24A4])
                self.assertEqual(len(calls),10)
                self.assertEqual(set(calls),{2148029784, 2148039000, 2148037288, 2148038136, 2148025000, 2148033112, 2148038456, 2147860504, 2148039864})
                self.assertEqual(jumps,[(164, 316), (848, 992), (1320, 1544), (1344, 1540), (1420, 1544)])

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
                self.assertTrue(context+0x1680 <= base or base+size <= context)
