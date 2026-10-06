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


class SpanishModelVariant464Tests(unittest.TestCase):
    spans = ((4, 0x1368), (0x1368, 0x18EC), (0x18EC, 0x1E94),
             (0x1E94, 0x2320), (0x2320, 0x2838), (0x2838, 0x2DAC))
    cases = {
        175: (7, 48480, 630001, 5, 30, 60, 160, 180),
        182: (9, 50432, 630002, 5, 20, 40, 120, 136),
        244: (7, 67524, 630000, 2, 160, 200, 500, 560),
    }
    register_writes = staticmethod(lifetimes.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(lifetimes.SpanishModelVariant460Tests.direct_stores)

    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m.get("linker_symbols", "").endswith("/model_variant464_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant464-instances.csv").open() as handle:
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
        self.assertEqual(len(self.modules), 6)
        self.assertEqual(len(self.instances), 6)
        checksums = load_checksum_manifest(self.config / "files.sha256")
        totals = load_spanish_overlay_inventories(ROOT)
        for module in self.modules:
            row = self.instances[module["name"]]
            model, slot, stage = int(row["model"]), int(row["slot"]), int(row["stage"])
            first_stage, sector, command, count, *_ = self.cases[model]
            base = 0x8013B000 + slot * 0x40000
            self.assertEqual((int(row["record"]), stage), (model, first_stage + slot))
            self.assertEqual(int(row["header"]), 464 + slot * 150)
            self.assertEqual((int(row["command_word"]), int(row["primary_count"])), (command, count))
            self.assertEqual(module["sector_offset"], model * 276 + 180 + (stage - 7) * 10)
            self.assertEqual(module["sector_offset"], sector + slot * 10)
            self.assertEqual(module["sector_offset"], int(row["sector"]))
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertNotIn("duplicate_sector_offsets", module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant464_sheets" + ("_slot1" if slot else "") + ".c"
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching, {"schema": 1, "functions": [{
                "address": f"0x{base+0x18EC:X}", "size": "0x5A8",
                "profile": "gcc_2_8_1_g0_split", "source": source.replace("_sheets", "_advancing")}, {
                "address": f"0x{base+0x1E94:X}", "size": "0x48C",
                "profile": "gcc_2_8_1_g0_split", "source": source}]})
            self.assertEqual([s["source"] for s in c_segments(ROOT, layout)],
                             [source.replace("_sheets", "_advancing"), source])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0)-base, int(r["size"], 0)) for r in inventory],
                             [(start, end-start) for start, end in self.spans])
            self.assertEqual([r["status"] for r in inventory],
                             ["unmatched_asm"] * 2 + ["matching_c"] * 2 + ["unmatched_asm"] * 2)
            for row in inventory[4:]:
                self.assertIn("No entry-reachable call observed", row["notes"])
            self.assertEqual(totals[layout.stem]["function_count"], 6)
            self.assertEqual(totals[layout.stem]["matching_c_function_count"], 2)
            self.assertEqual(totals[layout.stem]["matching_c_bytes"], 2612)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x2DAC, 0x2254)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0x2DAC, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())

    def test_all_physical_loads_and_three_actual_descriptors(self):
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
                    if header in (464, 614):
                        observed.add((model, record, stage, sector, header))
                        handle.seek((record * 276 + 275) * 2048 + 0x110 + (stage - 7) // 2 * 4)
                        commands[model, stage], = struct.unpack("<i", handle.read(4))
        self.assertEqual(observed, {(model, model, case[0]+slot, case[1]+slot*10, 464+slot*150)
                                    for model, case in self.cases.items() for slot in (0, 1)})
        for module, data in images:
            row = self.instances[module["name"]]
            model, stage = int(row["model"]), int(row["stage"])
            _, _, command, count, grow_begin, grow_end, fade_begin, fade_end = self.cases[model]
            self.assertEqual(commands[model, stage], command)
            descriptor = 0x2EA8 + command % 1000 * 72
            self.assertGreaterEqual(descriptor, 0x2DAC)
            self.assertLessEqual(descriptor + 72, len(data))
            self.assertEqual(struct.unpack_from("<2I", data, descriptor + 0x1C), (grow_begin, grow_end))
            self.assertEqual(struct.unpack_from("<2I", data, descriptor + 0x28), (fade_begin, fade_end))
            self.assertEqual(struct.unpack_from("<i", data, descriptor + 0x44), (count,))
            base = int(module["load_address"], 0)
            for offset, word in ((0xA8, 0x3C020000 | ((base + 0x2EA8 + 0x8000) >> 16)),
                                 (0xAC, 0x2442DEA8), (0xB8, 0x001918C0),
                                 (0xBC, 0x00791821), (0xC0, 0x000318C0)):
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
            self.assertIn(count, (2, 5))
            self.assertLessEqual(0x4E0 + (count + 1) * 132, 0x7F8)
            self.assertLessEqual(0x774 + (count + 1) * 156, 0xB1C)

    def test_closed_cfgs_two_unreached_helpers_and_original_context(self):
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
                self.assertEqual(flow["calls"], {base + off for off in (0x1368, 0x18EC, 0x1E94)}
                                 if start == 4 else set())
            self.assertEqual(struct.unpack_from("<II", data, 0x11D0),
                             (0x0C000000 | ((base + 0x1E94) >> 2 & 0x3FFFFFF), 0x02802021))
            self.assertEqual(struct.unpack_from("<II", data, 0x1200),
                             (0x0C000000 | ((base + 0x18EC) >> 2 & 0x3FFFFFF), 0x02802021))
            for offset, word in ((0x11E8, 0x8C430030), (0x11F4, 0x0043102B), (0x11F8, 0x14400003)):
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
            definitions = set(self.register_writes(data, 4, 0x1368, 20))
            pending, visited, reaching = [(4, None)], set(), {0x11D0: set(), 0x1200: set()}
            while pending:
                pc, definition = pending.pop()
                self.assertTrue(4 <= pc < 0x1368 and pc % 4 == 0)
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
                    if pc in reaching:
                        reaching[pc].add(definition)
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
            self.assertEqual(reaching, {0x11D0: {0xC}, 0x1200: {0xC}})

    def test_primary_bounds_packet_lifetimes_and_scale_paths(self):
        anchors = {
            0x18: 0x27D804E0, 0x1C: 0x27D90774, 0x518: 0x28C20005, 0x51C: 0x27180084,
            0xA94: 0x2AC20006, 0xA9C: 0x2463009C, 0x11C4: 0x0043102B, 0x11C8: 0x14400005,
            0x1E94: 0x27BDFEF8, 0x1ED0: 0xAFA800D8, 0x1EDC: 0xAFA200DC,
            0x1EE0: 0x8C7E0044, 0x1EE8: 0x07C00101, 0x1EEC: 0x26511AB4,
            0x1F2C: 0x16FE000A, 0x1F34: 0x8E421BD8, 0x1F58: 0x86421BE4,
            0x20D8: 0x000210C0, 0x20E4: 0x00440018, 0x2124: 0x04600008,
            0x2134: 0x04400004, 0x2144: 0x3066FFFF, 0x2158: 0x16FE003A,
            0x2168: 0x1440001B, 0x21A0: 0x0043001B, 0x21D4: 0xAE421C60,
            0x21E8: 0x0083102B, 0x2240: 0xAE421C60, 0x224C: 0x8D040080,
            0x2254: 0x1482001E, 0x2264: 0x28624000, 0x2270: 0x8E020010,
            0x2288: 0x00021300, 0x22A8: 0xAE040010, 0x22BC: 0x00021240,
            0x22CC: 0xAE000000, 0x22E4: 0x25290084, 0x22E8: 0x1040FF03,
        }
        for _, data in self.legal_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word, hex(offset))
            for register, expected in (
                    (18, [0x1E9C, 0x230C]), (17, [0x1EEC, 0x2310]),
                    (23, [0x1ED8, 0x22D0, 0x22F8]), (30, [0x1EE0, 0x22F4]),
                    (20, [0x1FA4, 0x2154, 0x2304]), (29, [0x1E94, 0x231C])):
                self.assertEqual(self.register_writes(data, 0x1E94, 0x2320, register), expected)
            self.assertEqual([(off, size) for _, off, size in self.direct_stores(data, 0x1E94, 0x2320, 17)],
                             [(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
            self.assertEqual([store for store in self.direct_stores(data, 0x1E94, 0x2320, 29)
                              if store[1] in (0xD8, 0xDC)],
                             [(0x1ED0, 0xD8, 4), (0x1EDC, 0xDC, 4), (0x22EC, 0xD8, 4)])
        self.assertEqual(0x4E0 + 5 * 132, 0x774)
        self.assertEqual(0x4E0 + 6 * 132, 0x7F8)
        self.assertLess(0x7F8, 0x1C64)
        self.assertEqual(0x774 + 6 * 156, 0xB1C)
        self.assertEqual(0x1AB4 + 52, 0x1AE8)

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
            ("sizeof(Sheets464Primary)", 132), ("O(Sheets464Primary,active)", 128),
            ("sizeof(Sheets464Timing)", 72), ("O(Sheets464Timing,grow_begin)", 0x1C),
            ("O(Sheets464Timing,grow_end)", 0x20), ("O(Sheets464Timing,fade_begin)", 0x28),
            ("O(Sheets464Timing,fade_end)", 0x2C), ("O(Sheets464Timing,count)", 0x44),
            ("O(Sheets464State,primary)", 0x4E0), ("O(Sheets464State,sheets)", 0x774),
            ("O(Sheets464State,polygon)", 0x1AB4), ("O(Sheets464State,origin)", 0x1BD8),
            ("O(Sheets464State,anchor)", 0x1BE4), ("O(Sheets464State,frame)", 0x1C18),
            ("O(Sheets464State,time)", 0x1C1C), ("O(Sheets464State,step)", 0x1C24),
            ("O(Sheets464State,timing)", 0x1C2C), ("O(Sheets464State,phase)", 0x1C60),
            ("sizeof(Sheets464State)", 0x1C64), ("O(POLY_GT4,x0)", 8),
            ("O(POLY_GT4,x1)", 20), ("O(POLY_GT4,x2)", 32), ("O(POLY_GT4,x3)", 44),
        )
        directory = ROOT / "tmp/test-spanish-model464-layout"
        directory.mkdir(parents=True, exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/overlays/spanish_model_variant/variant464_sheets.h"\n'
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
                self.skipTest("Build Spanish MODEL464 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "/variant464_sheets" in s["source"]]
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
                own, = original.get_symbol_by_name(f"func_{base+0x1E94:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1164, "STT_FUNC"))
                self.assertTrue(compiled.get_section(own["st_shndx"])["sh_flags"] & 4)
                self.assertIn(f"{selected_path.relative_to(ROOT)}(.text);", script)
                for start, end in self.spans:
                    name = f"func_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = owners[name]
                    self.assertEqual(owner[0] == selected_path, start == 0x1E94)
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x2DAC, 0x2254)):
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
                    word, = struct.unpack_from("<I", image, 0x1E94 + site)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    address = ((base + 0x1E94 + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
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
            (self.config / "overlays/model_variant464_linker_symbols.txt").read_text(), re.M))
        self.assertEqual((len(bindings), len(set(bindings.values()))), (40, 36))
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
                self.assertTrue(context + 0x1C64 <= base or base + size <= context)

    def test_terminal_records_match_selected_sources(self):
        with (ROOT / "notes/overlays/spanish-model-variant464-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        for module in self.modules:
            terminal = [r for r in rows if r["module"] == module["name"]][-1]
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "/variant464_sheets" in s["source"]]
            self.assertEqual(terminal["result"], "matched")
            self.assertEqual((terminal["instruction_bytes"], terminal["different_words"]), ("1164", "0"))
            self.assertEqual(terminal["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"],
                             hashlib.sha256(
                                 (ROOT / "src/overlays/spanish_model_variant/variant464_sheets.c").read_bytes() +
                                 (ROOT / "src/overlays/spanish_model_variant/variant464_sheets.h").read_bytes()).hexdigest())


if __name__ == "__main__":
    unittest.main()
