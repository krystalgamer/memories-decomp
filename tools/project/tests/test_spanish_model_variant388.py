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

BOUNDARIES = (4, 0xCD4, 0x1344, 0x19A4, 0x1E9C, 0x2410, 0x2E7C)
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52),
    ("sizeof(Feedback388)", 0x1A8), ("sizeof(((Feedback388 *)0)->points)", 0x198),
    ("O(Feedback388,points[1])", 0x88), ("O(Feedback388,points[2])", 0x110),
    ("O(Feedback388,inner)", 0x198), ("O(Feedback388,outer)", 0x19C),
    ("O(Feedback388,progress)", 0x1A0), ("O(Feedback388State,groups)", 0x5C8),
    ("O(Feedback388State,quad)", 0x175C), ("O(Feedback388State,origin)", 0x1824),
    ("O(Feedback388State,direction)", 0x182C), ("O(Feedback388State,step)", 0x1864),
    ("O(Feedback388State,phase)", 0x1898), ("sizeof(Feedback388State)", 0x189C),
    ("O(POLY_GT4,x0)", 8), ("O(POLY_GT4,x1)", 20), ("O(POLY_GT4,x2)", 32),
    ("O(POLY_GT4,x3)", 44), ("O(POLY_GT4,u0)", 12), ("O(POLY_GT4,v0)", 13),
    ("O(POLY_GT4,u1)", 24), ("O(POLY_GT4,v1)", 25), ("O(POLY_GT4,u2)", 36),
    ("O(POLY_GT4,v2)", 37), ("O(POLY_GT4,u3)", 48), ("O(POLY_GT4,v3)", 49),
    ("O(POLY_GT4,r0)", 4), ("O(POLY_GT4,r1)", 16), ("O(POLY_GT4,r2)", 28),
    ("O(POLY_GT4,r3)", 40), ("O(POLY_GT4,tpage)", 26),
)
ANCHORS = {
    0xC: 0x00809021, 0x1C: 0x27D905C8, 0x54: 0x27D9175C,
    0x6C8: 0xA6030088, 0x700: 0xA6030110, 0x708: 0x26100008,
    0x710: 0x00138A00, 0x720: 0x2A620011, 0x764: 0x271801A8,
    0x76C: 0xAE400000, 0x780: 0xAE42FFFC, 0x784: 0x2A820005,
    0xB58: 0x8FC21898, 0xB60: 0x28420002, 0xB64: 0x14400003,
    0x1E9C: 0x27BDFEE8, 0x1EA4: 0x0080A021, 0x1ED4: 0xAFAA00E0,
    0x1ED8: 0x0C0214AA, 0x1EE8: 0x269505C8, 0x1F20: 0x2691175C,
    0x1F40: 0x26B201A0, 0x20A8: 0x26660110, 0x20B0: 0x26670118,
    0x20E4: 0x0C021E56, 0x20F4: 0x284200A0,
    0x211C: 0x24060140, 0x2130: 0x3050FFFF, 0x2134: 0x0C020BBA,
    0x2190: 0x24060080, 0x21A0: 0x240601C0, 0x21B4: 0x3050FFFF,
    0x21B8: 0x0C020BBA, 0x221C: 0x26660088, 0x2224: 0x26670090,
    0x2258: 0x0C021E56, 0x2268: 0x0C020B6A, 0x2274: 0x0C020B76,
    0x2278: 0x00002821, 0x2304: 0x06000008, 0x230C: 0x8FA200D4,
    0x2314: 0x04400004, 0x2324: 0x3206FFFF, 0x2334: 0x2BC20010,
    0x2358: 0xAFA000E0, 0x235C: 0x00021140, 0x236C: 0xAE430000,
    0x2388: 0xAE420000, 0x23C0: 0xAE821898, 0x23C4: 0x265201A8,
    0x23D4: 0x29420005,
}


def reaching_context(image, base):
    writes = set(lifetimes.SpanishModelVariant460Tests.register_writes(image, 4, 0xCD4, 18))
    pending, visited, reaching = [(4, None)], set(), set()
    while pending:
        pc, definition = pending.pop()
        assert 4 <= pc < 0xCD4 and pc % 4 == 0
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
            if pc == 0xB6C:
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


