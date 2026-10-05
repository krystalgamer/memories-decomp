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
from verify_inputs import load_checksum_manifest
from tools.project.tests import test_spanish_model_variant460 as lifetimes

LAYOUT = (
    ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
    ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52), ("sizeof(ModelVariantSheet)", 152),
    ("O(ModelVariantSheet,v1)", 0x20), ("O(ModelVariantSheet,v2)", 0x40),
    ("O(ModelVariantSheet,v3)", 0x60), ("O(ModelVariantSheet,outer)", 0x80),
    ("O(ModelVariantSheet,inner)", 0x84), ("O(ModelVariantSheet,size)", 0x88),
    ("sizeof(Sheet438Timing)", 36), ("O(Sheet438Timing,grow_start)", 0x1C),
    ("O(Sheet438Timing,grow_end)", 0x20),
    ("O(Sheet438State,sheets)", 0x1938), ("O(Sheet438State,polygon)", 0x210C),
    ("O(Sheet438State,origin)", 0x2258), ("O(Sheet438State,target)", 0x2264),
    ("O(Sheet438State,direction)", 0x226C), ("O(Sheet438State,frame)", 0x2298),
    ("O(Sheet438State,time)", 0x229C), ("O(Sheet438State,step)", 0x22A4),
    ("O(Sheet438State,timing)", 0x22B4), ("O(Sheet438State,fixed_size)", 0x22D8),
    ("O(Sheet438State,progress)", 0x22DC), ("O(Sheet438State,phase)", 0x22E8),
    ("sizeof(Sheet438State)", 0x22EC),
    ("O(POLY_GT4,x0)", 8), ("O(POLY_GT4,x1)", 20),
    ("O(POLY_GT4,x2)", 32), ("O(POLY_GT4,x3)", 44),
)
ANCHORS = {
    0xC: 0x00809821, 0x14: 0x0260F021, 0x24: 0x27D81938, 0x30: 0xAFB8008C,
    0x54: 0x27D720D8, 0xCC: 0x001818C0, 0xD0: 0x00781821, 0xD4: 0x000318C0,
    0xDC: 0xAFC322B4, 0x2C8: 0x26F70034, 0x2CC: 0x0C020BBA, 0x2D0: 0x02E02021,
    0xA24: 0x27100020, 0xA28: 0x270F0040, 0xA2C: 0x270E0060,
    0xCCC: 0x27390098, 0xCE4: 0x2B020002, 0xCEC: 0x24630098,
    0x1464: 0x02602021, 0x3DF0: 0x00808821, 0x3DF8: 0x263E1938,
    0x3E20: 0x2632210C, 0x3E28: 0x263019C0, 0x3EA4: 0x28620400,
    0x3F3C: 0x86222264, 0x3F48: 0x86222266, 0x3F54: 0x86222268,
    0x41A8: 0x0043001B, 0x41B4: 0x0007000D, 0x41DC: 0x862222D8,
    0x4238: 0x00021280, 0x4298: 0x00021180, 0x42B4: 0x26100098,
    0x42BC: 0x27DE0098, 0x42C4: 0x29020002,
}
PHYSICAL = ((147, 147, 7, 40752, 438, 604001),
 (147, 147, 8, 40762, 588, 604001),
 (211, 211, 7, 58416, 438, 604002),
 (211, 211, 8, 58426, 588, 604002),
 (263, 263, 9, 72788, 438, 604000),
 (263, 263, 10, 72798, 588, 604000),
 (525, 475, 9, 131300, 438, 604003),
 (525, 475, 10, 131310, 588, 604003),
 (610, 560, 7, 154740, 438, 604002),
 (610, 560, 8, 154750, 588, 604002),
 (632, 582, 9, 160832, 438, 604000),
 (632, 582, 10, 160842, 588, 604000))


