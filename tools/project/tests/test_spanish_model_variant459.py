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

BOUNDARIES = (4, 0x13E0, 0x19AC, 0x1FB8, 0x2604, 0x3224, 0x3A24, 0x3FE0, 0x4548)
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52),
    ("sizeof(ModelVariantSheet)", 0x98), ("O(ModelVariantSheet,v1)", 0x20),
    ("O(ModelVariantSheet,v2)", 0x40), ("O(ModelVariantSheet,v3)", 0x60),
    ("O(ModelVariantSheet,outer)", 0x80), ("O(ModelVariantSheet,inner)", 0x84),
    ("O(ModelVariantSheet,size)", 0x88), ("O(Timing459,count)", 0x24),
    ("O(Timing459,limit)", 0x28), ("O(Timing459,grow_start)", 0x34),
    ("O(Timing459,grow_end)", 0x38), ("O(Sheet459State,sheets)", 0x2568),
    ("sizeof(((Sheet459State *)0)->sheets)", 0x7B8),
    ("O(Sheet459State,quad)", 0x2D94), ("O(Sheet459State,origins)", 0x2F8C),
    ("O(Sheet459State,paths)", 0x2FEC), ("O(Sheet459State,center)", 0x304C),
    ("O(Sheet459State,frame)", 0x3080), ("O(Sheet459State,time)", 0x3084),
    ("O(Sheet459State,step)", 0x308C), ("O(Sheet459State,timing)", 0x309C),
    ("O(Sheet459State,scale)", 0x30CC), ("O(Sheet459State,progress)", 0x30D0),
    ("O(Sheet459State,phase)", 0x30DC), ("sizeof(Sheet459State)", 0x30E0),
    ("O(POLY_GT4,x0)", 8), ("O(POLY_GT4,x1)", 20),
    ("O(POLY_GT4,x2)", 32), ("O(POLY_GT4,x3)", 44),
    ("O(POLY_GT4,r0)", 4), ("O(POLY_GT4,r1)", 16),
    ("O(POLY_GT4,r2)", 28), ("O(POLY_GT4,r3)", 40),
)
ANCHORS = {
    0xC: 0x0080A021, 0x24: 0x27D82568, 0x30: 0xAFB8008C,
    0x3C: 0x27D92D60, 0x50: 0xAFB9009C, 0xC8: 0xAFC3309C,
    0x314: 0x0C020BBA, 0x374: 0x27390034, 0x378: 0x03202021,
    0x37C: 0x0C020BBA, 0x380: 0xAFB9009C,
    0x38C: 0xA492001A, 0x398: 0xA302000C, 0x39C: 0xA300000D,
    0x3A0: 0xA3110018, 0x3A4: 0xA3000019, 0x3A8: 0xA3020024,
    0x3AC: 0xA3100025, 0x3B0: 0xA3110030, 0x3B4: 0xA3100031,
    0x3B8: 0x0C020B6A, 0x3BC: 0xA713000E, 0x3C4: 0x0C020B76,
    0x958: 0xA062FFF0, 0x96C: 0xA062FFF1, 0x980: 0xA062FFF2,
    0x994: 0xA062FFF4, 0x9A8: 0xA062FFF5, 0x9C4: 0x27390098,
    0x9CC: 0xAC60FFF8, 0x9D8: 0xA062FFF6,
    0x9DC: 0x2B02000D, 0x9E4: 0x24630098,
    0xE4C: 0xAE022F8C, 0xE94: 0xAE022FEC,
    0x1210: 0x0043102B, 0x1214: 0x14400003, 0x1220: 0x02802021,
    0x3A24: 0x27BDFEF8, 0x3A2C: 0x00808821, 0x3A34: 0x26372568,
    0x3A7C: 0x26322D94, 0x3A84: 0x263025F0, 0x3AEC: 0x8622304C,
    0x3B18: 0x8EA32FEC, 0x3BB0: 0x8C422F8C,
    0x3D1C: 0x9203FFFC, 0x3D88: 0x9203FFF8,
    0x3DD4: 0x3066FFFF, 0x3DDC: 0x2A620004,
    0x3E1C: 0xAE000000, 0x3EBC: 0xAE2230DC,
    0x3F0C: 0x0182001B, 0x3F54: 0x862230CC,
    0x3F78: 0x26B50010, 0x3F90: 0x26100098, 0x3FAC: 0x26F70098,
}


def reaching_context(image, base):
    writes = set(lifetimes.SpanishModelVariant460Tests.register_writes(image, 4, 0x13E0, 20))
    pending, visited, reaching = [(4, None)], set(), set()
    while pending:
        pc, definition = pending.pop()
        assert 4 <= pc < 0x13E0 and pc % 4 == 0
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
            if pc == 0x121C:
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