class SpanishModelVariant388Tests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [module for module in manifest["modules"]
                        if module.get("linker_symbols", "").endswith("/model_variant388_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant388-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.bindings = dict((name, int(address, 0)) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config / "overlays/model_variant388_linker_symbols.txt").read_text(), re.M))

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
        self.assertEqual(len(self.modules), 10)
        self.assertEqual(len(self.instances), 10)
        self.assertEqual(len({m["sha256"] for m in self.modules}), 8)
        self.assertEqual(len(self.bindings), 38)
        self.assertEqual(len(set(self.bindings.values())), 36)
        with (ROOT / "notes/overlays/spanish-model-variant388-feedback-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual([int(r["different_words"]) for r in attempts], [2] + [0]*12)
        self.assertEqual({r["instruction_bytes"] for r in attempts}, {"1396"})
        self.assertEqual([r["module"] for r in attempts[:3]],
                         ["spanish_model_variant_235_stage9_slot0"]*2 +
                         ["spanish_model_variant_235_stage10_slot1"])
        body = ROOT / "src/overlays/spanish_model_variant/variant388_feedback.c"
        dependency = hashlib.sha256(body.read_bytes()+body.with_suffix(".h").read_bytes()+
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        checksums = load_checksum_manifest(self.config / "files.sha256")
        totals = load_spanish_overlay_inventories(ROOT)
        expected = {8:(8,7,2388,554005), 43:(43,9,12068,554002),
                    235:(235,9,65060,554001), 269:(269,9,74444,554003),
                    706:(606,9,167456,554004)}
        for module in self.modules:
            row = self.instances[module["name"]]
            slot, model = int(row["slot"]), int(row["model"])
            record, stage, sector, command = expected[model]
            base = 0x8013B000 + slot*0x40000
            self.assertEqual((int(row["record"]),int(row["stage"]),int(row["sector"]),
                              int(row["header"]),int(row["command_word"])),
                             (record,stage+slot,sector+slot*10,388+150*slot,command))
            self.assertEqual((module["sector_offset"],module["sector_count"]), (sector+slot*10,10))
            self.assertEqual(module["sha256"],row["sha256"])
            self.assertEqual(module["archive_sha256"],checksums[module["archive"]])
            self.assertEqual(int(module["load_address"],0),base)
            self.assertNotIn("duplicate_sector_offsets",module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant388_feedback" + ("_slot1" if slot else "") + ".c"
            self.assertEqual(json.loads(layout.with_name(layout.stem+"_matching_c.json").read_text()),
                             {"schema":1,"functions":[{"address":f"0x{base+0x1344:X}","size":"0x660",
                              "profile":"gcc_2_8_1_g0_split",
                              "source":source.replace("variant388_feedback","variant388_ribbons")},
                              {"address":f"0x{base+0x1E9C:X}","size":"0x574",
                              "profile":"gcc_2_8_1_g0_split","source":source}]})
            self.assertEqual([s["source"] for s in c_segments(ROOT,layout)],
                             [source.replace("variant388_feedback","variant388_ribbons"),source])
            with layout.with_name(layout.stem+"_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"],0)-base,int(r["size"],0)) for r in rows],
                             [(a,b-a) for a,b in zip(BOUNDARIES,BOUNDARIES[1:])])
            self.assertEqual([r["status"] for r in rows],["unmatched_asm"]*2+["matching_c"]+["unmatched_asm"]+["matching_c","unmatched_asm"])
            self.assertEqual((totals[layout.stem]["function_count"],
                              totals[layout.stem]["matching_c_function_count"],
                              totals[layout.stem]["matching_c_bytes"]),(6,2,3028))
            terminal, = [r for r in attempts[3:] if r["module"]==module["name"]]
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
                    if header in (388,538):
                        observed.add((model,record,stage,sector,header))
                        row, = [r for r in self.instances.values() if int(r["sector"])==sector]
                        handle.seek((record*276+275)*2048+0x110+(stage-7)//2*4)
                        self.assertEqual(struct.unpack("<i",handle.read(4)),(int(row["command_word"]),))
        self.assertEqual(observed,{(int(r["model"]),int(r["record"]),int(r["stage"]),
                                    int(r["sector"]),int(r["header"])) for r in self.instances.values()})
        for module,image in images:
            row = self.instances[module["name"]]
            offset = 0x2F78+int(row["command_word"])%1000*36
            self.assertEqual(offset,int(row["descriptor_offset"]))
            self.assertEqual(hashlib.sha256(image[offset:offset+36]).hexdigest(),row["descriptor_sha256"])
            base = int(module["load_address"],0)
            self.assertEqual(struct.unpack_from("<II",image,0x98),
                             (0x3C020000|((base+0x2F78+0x8000)>>16),0x2442DF78))
            self.assertEqual(struct.unpack_from("<IIII",image,0xAC),
                             (0x001918C0,0x00791821,0x00031880,0x00621821))

    def test_original_context_cfg_packet_sampling_and_initialization(self):
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
            self.assertEqual(reaching_context(image,base),{0xC})
            self.assertEqual(struct.unpack_from("<II",image,0xB6C),
                             (0x0C000000|((base+0x1E9C)>>2&0x3FFFFFF),0x02402021))
            for offset,word in ANCHORS.items():
                self.assertEqual(struct.unpack_from("<I",image,offset)[0],word,hex(offset))
            self.assertEqual(decoder.register_writes(image,0x1E9C,0x2410,20),[0x1EA4,0x23F4])
            self.assertEqual(decoder.register_writes(image,0x1E9C,0x2410,17),[0x1F20,0x2400])
            stores = [(26,2),*((o,1) for o in (12,13,24,25,36,37,48,49)),
                      (12,1),(13,1),(24,1),(25,1),(36,1),(26,2),(37,1),(48,1),(49,1),
                      *((o,1) for o in (4,5,6,16,17,18,28,29,30,40,41,42))]
            self.assertEqual([(o,s) for _,o,s in decoder.direct_stores(image,0x1E9C,0x2410,17)],stores)
            self.assertEqual([(a,o,s) for a,o,s in decoder.direct_stores(image,0x1E9C,0x2410,29) if o==0xE0],
                             [(0x1ED4,0xE0,4),(0x2358,0xE0,4)])
        self.assertEqual(0x5C8+5*0x1A8,0xE10)
        self.assertEqual(0x175C+52,0x1790)
        self.assertEqual(3*17*8,0x198)

    def test_target_layout_and_repeated_header_inclusion(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model388-feedback-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' +
                          '#include "../../src/overlays/spanish_model_variant/variant388_feedback.h"\n' * 2 +
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
            self.assertEqual(elf.get_section(symbol["st_shndx"]).data(),
                             struct.pack("<" + "I"*len(LAYOUT), *(value for _, value in LAYOUT)))

    def test_all_input_final_owners_and_every_selected_relocation_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / "tmp/overlays" / module["name"]
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL388 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [segment for segment in c_segments(ROOT, ROOT / module["layout"])
                         if "/variant388_feedback" in segment["source"]]
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
                own, = original.get_symbol_by_name(f"func_{base+0x1E9C:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1396, "STT_FUNC"))
                for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                    symbol, = symbols.get_symbol_by_name(f"func_{base+start:X}")
                    owner, = owners[symbol.name]
                    self.assertEqual(owner[0] == selected_path, start == 0x1E9C)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"]-section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x2E7C, 0x2184)):
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
                        address = base+0x1E9C+symbol["st_value"]+addend
                        jumps.append((offset, address-base-0x1E9C))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0x1E9C:0x2410])
                self.assertEqual(len(calls), 18)
                self.assertEqual(set(calls), {0x8005C018, 0x800852A8, 0x80089928, 0x80086628,
                                             0x80087CB8, 0x800875F8, 0x80086258, 0x80085558,
                                             0x80087958, 0x80082CE8, 0x80082EE8, 0x80082DA8,
                                             0x80082DD8, 0x800842A8})
                self.assertEqual(jumps, [(0x90, 0xA0), (0x27C, 0x28C),
                                         (0x2E0, 0x378), (0x300, 0x310)])

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
                self.assertTrue(context+0x1034 <= base or base+size <= context)
