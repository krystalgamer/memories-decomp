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


class SpanishModelVariant405Tests(unittest.TestCase):
    spans = ((4, 0xD00), (0xD00, 0x12CC), (0x12CC, 0x1774),
             (0x1774, 0x1B64), (0x1B64, 0x1FBC))

    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m.get("linker_symbols", "").endswith("/model_variant405_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant405-instances.csv").open() as handle:
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

    def test_metadata_and_complete_inventory(self):
        self.assertEqual((len(self.modules), len(self.instances)), (8, 8))
        checksums = load_checksum_manifest(self.config / "files.sha256")
        totals = load_spanish_overlay_inventories(ROOT)
        for module in self.modules:
            row = self.instances[module["name"]]
            slot = int(row["slot"])
            base = 0x8013B000 + slot * 0x40000
            self.assertEqual(module["sector_offset"], int(row["sector"]))
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertNotIn("duplicate_sector_offsets", module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant405_bands" + ("_slot1" if slot else "") + ".c"
            outer_source = source.replace("variant405_bands", "variant405_outer_bands")
            pulses_source = source.replace("variant405_bands", "variant405_pulses")
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching, {"schema": 1, "functions": [{
                "address": f"0x{base+0xD00:X}", "size": "0x5CC",
                "profile": "gcc_2_8_1_g0_split", "source": pulses_source}, {
                "address": f"0x{base+0x1774:X}", "size": "0x3F0",
                "profile": "gcc_2_8_1_g0_split", "source": source}, {
                "address": f"0x{base+0x1B64:X}", "size": "0x458",
                "profile": "gcc_2_8_1_g0_split", "source": outer_source}]})
            self.assertEqual([s["source"] for s in c_segments(ROOT, layout)],
                             [pulses_source, source, outer_source])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0)-base, int(r["size"], 0)) for r in inventory],
                             [(start, end-start) for start, end in self.spans])
            self.assertEqual([r["status"] for r in inventory],
                             ["unmatched_asm", "matching_c", "unmatched_asm", "matching_c", "matching_c"])
            self.assertIn("No entry-reachable call observed", inventory[3]["notes"])
            self.assertEqual(totals[layout.stem]["function_count"], 5)
            self.assertEqual(totals[layout.stem]["matching_c_function_count"], 3)
            self.assertEqual(totals[layout.stem]["matching_c_bytes"], 3604)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x1FBC, 0x3044)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)

    def test_exhaustive_physical_census_and_descriptors(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import model_records

        images = list(self.legal_images())
        expected = set()
        observed = set()
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as handle:
            for record, model in enumerate(model_records()):
                for stage in range(7, 11):
                    sector = record * 276 + 180 + (stage - 7) * 10
                    handle.seek(sector * 2048)
                    header, = struct.unpack("<I", handle.read(4))
                    if header in (405, 555):
                        observed.add((model, record, stage, sector, header))
            for module, image in images:
                row = self.instances[module["name"]]
                model, record, stage, slot = (int(row[k]) for k in ("model", "record", "stage", "slot"))
                expected.add((model, record, stage, int(row["sector"]), 405 + slot * 150))
                self.assertEqual(record, model if model < 350 else model - 50)
                self.assertEqual(slot, (stage - 7) % 2)
                self.assertEqual(module["sector_offset"], record * 276 + 180 + (stage - 7) * 10)
                handle.seek((record * 276 + 275) * 2048 + 0x110)
                command = struct.unpack("<3i", handle.read(12))[(stage - 7) // 2]
                self.assertEqual(command, int(row["command_word"]))
                self.assertEqual(command, {57: 571000, 149: 571001, 419: 571002, 562: 571000}[model])
                descriptor = 0x1FF4 + command % 1000 * 32
                count, = struct.unpack_from("<i", image, descriptor + 12)
                self.assertEqual(count, 2 if model == 419 else 3)
                self.assertEqual(count, int(row["band_count"]))
                base = int(module["load_address"], 0)
                for offset, word in ((0x88, 0x3C030000 | ((base+0x1FF4+0x8000) >> 16)),
                                     (0x8C, 0x24630000 | ((base+0x1FF4) & 65535)),
                                     (0x9C, 0x00191140), (0xA4, 0xAEA23230)):
                    self.assertEqual(struct.unpack_from("<I", image, offset)[0], word)
        self.assertEqual(observed, expected)
        self.assertEqual({r[0] for r in observed}, {57, 149, 419, 562})

    def test_closed_cfgs_entry_layout_and_bounded_packet_stores(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import walk_function

        anchors = {
            0xC: 0x00809021, 0x14: 0x0240A821,
            0x1C: 0x26B90504, 0x24: 0x26B80864, 0x640: 0x2732011C,
            0x6C8: 0x2AE20011, 0x6FC: 0x27180120, 0x714: 0x2A620003,
            0x177C: 0x0080A821, 0x17B4: 0x26BE0504, 0x17C8: 0x26B10620,
            0x1918: 0x26B22E08, 0x1924: 0x26B02E32,
            0x1AA8: 0x26100034, 0x1AB0: 0x26520034, 0x1AB8: 0x29020010,
            0x1AD8: 0x8EA23224, 0x1AE0: 0x000211C0, 0x1B04: 0xAE230000,
            0x1B18: 0x8EA23230, 0x1B1C: 0x26310120, 0x1B30: 0x27DE0120,
        }
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            for start, end in self.spans:
                flow = walk_function(image, base, start, end-start)
                self.assertTrue(flow["closed"])
                self.assertEqual(flow["visited"], set(range(start, end, 4)))
                self.assertEqual((flow["returns"], flow["indirect_calls"]), (1, 0))
                self.assertEqual(flow["calls"], {base+x for x in (0xD00, 0x12CC, 0x1B64)}
                                 if start == 4 else set())
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(lifetimes.SpanishModelVariant460Tests.register_writes(image, 0x1774, 0x1B64, 21),
                             [0x177C, 0x1B44])
            self.assertEqual(lifetimes.SpanishModelVariant460Tests.register_writes(image, 0x1774, 0x1B64, 16),
                             [0x1924, 0x1AA8, 0x1B58])
            stores = sorted((off, size) for _, off, size in
                            lifetimes.SpanishModelVariant460Tests.direct_stores(image, 0x1774, 0x1B64, 16))
            self.assertEqual(stores, [(off, 1) for off in (-38, -37, -36, -26, -25, -24,
                                                         -14, -13, -12, -2, -1, 0)])
        self.assertEqual(0x504 + 3 * 0x120, 0x864)
        self.assertEqual(0x2E08 + 16 * 52, 0x3148)
        self.assertLessEqual(16 * 8 + 8, 17 * 8)
        self.assertLessEqual(0x2E32 + 15 * 52 + 1, 0x3148)

    def test_target_compiled_layout(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        constants = (
            ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16),
            ("sizeof(MATRIX)", 32), ("sizeof(GsCOORDINATE2)", 80),
            ("sizeof(POLY_GT4)", 52), ("sizeof(Bands405Band)", 0x120),
            ("OFF(Bands405Band, outer)", 0x88),
            ("OFF(Bands405Band, inner_color)", 0x110),
            ("OFF(Bands405Band, outer_color)", 0x114),
            ("OFF(Bands405Band, size)", 0x118),
            ("OFF(Bands405Band, completed)", 0x11C),
            ("sizeof(Bands405Descriptor)", 16),
            ("OFF(Bands405Descriptor, count)", 12),
            ("OFF(Bands405State, bands)", 0x504),
            ("OFF(Bands405State, polygons)", 0x2E08),
            ("OFF(Bands405State, translation)", 0x31D0),
            ("OFF(Bands405State, step)", 0x3224),
            ("OFF(Bands405State, descriptor)", 0x3230),
            ("sizeof(((Bands405State *)0)->descriptor)", 4),
            ("sizeof(Bands405State)", 0x3234),
            ("OFF(POLY_GT4, x0)", 8),
            ("OFF(POLY_GT4, x1)", 20),
            ("OFF(POLY_GT4, x2)", 32),
            ("OFF(POLY_GT4, x3)", 44),
        )
        directory = ROOT / "tmp/model405-layout-tests"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n'
                          '#include "../../src/overlays/spanish_model_variant/variant405_bands.h"\n'
                          '#define OFF(t, f) ((u32)&((t *)0)->f)\n'
                          'u32 layout[] = {\n' + ",\n".join(x for x, _ in constants) + "\n};\n")
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

    def test_terminal_records_match_production_sources(self):
        with (ROOT / "notes/overlays/spanish-model-variant405-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([int(r["different_words"]) for r in rows], [33, 13] + [0] * 10)
        body = ROOT / "src/overlays/spanish_model_variant/variant405_bands.c"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes()).hexdigest()
        for module in self.modules:
            terminal = [r for r in rows if r["module"] == module["name"]][-1]
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant405_bands" in s["source"]]
            self.assertEqual((terminal["result"], terminal["instruction_bytes"], terminal["different_words"]),
                             ("matched", "1008", "0"))
            self.assertEqual(terminal["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)


    def test_complete_images_input_final_owners_and_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build Spanish MODEL405 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant405_bands" in s["source"]]
            selected_path = directory / "build" / selected["object"]
            c_paths = {directory / "build" / s["object"]
                       for s in c_segments(ROOT, ROOT / module["layout"])}
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
                own, = original.get_symbol_by_name(f"func_{base+0x1774:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1008, "STT_FUNC"))
                self.assertTrue(compiled.get_section(own["st_shndx"])["sh_flags"] & 4)
                self.assertIn(f"{selected_path.relative_to(ROOT)}(.text);", script)
                for start, end in self.spans:
                    name = f"func_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = owners[name]
                    self.assertEqual(owner[0] == selected_path, start == 0x1774)
                    self.assertEqual(owner[0] in c_paths, start in (0xD00, 0x1774, 0x1B64))
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x1FBC, 0x3044)):
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
                local_jumps = []
                text = compiled.get_section(own["st_shndx"]).data()
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    symbol = original.get_symbol(relocation["r_info_sym"])
                    site = relocation["r_offset"]
                    word, = struct.unpack_from("<I", image, 0x1774 + site)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    address = ((base + 0x1774 + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    if symbol["st_shndx"] != "SHN_UNDEF":
                        self.assertEqual(word >> 26, 2)
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        self.assertEqual(address, base + 0x1774 + symbol["st_value"] + addend)
                        local_jumps.append((site, address - base - 0x1774))
                        continue
                    self.assertEqual(word >> 26, 3)
                    resolved, = symbols.get_symbol_by_name(symbol.name)
                    self.assertEqual(resolved["st_value"], bindings[symbol.name])
                    self.assertEqual(address, resolved["st_value"] + addend)
                    callees.add(address)
                    count += 1
                self.assertEqual(count, 10)
                self.assertEqual(local_jumps, [(0x138, 0x188)])
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
            (self.config / "overlays/model_variant405_linker_symbols.txt").read_text(), re.M))
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
                self.assertTrue(context + 0x3234 <= base or base + size <= context)



if __name__ == "__main__":
    unittest.main()
