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


class SpanishModelVariant426Tests(unittest.TestCase):
    boundaries = (4, 0xF24, 0x13B0, 0x1DF0, 0x26D0, 0x2BC0, 0x3440, 0x3874)

    def setUp(self):
        manifest = json.loads((ROOT / "config/sles_03951/overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m.get("linker_symbols", "").endswith("/model_variant426_linker_symbols.txt")]
        self.assertEqual(len(self.modules), 2)

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

        images = list(self.legal_images())
        checksums = load_checksum_manifest(ROOT / "config/sles_03951/files.sha256")
        with (ROOT / "notes/overlays/spanish-model-variant426-instances.csv").open() as handle:
            instances = {r["module"]: r for r in csv.DictReader(handle)}
        self.assertEqual(set(instances), {m["name"] for m in self.modules})
        observed = set()
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as handle:
            for record, model in enumerate(model_records()):
                for stage in range(7, 11):
                    sector = record * 276 + 180 + (stage - 7) * 10
                    handle.seek(sector * 2048)
                    header, = struct.unpack("<I", handle.read(4))
                    if header in (426, 576):
                        observed.add((model, record, stage, sector, header))
            self.assertEqual(observed, {(0, 0, 7, 180, 426), (0, 0, 8, 190, 576)})
            handle.seek((0 * 276 + 275) * 2048 + 0x110)
            self.assertEqual(struct.unpack("<3i", handle.read(12)), (592000, 591000, -2))
        for module, image in images:
            row = instances[module["name"]]
            slot = int(row["slot"])
            base = 0x8013B000 + slot * 0x40000
            self.assertEqual((int(row["model"]), int(row["record"]), int(row["stage"]),
                              int(row["command_word"])), (0, 0, 7 + slot, 592000))
            self.assertEqual((module["sector_offset"], module["sector_count"]), (180 + slot * 10, 10))
            self.assertEqual(module["sector_offset"], int(row["sector"]))
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertEqual(struct.unpack_from("<I", image)[0], 426 + slot * 150)
            layout = ROOT / module["layout"]
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            spans = list(zip(self.boundaries, self.boundaries[1:]))
            self.assertEqual([(int(r["address"], 0)-base, int(r["size"], 0)) for r in inventory],
                             [(start, end-start) for start, end in spans])
            self.assertEqual([r["status"] for r in inventory],
                             ["unmatched_asm", "matching_c", "unmatched_asm", "unmatched_asm",
                              "matching_c", "unmatched_asm", "unmatched_asm"])
            segments = c_segments(ROOT, layout)
            self.assertEqual([s["source"] for s in segments],
                             ["src/overlays/spanish_model_variant/variant426_mesh" + ("_slot1" if slot else "") + ".c",
                              "src/overlays/spanish_model_variant/variant426_sheets" + ("_slot1" if slot else "") + ".c"])
            selected = segments[0]
            source = "src/overlays/spanish_model_variant/variant426_mesh" + ("_slot1" if slot else "") + ".c"
            self.assertEqual(selected["source"], source)
            self.assertEqual(json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text()),
                             {"schema": 1, "functions": [{"address": f"0x{base+0xF24:X}", "size": "0x48C",
                              "profile": "gcc_2_8_1_g0_split", "source": source},
                              {"address": f"0x{base+0x26D0:X}", "size": "0x4F0",
                               "profile": "gcc_2_8_1_g0_split",
                               "source": source.replace("variant426_mesh", "variant426_sheets")}]})
            for start, end in spans:
                flow = walk_function(image, base, start, end-start)
                self.assertTrue(flow["closed"])
                self.assertEqual(flow["visited"], set(range(start, end, 4)))
                self.assertEqual((flow["returns"], flow["indirect_calls"]), (1, 0))
                self.assertTrue(flow["calls"] <= {base+x for x in self.boundaries[:-1]})
                if start == 4:
                    self.assertIn(base + 0xF24, flow["calls"])
                    self.assertNotIn(base + 0x3440, flow["calls"])

    def test_original_context_initialized_packet_and_mesh_bounds(self):
        decoder = lifetimes.SpanishModelVariant460Tests
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            definitions = set(decoder.register_writes(image, 4, 0xF24, 18))
            pending, visited, reaching = [(4, None)], set(), set()
            while pending:
                pc, definition = pending.pop()
                self.assertTrue(4 <= pc < 0xF24 and pc % 4 == 0)
                if (pc, definition) in visited:
                    continue
                visited.add((pc, definition))
                if pc in definitions:
                    definition = pc
                if pc == 0xD14:
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
            self.assertEqual(reaching, {0xC})
            self.assertEqual(struct.unpack_from("<I", image, 0xD14)[0],
                             0x0C000000 | ((base + 0xF24) >> 2 & 0x3FFFFFF))
            anchors = {
    0xC: 0x00809021, 0x14: 0x0240F021, 0x5C: 0x27D91E4C,
    0x68: 0xAFB90094, 0x4A4: 0x8FA40094, 0x4A8: 0x0C020BBA,
    0x4AC: 0, 0x4BC: 0xA498001A, 0x4D4: 0xA322000C,
    0x624: 0x00119200, 0x634: 0x2A220011, 0x63C: 0x26100008,
    0x64C: 0xA33804CA, 0x68C: 0x2B220009, 0xD00: 0x28420003,
    0xD18: 0x02402021, 0xF2C: 0x0080B021, 0xF74: 0x26D21E4C,
    0xFA0: 0x2462000F, 0xFA4: 0x00021103,
    0x1104: 0x2A020009, 0x1154: 0x2A020009, 0x12B4: 0x0C0210AA,
    0x12C8: 0x2AA20010, 0x12E0: 0x2A020008, 0x1320: 0x00021240,
}
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual([(pc, off, size) for pc, off, size in
                              decoder.direct_stores(image, 4, 0x4A8, 29) if off == 0x94],
                             [(0x68, 0x94, 4)])
            self.assertEqual(decoder.register_writes(image, 0xF24, 0x13B0, 18), [0xF74, 0x139C])
            stores = sorted((off, size) for _, off, size in decoder.direct_stores(image, 0xF24, 0x13B0, 18))
            self.assertEqual(stores, [(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
        self.assertEqual(9 * 17 * 8, 0x4C8)
        self.assertEqual(8 * 0x88 + 16 * 8 + 8, 0x4C8)
        self.assertEqual(0x4C8 + 9 * 4, 0x4EC)
        self.assertEqual(0x1E4C + 52, 0x1E80)

    def test_target_compiled_layout_and_terminal_fingerprints(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        expressions = """
sizeof(SVECTOR), sizeof(VECTOR), sizeof(MATRIX), sizeof(GsCOORDINATE2), sizeof(POLY_GT4),
sizeof(Mesh426Rows), OFF(Mesh426Rows, rows), OFF(Mesh426Rows, colors),
sizeof(((Mesh426Rows *)0)->rows), sizeof(((Mesh426Rows *)0)->rows[0]),
sizeof(((Mesh426Rows *)0)->colors), sizeof(((Mesh426Rows *)0)->colors[0]),
OFF(Mesh426State, mesh), OFF(Mesh426State, polygon), OFF(Mesh426State, origin),
OFF(Mesh426State, direction), OFF(Mesh426State, frame), OFF(Mesh426State, step),
OFF(Mesh426State, size), OFF(Mesh426State, intensity), OFF(Mesh426State, phase),
sizeof(Mesh426State), OFF(POLY_GT4, x0), OFF(POLY_GT4, x1), OFF(POLY_GT4, x2), OFF(POLY_GT4, x3)
"""
        expected = (8, 16, 32, 80, 52, 0x4EC, 0, 0x4C8, 0x4C8, 0x88, 36, 4,
            0, 0x1E4C, 0x20C8, 0x20D0, 0x20FC, 0x2108, 0x2128, 0x2138,
            0x2180, 0x2184, 8, 20, 32, 44)
        directory = ROOT / "tmp/model426mesh-layout-tests"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n'
                          '#include "../../src/overlays/spanish_model_variant/variant426_mesh.h"\n'
                          '#define OFF(t, f) ((u32)&((t *)0)->f)\n'
                          'u32 layout[] = {' + expressions + "};\n")
        obj = compile_c(ROOT, tool(ROOT, "as"),
                        {"source": str(source.relative_to(ROOT)), "profile": "gcc_2_8_1_g0_split", "object": "layout.o"},
                        load_compiler_profiles(ROOT), object_directory=str(directory.relative_to(ROOT)),
                        asm_directory=str(directory.relative_to(ROOT)))
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
            section = elf.get_section(symbol["st_shndx"])
            self.assertEqual((symbol["st_value"], section["sh_size"]), (0, len(expected) * 4))
            self.assertEqual(section.data(), struct.pack("<" + "I" * len(expected), *expected))
        with (ROOT / "notes/overlays/spanish-model-variant426-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([int(r["different_words"]) for r in rows], [0, 0, 0, 0])
        body = ROOT / "src/overlays/spanish_model_variant/variant426_mesh.c"
        shared = ROOT / "src/overlays/model_variant/model_variant.h"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes() +
                                    shared.read_bytes()).hexdigest()
        for module in self.modules:
            terminal = [r for r in rows if r["module"] == module["name"]][-1]
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"]) if "variant426_mesh" in s["source"]]
            self.assertEqual((terminal["result"], terminal["instruction_bytes"], terminal["different_words"]),
                             ("matched", "1164", "0"))
            self.assertEqual(terminal["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
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
                self.skipTest("Build MODEL426 overlays before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"]) if "variant426_mesh" in s["source"]]
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
                own, = original.get_symbol_by_name(f"func_{base+0xF24:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1164, "STT_FUNC"))
                for start, end in zip(self.boundaries, self.boundaries[1:]):
                    name = f"func_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = owners[name]
                    self.assertEqual(owner[0] == selected_path, start == 0xF24)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x3874, 0x178C)):
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
                    self.assertLess(site, 1164)
                    symbol = original.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", image, 0xF24+site)
                    address = ((base+0xF24+site+4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        resolved, = symbols.get_symbol_by_name(symbol.name)
                        self.assertEqual((word >> 26, address), (3, resolved["st_value"]+addend))
                        callees.append(address)
                    else:
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        self.assertEqual((word >> 26, address), (2, base+0xF24+symbol["st_value"]+addend))
                        jumps.append((site, address-base-0xF24))
                self.assertEqual(len(callees), 9)
                self.assertEqual(set(callees), {0x800842A8, 0x8005C018, 0x80085558, 0x80086258,
                                               0x800875F8, 0x80087958, 0x80087CB8, 0x80089928})
                self.assertEqual(callees.count(0x80089928), 2)
                self.assertEqual(jumps, [(0x84, 0x90), (0x1EC, 0x240)])

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
            (ROOT / "config/sles_03951/overlays/model_variant426_linker_symbols.txt").read_text(), re.M))
        self.assertEqual(len(bindings), 33)
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
                self.assertTrue(context + 0x2184 <= bank or bank + size <= context)


if __name__ == "__main__":
    unittest.main()