class SpanishModelVariant459Tests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [module for module in manifest["modules"]
                        if module.get("linker_symbols", "").endswith("/model_variant459_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant459-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.bindings = dict((name, int(address, 0)) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config / "overlays/model_variant459_linker_symbols.txt").read_text(), re.M))

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
        self.assertEqual(len(self.modules), 12)
        self.assertEqual(len(self.instances), 12)
        self.assertEqual(len({m["sha256"] for m in self.modules}), 11)
        self.assertEqual(len(self.bindings), 38)
        self.assertEqual(len(set(self.bindings.values())), 38)
        with (ROOT / "notes/overlays/spanish-model-variant459-sheets-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 15)
        self.assertEqual([(int(r["instruction_bytes"]),int(r["different_words"])) for r in attempts[:3]],
                         [(1464,351),(1468,0),(1468,0)])
        self.assertEqual([r["result"] for r in attempts], ["mismatch"]+["matched"]*14)
        self.assertEqual([r["module"] for r in attempts[:3]],
                         ["spanish_model_variant_238_stage7_slot0"]*2 +
                         ["spanish_model_variant_238_stage8_slot1"])
        body = ROOT / "src/overlays/spanish_model_variant/variant459_sheets.c"
        dependency = hashlib.sha256(body.read_bytes()+body.with_suffix(".h").read_bytes()+
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()+
                                    (ROOT / "src/game/gpu_packets.h").read_bytes()).hexdigest()
        checksums = load_checksum_manifest(self.config / "files.sha256")
        totals = load_spanish_overlay_inventories(ROOT)
        loads = {(47,47,7,13152,625000),(231,231,9,63956,625003),
                 (238,238,7,65868,625004),(411,361,7,99816,625001),
                 (417,367,9,101492,625002),(620,570,7,157500,625004)}
        self.assertEqual({(int(r["model"]),int(r["record"]),int(r["stage"]),
                           int(r["sector"]),int(r["command_word"])) for r in self.instances.values()},
                         {(m,r,s+slot,sector+slot*10,command)
                          for m,r,s,sector,command in loads for slot in (0,1)})
        for module in self.modules:
            row = self.instances[module["name"]]
            slot = int(row["slot"])
            base = 0x8013B000+slot*0x40000
            self.assertEqual(int(row["header"]),459+150*slot)
            self.assertEqual((module["sector_offset"],module["sector_count"]), (int(row["sector"]),10))
            self.assertEqual(module["sha256"],row["sha256"])
            self.assertEqual(module["archive_sha256"],checksums[module["archive"]])
            self.assertEqual(int(module["load_address"],0),base)
            self.assertNotIn("duplicate_sector_offsets",module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant459_sheets" + ("_slot1" if slot else "") + ".c"
            self.assertEqual(json.loads(layout.with_name(layout.stem+"_matching_c.json").read_text()),
                             {"schema":1,"functions":[{"address":f"0x{base+0x3A24:X}","size":"0x5BC",
                              "profile":"gcc_2_8_1_g0_split","source":source}]})
            self.assertEqual([s["source"] for s in c_segments(ROOT,layout)],[source])
            with layout.with_name(layout.stem+"_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"],0)-base,int(r["size"],0)) for r in rows],
                             [(a,b-a) for a,b in zip(BOUNDARIES,BOUNDARIES[1:])])
            self.assertEqual([r["status"] for r in rows],["unmatched_asm"]*6+["matching_c","unmatched_asm"])
            for row in (rows[1], rows[3]):
                self.assertIn("not entry-reachable", row["notes"])
            self.assertEqual((totals[layout.stem]["function_count"],
                              totals[layout.stem]["matching_c_function_count"],
                              totals[layout.stem]["matching_c_bytes"]),(8,1,1468))
            terminal, = [r for r in attempts[3:] if r["module"]==module["name"]]
            self.assertEqual((terminal["result"],terminal["profile"],terminal["instruction_bytes"],
                              terminal["different_words"]),("matched","gcc_2_8_1_g0_split","1468","0"))
            self.assertEqual(terminal["fingerprint"],hashlib.sha256((ROOT/source).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"],dependency)

    def test_exhaustive_loads_and_descriptors(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import model_records
        images = list(self.legal_images())
        observed, counts = set(), set()
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as handle:
            for record, model in enumerate(model_records()):
                for stage in range(7,11):
                    sector = record*276+180+(stage-7)*10
                    handle.seek(sector*2048)
                    header, = struct.unpack("<I",handle.read(4))
                    if header in (459,609):
                        observed.add((model,record,stage,sector,header))
                        row, = [r for r in self.instances.values() if int(r["sector"])==sector]
                        handle.seek((record*276+275)*2048+0x110+(stage-7)//2*4)
                        self.assertEqual(struct.unpack("<i",handle.read(4)),(int(row["command_word"]),))
        self.assertEqual(observed,{(int(r["model"]),int(r["record"]),int(r["stage"]),
                                    int(r["sector"]),int(r["header"])) for r in self.instances.values()})
        for module,image in images:
            row = self.instances[module["name"]]
            offset = 0x4644+int(row["command_word"])%1000*72
            self.assertEqual(offset,int(row["descriptor_offset"]))
            self.assertEqual(hashlib.sha256(image[offset:offset+72]).hexdigest(),row["descriptor_sha256"])
            base = int(module["load_address"],0)
            self.assertEqual(struct.unpack_from("<II",image,0xA8),
                             (0x3C020000|((base+0x4644+0x8000)>>16),0x2442F644))
            self.assertEqual(struct.unpack_from("<IIII",image,0xB8),
                             (0x001818C0,0x00781821,0x000318C0,0x00621821))
            count, = struct.unpack_from("<H",image,offset+0x24)
            counts.add(count)
            self.assertLessEqual(count,6)
            self.assertLessEqual(count*2+1,13)
            for i in range(count*2+1):
                self.assertLessEqual(0x2568+(i+1)*0x98,0x2D20)
                if i < count:
                    self.assertLessEqual(0x2F8C+i*16+12,0x2FEC)
                    self.assertLessEqual(0x2FEC+i*16+12,0x304C)
                elif i < count*2:
                    self.assertLessEqual(0x2F8C+(i-count)*16+12,0x2FEC)
        self.assertEqual(counts,{2,3,6})

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
                self.assertEqual(flow["calls"],
                                 {base+o for o in BOUNDARIES[1:-1] if o not in (0x13E0,0x1FB8)}
                                 if start==4 else set())
                self.assertLessEqual(flow["external"],set(self.bindings.values()))
            self.assertEqual(reaching_context(image,base),{0xC})
            self.assertEqual(struct.unpack_from("<II",image,0x121C),
                             (0x0C000000|((base+0x3A24)>>2&0x3FFFFFF),0x02802021))
            for offset,word in ANCHORS.items():
                self.assertEqual(struct.unpack_from("<I",image,offset)[0],word,hex(offset))
            self.assertEqual(decoder.register_writes(image,0x3A24,0x3FE0,17),[0x3A2C,0x3FD0])
            self.assertEqual(decoder.register_writes(image,0x3A24,0x3FE0,18),[0x3A7C,0x3FCC])
            self.assertEqual([(o,s) for _,o,s in decoder.direct_stores(image,0x3A24,0x3FE0,18)],
                             [(o,1) for o in (4,5,6,16,17,18,28,29,30,40,41,42)])
            self.assertEqual([(o,s) for _,o,s in decoder.direct_stores(image,0x3A24,0x3FE0,16)],[(0,4)]*8)
        self.assertEqual(0x2568+13*0x98,0x2D20)
        self.assertEqual(0x2D60+52,0x2D94)
        self.assertEqual(0x2D94+52,0x2DC8)
        for offset in (8,20,32,44):
            self.assertLessEqual(offset+4,52)

    def test_target_layout_and_repeated_header_inclusion(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model459-sheets-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' +
                          '#include "../../src/overlays/spanish_model_variant/variant459_sheets.h"\n' * 2 +
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
                self.skipTest("Build Spanish MODEL459 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [segment for segment in c_segments(ROOT, ROOT / module["layout"])
                         if "/variant459_sheets" in segment["source"]]
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
                own, = original.get_symbol_by_name(f"func_{base+0x3A24:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1468, "STT_FUNC"))
                for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                    symbol, = symbols.get_symbol_by_name(f"func_{base+start:X}")
                    owner, = owners[symbol.name]
                    self.assertEqual(owner[0] == selected_path, start == 0x3A24)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"]-section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x4548, 0xAB8)):
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
                text = bytearray(compiled.get_section(own["st_shndx"]).data()[:1468])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1468)
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
                        address = base+0x3A24+symbol["st_value"]+addend
                        jumps.append((offset, address-base-0x3A24))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0x3A24:0x3FE0])
                self.assertEqual(len(calls), 10)
                self.assertEqual(set(calls), {2148029784, 2148039000, 2148037288, 2148038136, 2148025000, 2148033112, 2148038456, 2147860504, 2148039864})
                self.assertEqual(jumps, [(228, 488), (380, 480), (1012, 1364), (1100, 1364), (1172, 1364), (1316, 1364)])

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
                self.assertTrue(context+0x30E0 <= base or base+size <= context)
