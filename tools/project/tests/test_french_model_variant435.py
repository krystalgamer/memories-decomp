import csv
import hashlib
import json
from pathlib import Path
import re
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_french_overlay_inventories
from verify_inputs import load_checksum_manifest


SPANS = ((4, 0x1084), (0x1084, 0x1AD4), (0x1AD4, 0x1E7C),
         (0x1E7C, 0x2258), (0x2258, 0x28A4), (0x28A4, 0x2FB0),
         (0x2FB0, 0x32B8), (0x32B8, 0x3634), (0x3634, 0x3998))
HELPERS = ((0x1084, 2640, "spiral", "func_8013C088"),
           (0x1AD4, 936, "sheet", "func_8013CAA4"),
           (0x1E7C, 988, "webs", "func_8013CE50"),
           (0x2258, 1612, "ribbons", "func_8013D238"),
           (0x28A4, 1804, "bands", "func_8013D86C"),
           (0x2FB0, 776, "spokes", "func_8013DF78"),
           (0x32B8, 892, "rings", "func_8013E284"),
           (0x3634, 868, "quad", "func_8013E604"))


class FrenchModelVariant435Tests(unittest.TestCase):
    region = "france"
    module_prefix = "french"
    config_name = "sles_03948"
    load_inventories = staticmethod(load_french_overlay_inventories)
    family = 435
    slot_header_delta = 150
    source_family = 418
    source_directories = {}
    standalone_helpers = frozenset()
    module_count = 26
    distinct_images = 23
    binding_count = 37
    tail_start = 0x3998
    spans = SPANS
    helpers = HELPERS
    reachable_helpers = {0x1084, 0x1AD4, 0x1E7C}
    local_call_targets = {0x1084, 0x1AD4, 0x1E7C}
    models_by_stage = ((7, (34, 71, 124, 182, 279, 361, 491, 580, 640)),
                       (9, (166, 275, 469, 590)))
    entry_anchors = {0x0C: 0x00809021, 0x14: 0x0240B021, 0x28: 0x26D81374,
                     0x30: 0x26D816D4, 0x38: 0x26D81914, 0x5D0: 0x27180090,
                     0xAA0: 0x27180090, 0xCA8: 0x27180090, 0xAE0: 0x2B020006, 0xCE8: 0xAEC01B94,
                     0x20: 0x26D812DC, 0x24: 0xAFB80084, 0x5DC: 0x0000F021,
                     0x780: 0x27DE0001, 0x78C: 0x8FB80084, 0x794: 0x27180098,
                     0x798: 0xAFB80084, 0x7AC: 0x1BC0FF90,
                     0x18: 0x26D810F0, 0x1C: 0xAFB80080, 0x3F8: 0x00002821,
                     0x3FC: 0x8FB80080, 0x42C: 0xA0620144, 0x468: 0xA0620168,
                     0x494: 0x2A620009, 0x4A0: 0x24A50001, 0x4B0: 0x8FB80080,
                     0x4B8: 0x271801EC, 0x4BC: 0x18A0FFD2, 0x4C0: 0xAFB80080,
                     0x40: 0x26D80720, 0x84: 0xAFB60094,
                     0x7B4: 0x8FB80094, 0x7B8: 0x0000F021, 0x7C0: 0x2715025C,
                     0x8B0: 0x26500120, 0x8EC: 0x26520008, 0x908: 0x2A620006,
                     0x92C: 0x27180030, 0x958: 0x2B020006, 0x9A8: 0x27180260,
                     0x9DC: 0x2BC20003, 0x9E4: 0x26B50260, 0xF18: 0x02402021,
                     0x1E84: 0x00809821, 0x1EC8: 0x26711AE0, 0x1F08: 0x26720254,
                     0x2124: 0x28420006, 0x2140: 0x28420006, 0x2200: 0x26520260,
                     0x221C: 0x28420003, 0x2224: 0x26730260}

    entry_anchors.update({
        0xC: 0x00809021, 0x14: 0x0240B021, 0x18: 0x26D810F0,
        0x20: 0x26D812DC, 0x2258: 0x27BDFED8, 0x2260: 0x0080F021,
        0x22BC: 0x27D30D50, 0x22D4: 0x27C812DC, 0x22DC: 0x87C31BB0,
        0x22E8: 0x27D619A4, 0x2300: 0x8FC21B58, 0x2304: 0x97C41B98,
        0x2350: 0x001280C0, 0x2364: 0xA6020000, 0x23AC: 0xA6020020,
        0x23BC: 0xA6220002, 0x23CC: 0x00021400, 0x23D4: 0x28420002,
        0x23DC: 0x00006012, 0x23E8: 0x1440FFD0, 0x23EC: 0xA6230004,
        0x23F4: 0x26730074, 0x2408: 0x00021A40, 0x2410: 0x28420008,
        0x24C8: 0x8D020088, 0x24D4: 0x8D020088, 0x24E4: 0x8D020088,
        0x24E8: 0x27D70D60, 0x2564: 0x26690008, 0x256C: 0x266A0004,
        0x2570: 0xAFAA00F8, 0x258C: 0x8FA500F4, 0x2590: 0x26620014,
        0x25B4: 0x0C021E56, 0x25C4: 0x86E30000, 0x25C8: 0x86E20002,
        0x25CC: 0x85650010, 0x25D0: 0x85640012, 0x2678: 0x0C021E1A,
        0x2684: 0x0C02264A, 0x2694: 0xAE020018, 0x269C: 0x86030010,
        0x26AC: 0xAE020038, 0x26F8: 0x28420002, 0x270C: 0x26F70074,
        0x2808: 0x0C01356E, 0x2848: 0x8FC21B74, 0x284C: 0x87C31B88,
        0x2850: 0x94420020, 0x2860: 0x8FC21B64, 0x2864: 0x8FC31B98,
        0x2868: 0x00021140, 0x2870: 0xAFC31B98, 0x27F4: 0x04400007,
        0x27EC: 0x8CC20064, 0xB0: 0x00181900, 0xB4: 0x00781821,
        0xB8: 0x00031880, 0xC0: 0xAEC31B74,
    })

    def setUp(self):
        self.config = ROOT / "config" / self.config_name
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m["linker_symbols"].endswith(f"/model_variant{self.family}_linker_symbols.txt")]
        with (ROOT / f"notes/overlays/{self.module_prefix}-model-variant{self.family}-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}

    def test_loader_slices_and_independent_hashes(self):
        self.assertEqual(len(self.modules), self.module_count)
        self.assertEqual(len({m["sha256"] for m in self.modules}), self.distinct_images)
        checksums = load_checksum_manifest(self.config / "files.sha256")
        expected = {(model, stage + slot, slot)
                    for stage, models in self.models_by_stage
                    for model in models for slot in (0, 1)}
        observed = set()
        for module in self.modules:
            row = self.instances[module["name"]]
            model, stage, slot = (int(row[key]) for key in ("model", "stage", "slot"))
            self.assertTrue(0 <= model < 722)
            self.assertFalse(300 <= model < 350 or 650 <= model < 700 or model == 720)
            record = model
            if record >= 721:
                record -= 1
            if record >= 700:
                record -= 50
            if record >= 350:
                record -= 50
            observed.add((model, stage, slot))
            self.assertEqual(module["name"], f"{self.module_prefix}_model_variant_{model}_stage{stage}_slot{slot}")
            self.assertEqual(int(row["record"]), record)
            self.assertEqual(module["archive"], f"game/{self.region}/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sector_offset"], record * 276 + 180 + (stage - 7) * 10)
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(int(module["load_address"], 0), 0x8013B000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)
        self.assertEqual(observed, expected)

    def test_selected_sources_and_assembly_inventory(self):
        counts = self.load_inventories(ROOT)
        for module in self.modules:
            layout = ROOT / module["layout"]
            base = int(module["load_address"], 0)
            slot = int(module["name"][-1])
            expected = [{"address": f"0x{base + offset:X}", "size": f"0x{size:X}",
                         "profile": "gcc_2_8_1_g0_split",
                         "source": f"src/overlays/{self.source_directories.get(label, 'french_model_variant')}/variant{self.family}_{label}" +
                         ("_slot1" if slot else "") + ".c"}
                        for offset, size, label, _ in self.helpers]
            actual = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(actual["functions"], expected)
            self.assertEqual([s["source"] for s in c_segments(ROOT, layout)], [s["source"] for s in expected])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0) - base, int(r["size"], 0)) for r in rows],
                             [(start, end - start) for start, end in self.spans])
            helper_offsets = {offset for offset, _, _, _ in self.helpers}
            self.assertEqual([r["status"] for r in rows],
                             ["matching_c" if start in helper_offsets else "unmatched_asm"
                              for start, _ in self.spans])
            for row in rows:
                offset = int(row["address"], 0) - base
                if offset in helper_offsets:
                    self.assertIn("direct-entry reachable" if offset in self.reachable_helpers else
                                  "no direct entry-call path", row["notes"])
            self.assertEqual(counts[layout.stem]["matching_c_bytes"], sum(size for _, size, _, _ in self.helpers))
            self.assertEqual(counts[layout.stem]["function_count"], len(self.spans))
            for start, _ in self.spans:
                if start not in helper_offsets:
                    self.assertIn(f"[0x{start:X}, asm,", layout.read_text())

    def test_wrappers_only_rename_verified_functions(self):
        for slot in (0, 1):
            for offset, _, label, original in self.helpers:
                directory = ROOT / "src/overlays" / self.source_directories.get(label, "french_model_variant")
                name = f"variant{self.family}_{label}" + ("_slot1" if slot else "") + ".c"
                if label in self.standalone_helpers:
                    if slot == 0:
                        self.assertIn(f"void {original}(u8 *ctx)", (directory / name).read_text())
                        continue
                    included = f"variant{self.family}_{label}.c"
                else:
                    included = f"../model_variant/variant{self.source_family}_{label}.c"
                regional = self.region == "france" and (self.family, label) in (
                    (414, "bands"), (415, "bands"), (421, "bands"), (435, "ribbons"), (435, "spiral"),
                    (439, "bands"), (442, "ribbons"))
                expected = ('#include "../../types.h"\n' +
                            ("#define VERSION_FRENCH\n" if regional else "") +
                            f"#define {original} func_{0x8013B000 + slot * 0x40000 + offset:X}\n"
                            f'#include "{included}"\n')
                self.assertEqual((directory / name).read_text(), expected)
        with (ROOT / f"notes/overlays/{self.module_prefix}-model-variant{self.family}-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        terminal = [row for row in rows if row["result"] == "matched"]
        if self.region == "france" and self.family in (414, 415, 421, 435, 439, 442, 465):
            experiments = [row for row in rows if row["result"] != "matched"]
            expected = {
                414: [("1896", "4"), ("1896", "4"), ("1896", "0"), ("1896", "0")],
                415: [("1920", "4"), ("1920", "4"), ("1920", "0"), ("1920", "0")],
                421: [("1964", "4"), ("1964", "4"), ("1964", "0"), ("1964", "0")],
                435: [("1600", "218"), ("1644", "399"), ("1600", "218"),
                      ("1592", "258"), ("1604", "215"), ("1604", "215"), ("1612", "0"),
                      ("2608", "477"), ("2608", "477"), ("2640", "0"), ("2640", "0")],
                439: [("1912", "4"), ("1912", "4"), ("1912", "0"), ("1912", "0")],
                442: [("1600", "218"), ("1604", "215"), ("1612", "0"), ("1612", "0")],
                465: [("1044", "142"), ("1044", "142")],
            }[self.family]
            self.assertEqual(len(rows), len(self.helpers) * 2 + len(expected))
            self.assertEqual([(row["instruction_bytes"], row["different_words"]) for row in experiments], expected)
            for row in experiments:
                self.assertEqual(row["result"], "text_exact" if row["different_words"] == "0" else "mismatch")
        else:
            self.assertEqual(len(rows), len(terminal))
        self.assertEqual(len(terminal), len(self.helpers) * 2)
        for row in terminal:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            label = next(label for start, _, label, _ in self.helpers if start == offset)
            directory = ROOT / "src/overlays" / self.source_directories.get(label, "french_model_variant")
            source = directory / (f"variant{self.family}_{label}" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["result"], row["different_words"], row["profile"]),
                             ("matched", "0", "gcc_2_8_1_g0_split"))

    def test_raw_storage_has_real_extents(self):
        for module in self.modules:
            layout = ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (self.tail_start, 0x5000 - self.tail_start)):
                self.assertIn(f"D_{base + offset:X} = 0x{base + offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0x{self.tail_start:X}, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())
            bindings = (ROOT / module["linker_symbols"]).read_text()
            self.assertEqual(len(re.findall(r"^\w+ =", bindings, re.M)), self.binding_count)
            self.assertNotRegex(bindings, r"=\s*0x801[37]")
            if self.family == 435:
                for name, address in (("RotTransPers3", 0x80087898),
                                      ("rcos", 0x800866F8), ("ratan2", 0x80089928)):
                    self.assertIn(f"{name} = 0x{address:X};", bindings)
                    self.assertIn(f"{name} = 0x{address:X}; // type:func absolute:true", symbols)
                    self.assertNotIn(f"func_french_{address:X}", symbols)

    def test_legal_images_layout_anchors_and_reachability(self):
        path = ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<I", data)[0],
                                 self.family + int(row["slot"]) * self.slot_header_delta)
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             4 * ((int(row["stage"]) - 7) // 2))
                command = struct.unpack("<i", archive.read(4))[0]
                self.assertEqual(command, int(row["command_word"]))
                self.assertGreaterEqual(command, 0)
                for offset, word in self.entry_anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                local_calls = set()
                for start, end in self.spans:
                    pending, visited, returns = [start], set(), set()
                    while pending:
                        pc = pending.pop()
                        self.assertTrue(start <= pc < end and pc % 4 == 0)
                        if pc in visited:
                            continue
                        visited.add(pc)
                        word = struct.unpack_from("<I", data, pc)[0]
                        op, rs, rt = word >> 26, word >> 21 & 31, word >> 16 & 31
                        if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
                            self.assertLess(pc + 4, end)
                            visited.add(pc + 4)
                            delay = struct.unpack_from("<I", data, pc + 4)[0]
                            self.assertNotIn(delay >> 26, (1, 2, 3, 4, 5, 6, 7))
                            self.assertFalse(delay >> 26 == 0 and delay & 63 in (8, 9))
                            if word == 0x03E00008:
                                returns.add(pc)
                            elif op in (2, 3):
                                target = ((base + pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                                if op == 2:
                                    pending.append(target - base)
                                else:
                                    pending.append(pc + 8)
                                    if base <= target < base + len(data):
                                        local_calls.add(target - base)
                            else:
                                if op == 1:
                                    self.assertIn(rt, (0, 1))
                                imm = (word & 0xFFFF) - (0x10000 if word & 0x8000 else 0)
                                if not (op == 5 and rs == rt):
                                    pending.append(pc + 4 + imm * 4)
                                if not (op == 4 and rs == rt):
                                    pending.append(pc + 8)
                        else:
                            self.assertFalse(op == 0 and word & 63 in (8, 9))
                            self.assertFalse(op in (16, 17, 18) and rs == 8)
                            pending.append(pc + 4)
                    self.assertEqual(visited, set(range(start, end, 4)))
                    self.assertEqual(returns, {end - 8})
                self.assertEqual(local_calls, self.local_call_targets)


class FrenchModelRibbonDescriptorTests(unittest.TestCase):
    def test_retained_ribbon_descriptor_comparisons_and_bindings(self):
        path = ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        manifest = json.loads((ROOT / "config/sles_03948/overlays.json").read_text())
        with path.open("rb") as archive:
            for family, count, start, table, stride, comparison, ribbon, record, band, timing, packet in (
                (435, 26, 0x2258, 0x3A94, 68, 0x20, 0xD50, 116, 0x10F0, 0x12DC, 0x19A4),
                (442, 4, 0x2CE8, 0x452C, 56, 0x18, 0x1940, 108, 0x1CA0, 0x1E68, 0x2530),
            ):
                modules = [row for row in manifest["modules"]
                           if row["linker_symbols"].endswith(f"/model_variant{family}_linker_symbols.txt")]
                self.assertEqual(len(modules), count)
                with (ROOT / f"notes/overlays/french-model-variant{family}-instances.csv").open() as handle:
                    instances = {row["module"]: row for row in csv.DictReader(handle)}
                self.assertEqual(ribbon + 8 * record, band)
                self.assertLessEqual(timing + 152, packet)
                commands = set()
                for module in modules:
                    row = instances[module["name"]]
                    archive.seek(module["sector_offset"] * 2048)
                    data = archive.read(20480)
                    archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                                 (int(row["stage"]) - 7) // 2 * 4)
                    command, = struct.unpack("<i", archive.read(4))
                    self.assertEqual(command, int(row["command_word"]))
                    commands.add(command)
                    descriptor = table + command % 1000 * stride
                    self.assertLessEqual(descriptor + stride, len(data))
                    self.assertEqual(struct.unpack_from("<H", data, descriptor + comparison)[0], 1)
                    self.assertEqual(struct.unpack_from("<I", data, start + 0x5F8)[0],
                                     0x94420000 | comparison)
                    self.assertEqual(struct.unpack_from("<I", data, start + 0x59C)[0],
                                     0x04400007 if family == 435 else 0x18400007)
                    bindings = (ROOT / module["linker_symbols"]).read_text()
                    self.assertIn("RotTransPers = 0x80087868;", bindings)
                    self.assertNotIn("func_french_80087868", bindings)
                self.assertEqual(commands, {601000, 601001, 601002, 601003, 601005,
                                            601006, 601007, 601009, 601010, 601011}
                                 if family == 435 else {608000})


class FrenchModelVariant435WebLayoutTests(unittest.TestCase):
    def test_selected_web_descriptors_and_context_separation(self):
        path = ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        manifest = json.loads((ROOT / "config/sles_03948/overlays.json").read_text())
        modules = [row for row in manifest["modules"]
                   if row["linker_symbols"].endswith("/model_variant435_linker_symbols.txt")]
        with (ROOT / "notes/overlays/french-model-variant435-instances.csv").open() as handle:
            instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.assertEqual(len(modules), 26)
        with path.open("rb") as archive:
            for module in modules:
                instance = instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(instance["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x3A94
                self.assertEqual(struct.unpack_from("<I", data, 0xA0)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xA4)[0],
                                 0x24420000 | (table & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xF14)[0],
                                 0x0C000000 | (((base + 0x1E7C) >> 2) & 0x3FFFFFF))
                archive.seek((int(instance["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(instance["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(instance["command_word"]))
                descriptor = 0x3A94 + command % 1000 * 68
                self.assertGreaterEqual(descriptor, 0x3998)
                self.assertLessEqual(descriptor + 68, len(data))
                self.assertEqual(6 * 6 * 8, 0x120)
                self.assertEqual(3 * 0x260, 0x720)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0x1BB4 <= start or start + size <= context)


class FrenchModelSpiralDescriptorTests(unittest.TestCase):
    def test_entry_called_spiral_layout_and_timings(self):
        path = ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        manifest = json.loads((ROOT / "config/sles_03948/overlays.json").read_text())
        modules = [m for m in manifest["modules"]
                   if m["linker_symbols"].endswith("/model_variant435_linker_symbols.txt")]
        with (ROOT / "notes/overlays/french-model-variant435-instances.csv").open() as handle:
            instances = {row["module"]: row for row in csv.DictReader(handle)}
        anchors = {
            0x48: 0x26D719E4, 0x5C: 0xAFB80098, 0xAEC: 0x00002821,
            0xAF0: 0x24060001, 0xAF4: 0x00009821, 0xAF8: 0x8FA40098,
            0xB14: 0xA0980058, 0xB30: 0xA0980059, 0xB4C: 0xA098005A,
            0xB68: 0xA0980050, 0xB84: 0xA0980051, 0xBA0: 0x2A620002,
            0xBA8: 0xA0980052, 0xBB0: 0x24840004, 0xBB4: 0x24A50001,
            0xBB8: 0x8FB80098, 0xBBC: 0x28A2000C, 0xBC0: 0x27180084,
            0xBC8: 0xAFB80098, 0xF20: 0x02402021, 0x1084: 0x27BDFEC0,
            0x108C: 0x0080F021, 0x10C0: 0x27C90734, 0x10D0: 0x8FC41B48,
            0x10D4: 0x8FC51B40, 0x10D8: 0x0C02264A, 0x10E0: 0x24420C00,
            0x10E8: 0x8FCA1B2C, 0x10F4: 0x8FCB1B30, 0x10F8: 0x27D50720,
            0x1100: 0x8FC81B34, 0x1104: 0x27D319E4, 0x110C: 0x87C31B72,
            0x1110: 0x24020400, 0x112C: 0x00031300, 0x1138: 0x00031B40,
            0x1148: 0x87C51B6C, 0x1178: 0x8FC21B58, 0x1180: 0x30420001,
            0x1190: 0x000212C3, 0x119C: 0x24420010, 0x11B4: 0x2442000C,
            0x11CC: 0x0002A282, 0x11E8: 0xA520FFFC, 0x11EC: 0xA520FFFE,
            0x11F4: 0xA5200000, 0x1210: 0x00021240, 0x1250: 0xA6020010,
            0x1304: 0xA6020030, 0x132C: 0x28420002, 0x1348: 0x26B50084,
            0x1368: 0x2842000C, 0x1388: 0x97C31B70, 0x13A8: 0x000210C3,
            0x13D0: 0x8FC21B08, 0x13D4: 0x27AB00D0, 0x13E0: 0x8FC21B0C,
            0x13E4: 0x27D70740, 0x13F0: 0x8FC31B10, 0x147C: 0x26A80010,
            0x1480: 0x26A90018, 0x1484: 0x26B20004, 0x1488: 0x26AA0002,
            0x14A0: 0x24020001, 0x14A4: 0x16820036, 0x14B8: 0x26A20024,
            0x14C4: 0x26A20068, 0x14DC: 0x0C021E56, 0x14E4: 0x26A40038,
            0x14E8: 0x26A50044, 0x14F0: 0x27A700D4, 0x14F4: 0x0C021E1A,
            0x14F8: 0xAE42006C, 0x151C: 0xAE420028, 0x1534: 0xAE420048,
            0x1550: 0xA5220074, 0x157C: 0xA5220078, 0x1598: 0x00148080,
            0x15B4: 0x26020064, 0x15CC: 0x0C021E56, 0x15F0: 0x0C021E1A,
            0x15F4: 0xAE02006C, 0x1624: 0xAE020028, 0x163C: 0xAE020048,
            0x165C: 0xA6220074, 0x1680: 0xA6220078, 0x1694: 0x28420002,
            0x16BC: 0x2842000C, 0x16C4: 0x26B50084, 0x16EC: 0x96020020,
            0x16F0: 0x96830074, 0x16FC: 0xA6620008, 0x1734: 0xA6620014,
            0x1754: 0xA6620020, 0x176C: 0xA662002C, 0x177C: 0x92020058,
            0x17C4: 0x92020050, 0x180C: 0xAE00006C, 0x1810: 0x8E02006C,
            0x1818: 0x04400005, 0x181C: 0xAE000064, 0x1820: 0x9606006C,
            0x1828: 0x0C0210AA, 0x1840: 0xA6620008, 0x1868: 0xA6620014,
            0x1888: 0xA6620020, 0x18A0: 0xA662002C, 0x1940: 0xAE00006C,
            0x1944: 0x8E02006C, 0x194C: 0x04400005, 0x1950: 0xAE000064,
            0x1954: 0x9606006C, 0x195C: 0x0C0210AA, 0x1970: 0x1840FF58,
            0x1990: 0x2842000C, 0x1998: 0x26B50084, 0x199C: 0x97C21B6C,
            0x19A0: 0x8FC51B9C, 0x19A4: 0x24420010, 0x19B8: 0x87C21B70,
            0x19C0: 0x28421000, 0x19CC: 0x8FC31B74, 0x19D0: 0x8FC21B5C,
            0x19D4: 0x8C64002C, 0x19D8: 0x8C630030, 0x19E8: 0x0043001B,
            0x1A18: 0xA7C21B70, 0x1A20: 0x24020002, 0x1A2C: 0x8CC40034,
            0x1A34: 0x0083102B, 0x1A48: 0x87C21B72, 0x1A50: 0x28420400,
            0x1A5C: 0x8CC30038, 0x1A68: 0x0043001B, 0x1A98: 0xA7C21B72,
            0x1A9C: 0x24020003, 0x1AA0: 0xAFC21B9C,
        }
        timings = {
            601000: (120, 136, 280, 320),
            601001: (110, 122, 320, 380),
            601002: (72, 84, 150, 180),
            601003: (100, 112, 280, 320),
            601005: (40, 52, 220, 250),
            601006: (80, 92, 240, 260),
            601007: (60, 72, 100, 160),
            601009: (40, 48, 60, 160),
            601010: (140, 148, 360, 400),
            601011: (100, 108, 240, 260),
        }
        self.assertEqual(len(modules), 26)
        self.assertEqual(0x720 + 12 * 132, 0xD50)
        requests = set()
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        with path.open("rb") as archive:
            for module in modules:
                row = instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(struct.unpack_from("<I", data, 0xF1C)[0],
                                 0x0C000000 | ((base + 0x1084) >> 2 & 0x3FFFFFF))
                table = base + 0x3A94
                self.assertEqual(struct.unpack_from("<I", data, 0xA0)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xA4)[0],
                                 0x24420000 | (table & 0xFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 +
                             0x110 + (int(row["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                requests.add(command)
                descriptor = 0x3A94 + command % 1000 * 68
                self.assertGreaterEqual(descriptor, 0x3998)
                self.assertLessEqual(descriptor + 68, len(data))
                actual = struct.unpack_from("<4i", data, descriptor + 0x2C)
                self.assertEqual(actual, timings[command])
                self.assertGreater(actual[1], actual[0])
                self.assertGreater(actual[3], actual[2])
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1056I", data[4:0x1084])
                            if word >> 26 in widths and word >> 21 & 31 == 22 and not word & 0x8000]
                self.assertEqual(max(accesses), 0x1BB4)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 4096), (base, 20480)):
                    self.assertTrue(context + 0x1BB4 <= start or start + size <= context)
        self.assertEqual(requests, set(timings))
