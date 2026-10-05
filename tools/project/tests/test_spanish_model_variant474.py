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


class SpanishModelVariant474Tests(unittest.TestCase):
    spans = ((4, 0xE38), (0xE38, 0x1738), (0x1738, 0x1DC0), (0x1DC0, 0x2240),
             (0x2240, 0x2BD8), (0x2BD8, 0x30D0), (0x30D0, 0x38BC))
    register_writes = staticmethod(lifetimes.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(lifetimes.SpanishModelVariant460Tests.direct_stores)

    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m["linker_symbols"].endswith("/model_variant474_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant474-instances.csv").open() as handle:
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
        self.assertEqual(len(self.modules), 4)
        self.assertEqual(len(self.instances), 4)
        checksums = load_checksum_manifest(self.config / "files.sha256")
        totals = load_spanish_overlay_inventories(ROOT)
        for module in self.modules:
            row = self.instances[module["name"]]
            model, record = int(row["model"]), int(row["record"])
            slot, stage = int(row["slot"]), int(row["stage"])
            base = 0x8013B000 + slot * 0x40000
            expected = {121: (121, 7, 640001), 431: (381, 9, 640000)}
            self.assertEqual((record, stage-slot, int(row["command_word"])), expected[model])
            self.assertEqual(int(row["header"]), 474 + slot * 150)
            self.assertEqual(module["sector_offset"], record * 276 + 180 + (stage - 7) * 10)
            self.assertEqual(module["sector_offset"], int(row["sector"]))
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertNotIn("duplicate_sector_offsets", module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant474_sheets" + ("_slot1" if slot else "") + ".c"
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching, {"schema": 1, "functions": [{
                "address": f"0x{base+0x2BD8:X}", "size": "0x4F8",
                "profile": "gcc_2_8_1_g0_split", "source": source}]})
            self.assertEqual([s["source"] for s in c_segments(ROOT, layout)], [source])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0)-base, int(r["size"], 0)) for r in inventory],
                             [(start, end-start) for start, end in self.spans])
            self.assertEqual([r["status"] for r in inventory],
                             ["unmatched_asm"] * 5 + ["matching_c", "unmatched_asm"])
            for row in inventory[3:]:
                self.assertIn("No entry-reachable call", row["notes"])
            self.assertIn("execution not established", inventory[5]["notes"])
            self.assertEqual(totals[layout.stem]["function_count"], 7)
            self.assertEqual(totals[layout.stem]["matching_c_function_count"], 1)
            self.assertEqual(totals[layout.stem]["matching_c_bytes"], 1272)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x38BC, 0x1744)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0x38BC, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())

    def test_all_physical_loads_and_actual_descriptors(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import model_records

        images = list(self.legal_images())
        observed, commands = set(), {}
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as handle:
            for record, model in enumerate(model_records()):
                for stage in range(7, 11):
                    sector = record * 276 + 180 + (stage - 7) * 10
                    handle.seek(sector * 2048)
                    header, = struct.unpack("<I", handle.read(4))
                    if header in (474, 624):
                        observed.add((model, record, stage, sector, header))
                        handle.seek((record * 276 + 275) * 2048 + 0x110 + (stage - 7) // 2 * 4)
                        commands[model, stage], = struct.unpack("<i", handle.read(4))
        self.assertEqual(observed, {(121, 121, 7, 33576, 474), (121, 121, 8, 33586, 624),
                                    (431, 381, 9, 105356, 474), (431, 381, 10, 105366, 624)})
        for module, data in images:
            row = self.instances[module["name"]]
            model, stage = int(row["model"]), int(row["stage"])
            command = commands[model, stage]
            self.assertEqual(command, 640001 if model == 121 else 640000)
            descriptor = 0x399C + command % 1000 * 52
            self.assertGreaterEqual(descriptor, 0x38BC)
            self.assertLessEqual(descriptor + 52, len(data))
            self.assertEqual(struct.unpack_from("<2I", data, descriptor + 0x18),
                             (120, 140) if model == 121 else (40, 80))
            self.assertEqual(struct.unpack_from("<2I", data, descriptor + 0x28), (500, 520))
            base = int(module["load_address"], 0)
            for offset, word in ((0x94, 0x3C030000 | ((base + 0x399C + 0x8000) >> 16)),
                                 (0x98, 0x2463E99C), (0xA8, 0x00181040), (0xAC, 0x00581021),
                                 (0xB0, 0x00021080), (0xB4, 0x00581021), (0xB8, 0x00021080),
                                 (0xC0, 0xAFC21BA4)):
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)

    def test_closed_cfgs_and_selected_helper_without_observed_caller(self):
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
                self.assertEqual(flow["calls"], {base + 0xE38, base + 0x1738} if start == 4 else set())
            self.assertNotIn(struct.pack("<I", base + 0x2BD8), data)

    def test_entry_layout_and_helper_internal_parameter_packet_lifetimes(self):
        anchors = {
            0x1C: 0x27D91198, 0x954: 0x2A820002, 0x958: 0x27390098,
            0x2BD8: 0x27BDFEF0, 0x2BE0: 0x00809021, 0x2BE8: 0x265E1198,
            0x2C10: 0x26511AA8, 0x2C14: 0x27A80060, 0x2C18: 0x26501220,
            0x2C20: 0xAFA000DC,
            0x2C28: 0x8E421B90, 0x2C48: 0x000238C3, 0x2C4C: 0x24420007,
            0x2C68: 0x8E421B50, 0x2C74: 0x8E421B54, 0x2C80: 0x8E421B58,
            0x2C8C: 0x8E421B64, 0x2C90: 0x8E431BE0, 0x2CAC: 0x8E421B68,
            0x2CC8: 0x8E421B6C, 0x2CC0: 0x00043283, 0x2CDC: 0x00052283,
            0x2D08: 0x00031A83, 0x2DD8: 0x03C09821, 0x2DE0: 0x03D42821,
            0x2DE4: 0x03D53021, 0x2E08: 0x27A200D0, 0x2E10: 0x27A200D4,
            0x2E14: 0x03D63821, 0x2E1C: 0xAFA20024, 0x2EA8: 0x18400005,
            0x2EBC: 0x3046FFFF, 0x2ED0: 0x2AE20004, 0x2F04: 0x28421000,
            0x2F24: 0x00021300, 0x2F2C: 0x0043001B, 0x2F5C: 0xAE421C1C,
            0x2F70: 0x0064102B, 0x2F90: 0x00021300, 0x2FC4: 0x2C420002,
            0x2FEC: 0x28622000, 0x3000: 0x00021240, 0x3020: 0xAE421C1C,
            0x3034: 0x0064102B, 0x3054: 0x00031B40, 0x305C: 0x0062001B,
            0x3080: 0xAE000000, 0x3084: 0x26100098, 0x308C: 0x27DE0098,
            0x3094: 0x29020002,
        }
        for _, data in self.legal_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word, hex(offset))
            for register, expected in (
                    (18, [0x2BE0, 0x30BC]), (17, [0x2C10, 0x30C0]),
                    (30, [0x2BE8, 0x308C, 0x30A4]), (19, [0x2DD8, 0x2ED8, 0x30B8]),
                    (20, [0x2DB4, 0x2EC8, 0x30B4]), (21, [0x2DA8, 0x2EC4, 0x30B0]),
                    (22, [0x2D3C, 0x2EC0, 0x30AC]), (23, [0x2D2C, 0x2ECC, 0x30A8]),
                    (29, [0x2BD8, 0x30CC])):
                self.assertEqual(self.register_writes(data, 0x2BD8, 0x30D0, register), expected)
            self.assertEqual([(off, size) for _, off, size in self.direct_stores(data, 0x2BD8, 0x30D0, 17)],
                             [(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
            stack_stores = self.direct_stores(data, 0x2BD8, 0x30D0, 29)
            for offset, sites in ((0xD8, [0x2C1C]), (0xDC, [0x2C20, 0x309C]), (0xE0, [0x2C24])):
                self.assertEqual([(site, size) for site, off, size in stack_stores if off == offset],
                                 [(site, 4) for site in sites])
        self.assertEqual(0x1198 + 2 * 152, 0x12C8)
        self.assertEqual(0x1AA8 + 52, 0x1ADC)
        self.assertEqual(0x1B50 + 12, 0x1B5C)
        self.assertEqual(0x1B64 + 12, 0x1B70)

    def test_target_compiled_layout(self):
        compiler = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
        if not compiler.is_file() or importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        constants = (
            ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
            ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52),
            ("sizeof(ModelVariantSheet)", 152), ("O(ModelVariantSheet,v1)", 32),
            ("O(ModelVariantSheet,v2)", 64), ("O(ModelVariantSheet,v3)", 96),
            ("O(ModelVariantSheet,outer)", 128), ("O(ModelVariantSheet,inner)", 132),
            ("O(ModelVariantSheet,size)", 136), ("sizeof(Sheets474Timing)", 52),
            ("O(Sheets474Timing,grow_begin)", 0x18), ("O(Sheets474Timing,grow_end)", 0x1C),
            ("O(Sheets474Timing,fade_begin)", 0x28), ("O(Sheets474Timing,fade_end)", 0x2C),
            ("O(Sheets474State,sheets)", 0x1198), ("O(Sheets474State,polygon)", 0x1AA8),
            ("O(Sheets474State,origin)", 0x1B50), ("O(Sheets474State,direction)", 0x1B64),
            ("O(Sheets474State,frame)", 0x1B90), ("O(Sheets474State,time)", 0x1B94),
            ("O(Sheets474State,step)", 0x1B9C), ("O(Sheets474State,timing)", 0x1BA4),
            ("O(Sheets474State,displacement)", 0x1BE0), ("O(Sheets474State,phase)", 0x1C1C),
            ("sizeof(Sheets474State)", 0x1C20), ("sizeof(((Sheets474State *)0)->origin)", 12),
            ("sizeof(((Sheets474State *)0)->direction)", 12), ("O(POLY_GT4,x0)", 8),
            ("O(POLY_GT4,x1)", 20), ("O(POLY_GT4,x2)", 32), ("O(POLY_GT4,x3)", 44),
        )
        directory = ROOT / "tmp/test-spanish-model474-layout"
        directory.mkdir(parents=True, exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/overlays/spanish_model_variant/variant474_sheets.h"\n'
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
                self.skipTest("Build Spanish MODEL474 images before checking owners")
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
                own, = original.get_symbol_by_name(f"func_{base+0x2BD8:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1272, "STT_FUNC"))
                self.assertTrue(compiled.get_section(own["st_shndx"])["sh_flags"] & 4)
                self.assertIn(f"{selected_path.relative_to(ROOT)}(.text);", script)
                for start, end in self.spans:
                    name = f"func_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = owners[name]
                    self.assertEqual(owner[0] == selected_path, start == 0x2BD8)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x38BC, 0x1744)):
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
                    word, = struct.unpack_from("<I", image, 0x2BD8 + site)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    address = ((base + 0x2BD8 + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    resolved, = symbols.get_symbol_by_name(symbol.name)
                    self.assertEqual(resolved["st_value"], bindings[symbol.name])
                    self.assertEqual(address, resolved["st_value"] + addend)
                    callees.add(address)
                    count += 1
                self.assertEqual(count, 10)
                self.assertEqual(callees, {0x8005C018, 0x800842A8, 0x80085558, 0x80086258,
                                          0x800872A8, 0x800875F8, 0x80087738, 0x80087958, 0x80087CB8})

    def test_resident_callee_loader_and_pointer_storage_layout_compatibility_when_built(self):
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
            (self.config / "overlays/model_variant474_linker_symbols.txt").read_text(), re.M))
        self.assertEqual(len(bindings), 35)
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
                self.assertTrue(context + 0x1C20 <= base or base + size <= context)

    def test_terminal_records_match_selected_sources(self):
        with (ROOT / "notes/overlays/spanish-model-variant474-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 9)
        self.assertEqual([int(row["different_words"]) for row in rows], [12, 15, 12, 0, 0, 0, 0, 0, 0])
        for module in self.modules:
            terminal = [r for r in rows if r["module"] == module["name"]][-1]
            selected, = c_segments(ROOT, ROOT / module["layout"])
            self.assertEqual(terminal["result"], "matched")
            self.assertEqual((terminal["instruction_bytes"], terminal["different_words"]), ("1272", "0"))
            self.assertEqual(terminal["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"],
                             hashlib.sha256(
                                 (ROOT / "src/overlays/spanish_model_variant/variant474_sheets.c").read_bytes() +
                                 (ROOT / "src/overlays/spanish_model_variant/variant474_sheets.h").read_bytes()).hexdigest())


if __name__ == "__main__":
    unittest.main()
