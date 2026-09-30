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
HELPERS = ((0x1AD4, 936, "sheet", "func_8013CAA4"),
           (0x1E7C, 988, "webs", "func_8013CE50"),
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
    module_count = 26
    distinct_images = 23
    binding_count = 37
    tail_start = 0x3998
    spans = SPANS
    helpers = HELPERS
    reachable_helpers = {0x1AD4, 0x1E7C}
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
                         "source": f"src/overlays/french_model_variant/variant{self.family}_{label}" +
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
        directory = ROOT / "src/overlays/french_model_variant"
        for slot in (0, 1):
            for offset, _, label, original in self.helpers:
                name = f"variant{self.family}_{label}" + ("_slot1" if slot else "") + ".c"
                expected = ('#include "../../types.h"\n'
                            f"#define {original} func_{0x8013B000 + slot * 0x40000 + offset:X}\n"
                            f'#include "../model_variant/variant{self.source_family}_{label}.c"\n')
                self.assertEqual((directory / name).read_text(), expected)
        with (ROOT / f"notes/overlays/{self.module_prefix}-model-variant{self.family}-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), len(self.helpers) * 2)
        for row in rows:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            label = next(label for start, _, label, _ in self.helpers if start == offset)
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
