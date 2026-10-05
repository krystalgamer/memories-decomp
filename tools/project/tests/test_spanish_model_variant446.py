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

BOUNDARIES = (4, 0xE00, 0x12F8, 0x195C, 0x1FE8, 0x24BC, 0x2CAC, 0x36D4)
LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(GsGLINE)", 20),
    ("sizeof(CVECTOR)", 4), ("sizeof(DVECTOR)", 4),
    ("sizeof(Lines446)", 0x1A0), ("O(Lines446,points[1])", 0xC0),
    ("O(Lines446,color)", 0x180), ("O(Lines446,size)", 0x194),
    ("O(Lines446,finished)", 0x198), ("sizeof(Lines446Timing)", 0x34),
    ("O(Lines446Timing,duration)", 0x30), ("O(Lines446State,groups)", 0xB80),
    ("O(Lines446State,line)", 0x1B08), ("O(Lines446State,origin)", 0x1B30),
    ("O(Lines446State,target)", 0x1B5C), ("O(Lines446State,direction_a)", 0x1B68),
    ("O(Lines446State,projected)", 0x1B74), ("O(Lines446State,direction)", 0x1B78),
    ("O(Lines446State,time)", 0x1B94), ("O(Lines446State,step)", 0x1B9C),
    ("O(Lines446State,timing)", 0x1BA4), ("O(Lines446State,phase)", 0x1BDC),
    ("sizeof(Lines446State)", 0x1BE0), ("O(GsGLINE,x0)", 4),
    ("O(GsGLINE,x1)", 8), ("O(GsGLINE,r0)", 12), ("O(GsGLINE,r1)", 15),
)


def reaching_context(image, base):
    writes = set(lifetimes.SpanishModelVariant460Tests.register_writes(image, 4, 0xE00, 18))
    pending, visited, reaching = [(4, None)], set(), set()
    while pending:
        pc, definition = pending.pop()
        if not 4 <= pc < 0xE00 or pc % 4:
            raise AssertionError(f"Entry flow escaped at {pc:#x}")
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
            if pc == 0xCF0:
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


