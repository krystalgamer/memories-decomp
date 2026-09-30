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
HELPERS = ((0x2FB0, 776, "spokes", "func_8013DF78"),
           (0x32B8, 892, "rings", "func_8013E284"),
           (0x3634, 868, "quad", "func_8013E604"))


class FrenchModelVariant435Tests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03948"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m["linker_symbols"].endswith("/model_variant435_linker_symbols.txt")]
        with (ROOT / "notes/overlays/french-model-variant435-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}

    def test_loader_slices_and_independent_hashes(self):
        self.assertEqual(len(self.modules), 26)
        self.assertEqual(len({m["sha256"] for m in self.modules}), 23)
        checksums = load_checksum_manifest(self.config / "files.sha256")
        expected = {(model, stage + slot, slot)
                    for stage, models in ((7, (34, 71, 124, 182, 279, 361, 491, 580, 640)),
                                          (9, (166, 275, 469, 590)))
                    for model in models for slot in (0, 1)}
        observed = set()
        for module in self.modules:
            row = self.instances[module["name"]]
            model, stage, slot = (int(row[key]) for key in ("model", "stage", "slot"))
            record = model - (50 if model >= 350 else 0)
            observed.add((model, stage, slot))
            self.assertEqual(module["name"], f"french_model_variant_{model}_stage{stage}_slot{slot}")
            self.assertEqual(int(row["record"]), record)
            self.assertEqual(module["archive"], "game/france/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sector_offset"], record * 276 + 180 + (stage - 7) * 10)
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(int(module["load_address"], 0), 0x8013B000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)
        self.assertEqual(observed, expected)

    def test_selected_sources_and_assembly_inventory(self):
        counts = load_french_overlay_inventories(ROOT)
        for module in self.modules:
            layout = ROOT / module["layout"]
            base = int(module["load_address"], 0)
            slot = int(module["name"][-1])
            expected = [{"address": f"0x{base + offset:X}", "size": f"0x{size:X}",
                         "profile": "gcc_2_8_1_g0_split",
                         "source": f"src/overlays/french_model_variant/variant435_{label}" +
                         ("_slot1" if slot else "") + ".c"}
                        for offset, size, label, _ in HELPERS]
            actual = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(actual["functions"], expected)
            self.assertEqual([s["source"] for s in c_segments(ROOT, layout)], [s["source"] for s in expected])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0) - base, int(r["size"], 0)) for r in rows],
                             [(start, end - start) for start, end in SPANS])
            self.assertEqual([r["status"] for r in rows], ["unmatched_asm"] * 6 + ["matching_c"] * 3)
            self.assertTrue(all("no direct entry-call path" in row["notes"] for row in rows[6:]))
            self.assertEqual(counts[layout.stem]["matching_c_bytes"], 2536)
            self.assertEqual(counts[layout.stem]["function_count"], 9)
            for start, _ in SPANS[:6]:
                self.assertIn(f"[0x{start:X}, asm,", layout.read_text())

    def test_wrappers_only_rename_verified_functions(self):
        directory = ROOT / "src/overlays/french_model_variant"
        for slot in (0, 1):
            for offset, _, label, original in HELPERS:
                name = f"variant435_{label}" + ("_slot1" if slot else "") + ".c"
                expected = ('#include "../../types.h"\n'
                            f"#define {original} func_{0x8013B000 + slot * 0x40000 + offset:X}\n"
                            f'#include "../model_variant/variant418_{label}.c"\n')
                self.assertEqual((directory / name).read_text(), expected)
        with (ROOT / "notes/overlays/french-model-variant435-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 6)
        for row in rows:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            label = next(label for start, _, label, _ in HELPERS if start == offset)
            source = directory / (f"variant435_{label}" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["result"], row["different_words"], row["profile"]),
                             ("matched", "0", "gcc_2_8_1_g0_split"))

    def test_raw_storage_has_real_extents(self):
        for module in self.modules:
            layout = ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x3998, 5736)):
                self.assertIn(f"D_{base + offset:X} = 0x{base + offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0x3998, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())
            bindings = (ROOT / module["linker_symbols"]).read_text()
            self.assertEqual(len(re.findall(r"^\w+ =", bindings, re.M)), 37)
            self.assertNotRegex(bindings, r"=\s*0x801[37]")

    def test_legal_images_layout_anchors_and_reachability(self):
        path = ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        anchors = {0x0C: 0x00809021, 0x14: 0x0240B021, 0x28: 0x26D81374,
                   0x30: 0x26D816D4, 0x38: 0x26D81914, 0x5D0: 0x27180090,
                   0xAA0: 0x27180090, 0xCA8: 0x27180090, 0xAE0: 0x2B020006, 0xCE8: 0xAEC01B94}
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<I", data)[0], 435 + int(row["slot"]) * 150)
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             4 * ((int(row["stage"]) - 7) // 2))
                command = struct.unpack("<i", archive.read(4))[0]
                self.assertEqual(command, int(row["command_word"]))
                self.assertGreaterEqual(command, 0)
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                local_calls = set()
                for start, end in SPANS:
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
                self.assertEqual(local_calls, {0x1084, 0x1AD4, 0x1E7C})