def reaching_context(image, base):
    definitions = set(lifetimes.SpanishModelVariant460Tests.register_writes(image, 4, 0x1614, 19))
    pending, visited, reaching = [(4, None)], set(), set()
    while pending:
        pc, definition = pending.pop()
        assert 4 <= pc < 0x1614 and pc % 4 == 0
        if (pc, definition) in visited:
            continue
        visited.add((pc, definition))
        if pc in definitions:
            definition = pc
        if pc == 0x1460:
            reaching.add(definition)
        word, = struct.unpack_from("<I", image, pc)
        op = word >> 26
        if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
            if pc + 4 in definitions:
                definition = pc + 4
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


class SpanishModelVariant438Tests(unittest.TestCase):
    boundaries = (4, 0x1614, 0x1B5C, 0x23E8, 0x2A34, 0x3638, 0x3DE8, 0x4300, 0x4868)

    def setUp(self):
        manifest = json.loads((ROOT / "config/sles_03951/overlays.json").read_text())
        self.modules = [module for module in manifest["modules"]
                        if module["linker_symbols"].endswith("/model_variant438_linker_symbols.txt")]
        self.assertEqual(len(self.modules), 12)

    def legal_images(self):
        path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.is_file():
            self.skipTest("Legal Spanish MODEL input required")
        with path.open("rb") as handle:
            for module in self.modules:
                handle.seek(module["sector_offset"] * 2048)
                image = handle.read(20480)
                self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
                yield module, image

    def test_exhaustive_physical_inputs_and_function_inventory(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import model_records, walk_function

        checksums = load_checksum_manifest(ROOT / "config/sles_03951/files.sha256")
        with (ROOT / "notes/overlays/spanish-model-variant438-instances.csv").open() as handle:
            instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.assertEqual(set(instances), {module["name"] for module in self.modules})
        observed = set()
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as handle:
            for record, model in enumerate(model_records()):
                for stage in range(7, 11):
                    sector = record * 276 + 180 + (stage - 7) * 10
                    handle.seek(sector * 2048)
                    header, = struct.unpack("<I", handle.read(4))
                    if header in (438, 588):
                        handle.seek((record * 276 + 275) * 2048 + 0x110 + (stage - 7) // 2 * 4)
                        command, = struct.unpack("<i", handle.read(4))
                        observed.add((model, record, stage, sector, header, command))
        self.assertEqual(observed, set(PHYSICAL))
        for module, image in self.legal_images():
            row = instances[module["name"]]
            slot = int(row["slot"])
            base = 0x8013B000 + slot * 0x40000
            self.assertIn(tuple(int(row[key]) for key in
                                ("model", "record", "stage", "sector", "header", "command_word")), PHYSICAL)
            self.assertEqual(module["sector_offset"], int(row["sector"]))
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertEqual(struct.unpack_from("<I", image)[0], 438 + slot * 150)
            layout = ROOT / module["layout"]
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            spans = list(zip(self.boundaries, self.boundaries[1:]))
            self.assertEqual([(int(item["address"], 0)-base, int(item["size"], 0)) for item in inventory],
                             [(start, end-start) for start, end in spans])
            self.assertEqual([item["status"] for item in inventory],
                             ["matching_c" if start == 0x3DE8 else "unmatched_asm" for start, _ in spans])
            selected, = c_segments(ROOT, layout)
            source = "src/overlays/spanish_model_variant/variant438_sheets" + ("_slot1" if slot else "") + ".c"
            self.assertEqual(selected["source"], source)
            self.assertEqual(json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text()),
                             {"schema": 1, "functions": [{"address": f"0x{base+0x3DE8:X}", "size": "0x518",
                              "profile": "gcc_2_8_1_g0_split", "source": source}]})
            for start, end in spans:
                flow = walk_function(image, base, start, end-start)
                self.assertTrue(flow["closed"])
                self.assertEqual(flow["visited"], set(range(start, end, 4)))
                self.assertEqual((flow["returns"], flow["indirect_calls"]), (1, 0))
                self.assertTrue(flow["calls"] <= {base+offset for offset in self.boundaries[:-1]})
                if start == 4:
                    self.assertIn(base + 0x3DE8, flow["calls"])
                    self.assertNotIn(base + 0x23E8, flow["calls"])
            command = int(row["command_word"])
            self.assertEqual(command // 1000, 604)
            descriptor = 0x4964 + command % 1000 * 72
            self.assertLessEqual(descriptor + 72, len(image))
            timings = ((0, 60), (0, 48), (0, 102), (20, 48))
            self.assertEqual(struct.unpack_from("<II", image, descriptor + 0x1C), timings[command % 1000])

    def test_original_context_initialized_packet_and_sheet_bounds(self):
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            self.assertEqual(reaching_context(image, base), {0xC})
            self.assertEqual(struct.unpack_from("<I", image, 0x1460)[0],
                             0x0C000000 | ((base + 0x3DE8) >> 2 & 0x3FFFFFF))
            for offset, word in ANCHORS.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            decoder = lifetimes.SpanishModelVariant460Tests
            self.assertEqual(decoder.register_writes(image, 0x54, 0x2D4, 23), [0x54, 0x2C8])
            self.assertEqual(decoder.register_writes(image, 0x3DE8, 0x4300, 18), [0x3E20, 0x42EC])
            self.assertEqual(sorted((off, size) for _, off, size in
                                    decoder.direct_stores(image, 0x3DE8, 0x4300, 18)),
                             [(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
        self.assertEqual(0x1938 + 2 * 152, 0x1A68)
        self.assertEqual(0x60 + 3 * 8 + 8, 0x80)
        self.assertEqual(0x20D8 + 52, 0x210C)
        self.assertEqual(0x210C + 52, 0x2140)

    def test_target_compiled_layout_and_terminal_fingerprints(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        directory = ROOT / "tmp/model438-layout-tests"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n'
                          '#include "../../src/overlays/spanish_model_variant/variant438_sheets.h"\n'
                          '#define O(t,f) ((u32)&((t *)0)->f)\n'
                          'u32 layout[] = {' + ",".join(expression for expression, _ in LAYOUT) + "};\n")
        obj = compile_c(ROOT, tool(ROOT, "as"),
                        {"source": str(source.relative_to(ROOT)), "profile": "gcc_2_8_1_g0_split",
                         "object": "layout.o"}, load_compiler_profiles(ROOT),
                        object_directory=str(directory.relative_to(ROOT)),
                        asm_directory=str(directory.relative_to(ROOT)))
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
            section = elf.get_section(symbol["st_shndx"])
            self.assertEqual((symbol["st_value"], section["sh_size"]), (0, 128))
            self.assertEqual(section.data(), struct.pack("<32I", *(value for _, value in LAYOUT)))
        with (ROOT / "notes/overlays/spanish-model-variant438-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([row["different_words"] for row in rows], ["0"] * 14)
        body = ROOT / "src/overlays/spanish_model_variant/variant438_sheets.c"
        shared = ROOT / "src/overlays/model_variant/model_variant.h"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes()
                                    + shared.read_bytes()).hexdigest()
        for module in self.modules:
            terminal = [row for row in rows if row["module"] == module["name"]][-1]
            selected, = c_segments(ROOT, ROOT / module["layout"])
            self.assertEqual((terminal["result"], terminal["instruction_bytes"], terminal["different_words"]),
                             ("matched", "1304", "0"))
            self.assertEqual(terminal["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(terminal["fingerprint"],
                             hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)


    def test_all_input_final_owners_and_selected_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build MODEL438 overlays before checking owners")
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
                own, = original.get_symbol_by_name(f"func_{base+0x3DE8:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1304, "STT_FUNC"))
                for start, end in zip(self.boundaries, self.boundaries[1:]):
                    name = f"func_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = owners[name]
                    self.assertEqual(owner[0] == selected_path, start == 0x3DE8)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x4868, 0x798)):
                    name = f"D_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = owners[name]
                    self.assertFalse(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual(owner[4], image[start:start+size])
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertEqual(symbol["st_value"], base+start)
                    self.assertFalse(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+size], image[start:start+size])
                text = compiled.get_section(own["st_shndx"]).data()
                callees, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    site = relocation["r_offset"]
                    self.assertLess(site, 1304)
                    symbol = original.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", image, 0x3DE8+site)
                    address = ((base+0x3DE8+site+4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        resolved, = symbols.get_symbol_by_name(symbol.name)
                        self.assertEqual((word >> 26, address), (3, resolved["st_value"]+addend))
                        callees.append(address)
                    else:
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        self.assertEqual((word >> 26, address), (2, base+0x3DE8+symbol["st_value"]+addend))
                        jumps.append((site, address-base-0x3DE8))
                self.assertEqual(len(callees), 10)
                self.assertEqual(set(callees), {0x800842A8, 0x8005C018, 0x80085558, 0x80086258,
                                               0x800875F8, 0x80087958, 0x80087CB8, 0x800872A8, 0x80087738})
                self.assertEqual(callees.count(0x80087CB8), 2)
                self.assertEqual(jumps, [(0xAC, 0x178), (0x14C, 0x170), (0x3EC, 0x4C8), (0x3F8, 0x4CC), (0x410, 0x4CC), (0x468, 0x4CC), (0x484, 0x4CC)])

    def test_resident_function_and_context_owners_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        path = ROOT / "tmp/project-build/SLES_039.51.elf"
        if not path.is_file():
            self.skipTest("Build Spanish resident before checking owners")
        retail = (ROOT / "game/spain/SLES_039.51").read_bytes()
        self.assertEqual((ROOT / "tmp/project-build/SLES_039.51").read_bytes(), retail)
        with (ROOT / "config/sles_03951/functions.csv").open() as handle:
            inventory = {int(r["address"], 0): r for r in csv.DictReader(handle)}
        bindings = dict((n, int(a, 0)) for n, a in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (ROOT / "config/sles_03951/overlays/model_variant438_linker_symbols.txt").read_text(), re.M))
        self.assertEqual(len(bindings), 37)
        self.assertEqual(bindings["GsSortPoly"], 0x800842A8)
        self.assertEqual(bindings["ratan2"], 0x80089928)
        selections = [(kind, int(address, 16), int(size, 16), filename)
                      for kind, address, size, filename in re.findall(
                          r"^\s+\.(text|data|rodata|sdata)\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+\.o)\s*$",
                          (ROOT / "tmp/project-build/SLES_039.51.map").read_text(), re.M)]
        script = (ROOT / "tmp/splat/sles_03951/sles_03951.ld").read_text()
        addresses = set(bindings.values())
        for name in ("Model_LoadMonsterMerge", "func_80056D7C", "func_8004CB0C", "func_800559D4"):
            address, = [a for a, row in inventory.items() if row["name"] == name]
            self.assertEqual(inventory[address]["status"], "matching_c")
            addresses.add(address)
        pointers = {0x80010000: 0x80100000, 0x80010004: 0x80140000,
                    0x8001000C: 0x8013A000, 0x80010010: 0x8017A000,
                    0x80010014: 0x8013B000, 0x80010018: 0x8017B000,
                    0x80010024: 0x80136000, 0x80010028: 0x80176000}
        with path.open("rb") as handle:
            linked = ELFFile(handle)
            symbols = linked.get_section_by_name(".symtab")
            for address in sorted(addresses | pointers.keys()):
                executable = address in addresses
                size = int(inventory[address]["size"], 0) if executable else 4
                kind, start, extent, filename = next(
                    s for s in selections if (s[0] == "text") == executable
                    and s[1] <= address and address+size <= s[1]+s[2])
                self.assertIn(f"{filename}(.{kind})", script)
                with (ROOT / filename).open("rb") as source_handle:
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
                        final_section, = [s for s in linked.iter_sections() if s["sh_type"] == "SHT_PROGBITS"
                                          and s["sh_addr"] <= address and address+size <= s["sh_addr"]+s["sh_size"]]
                    offset = address-final_section["sh_addr"]
                    self.assertEqual(final_section.data()[offset:offset+size],
                                     retail[address-0x8000F800:address-0x8000F800+size])
        for slot in (0, 1):
            context = pointers[0x80010024 + slot * 4]
            for bank, size in ((0x80100000, 96*2048), (0x8013A000, 4096), (0x8013B000, 20480)):
                bank += slot * 0x40000
                self.assertTrue(context + 0x22EC <= bank or bank + size <= context)
