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

BOUNDARIES = (4, 0x11BC, 0x173C, 0x1DDC, 0x23FC, 0x2A48, 0x3060, 0x3630, 0x414C, 0x4754)
ENTRY_CALLS = {0x173C, 0x2A48, 0x3060, 0x414C}
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(GsGLINE)", 20),
    ("sizeof(CVECTOR)", 4), ("sizeof(DVECTOR)", 4),
    ("sizeof(Lines441)", 0x1A0), ("O(Lines441,points[1])", 0xC0),
    ("O(Lines441,color)", 0x180), ("O(Lines441,size)", 0x194),
    ("sizeof(Lines441Timing)", 0x24), ("O(Lines441Timing,start)", 0x1C),
    ("O(Lines441Timing,end)", 0x20), ("O(Lines441State,groups)", 0xAF8),
    ("O(Lines441State,line)", 0x2668), ("O(Lines441State,origin)", 0x2690),
    ("O(Lines441State,target)", 0x269C), ("O(Lines441State,direction_a)", 0x26A8),
    ("O(Lines441State,projected)", 0x26B4), ("O(Lines441State,direction)", 0x26B8),
    ("O(Lines441State,time)", 0x26D4), ("O(Lines441State,step)", 0x26DC),
    ("O(Lines441State,timing)", 0x26E4), ("O(Lines441State,phase)", 0x271C),
    ("sizeof(Lines441State)", 0x2720), ("O(GsGLINE,x0)", 4),
    ("O(GsGLINE,x1)", 8), ("O(GsGLINE,r0)", 12), ("O(GsGLINE,r1)", 15),
)

