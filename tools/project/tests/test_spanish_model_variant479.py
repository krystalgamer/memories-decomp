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


class SpanishModelVariant479Tests(unittest.TestCase):
    spans = ((4, 0xFD0), (0xFD0, 0x14F8), (0x14F8, 0x1F68), (0x1F68, 0x2B90),
             (0x2B90, 0x309C), (0x309C, 0x3860), (0x3860, 0x3D20))
    register_writes = staticmethod(lifetimes.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(lifetimes.SpanishModelVariant460Tests.direct_stores)

    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m.get("linker_symbols", "").endswith("/model_variant479_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant479-instances.csv").open() as handle:
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
            self.assertEqual((int(row["model"]), int(row["record"]), stage), (379, 329, 7 + slot))
            self.assertEqual(int(row["header"]), 479 + slot * 150)
            self.assertEqual(int(row["command_word"]), 645000)
            self.assertEqual(module["sector_offset"], 329 * 276 + 180 + slot * 10)
            self.assertEqual(module["sector_offset"], int(row["sector"]))
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertNotIn("duplicate_sector_offsets", module)
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant479_curtains" + ("_slot1" if slot else "") + ".c"
            sheet_source = source.replace("_curtains", "_sheets")
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching, {"schema": 1, "functions": [{
                "address": f"0x{base+0x2B90:X}", "size": "0x50C",
                "profile": "gcc_2_8_1_g0_split", "source": sheet_source}, {
                "address": f"0x{base+0x3860:X}", "size": "0x4C0",
                "profile": "gcc_2_8_1_g0_split", "source": source}]})
            self.assertEqual([s["source"] for s in c_segments(ROOT, layout)], [sheet_source, source])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0)-base, int(r["size"], 0)) for r in inventory],
                             [(start, end-start) for start, end in self.spans])
            self.assertEqual([r["status"] for r in inventory], ["unmatched_asm"] * 4 + ["matching_c", "unmatched_asm", "matching_c"])
            self.assertIn("No entry-reachable call observed", inventory[5]["notes"])
            self.assertEqual(totals[layout.stem]["function_count"], 7)
            self.assertEqual(totals[layout.stem]["matching_c_function_count"], 2)
            self.assertEqual(totals[layout.stem]["matching_c_bytes"], 2508)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x3D20, 0x12E0)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0x3D20, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())

    def test_physical_loads_descriptor_and_inactive_primary_bank(self):
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
                    if header in (479, 629):
                        observed.add((model, record, stage, sector, header))
            handle.seek((329 * 276 + 275) * 2048 + 0x110)
            self.assertEqual(struct.unpack("<3i", handle.read(12)), (645000, 593000, -2))
            for slot, digest in enumerate((
                    "c50696aec6be6a7b456a35833901a32481153a63cfc6a83facff498bcf4011af",
                    "7de25b039253ab24fd47a3398064cf72c1bbc3b08c0087762a919bcf68c821f9")):
                handle.seek((329 * 276 + 220 + slot * 2) * 2048)
                primary = handle.read(4096)
                self.assertEqual(hashlib.sha256(primary).hexdigest(), digest)
                self.assertEqual(struct.unpack_from("<II", primary, 4), (0x03E00008, 0x24020002))
        self.assertEqual(observed, {(379, 329, 7, 90984, 479), (379, 329, 8, 90994, 629)})
        for module, data in images:
            base = int(module["load_address"], 0)
            self.assertEqual(data[0x3E20:0x3E23], bytes((180, 192, 192)))
            self.assertLessEqual(0x3E1C + 44, len(data))
            for offset, word in ((0xB0, 0x3C030000 | ((base + 0x3E1C + 0x8000) >> 16)),
                                 (0xBC, 0x2463EE1C), (0xC0, 0x00191040), (0xC4, 0x00591021),
                                 (0xC8, 0x00021080), (0xCC, 0x00591023), (0xD0, 0x00021080),
                                 (0xD8, 0xAFC24178)):
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
        retail = (ROOT / "game/spain/SLES_039.51").read_bytes()
        for address, word in ((0x80058BA8, 0x8E020D10), (0x80058BB0, 0x04400012),
                              (0x80058BEC, 0x0220F809), (0x80058BFC, 0x92030E0E)):
            self.assertEqual(struct.unpack_from("<I", retail, address-0x8000F800)[0], word)
        self.assertEqual(0x80058BB0 + 4 + 0x12 * 4, 0x80058BFC)
        self.assertGreater(0x80058BFC, 0x80058BEC)

    def test_closed_cfgs_retained_helper_and_original_context(self):
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
                self.assertEqual(flow["calls"], {base+x for x in (0xFD0, 0x14F8, 0x1F68, 0x2B90, 0x3860)}
                                 if start == 4 else set())
            for call, target in ((0xDE4, 0x3860), (0xE68, 0x2B90)):
                self.assertEqual(struct.unpack_from("<II", data, call),
                                 (0x0C000000 | ((base + target) >> 2 & 0x3FFFFFF), 0x02C02021))
            definitions = set(self.register_writes(data, 4, 0xFD0, 22))
            pending, visited, reaching = [(4, None)], set(), {0xDE4: set(), 0xE68: set()}
            while pending:
                pc, definition = pending.pop()
                self.assertTrue(4 <= pc < 0xFD0 and pc % 4 == 0)
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
            self.assertEqual(reaching, {0xDE4: {0xC}, 0xE68: {0xC}})

    def test_geometry_packet_bounds_and_conditional_bank_overlap(self):
        anchors = {
            0x18: 0x27D838C0, 0x54: 0x27D93E6C, 0x600: 0x271804F8,
            0x610: 0x2B020005, 0x9E0: 0x2A220011, 0xA30: 0x26940118,
            0xA44: 0x2B220005, 0xDD4: 0x2442FFFD, 0xDD8: 0x2C420003,
            0x3860: 0x27BDFED0, 0x3868: 0x0080A021, 0x3894: 0x8E834164,
            0x3898: 0x8E842EB8, 0x38A0: 0x00031940, 0x38AC: 0x26923E6C,
            0x38C4: 0x24630FFF, 0x38C8: 0x24040514, 0x38CC: 0x00031B03,
            0x3904: 0x044000E4, 0x39C0: 0x2A620011, 0x3AA4: 0x28821000,
            0x3AD4: 0x24022000, 0x3C38: 0x04C00010, 0x3C48: 0x0440000C,
            0x3C54: 0x30C6FFFF, 0x3C8C: 0x2A620010, 0x3CB0: 0x25EF0118,
            0x3CB4: 0x258C04F8, 0x3CB8: 0x29C20005, 0x3CD0: 0x8E824170,
            0x3CD8: 0x00021880, 0x3CDC: 0x00621821, 0x3CE4: 0x00031900,
            0x3CEC: 0xAE8241D8,
        }
        for _, data in self.legal_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word, hex(offset))
            for register, expected in ((20, [0x3868, 0x3D04]), (18, [0x38AC, 0x3D0C]),
                                       (29, [0x3860, 0x3D1C])):
                self.assertEqual(self.register_writes(data, 0x3860, 0x3D20, register), expected)
            self.assertEqual(sorted((off, size) for _, off, size in self.direct_stores(data, 0x3860, 0x3D20, 18)),
                             [(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
        self.assertEqual(5 * 1272, 0x18D8)
        self.assertEqual(0x38C0 + 5 * 280, 0x3E38)
        self.assertEqual(0x3E6C + 52, 0x3EA0)
        self.assertEqual(0x4078 + 5 * 16, 0x40C8)
        for slot in (0, 1):
            context = 0x80136000 + slot * 0x40000
            primary = 0x8013A000 + slot * 0x40000
            code = 0x8013B000 + slot * 0x40000
            self.assertEqual(context + 0x41DC - primary, 476)
            self.assertLess(context + 0x41DC, code)

    def test_target_compiled_layout(self):
        compiler = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
        if not compiler.is_file() or importlib.util.find_spec("elftools") is None:
            self.skipTest("Local target compiler and pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        constants = (
            ("sizeof(SVECTOR)", 8), ("sizeof(VECTOR)", 16), ("sizeof(MATRIX)", 32),
            ("sizeof(GsCOORDINATE2)", 80), ("sizeof(POLY_GT4)", 52),
            ("sizeof(Curtains479Primary)", 1272), ("O(Curtains479Primary,size)", 0x4EC),
            ("sizeof(ModelVariantCurtain)", 280), ("O(ModelVariantCurtain,b)", 0x88),
            ("O(Curtains479State,primary)", 0), ("O(Curtains479State,pulse_scale)", 0x2EB8),
            ("O(Curtains479State,curtains)", 0x38C0), ("O(Curtains479State,polygon)", 0x3E6C),
            ("O(Curtains479State,positions)", 0x4078), ("O(Curtains479State,frame)", 0x4164),
            ("O(Curtains479State,step)", 0x4170), ("O(Curtains479State,gate)", 0x4198),
            ("O(Curtains479State,inner)", 0x41D0), ("O(Curtains479State,outer)", 0x41D4),
            ("O(Curtains479State,angle)", 0x41D8), ("sizeof(Curtains479State)", 0x41DC),
            ("O(POLY_GT4,x0)", 8), ("O(POLY_GT4,x1)", 20), ("O(POLY_GT4,x2)", 32),
            ("O(POLY_GT4,x3)", 44), ("O(ModelSlot,field_CF8)", 0xCF8),
            ("O(ModelSlot,field_DE8)", 0xDE8), ("O(ModelSlot,field_DEC)", 0xDEC),
            ("O(ModelControlCommandView,commands)", 0xD08),
            ("O(ModelSlot,field_CF8.field_18)", 0xD10),
            ("O(ModelTransferMetadata,field_CF8)", 0x100), ("sizeof(ModelSlotCF8BlockWords)", 0x1C),
        )
        directory = ROOT / "tmp/test-spanish-model479-layout"
        directory.mkdir(parents=True, exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/overlays/spanish_model_variant/variant479_curtains.h"\n'
                          '#include "../../src/game/model_control.h"\n'
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
                self.skipTest("Build Spanish MODEL479 images before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant479_curtains" in s["source"]]
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
                own, = original.get_symbol_by_name(f"func_{base+0x3860:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1216, "STT_FUNC"))
                self.assertTrue(compiled.get_section(own["st_shndx"])["sh_flags"] & 4)
                self.assertIn(f"{selected_path.relative_to(ROOT)}(.text);", script)
                for start, end in self.spans:
                    name = f"func_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = owners[name]
                    self.assertEqual(owner[0] == selected_path, start == 0x3860)
                    self.assertEqual(owner[0] in c_paths, start in (0x2B90, 0x3860))
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x3D20, 0x12E0)):
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
                    word, = struct.unpack_from("<I", image, 0x3860 + site)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    address = ((base + 0x3860 + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    resolved, = symbols.get_symbol_by_name(symbol.name)
                    self.assertEqual(resolved["st_value"], bindings[symbol.name])
                    self.assertEqual(address, resolved["st_value"] + addend)
                    callees.add(address)
                    count += 1
                self.assertEqual(count, 13)
                self.assertEqual(callees, {0x8005C018, 0x800842A8, 0x80085558, 0x80086258,
                                          0x80086628, 0x800875F8, 0x800866F8, 0x80087958, 0x80087CB8})

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
            (self.config / "overlays/model_variant479_linker_symbols.txt").read_text(), re.M))
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
                if size == 4096:
                    self.assertEqual(context + 0x41DC - base, 476)
                else:
                    self.assertTrue(context + 0x41DC <= base or base + size <= context)

    def test_terminal_records_match_selected_sources(self):
        with (ROOT / "notes/overlays/spanish-model-variant479-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 11)
        self.assertEqual([int(r['different_words']) for r in rows],
                         [270, 286, 263, 272, 18, 4, 24, 0, 0, 0, 0])
        for module in self.modules:
            terminal = [r for r in rows if r["module"] == module["name"]][-1]
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant479_curtains" in s["source"]]
            self.assertEqual(terminal["result"], "matched")
            self.assertEqual((terminal["instruction_bytes"], terminal["different_words"]), ("1216", "0"))
            self.assertEqual(terminal["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"],
                             hashlib.sha256(
                                 (ROOT / "src/overlays/spanish_model_variant/variant479_curtains.c").read_bytes() +
                                 (ROOT / "src/overlays/spanish_model_variant/variant479_curtains.h").read_bytes()).hexdigest())


if __name__ == "__main__":
    unittest.main()
