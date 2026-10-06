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
from tools.project.tests import test_french_model_variant338 as reference


class SpanishModelVariant338Tests(unittest.TestCase):
    family = 338
    module_count = 16
    tail_start = 0x270C
    models_by_stage = ((7, (164, 165, 210, 424, 609)), (9, (34, 443, 459)))
    helpers = ((0x4, 2972, "entry"), (0xBA0, 2872, "ribbon"),
               (0x16D8, 1208, "rings"), (0x1B90, 836, "strand"),
               (0x1ED4, 2104, "streamers"))
    reachable_helpers = {0xBA0, 0x16D8, 0x1ED4}
    entry_calls = {0xBA0, 0x16D8, 0x1ED4}
    entry_anchors = reference.FrenchModelVariant338Tests.entry_anchors
    spans = ((4, 0xBA0), (0xBA0, 0x16D8), (0x16D8, 0x1B90),
             (0x1B90, 0x1ED4), (0x1ED4, 0x270C))

    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m.get("linker_symbols", "").endswith(f"/model_variant{self.family}_linker_symbols.txt")]
        with (ROOT / f"notes/overlays/spanish-model-variant{self.family}-instances.csv").open() as handle:
            self.instances = {r["module"]: r for r in csv.DictReader(handle)}

    def test_independent_loader_slices_and_stage_selection(self):
        self.assertEqual(len(self.modules), self.module_count)
        self.assertEqual(len({m["sha256"] for m in self.modules}), self.module_count)
        checksums = load_checksum_manifest(self.config / "files.sha256")
        observed = set()
        for module in self.modules:
            row = self.instances[module["name"]]
            model, record, stage, slot = (int(row[k]) for k in ("model", "record", "stage", "slot"))
            observed.add((model, stage, slot))
            self.assertEqual(record, model - (50 if model >= 350 else 0))
            self.assertEqual(module["sector_offset"], record * 276 + 180 + (stage - 7) * 10)
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(int(module["load_address"], 0), 0x8013B000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)
        expected = {(m, stage+s, s) for stage, models in self.models_by_stage for m in models for s in (0, 1)}
        self.assertEqual(observed, expected)

    def test_selected_c_and_honest_remaining_assembly(self):
        counts = load_spanish_overlay_inventories(ROOT)
        for module in self.modules:
            layout = ROOT / module["layout"]
            slot, base = int(self.instances[module["name"]]["slot"]), int(module["load_address"], 0)
            expected = [{"address": f"0x{base+offset:X}", "size": f"0x{size:X}",
                         "profile": "gcc_2_8_1_g0_split",
                         "source": f"src/overlays/french_model_variant/variant{self.family}_{label}" +
                         ("_slot1" if slot else "") + ".c"}
                        for offset, size, label in self.helpers]
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching["functions"], expected)
            self.assertEqual([r["source"] for r in c_segments(ROOT, layout)], [r["source"] for r in expected])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0)-base, int(r["size"], 0)) for r in rows],
                             [(start, end-start) for start, end in self.spans])
            helper_offsets = {offset for offset, _, _ in self.helpers}
            self.assertEqual([r["status"] for r in rows],
                             ["matching_c" if start in helper_offsets else "unmatched_asm"
                              for start, _ in self.spans])
            for row in rows:
                offset = int(row["address"], 0) - base
                if offset == 4 and offset in helper_offsets:
                    self.assertIn("loader entry", row["notes"])
                elif offset in helper_offsets:
                    self.assertIn("direct-entry reachable" if offset in self.reachable_helpers
                                  else "no direct entry-call path", row["notes"])
            self.assertEqual(counts[layout.stem]["function_count"], 5)
            self.assertEqual(counts[layout.stem]["matching_c_function_count"], len(self.helpers))
            self.assertEqual(counts[layout.stem]["matching_c_bytes"], sum(size for _, size, _ in self.helpers))

    def test_streamer_compiler_linked_calls_and_raw_owners_when_built(self):
        if self.family != 338:
            self.skipTest("This ownership check covers the MODEL338 streamer")
        for module in self.modules:
            if not (ROOT / f"tmp/overlays/{module['name']}/build/{module['name']}.elf").is_file():
                self.skipTest("Build Spanish MODEL338 images before checking C owners")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools is required for ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        with (self.config / "functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        for module in self.modules:
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            image = (ROOT / module["output"]).read_bytes()
            self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant338_streamers" in s["source"]]
            obj = directory / "build" / selected["object"]
            self.assertIn(f"{obj.relative_to(ROOT)}(.text);",
                          (directory / f"{module['name']}.ld").read_text())
            with (directory / f"build/{module['name']}.elf").open("rb") as linked_handle, obj.open("rb") as handle:
                linked, compiled = ELFFile(linked_handle), ELFFile(handle)
                final_symbols = linked.get_section_by_name(".symtab")
                object_symbols = compiled.get_section_by_name(".symtab")
                name = f"func_{base+0x1ED4:X}"
                own, = object_symbols.get_symbol_by_name(name)
                definition, = final_symbols.get_symbol_by_name(name)
                for symbol, elf, address in ((own, compiled, 0), (definition, linked, base + 0x1ED4)):
                    self.assertIsInstance(symbol["st_shndx"], int)
                    self.assertEqual((symbol["st_value"], symbol["st_size"],
                                      symbol["st_info"]["type"]), (address, 2104, "STT_FUNC"))
                    self.assertTrue(elf.get_section(symbol["st_shndx"])["sh_flags"] & 4)
                section = linked.get_section(definition["st_shndx"])
                start = definition["st_value"] - section["sh_addr"]
                self.assertEqual(section.data()[start:start + 2104], image[0x1ED4:0x270C])
                text = compiled.get_section(own["st_shndx"]).data()
                relocations = compiled.get_section_by_name(".rel.text")
                self.assertIsNotNone(relocations)
                calls = 0
                for relocation in relocations.iter_relocations():
                    if relocation["r_info_type"] != 4:
                        continue
                    target = object_symbols.get_symbol(relocation["r_info_sym"])
                    if target["st_shndx"] != "SHN_UNDEF":
                        continue
                    resolved, = final_symbols.get_symbol_by_name(target.name)
                    self.assertIn(resolved["st_value"], resident)
                    site = relocation["r_offset"]
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    word = struct.unpack_from("<I", image, 0x1ED4 + site)[0]
                    address = ((base + 0x1ED4 + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    self.assertEqual(address, resolved["st_value"] + addend, target.name)
                    calls += 1
                self.assertGreater(calls, 0)
                for offset, size in ((0, 4), (0x270C, 10484)):
                    raw, = final_symbols.get_symbol_by_name(f"D_{base+offset:X}")
                    self.assertIsInstance(raw["st_shndx"], int)
                    self.assertEqual(raw["st_value"], base + offset)
                    section = linked.get_section(raw["st_shndx"])
                    self.assertFalse(section["sh_flags"] & 4)
                    start = raw["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[start:start + size], image[offset:offset + size])

    def test_real_raw_extents_and_complete_resident_bindings(self):
        for module in self.modules:
            base = int(module["load_address"], 0)
            layout = ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (self.tail_start, 0x5000-self.tail_start)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0x{self.tail_start:X}, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())
            bindings = (ROOT / module["linker_symbols"]).read_text()
            self.assertEqual(len(re.findall(r"^\w+ =", bindings, re.M)), 35)
            self.assertNotRegex(bindings, r"=\s*0x801[37]")
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_terminal_records_identify_selected_wrappers(self):
        with (ROOT / f"notes/overlays/spanish-model-variant{self.family}-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.check_terminal_records(rows)

    def check_terminal_records(self, rows):
        self.assertEqual(len(rows), 2 * len(self.helpers))
        self.assertEqual({(r["function_offset"], r["slot"]) for r in rows},
                         {(f"0x{o:X}", s) for o, _, _ in self.helpers for s in ("0", "1")})
        for row in rows:
            size, label = next((size, label) for offset, size, label in self.helpers
                               if int(row["function_offset"], 0) == offset)
            suffix = "_slot1" if row["slot"] == "1" else ""
            source = ROOT / f"src/overlays/french_model_variant/variant{self.family}_{label}{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["result"], row["instruction_bytes"], row["different_words"]),
                             ("matched", str(size), "0"))
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")

    def test_legal_spanish_anchors_descriptors_and_all_function_boundaries(self):
        path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<I", data)[0], self.family + slot * 150)
                for offset, word in self.entry_anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                archive.seek((int(row["record"])*276+275)*2048+0x110+(int(row["stage"])-7)//2*4)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, int(row["command_word"]))
                self.assertGreaterEqual(request, 0)
                self.check_descriptor(data, base, request)
                entry_calls = set()
                for start, end in self.spans:
                    pending, visited, returns = [start], set(), set()
                    while pending:
                        pc = pending.pop()
                        self.assertTrue(start <= pc < end and pc % 4 == 0)
                        if pc in visited:
                            continue
                        visited.add(pc)
                        word = struct.unpack_from("<I", data, pc)[0]
                        op = word >> 26
                        if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
                            self.assertLess(pc+4, end)
                            visited.add(pc+4)
                            delay = struct.unpack_from("<I", data, pc+4)[0]
                            self.assertNotIn(delay >> 26, (1, 2, 3, 4, 5, 6, 7))
                            self.assertFalse(delay >> 26 == 0 and delay & 63 in (8, 9))
                            if word == 0x03E00008:
                                returns.add(pc)
                            elif op in (2, 3):
                                target = ((base+pc+4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                                if op == 2:
                                    pending.append(target-base)
                                else:
                                    pending.append(pc+8)
                                    if start == 4 and base <= target < base+len(data):
                                        entry_calls.add(target-base)
                            else:
                                imm = (word & 0xFFFF) - (0x10000 if word & 0x8000 else 0)
                                pending.extend((pc+8, pc+4+imm*4))
                        else:
                            self.assertFalse(op == 0 and word & 63 in (8, 9))
                            pending.append(pc+4)
                    self.assertEqual(visited, set(range(start, end, 4)))
                    self.assertEqual(returns, {end-8})
                self.assertEqual(entry_calls, self.entry_calls)
                for offset, _, _ in self.helpers:
                    self.assertEqual(offset in entry_calls, offset in self.reachable_helpers)

    def check_descriptor(self, data, base, request):
        config = base + 0x2808
        self.assertEqual(struct.unpack_from("<II", data, 0x78),
                         (0x3C030000 | ((config+0x8000) >> 16 & 0xFFFF),
                          0x24630000 | (config & 0xFFFF)))
        offset = 0x2808 + request % 1000 * 52
        self.assertTrue(self.tail_start <= offset and offset+52 <= len(data))
        start, end, _, fade_start, fade_end = struct.unpack_from("<5I", data, offset+32)
        self.assertLess(start, end)
        self.assertLess(fade_start, fade_end)


class SpanishModelVariant338RibbonTests(unittest.TestCase):
    family = 338
    setUp = SpanishModelVariant338Tests.setUp

    def test_ribbon_original_context_frame_and_indexed_projection(self):
        path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                base = int(module["load_address"], 0)
                for offset, word in reference.FrenchModelVariant338Tests.ribbon_anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                for offset, word in {
                    0x68: 0x04A001F8, 0xBA8: 0x0080F021, 0xBE0: 0xAFA200D8,
                    0xF54: 0x27A40028, 0xF58: 0x27B00040, 0xF78: 0x27B600D0,
                    0xFB0: 0x27A50030, 0xFB4: 0x27A40080, 0xFB8: 0x27B00060,
                    0x1088: 0x27A700D4, 0x14F0: 0x8FA500D8, 0x16D4: 0x27BD0140,
                }.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(struct.unpack_from("<I", data, 0x9F4)[0],
                                 0x0C000000 | ((base + 0xBA0) >> 2 & 0x3FFFFFF))
                for start, end, register, expected in (
                    (4, 0x70, 19, [0xC]), (0x84C, 0x9FC, 19, []),
                    (0x9DC, 0x9FC, 4, [0x9DC]),
                    (0xBA0, 0x16D8, 30, [0xBA8, 0x16AC]),
                    (0xBA0, 0x16D8, 29, [0xBA0, 0x16D4]),
                ):
                    writes = []
                    for offset in range(start, end, 4):
                        word, = struct.unpack_from("<I", data, offset)
                        op = word >> 26
                        destination = word >> 11 & 31 if op == 0 else word >> 16 & 31 if op in (
                            8, 9, 10, 11, 12, 13, 14, 15, 32, 33, 35, 36, 37) else None
                        if destination == register:
                            writes.append(offset)
                    self.assertEqual(writes, expected)
                bindings = (ROOT / module["linker_symbols"]).read_text()
                self.assertIn("RotTransPers = 0x80087868;", bindings)
                self.assertIn("ratan2 = 0x80089928;", bindings)

    def test_ribbon_record_and_packet_boundaries_from_spanish_instructions(self):
        path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                stride = struct.unpack_from("<I", data, 0x5D8)[0] & 0xFFFF
                count = struct.unpack_from("<I", data, 0x5D0)[0] & 0xFFFF
                rings = struct.unpack_from("<I", data, 0x1C)[0] & 0xFFFF
                self.assertEqual((stride, count, count * stride), (932, 5, rings))
                start = struct.unpack_from("<I", data, 0x30)[0] & 0xFFFF
                end = struct.unpack_from("<I", data, 0x38)[0] & 0xFFFF
                packet_size = struct.unpack_from("<I", data, 0x12AC)[0] & 0xFFFF
                self.assertEqual((start, packet_size, end - start), (0x1E14, 40, 80))
                fields = set()
                for offset in range(0x1280, 0x1550, 4):
                    word, = struct.unpack_from("<I", data, offset)
                    if word >> 26 in (40, 41, 43) and (word >> 21 & 31) == 16:
                        displacement = (word & 0xFFFF) - (0x10000 if word & 0x8000 else 0)
                        fields.add((6 + displacement, {40: 1, 41: 2, 43: 4}[word >> 26]))
                self.assertEqual(fields, {(4, 1), (5, 1), (6, 1), (8, 2), (10, 2),
                                          (16, 2), (18, 2), (24, 2), (26, 2), (32, 2), (34, 2)})
                self.assertEqual(max(offset + size for offset, size in fields), 36)
                self.assertLessEqual(36, packet_size)

if __name__ == "__main__":
    unittest.main()