class SpanishModelVariant446Tests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [module for module in manifest["modules"]
                        if module["linker_symbols"].endswith("/model_variant446_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant446-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.bindings = dict((name, int(address, 0)) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config / "overlays/model_variant446_linker_symbols.txt").read_text(), re.M))

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
        self.assertEqual(len(self.modules), 2)
        self.assertEqual(len(self.instances), 2)
        self.assertEqual(len(self.bindings), 35)
        self.assertEqual(len(set(self.bindings.values())), 35)
        totals = load_spanish_overlay_inventories(ROOT)
        checksums = load_checksum_manifest(self.config / "files.sha256")
        with (ROOT / "notes/overlays/spanish-model-variant446-lines-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual([int(row["different_words"]) for row in attempts],
                         [263, 272, 272, 270, 257, 257, 278, 3, 0, 0, 0, 0])
        body = ROOT / "src/overlays/spanish_model_variant/variant446_lines.c"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes()
                                    + (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes()).hexdigest()
        for module in self.modules:
            row = self.instances[module["name"]]
            slot = int(row["slot"])
            base = 0x8013B000 + slot * 0x40000
            self.assertEqual((int(row["model"]), int(row["record"]), int(row["stage"])), (146, 146, 9+slot))
            self.assertEqual((int(row["header"]), int(row["command_word"]), int(row["duration"])),
                             (446+slot*150, 612000, 184))
            self.assertEqual((module["sector_offset"], module["sector_count"]), (40496+slot*10, 10))
            self.assertEqual(module["sector_offset"], int(row["sector"]))
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertNotIn("duplicate_sector_offsets", module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant446_lines" + ("_slot1" if slot else "") + ".c"
            self.assertEqual(json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text()),
                             {"schema": 1, "functions": [{"address": f"0x{base+0xE00:X}", "size": "0x4F8",
                              "profile": "gcc_2_8_1_g0_split", "source": source}]})
            self.assertEqual([segment["source"] for segment in c_segments(ROOT, layout)], [source])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual([(int(item["address"], 0)-base, int(item["size"], 0)) for item in inventory],
                             [(start, end-start) for start, end in zip(BOUNDARIES, BOUNDARIES[1:])])
            self.assertEqual([item["status"] for item in inventory],
                             ["unmatched_asm", "matching_c"] + ["unmatched_asm"] * 5)
            self.assertEqual((totals[layout.stem]["function_count"],
                              totals[layout.stem]["matching_c_function_count"],
                              totals[layout.stem]["matching_c_bytes"]), (7, 1, 1272))
            terminal, = [item for item in attempts[10:] if item["module"] == module["name"]]
            self.assertEqual((terminal["result"], terminal["profile"], terminal["instruction_bytes"]),
                             ("matched", "gcc_2_8_1_g0_split", "1272"))
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
                    sector = record * 276 + 180 + (stage-7)*10
                    handle.seek(sector * 2048)
                    header, = struct.unpack("<I", handle.read(4))
                    if header in (446, 596):
                        observed.add((model, record, stage, sector, header))
                        handle.seek((record*276+275)*2048 + 0x110 + (stage-7)//2*4)
                        self.assertEqual(struct.unpack("<i", handle.read(4)), (612000,))
        self.assertEqual(observed, {(146, 146, 9, 40496, 446), (146, 146, 10, 40506, 596)})
        for module, image in images:
            base = int(module["load_address"], 0)
            self.assertEqual(struct.unpack_from("<I", image, 0x37D0+0x30), (184,))
            self.assertGreaterEqual(0x37D0, BOUNDARIES[-1])
            self.assertLessEqual(0x37D0+76, len(image))
            anchors = {0x98: 0x3C030000 | ((base+0x37D0+0x8000) >> 16), 0x9C: 0x2463E7D0,
                       0xAC: 0x00181080, 0xB0: 0x00581021, 0xB4: 0x00021080,
                       0xB8: 0x00581023, 0xBC: 0x00021080, 0xC4: 0xAFC21BA4}
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))

    def test_closed_cfg_original_context_initialization_and_line_bounds(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import walk_function

        decoder = lifetimes.SpanishModelVariant460Tests
        anchors = {
            0xC: 0x00809021, 0x18: 0x27D80B80, 0x20: 0xAFB80084,
            0x4BC: 0x8FB80084, 0x4C8: 0x2714019C, 0x5A4: 0x265000C0,
            0x5E0: 0x26520008, 0x5FC: 0x2A620006, 0x624: 0x27180030,
            0x63C: 0x2B220004, 0x660: 0x271801A0,
            0x690: 0xA282FFE4, 0x694: 0xA282FFE5, 0x698: 0xA282FFE6,
            0x69C: 0xAE80FFFC, 0x6B0: 0xAE83FFF8, 0x6BC: 0x2B020003,
            0x6C4: 0x269401A0, 0xCE0: 0x8FC21BDC, 0xCE8: 0x1C400003,
            0xCF4: 0x02402021, 0xE48: 0x26711B08,
            0xE5C: 0x26691B0C, 0xE70: 0x266A1B10, 0xE88: 0x26720D14,
            0x10AC: 0x3C025000, 0x10B0: 0xAE220000,
            0x10E0: 0xAFAB0010, 0x10E4: 0xAFA80014, 0x10E8: 0xAFAB0018, 0x10F0: 0xAFA8001C,
            0x113C: 0x18400004, 0x114C: 0x3046FFFF, 0x1160: 0x28420006,
            0x117C: 0x28420004, 0x11B8: 0x8C820030, 0x129C: 0x265201A0,
            0x12BC: 0x28420003,
        }
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                flow = walk_function(image, base, start, end-start)
                self.assertTrue(flow["closed"])
                self.assertEqual(flow["visited"], set(range(start, end, 4)))
                self.assertEqual((flow["returns"], flow["indirect_calls"]), (1, 0))
                self.assertEqual(flow["calls"], {base+offset for offset in BOUNDARIES[1:-1]}
                                 if start == 4 else set())
                self.assertLessEqual(flow["external"], set(self.bindings.values()))
            self.assertEqual(reaching_context(image, base), {0xC})
            self.assertEqual(struct.unpack_from("<I", image, 0xCF0)[0],
                             0x0C000000 | ((base+0xE00) >> 2 & 0x3FFFFFF))
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(decoder.register_writes(image, 0xE00, 0x12F8, 19), [0xE08, 0x12E0])
            self.assertEqual(decoder.register_writes(image, 0xE00, 0x12F8, 17), [0xE48, 0x12E8])
            self.assertEqual([(offset, size) for _, offset, size in
                              decoder.direct_stores(image, 0xE00, 0x12F8, 17)],
                             [(0, 4), *((offset, 1) for offset in (*range(12, 18), *range(12, 18)))])
            self.assertEqual([(site, offset, size) for site, offset, size in
                              decoder.direct_stores(image, 0xE00, 0x12F8, 29) if offset in (0xEC, 0xF0)],
                             [(0xE64, 0xEC, 4), (0xE78, 0xF0, 4)])
        self.assertEqual(2*4*6*8, 0x180)
        self.assertEqual(0xB80+3*0x1A0, 0x1060)
        self.assertLessEqual(0x1B08+20, 0x1B30)

    def test_target_layout_and_repeated_header_inclusion(self):
        if not (ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file() or \
                importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model446-lines-layout-test"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' +
                          '#include "../../src/overlays/spanish_model_variant/variant446_lines.h"\n' * 2 +
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
                self.skipTest("Build Spanish MODEL446 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = c_segments(ROOT, ROOT / module["layout"])
            selected_path = directory / "build" / selected["object"]
            owners = {}
            for path in (directory / "build").rglob("*.o"):
                with path.open("rb") as handle:
                    elf = ELFFile(handle)
                    for symbol in elf.get_section_by_name(".symtab").iter_symbols():
                        if isinstance(symbol["st_shndx"], int) and symbol.name.startswith(("func_", "D_")):
                            section = elf.get_section(symbol["st_shndx"])
                            owners.setdefault(symbol.name, []).append(
                                (path, section.name, section["sh_flags"], symbol["st_size"],
                                 section.data()[symbol["st_value"]:]))
            with linked_path.open("rb") as handle, selected_path.open("rb") as obj_handle:
                linked, compiled = ELFFile(handle), ELFFile(obj_handle)
                symbols, original = linked.get_section_by_name(".symtab"), compiled.get_section_by_name(".symtab")
                own, = original.get_symbol_by_name(f"func_{base+0xE00:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1272, "STT_FUNC"))
                for start, end in zip(BOUNDARIES, BOUNDARIES[1:]):
                    symbol, = symbols.get_symbol_by_name(f"func_{base+start:X}")
                    owner, = owners[symbol.name]
                    self.assertEqual(owner[0] == selected_path, start == 0xE00)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"]-section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x36D4, 0x192C)):
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
                text = bytearray(compiled.get_section(own["st_shndx"]).data()[:1272])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    offset = relocation["r_offset"]
                    self.assertLess(offset, 1272)
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
                        address = base+0xE00+symbol["st_value"]+addend
                        jumps.append((offset, address-base-0xE00))
                    struct.pack_into("<I", text, offset, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[0xE00:0x12F8])
                self.assertEqual(len(calls), 11)
                self.assertEqual(set(calls), {0x8005C018, 0x80089928, 0x80087CB8, 0x800875F8,
                                             0x80086258, 0x80085558, 0x80087958, 0x800840B8})
                self.assertEqual(jumps, [(0xB0, 0xC0), (0xCC, 0x118), (0xE4, 0x180), (0xFC, 0x10C),
                                         (0x16C, 0x180), (0x1BC, 0x1E8), (0x31C, 0x33C),
                                         (0x408, 0x498), (0x438, 0x498), (0x490, 0x49C)])

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
                self.assertTrue(context+0x1BE0 <= base or base+size <= context)
