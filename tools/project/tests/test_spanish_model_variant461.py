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


class SpanishModelVariant461Tests(unittest.TestCase):
    spans = ((4, 0x1508), (0x1508, 0x1F54), (0x1F54, 0x2498), (0x2498, 0x28D4))
    register_writes = staticmethod(lifetimes.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(lifetimes.SpanishModelVariant460Tests.direct_stores)

    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m.get("linker_symbols", "").endswith("/model_variant461_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant461-instances.csv").open() as handle:
            self.instances = {r["module"]: r for r in csv.DictReader(handle)}

    def legal_images(self):
        path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.is_file():
            self.skipTest("Legal Spanish MODEL input required")
        with path.open("rb") as handle:
            for module in self.modules:
                handle.seek(module["sector_offset"] * 2048)
                data = handle.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                yield module, data

    def test_loader_records_and_exact_selected_spans(self):
        self.assertEqual(len(self.modules), 2)
        self.assertEqual(len(self.instances), 2)
        checksums = load_checksum_manifest(self.config / "files.sha256")
        totals = load_spanish_overlay_inventories(ROOT)
        for module in self.modules:
            row = self.instances[module["name"]]
            slot, stage = int(row["slot"]), int(row["stage"])
            base = 0x8013B000 + slot * 0x40000
            self.assertEqual((int(row["model"]), int(row["record"]), stage), (705, 605, 9 + slot))
            self.assertEqual(int(row["header"]), 461 + slot * 150)
            self.assertEqual(int(row["command_word"]), 627001)
            self.assertEqual(module["sector_offset"], 605 * 276 + 200 + slot * 10)
            self.assertEqual(module["sector_offset"], int(row["sector"]))
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertNotIn("duplicate_sector_offsets", module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant461_sheets" + ("_slot1" if slot else "") + ".c"
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching, {"schema": 1, "functions": [{
                "address": f"0x{base+0x1F54:X}", "size": "0x544",
                "profile": "gcc_2_8_1_g0_split", "source": source}]})
            self.assertEqual([s["source"] for s in c_segments(ROOT, layout)], [source])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0)-base, int(r["size"], 0)) for r in inventory],
                             [(start, end-start) for start, end in self.spans])
            self.assertEqual([r["status"] for r in inventory],
                             ["unmatched_asm", "unmatched_asm", "matching_c", "unmatched_asm"])
            self.assertTrue(all("direct-entry reachable" in r["notes"] for r in inventory[1:]))
            self.assertEqual(totals[layout.stem]["function_count"], 4)
            self.assertEqual(totals[layout.stem]["matching_c_function_count"], 1)
            self.assertEqual(totals[layout.stem]["matching_c_bytes"], 1348)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x28D4, 0x272C)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0x28D4, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())

    def test_all_physical_loads_and_actual_descriptor(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import model_records

        images = list(self.legal_images())
        observed = set()
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as handle:
            for record, model in enumerate(model_records()):
                for stage in range(7, 11):
                    sector = record * 276 + 180 + (stage - 7) * 10
                    handle.seek(sector * 2048)
                    header, = struct.unpack("<I", handle.read(4))
                    if header in (461, 611):
                        observed.add((model, record, stage, sector, header))
            handle.seek((605 * 276 + 275) * 2048 + 0x114)
            command, = struct.unpack("<i", handle.read(4))
        self.assertEqual(observed, {(705, 605, 9, 167180, 461), (705, 605, 10, 167190, 611)})
        self.assertEqual(command, 627001)
        for _, data in images:
            descriptor = 0x29D0 + command % 1000 * 64
            self.assertEqual(descriptor, 0x2A10)
            self.assertGreaterEqual(descriptor, 0x28D4)
            self.assertLessEqual(descriptor + 64, len(data))
            self.assertEqual(struct.unpack_from("<2I", data, descriptor + 0x1C), (5, 1))
            self.assertEqual(struct.unpack_from("<2I", data, descriptor + 0x30), (550, 560))

    def test_closed_cfgs_and_original_context_reaches_sheet_call(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import walk_function

        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            for start, end in self.spans:
                flow = walk_function(data, base, start, end-start)
                self.assertTrue(flow["closed"])
                self.assertEqual(flow["visited"], set(range(start, end, 4)))
                self.assertEqual((flow["returns"], flow["indirect_calls"]), (1, 0))
                self.assertEqual(flow["calls"], {base + off for off in (0x1508, 0x1F54, 0x2498)}
                                 if start == 4 else set())
            self.assertEqual(struct.unpack_from("<II", data, 0x135C),
                             (0x0C000000 | ((base + 0x1F54) >> 2 & 0x3FFFFFF), 0x02E02021))
            definitions = set(self.register_writes(data, 4, 0x1508, 23))
            pending, visited, reaching = [(4, None)], set(), set()
            while pending:
                pc, definition = pending.pop()
                self.assertTrue(4 <= pc < 0x1508 and pc % 4 == 0)
                if (pc, definition) in visited:
                    continue
                visited.add((pc, definition))
                word, = struct.unpack_from("<I", data, pc)
                op = word >> 26
                if pc in definitions:
                    definition = pc
                if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
                    if pc + 4 in definitions:
                        definition = pc + 4
                    if pc == 0x135C:
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
            self.assertEqual(reaching, {0xC})

    def test_record_lifetimes_packet_stores_and_scale_gates(self):
        anchors = {
            0x9E0: 0x2484009C, 0x9E8: 0x2AE2000A, 0xEF0: 0xA6D933BE,
            0x1354: 0x18400006, 0x1F54: 0x27BDFEE0, 0x1F58: 0x24882144,
            0x1F60: 0x24912DCC, 0x1F9C: 0x8D24336C, 0x1FA4: 0x8C830020,
            0x1FB4: 0x8C82001C, 0x1FC8: 0x8C82001C, 0x1FDC: 0x26310034,
            0x2000: 0x25500088, 0x2020: 0x04410003, 0x2024: 0x000228C3,
            0x2028: 0x24420007, 0x228C: 0x04C00008, 0x229C: 0x04400004,
            0x22AC: 0x30C6FFFF, 0x22C0: 0x2AC20004, 0x22E8: 0x14620015,
            0x22EC: 0x28620004, 0x230C: 0x00021042, 0x2310: 0x000212C0,
            0x233C: 0xAEE233AC, 0x2368: 0x0064102B, 0x2380: 0x0062001B,
            0x23C0: 0x8D030188, 0x23D0: 0x8E020010, 0x23F4: 0x8D22018C,
            0x2414: 0x28420005, 0x2434: 0x254A0250, 0x2440: 0x2610009C,
            0x2454: 0x2508009C, 0x2494: 0x27BD0120,
        }
        for _, data in self.legal_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word, hex(offset))
            for register, expected in (
                    (16, [0x2000, 0x2440, 0x248C]), (17, [0x1F60, 0x1FDC, 0x2488]),
                    (23, [0x1FA8, 0x2470]), (29, [0x1F54, 0x2494]),
                    (30, [0x1FEC, 0x243C, 0x246C])):
                self.assertEqual(self.register_writes(data, 0x1F54, 0x2498, register), expected)
            self.assertEqual([(off, size) for _, off, size in self.direct_stores(data, 0x1F54, 0x2498, 17)],
                             [(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
            for spill, expected in ((216, [0x1F88, 0x2438]), (220, [0x1F90, 0x2464])):
                self.assertEqual([pc for pc, off, size in self.direct_stores(data, 0x1F54, 0x2498, 29)
                                  if off < spill + 4 and spill < off + size], expected)
        self.assertEqual(5 * 592, 0xB90)
        self.assertEqual(0x2144 + 10 * 156, 0x275C)
        self.assertEqual(0x2DCC + 2 * 52, 0x2E34)
        self.assertEqual(0x2F3C + 5 * 16, 0x2F8C)
        self.assertEqual(0x2F8C + 5 * 16, 0x2FDC)

    def test_target_compiled_layout(self):
        compiler = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
        if not compiler.is_file() or importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        constants = (
            ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
            ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52),
            ("sizeof(ModelVariantSheetSet)", 156), ("O(ModelVariantSheetSet,v1)", 32),
            ("O(ModelVariantSheetSet,v2)", 64), ("O(ModelVariantSheetSet,v3)", 96),
            ("O(ModelVariantSheetSet,outer)", 128), ("O(ModelVariantSheetSet,inner)", 132),
            ("O(ModelVariantSheetSet,size)", 136), ("O(ModelVariantSheetSet,shown)", 152),
            ("sizeof(Sheets461Primary)", 592), ("O(Sheets461Primary,active)", 0x188),
            ("O(Sheets461Primary,size)", 0x18C), ("sizeof(Sheets461Timing)", 64),
            ("O(Sheets461Timing,first_count)", 28), ("O(Sheets461Timing,paired)", 32),
            ("O(Sheets461Timing,fade_begin)", 48), ("O(Sheets461Timing,fade_end)", 52),
            ("O(Sheets461State,sheets)", 0x2144), ("O(Sheets461State,polygons)", 0x2DCC),
            ("O(Sheets461State,positions)", 0x2F3C), ("O(Sheets461State,other_positions)", 0x2F8C),
            ("O(Sheets461State,frame)", 0x3358), ("O(Sheets461State,time)", 0x335C),
            ("O(Sheets461State,step)", 0x3364), ("O(Sheets461State,timing)", 0x336C),
            ("O(Sheets461State,phase)", 0x33AC), ("sizeof(Sheets461State)", 0x33B0),
        )
        directory = ROOT / "tmp/test-spanish-model461-layout"
        directory.mkdir(parents=True, exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/overlays/spanish_model_variant/variant461_sheets.h"\n'
                          '#define O(t,f) ((u32)&((t *)0)->f)\n'
                          'const u32 layout[] = {' + ",".join(c for c, _ in constants) + '};\n')
        obj = compile_c(ROOT, tool(ROOT, "as"),
                        {"source": str(source.relative_to(ROOT)), "profile": "gcc_2_8_1_g0_split", "object": "layout.o"},
                        load_compiler_profiles(ROOT), object_directory=str(directory.relative_to(ROOT)),
                        asm_directory=str(directory.relative_to(ROOT)))
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
            section = elf.get_section(symbol["st_shndx"])
            self.assertEqual((symbol["st_value"], section["sh_size"]), (0, len(constants) * 4))
            self.assertEqual(section.data(), struct.pack("<" + "I" * len(constants), *(v for _, v in constants)))

    def test_complete_images_input_final_owners_and_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL461 images before checking owners")
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
                own, = original.get_symbol_by_name(f"func_{base+0x1F54:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1348, "STT_FUNC"))
                self.assertTrue(compiled.get_section(own["st_shndx"])["sh_flags"] & 4)
                self.assertIn(f"{selected_path.relative_to(ROOT)}(.text);", script)
                for start, end in self.spans:
                    name = f"func_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = owners[name]
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x28D4, 0x272C)):
                    name = f"D_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = owners[name]
                    self.assertFalse(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual(owner[4][:size], image[start:start+size])
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertEqual(symbol["st_value"], base+start)
                    self.assertFalse(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+size], image[start:start+size])
                bindings = dict((n, int(a, 0)) for n, a in re.findall(
                    r"^(\w+) = (0x[0-9A-F]+);", (ROOT / module["linker_symbols"]).read_text(), re.M))
                count, callees = 0, set()
                text = compiled.get_section(own["st_shndx"]).data()
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    if relocation["r_info_type"] != 4:
                        continue
                    symbol = original.get_symbol(relocation["r_info_sym"])
                    if symbol["st_shndx"] != "SHN_UNDEF":
                        continue
                    site = relocation["r_offset"]
                    word, = struct.unpack_from("<I", image, 0x1F54 + site)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    address = ((base + 0x1F54 + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    resolved, = symbols.get_symbol_by_name(symbol.name)
                    self.assertEqual(resolved["st_value"], bindings[symbol.name])
                    self.assertEqual(address, resolved["st_value"] + addend)
                    callees.add(address)
                    count += 1
                self.assertEqual(count, 10)
                self.assertEqual(callees, {0x8005C018, 0x800842A8, 0x80085558, 0x80086258,
                                          0x800872A8, 0x800875F8, 0x80087738, 0x80087958, 0x80087CB8})

    def test_resident_callee_loader_and_context_storage_owners_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        elf_path = ROOT / "tmp/project-build/SLES_039.51.elf"
        if not elf_path.is_file():
            self.skipTest("Build Spanish resident before checking owners")
        retail = (ROOT / "game/spain/SLES_039.51").read_bytes()
        self.assertEqual((ROOT / "tmp/project-build/SLES_039.51").read_bytes(), retail)
        selections = [(kind, int(address, 16), int(size, 16), path)
                      for kind, address, size, path in re.findall(
                          r"^\s+\.(text|data|rodata|sdata)\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+\.o)\s*$",
                          (ROOT / "tmp/project-build/SLES_039.51.map").read_text(), re.M)]
        script = (ROOT / "tmp/splat/sles_03951/sles_03951.ld").read_text()
        with (self.config / "functions.csv").open() as handle:
            inventory = {int(r["address"], 0): r for r in csv.DictReader(handle)}
        bindings = dict((n, int(a, 0)) for n, a in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config / "overlays/model_variant461_linker_symbols.txt").read_text(), re.M))
        self.assertEqual(len(bindings), 34)
        addresses = set(bindings.values())
        for name in ("Model_LoadMonsterMerge", "func_80056D7C", "func_8004CB0C", "func_800559D4"):
            address, = [a for a, row in inventory.items() if row["name"] == name]
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
                selection, = [s for s in selections if (s[0] == "text") == executable
                              and s[1] <= address and address+size <= s[1]+s[2]]
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
                        final_section, = [s for s in linked.iter_sections() if s["sh_type"] == "SHT_PROGBITS"
                                          and s["sh_addr"] <= address and address+size <= s["sh_addr"]+s["sh_size"]]
                    data = final_section.data()[address-final_section["sh_addr"]:address-final_section["sh_addr"]+size]
                    self.assertEqual(data, retail[address-0x8000F800:address-0x8000F800+size])
        for slot in (0, 1):
            context = pointers[0x80010024 + slot * 4]
            for base, size in ((0x80100000, 96*2048), (0x8013A000, 4096), (0x8013B000, 20480)):
                base += slot * 0x40000
                self.assertTrue(context + 0x33C0 <= base or base + size <= context)

    def test_terminal_records_match_selected_sources(self):
        with (ROOT / "notes/overlays/spanish-model-variant461-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        for module in self.modules:
            terminal = [r for r in rows if r["module"] == module["name"]][-1]
            selected, = c_segments(ROOT, ROOT / module["layout"])
            self.assertEqual(terminal["result"], "matched")
            self.assertEqual((terminal["instruction_bytes"], terminal["different_words"]), ("1348", "0"))
            self.assertEqual(terminal["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"],
                             hashlib.sha256(
                                 (ROOT / "src/overlays/spanish_model_variant/variant461_sheets.c").read_bytes() +
                                 (ROOT / "src/overlays/spanish_model_variant/variant461_sheets.h").read_bytes()).hexdigest())


if __name__ == "__main__":
    unittest.main()
