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


def reaching_context(image, base, target):
    definitions = set(lifetimes.SpanishModelVariant460Tests.register_writes(image, 4, 0xFB4, 21))
    pending, visited, reaching = [(4, None)], set(), set()
    while pending:
        pc, definition = pending.pop()
        if not (4 <= pc < 0xFB4 and pc % 4 == 0):
            raise AssertionError(f"Entry edge escapes its span: {pc:#x}")
        if (pc, definition) in visited:
            continue
        visited.add((pc, definition))
        if pc in definitions:
            definition = pc
        if pc == target:
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
            pending.extend((next_pc, definition) for next_pc in targets)
        else:
            pending.append((pc + 4, definition))
    return reaching


class SpanishModelVariant427Tests(unittest.TestCase):
    boundaries = (4, 0xFB4, 0x157C, 0x1FC4, 0x28A0, 0x2DF8, 0x35BC, 0x3ACC)

    def setUp(self):
        manifest = json.loads((ROOT / "config/sles_03951/overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m["linker_symbols"].endswith("/model_variant427_linker_symbols.txt")]
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

    def test_metadata_exhaustive_physical_inputs_and_inventory(self):
        if importlib.util.find_spec("rabbitizer") is None:
            self.skipTest("Optional rabbitizer required")
        from overlay_function_inventory import model_records, walk_function

        images = list(self.legal_images())
        checksums = load_checksum_manifest(ROOT / "config/sles_03951/files.sha256")
        with (ROOT / "notes/overlays/spanish-model-variant427-instances.csv").open() as handle:
            instances = {r["module"]: r for r in csv.DictReader(handle)}
        self.assertEqual(set(instances), {m["name"] for m in self.modules})
        observed = set()
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as handle:
            for record, model in enumerate(model_records()):
                for stage in range(7, 11):
                    sector = record * 276 + 180 + (stage - 7) * 10
                    handle.seek(sector * 2048)
                    header, = struct.unpack("<I", handle.read(4))
                    if header in (427, 577):
                        observed.add((model, record, stage, sector, header))
            self.assertEqual(observed, {(379, 329, 9, 91004, 427), (379, 329, 10, 91014, 577)})
            handle.seek((329 * 276 + 275) * 2048 + 0x110)
            self.assertEqual(struct.unpack("<3i", handle.read(12))[1], 593000)
        for module, image in images:
            row = instances[module["name"]]
            slot = int(row["slot"])
            base = 0x8013B000 + slot * 0x40000
            self.assertEqual((int(row["model"]), int(row["record"]), int(row["stage"]),
                              int(row["command_word"])), (379, 329, 9 + slot, 593000))
            self.assertEqual((module["sector_offset"], module["sector_count"]), (91004 + slot * 10, 10))
            self.assertEqual(module["sector_offset"], int(row["sector"]))
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(int(module["load_address"], 0), base)
            self.assertEqual(struct.unpack_from("<I", image)[0], 427 + slot * 150)
            layout = ROOT / module["layout"]
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            spans = list(zip(self.boundaries, self.boundaries[1:]))
            self.assertEqual([(int(r["address"], 0)-base, int(r["size"], 0)) for r in inventory],
                             [(start, end-start) for start, end in spans])
            self.assertEqual([r["status"] for r in inventory],
                             ["unmatched_asm"] * 4 + ["matching_c", "unmatched_asm", "matching_c"])
            selected, = [s for s in c_segments(ROOT, layout) if "variant427_curtains" in s["source"]]
            source = "src/overlays/spanish_model_variant/variant427_curtains" + ("_slot1" if slot else "") + ".c"
            panels = "src/overlays/spanish_model_variant/variant427_panels" + ("_slot1" if slot else "") + ".c"
            self.assertEqual(selected["source"], source)
            self.assertEqual(json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text()),
                             {"schema": 1, "functions": [
                              {"address": f"0x{base+0x28A0:X}", "size": "0x558",
                               "profile": "gcc_2_8_1_g0_split", "source": panels},
                              {"address": f"0x{base+0x35BC:X}", "size": "0x510",
                              "profile": "gcc_2_8_1_g0_split", "source": source}]})
            for start, end in spans:
                flow = walk_function(image, base, start, end-start)
                self.assertTrue(flow["closed"])
                self.assertEqual(flow["visited"], set(range(start, end, 4)))
                self.assertEqual((flow["returns"], flow["indirect_calls"]), (1, 0))
                self.assertTrue(flow["calls"] <= {base+x for x in self.boundaries[:-1]})
                if start == 4:
                    self.assertIn(base + 0x35BC, flow["calls"])
                    self.assertNotIn(base + 0x2DF8, flow["calls"])

    def test_original_context_phase_gate_and_record_packet_bounds(self):
        decoder = lifetimes.SpanishModelVariant460Tests
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            self.assertEqual(reaching_context(image, base, 0xE04), {0xC})
            for call, target in ((0xE04, 0x35BC), (0xE0C, 0xFB4)):
                self.assertEqual(struct.unpack_from("<I", image, call)[0],
                                 0x0C000000 | ((base + target) >> 2 & 0x3FFFFFF))
            anchors = {
                0xC: 0x0080A821, 0x18: 0x27D82A78, 0x54: 0x27D93484,
                0x60: 0xAFB9009C, 0x4A4: 0x8FA4009C, 0x4A8: 0x0C020BBA,
                0x4AC: 0, 0x4C0: 0xA482001A, 0x4CC: 0xA331000C,
                0x4E4: 0xA3300030, 0x4F4: 0xA738000E,
                0x610: 0xAF2004EC, 0x618: 0x273904F0, 0x624: 0x2B020003,
                0x92C: 0x27340114, 0x980: 0x26700088, 0x9A4: 0x2A420011,
                0x9AC: 0x00128A00, 0x9C8: 0xAE80FFFC, 0x9EC: 0xAE800000,
                0x9F0: 0x26940118, 0xA00: 0x27180118, 0xA04: 0x2B220009,
                0xDF4: 0x2442FFFD, 0xDF8: 0x2C420003, 0xE08: 0x02A02021,
                0x35C4: 0x00808821, 0x35CC: 0x263E2A78, 0x35D4: 0x26323484,
                0x3A60: 0x26F70001, 0x3A64: 0x27DE0118, 0x3A6C: 0x2AE20009,
                0x3A70: 0x25EF0118,
            }
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(decoder.register_writes(image, 0x35BC, 0x3ACC, 18), [0x35D4, 0x3AB8])
            stores = sorted((off, size) for _, off, size in decoder.direct_stores(image, 0x35BC, 0x3ACC, 18))
            self.assertEqual(stores, [(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)])
        self.assertEqual(3 * 0x4F0, 0xED0)
        self.assertEqual(0x2A78 + 9 * 0x118, 0x3450)
        self.assertEqual(16 * 8 + 8, 17 * 8)
        self.assertEqual(0x3484 + 52, 0x34B8)

    def test_target_compiled_layout_and_terminal_fingerprints(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        expressions = """
sizeof(SVECTOR), sizeof(VECTOR), sizeof(MATRIX), sizeof(GsCOORDINATE2),
sizeof(POLY_GT4), sizeof(Curtain427Primary), OFF(Curtain427Primary, scale),
sizeof(ModelVariantCurtain), OFF(ModelVariantCurtain, b),
OFF(ModelVariantCurtain, scale), OFF(ModelVariantCurtain, count),
OFF(Curtain427State, primary), OFF(Curtain427State, curtains),
OFF(Curtain427State, polygon), OFF(Curtain427State, origins),
OFF(Curtain427State, step), OFF(Curtain427State, size),
OFF(Curtain427State, intensity), OFF(Curtain427State, inner),
OFF(Curtain427State, outer), OFF(Curtain427State, angle),
OFF(Curtain427State, phase), sizeof(Curtain427State),
OFF(POLY_GT4, x0), OFF(POLY_GT4, x1), OFF(POLY_GT4, x2), OFF(POLY_GT4, x3)
"""
        expected = (8, 16, 32, 80, 52, 0x4F0, 0x4EC, 0x118, 0x88, 0x110, 0x114,
                    0, 0x2A78, 0x3484, 0x3690, 0x3748, 0x3770, 0x3780, 0x37A8,
                    0x37AC, 0x37B0, 0x37C4, 0x37C8, 8, 20, 32, 44)
        directory = ROOT / "tmp/model427-layout-tests"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n'
                          '#include "../../src/overlays/spanish_model_variant/variant427_curtains.h"\n'
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
        with (ROOT / "notes/overlays/spanish-model-variant427-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([int(r["different_words"]) for r in rows], [275, 4, 0, 0, 0, 0])
        body = ROOT / "src/overlays/spanish_model_variant/variant427_curtains.c"
        shared = ROOT / "src/overlays/model_variant/model_variant.h"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes() +
                                    shared.read_bytes()).hexdigest()
        for module in self.modules:
            terminal = [r for r in rows if r["module"] == module["name"]][-1]
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant427_curtains" in s["source"]]
            self.assertEqual((terminal["result"], terminal["instruction_bytes"], terminal["different_words"]),
                             ("matched", "1296", "0"))
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
                self.skipTest("Build MODEL427 overlays before checking owners")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            script = (directory / f"{module['name']}.ld").read_text()
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant427_curtains" in s["source"]]
            selected_path = directory / "build" / selected["object"]
            c_paths = {directory / "build" / s["object"] for s in c_segments(ROOT, ROOT / module["layout"])}
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
                own, = original.get_symbol_by_name(f"func_{base+0x35BC:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, 1296, "STT_FUNC"))
                for start, end in zip(self.boundaries, self.boundaries[1:]):
                    name = f"func_{base+start:X}"
                    symbol, = symbols.get_symbol_by_name(name)
                    owner, = owners[name]
                    self.assertEqual(owner[0] == selected_path, start == 0x35BC)
                    self.assertEqual(owner[0] in c_paths, start in (0x28A0, 0x35BC))
                    self.assertEqual(owner[3], end-start)
                    self.assertTrue(owner[2] & 4)
                    self.assertIn(f"{owner[0].relative_to(ROOT)}({owner[1]});", script)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (base+start, end-start, "STT_FUNC"))
                    section = linked.get_section(symbol["st_shndx"])
                    self.assertTrue(section["sh_flags"] & 4)
                    offset = symbol["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[offset:offset+end-start], image[start:end])
                for start, size in ((0, 4), (0x3ACC, 0x1534)):
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
                    self.assertLess(site, 1296)
                    symbol = original.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", image, 0x35BC+site)
                    address = ((base+0x35BC+site+4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        resolved, = symbols.get_symbol_by_name(symbol.name)
                        self.assertEqual((word >> 26, address), (3, resolved["st_value"]+addend))
                        callees.append(address)
                    else:
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        self.assertEqual((word >> 26, address),
                                         (2, base+0x35BC+symbol["st_value"]+addend))
                        jumps.append((site, address-base-0x35BC))
                self.assertEqual(len(callees), 8)
                self.assertEqual(set(callees), {0x8005C018, 0x800842A8, 0x80085558, 0x80086258,
                                               0x80086628, 0x800875F8, 0x80087958, 0x80087CB8})
                self.assertEqual(jumps, [(0xBC, 0x130), (0x2C4, 0x2E4)])

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
            (ROOT / "config/sles_03951/overlays/model_variant427_linker_symbols.txt").read_text(), re.M))
        self.assertEqual(len(bindings), 34)
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
                self.assertTrue(context + 0x37C8 <= bank or bank + size <= context)


if __name__ == "__main__":
    unittest.main()