class SpanishModelVariant441Tests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [module for module in manifest["modules"]
                        if module["linker_symbols"].endswith("/model_variant441_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant441-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.bindings = dict((name, int(address, 0)) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config / "overlays/model_variant441_linker_symbols.txt").read_text(), re.M))

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

    def test_metadata_inventory_and_terminal_fingerprints(self):
        self.assertEqual(len(self.modules), 10)
        self.assertEqual(len(self.instances), 10)
        self.assertEqual(len(self.bindings), 37)
        self.assertEqual(len(set(self.bindings.values())), 37)
        totals = load_spanish_overlay_inventories(ROOT)
        checksums = load_checksum_manifest(self.config / "files.sha256")
        with (ROOT / "notes/overlays/spanish-model-variant441-lines-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 12)
        self.assertEqual({row["different_words"] for row in attempts}, {"0"})
        self.assertEqual({row["result"] for row in attempts}, {"matched"})
        body = ROOT / "src/overlays/spanish_model_variant/variant441_lines.c"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes()
                                    + (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        expected = {166: (166, 7, 45996, 607000, 46, 76), 360: (310, 7, 85740, 607002, 60, 168),
                    487: (437, 7, 120792, 607003, 50, 128), 590: (540, 7, 149220, 607000, 46, 76),
                    709: (609, 9, 168284, 607004, 0, 84)}
        for module in self.modules:
            row = self.instances[module["name"]]
            slot, model = int(row["slot"]), int(row["model"])
            base = 0x8013B000 + slot*0x40000
            record, stage, sector, command, start_time, end_time = expected[model]
            self.assertEqual((int(row["record"]), int(row["stage"]), int(row["sector"])),
                             (record, stage+slot, sector+slot*10))
            self.assertEqual((int(row["header"]), int(row["command_word"]), int(row["start_time"]),
                              int(row["end_time"])), (441+slot*150, command, start_time, end_time))
            self.assertEqual((module["sector_offset"], module["sector_count"]), (sector+slot*10, 10))
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertNotIn("duplicate_sector_offsets", module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant441_lines" + ("_slot1" if slot else "") + ".c"
            self.assertEqual(json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text()),
                             {"schema": 1, "functions": [{"address": f"0x{base+0x11BC:X}", "size": "0x580",
                              "profile": "gcc_2_8_1_g0_split", "source": source}]})
            self.assertEqual([segment["source"] for segment in c_segments(ROOT, layout)], [source])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual([(int(item["address"], 0)-base, int(item["size"], 0)) for item in inventory],
                             [(a, b-a) for a, b in zip(BOUNDARIES, BOUNDARIES[1:])])
            self.assertEqual([item["status"] for item in inventory],
                             ["unmatched_asm", "matching_c"] + ["unmatched_asm"]*7)
            self.assertIn("no in-image direct caller", inventory[1]["notes"])
            self.assertEqual((totals[layout.stem]["function_count"],
                              totals[layout.stem]["matching_c_function_count"],
                              totals[layout.stem]["matching_c_bytes"]), (9, 1, 1408))
            terminal, = [item for item in attempts[2:] if item["module"] == module["name"]]
            self.assertEqual((terminal["profile"], terminal["instruction_bytes"]),
                             ("gcc_2_8_1_g0_split", "1408"))
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / source).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)

    def test_exhaustive_physical_loads_and_actual_descriptor(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import model_records

        images = list(self.legal_images())
        observed = set()
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as handle:
            for record, model in enumerate(model_records()):
                for stage in range(7, 11):
                    sector = record*276 + 180 + (stage-7)*10
                    handle.seek(sector*2048)
                    header, = struct.unpack("<I", handle.read(4))
                    if header in (441, 591):
                        observed.add((model, record, stage, sector, header))
                        row, = [r for r in self.instances.values() if int(r["sector"]) == sector]
                        handle.seek((record*276+275)*2048 + 0x110 + (stage-7)//2*4)
                        self.assertEqual(struct.unpack("<i", handle.read(4)), (int(row["command_word"]),))
        self.assertEqual(observed, {(int(r["model"]), int(r["record"]), int(r["stage"]),
                                    int(r["sector"]), int(r["header"])) for r in self.instances.values()})
        for module, image in images:
            row = self.instances[module["name"]]
            base = int(module["load_address"], 0)
            descriptor = 0x4850 + int(row["command_word"]) % 1000 * 48
            self.assertEqual(struct.unpack_from("<ii", image, descriptor+0x1C),
                             (int(row["start_time"]), int(row["end_time"])))
            self.assertGreater(int(row["end_time"]), int(row["start_time"]))
            self.assertGreaterEqual(descriptor, BOUNDARIES[-1])
            self.assertLessEqual(descriptor+48, len(image))
            self.assertEqual(struct.unpack_from("<II", image, 0xA8),
                             (0x3C020000 | ((base+0x4850+0x8000) >> 16), 0x2442F850))

    def test_closed_cfg_retained_helper_initialization_and_packet_bounds(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import walk_function

        decoder = lifetimes.SpanishModelVariant460Tests
        anchors = {
            0xC: 0x00809021, 0x14: 0x0240B021, 0x18: 0x26D80AF8,
            0x1C: 0xAFB80084, 0xB4: 0x8FB800F4, 0xBC: 0x00181840,
            0xC0: 0x00781821, 0xC4: 0x00031900, 0xCC: 0xAEC326E4,
            0x78C: 0x271801A0, 0x7C8: 0x2B020003, 0x7D0: 0x269401A0,
            0x11C4: 0x00809821, 0x11C8: 0x26680AF8, 0x120C: 0x26712668,
            0x1220: 0x266A266C, 0x1234: 0x266B2670, 0x124C: 0x26720C8C,
            0x149C: 0x00803021, 0x14A0: 0x00A03821,
            0x14A4: 0xAFAB0010, 0x14A8: 0xAFA80014, 0x14AC: 0xAFAB0018, 0x14B4: 0xAFA8001C,
            0x1500: 0x04C0000A, 0x1508: 0x8FA200D4, 0x1510: 0x04400006,
            0x1524: 0x30C6FFFF, 0x1538: 0x28420006, 0x1554: 0x28420004,
            0x16D4: 0xAE62271C, 0x16DC: 0xAFA000E8, 0x16E0: 0x265201A0, 0x1700: 0x28420003,
        }
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                flow = walk_function(image, base, start, end-start)
                self.assertTrue(flow["closed"])
                self.assertEqual(flow["visited"], set(range(start, end, 4)))
                self.assertEqual((flow["returns"], flow["indirect_calls"]), (1, 0))
                self.assertEqual(flow["calls"], {base+o for o in ENTRY_CALLS} if start == 4 else set())
                self.assertLessEqual(flow["external"], set(self.bindings.values()))
            target = base+0x11BC
            for offset in range(0, len(image), 4):
                word, = struct.unpack_from("<I", image, offset)
                self.assertNotEqual(word, target)
                if word >> 26 in (2, 3):
                    destination = ((base+offset+4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    self.assertNotEqual(destination, target)
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(decoder.register_writes(image, 0x11BC, 0x173C, 19), [0x11C4, 0x1724])
            self.assertEqual(decoder.register_writes(image, 0x11BC, 0x173C, 17), [0x120C, 0x172C])
            self.assertEqual([(o, s) for _, o, s in decoder.direct_stores(image, 0x11BC, 0x173C, 17)],
                             [(0, 4), *((o, 1) for o in (*range(12, 18), *range(12, 18)))])
            self.assertEqual([(a, o, s) for a, o, s in decoder.direct_stores(image, 0x11BC, 0x173C, 29)
                              if o in (0xF0, 0xF4)], [(0x1228, 0xF0, 4), (0x123C, 0xF4, 4)])
        self.assertEqual(2*4*6*8, 0x180)
        self.assertEqual(0xAF8+3*0x1A0, 0xFD8)
        self.assertEqual(0x2668+20, 0x267C)
        self.assertLess(0x267C, 0x2690)

    def test_target_layout_and_repeated_header_inclusion(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model441-lines-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' +
                          '#include "../../src/overlays/spanish_model_variant/variant441_lines.h"\n' * 2 +
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
                self.skipTest("Build Spanish MODEL441 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [segment for segment in c_segments(ROOT, ROOT / module["layout"])
                         if "/variant441_lines" in segment["source"]]
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
                own, = original.get_symbol_by_name(f"func_{base+0x11BC:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1408, "STT_FUNC"))
                for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                    symbol, = symbols.get_symbol_by_name(f"func_{base+start:X}")
                    owner, = owners[symbol.name]
                    self.assertEqual(owner[0] == selected_path, start == 0x11BC)
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
                text = bytearray(compiled.get_section(own["st_shndx"]).data()[:1408])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1408)
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
                        address = base+0x11BC+symbol["st_value"]+addend
                        jumps.append((offset, address-base-0x11BC))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0x11BC:0x173C])
                self.assertEqual(len(calls), 11)
                self.assertEqual(set(calls), {0x8005C018, 0x80089928, 0x80087CB8, 0x800875F8,
                                             0x80086258, 0x80085558, 0x80087958, 0x800840B8})
                self.assertEqual(jumps, [(0xB8, 0xC8), (0xD4, 0x120), (0xEC, 0x188), (0x104, 0x114),
                                         (0x174, 0x188), (0x1C4, 0x1F0), (0x324, 0x344),
                                         (0x450, 0x524), (0x484, 0x524), (0x514, 0x524)])

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
                self.assertTrue(context+0x2720 <= base or base+size <= context)
