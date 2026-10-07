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


class SpanishModelVariant472Tests(unittest.TestCase):
    spans = ((4, 0x1110), (0x1110, 0x1734), (0x1734, 0x1D30),
             (0x1D30, 0x28B0), (0x28B0, 0x2C58))
    register_writes = staticmethod(lifetimes.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(lifetimes.SpanishModelVariant460Tests.direct_stores)

    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m.get("linker_symbols", "").endswith("/model_variant472_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant472-instances.csv").open() as handle:
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
            self.assertEqual((int(row["model"]), int(row["record"]), stage), (719, 619, 7 + slot))
            self.assertEqual(int(row["header"]), 472 + slot * 150)
            self.assertEqual(int(row["command_word"]), 638000)
            self.assertEqual(module["sector_offset"], 619 * 276 + 180 + slot * 10)
            self.assertEqual(module["sector_offset"], int(row["sector"]))
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertNotIn("duplicate_sector_offsets", module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant472_sheets" + ("_slot1" if slot else "") + ".c"
            spiral = source.replace("variant472_sheets", "variant472_spiral")
            expected = [{"address": f"0x{base+0x1D30:X}", "size": "0xB80",
                         "profile": "gcc_2_8_1_g0_split", "source": spiral},
                        {"address": f"0x{base+0x28B0:X}", "size": "0x3A8",
                         "profile": "gcc_2_8_1_g0_split", "source": source}]
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching, {"schema": 1, "functions": expected})
            self.assertEqual([s["source"] for s in c_segments(ROOT, layout)], [spiral, source])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0)-base, int(r["size"], 0)) for r in inventory],
                             [(start, end-start) for start, end in self.spans])
            self.assertEqual([r["status"] for r in inventory], ["unmatched_asm"] * 3 + ["matching_c"] * 2)
            self.assertIn("loader entry", inventory[0]["notes"])
            self.assertTrue(all("direct-entry reachable" in r["notes"] for r in inventory[1:]))
            self.assertEqual(totals[layout.stem]["function_count"], 5)
            self.assertEqual(totals[layout.stem]["matching_c_function_count"], 2)
            self.assertEqual(totals[layout.stem]["matching_c_bytes"], 936 + 2944)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x2C58, 0x23A8)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0x2C58, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())

    def test_all_physical_loads_and_actual_descriptor(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer is required for the model census")
        from overlay_function_inventory import model_records

        images = list(self.legal_images())
        observed = set()
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as handle:
            for record, model in enumerate(model_records()):
                for stage in range(7, 11):
                    sector = record * 276 + 180 + (stage - 7) * 10
                    handle.seek(sector * 2048)
                    header, = struct.unpack("<I", handle.read(4))
                    if header in (472, 622):
                        observed.add((model, record, stage, sector, header))
            handle.seek((619 * 276 + 275) * 2048 + 0x110)
            command, = struct.unpack("<i", handle.read(4))
        self.assertEqual(observed, {(719, 619, 7, 171024, 472), (719, 619, 8, 171034, 622)})
        self.assertEqual(command, 638000)
        for _, data in images:
            descriptor = 0x2D54 + command % 1000 * 52
            self.assertGreaterEqual(descriptor, 0x2C58)
            self.assertLessEqual(descriptor + 52, len(data))
            self.assertEqual(struct.unpack_from("<5I", data, descriptor + 0x1C), (40, 80, 160, 240, 600))

    def test_closed_cfgs_and_original_context_reaches_sheet_call(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer is required for CFG checks")
        from overlay_function_inventory import walk_function

        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            for start, end in self.spans:
                flow = walk_function(data, base, start, end-start)
                self.assertTrue(flow["closed"])
                self.assertEqual(flow["visited"], set(range(start, end, 4)))
                self.assertEqual((flow["returns"], flow["indirect_calls"]), (1, 0))
                self.assertEqual(flow["calls"], {base + off for off in (0x1110, 0x1734, 0x1D30, 0x28B0)}
                                 if start == 4 else set())
            self.assertEqual(struct.unpack_from("<II", data, 0xF28),
                             (0x0C000000 | ((base + 0x28B0) >> 2 & 0x3FFFFFF), 0x02402021))
            definitions = set(self.register_writes(data, 4, 0x1110, 18))
            pending, visited, reaching = [(4, None)], set(), set()
            while pending:
                pc, definition = pending.pop()
                self.assertTrue(4 <= pc < 0x1110 and pc % 4 == 0)
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
                    if pc == 0xF28:
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
            0x1C: 0x27D92B24, 0x150: 0x27C52E84, 0x930: 0x27230090,
            0x93C: 0x27110020, 0x940: 0x27100040, 0x944: 0x270F0060,
            0xA5C: 0x28820004, 0xAD8: 0x27180098, 0xAF0: 0x18E0FF90,
            0xCD4: 0xA7D82F46,
            0x28B0: 0x27BDFF00, 0x28C0: 0x26542B24, 0x28EC: 0x26512C48,
            0x28F8: 0x26502BAC, 0x2918: 0x04410003, 0x291C: 0x000218C3,
            0x2920: 0x24420007, 0x2AC4: 0x000218C0, 0x2AC8: 0x00621823,
            0x2ACC: 0x00640018, 0x2B0C: 0x04C00008, 0x2B1C: 0x04400004,
            0x2B2C: 0x30C6FFFF, 0x2B34: 0x2A620004, 0x2B44: 0,
            0x2B48: 0x28421000, 0x2B54: 0x8E422F40, 0x2B80: 0x0043001B,
            0x2BA8: 0xAE020000, 0x2BC4: 0x0064102B, 0x2BDC: 0x0062001B,
            0x2C00: 0xAE000000, 0x2C08: 0x24020003, 0x2C10: 0x24020004,
            0x2C14: 0xAE422F40, 0x2C1C: 0x26100098, 0x2C20: 0x1AE0FF36,
            0x2C24: 0x26940098, 0x2C54: 0x27BD0100,
        }
        for _, data in self.legal_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word, hex(offset))
            for register, expected in (
                    (18, [0x28B8, 0x2C44]), (20, [0x28C0, 0x2C24, 0x2C3C]),
                    (17, [0x28EC, 0x2C48]), (16, [0x28F8, 0x2C1C, 0x2C4C]),
                    (29, [0x28B0, 0x2C54])):
                self.assertEqual(self.register_writes(data, 0x28B0, 0x2C58, register), expected)
            self.assertEqual([(off, size) for _, off, size in self.direct_stores(data, 0x28B0, 0x2C58, 17)],
                             [(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])

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
            ("O(ModelVariantSheet,size)", 136), ("sizeof(Sheets472Timing)", 52),
            ("O(Sheets472Timing,grow_begin)", 28), ("O(Sheets472Timing,grow_end)", 32),
            ("O(Sheets472Timing,fade_begin)", 40), ("O(Sheets472Timing,fade_end)", 44),
            ("O(Sheets472State,sheets)", 0x2B24), ("O(Sheets472State,polygon)", 0x2C48),
            ("O(Sheets472State,world_matrix)", 0x2E84), ("O(MATRIX,t)", 20),
            ("O(Sheets472State,frame)", 0x2EE8), ("O(Sheets472State,time)", 0x2EEC),
            ("O(Sheets472State,timing)", 0x2EFC), ("O(Sheets472State,phase)", 0x2F40),
            ("sizeof(Sheets472State)", 0x2F44),
            ("O(POLY_GT4,x0)", 8), ("O(POLY_GT4,x1)", 20),
            ("O(POLY_GT4,x2)", 32), ("O(POLY_GT4,x3)", 44),
        )
        directory = ROOT / "tmp/test-spanish-model472-layout"
        directory.mkdir(parents=True, exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/overlays/spanish_model_variant/variant472_sheets.c"\n'
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
            data = section.data()
        self.assertEqual(data, struct.pack("<" + "I" * len(constants), *(v for _, v in constants)))

    def test_complete_images_c_assembly_raw_owners_and_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL472 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"]) if "variant472_sheets" in s["source"]]
            selected_path = directory / "build" / selected["object"]
            input_owners = {}
            for path in (directory / "build").rglob("*.o"):
                with path.open("rb") as input_handle:
                    input_elf = ELFFile(input_handle)
                    for symbol in input_elf.get_section_by_name(".symtab").iter_symbols():
                        if not isinstance(symbol["st_shndx"], int):
                            continue
                        if not symbol.name.startswith(("func_", "D_")):
                            continue
                        section = input_elf.get_section(symbol["st_shndx"])
                        input_owners.setdefault(symbol.name, []).append(
                            (path, section.name, section["sh_flags"], symbol["st_size"],
                             section.data()[symbol["st_value"]:]))
            with linked_path.open("rb") as handle, selected_path.open("rb") as obj_handle:
                linked, compiled = ELFFile(handle), ELFFile(obj_handle)
                symbols, original = linked.get_section_by_name(".symtab"), compiled.get_section_by_name(".symtab")
                function_name = f"func_{base+0x28B0:X}"
                own, = original.get_symbol_by_name(function_name)
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 936, "STT_FUNC"))
                self.assertTrue(compiled.get_section(own["st_shndx"])["sh_flags"] & 4)
                self.assertIn(f"{selected_path.relative_to(ROOT)}(.text);", script)
                for start, end in self.spans:
                    name = f"func_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = input_owners[name]
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x2C58, 0x23A8)):
                    name = f"D_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = input_owners[name]
                    self.assertFalse(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual(owner[4][:size], image[start:start+size])
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertEqual(symbol["st_value"], base + start)
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
                    word, = struct.unpack_from("<I", image, 0x28B0 + site)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    address = ((base + 0x28B0 + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
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
            self.skipTest("Build the Spanish resident before checking its input owners")
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
            (self.config / "overlays/model_variant472_linker_symbols.txt").read_text(), re.M))
        self.assertEqual(len(bindings), 40)
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
                selection, = [
                    s for s in selections if (s[0] == "text") == executable
                    and s[1] <= address and address + size <= s[1] + s[2]]
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
                        self.assertEqual((definition["st_value"] + start, definition["st_size"],
                                          definition["st_info"]["type"]), (address, size, "STT_FUNC"))
                        self.assertEqual((final["st_value"], final["st_size"], final["st_info"]["type"]),
                                         (address, size, "STT_FUNC"))
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
                self.assertTrue(context + 0x2F48 <= base or base + size <= context)

    def test_terminal_records_match_selected_sources(self):
        with (ROOT / "notes/overlays/spanish-model-variant472-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        for module in self.modules:
            terminal = [r for r in rows if r["module"] == module["name"]][-1]
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"]) if "variant472_sheets" in s["source"]]
            self.assertEqual(terminal["result"], "matched")
            self.assertEqual((terminal["instruction_bytes"], terminal["different_words"]), ("936", "0"))
            self.assertEqual(terminal["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"],
                             hashlib.sha256(
                                 (ROOT / "src/overlays/spanish_model_variant/variant472_sheets.c").read_bytes() +
                                 (ROOT / "src/overlays/spanish_model_variant/variant472_sheets.h").read_bytes()).hexdigest())


if __name__ == "__main__":
    unittest.main()
