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
from progress import load_spanish_overlay_inventories
from verify_inputs import load_checksum_manifest
from tools.project.tests import test_french_model_variant338 as reference


class SpanishModelVariant338Tests(unittest.TestCase):
    spans = ((4, 0xBA0), (0xBA0, 0x16D8), (0x16D8, 0x1B90),
             (0x1B90, 0x1ED4), (0x1ED4, 0x270C))

    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m["linker_symbols"].endswith("/model_variant338_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant338-instances.csv").open() as handle:
            self.instances = {r["module"]: r for r in csv.DictReader(handle)}

    def test_independent_loader_slices_and_stage_selection(self):
        self.assertEqual(len(self.modules), 16)
        self.assertEqual(len({m["sha256"] for m in self.modules}), 16)
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
        expected = {(m, stage+s, s) for stage, models in
                    ((7, (164, 165, 210, 424, 609)), (9, (34, 443, 459))) for m in models for s in (0, 1)}
        self.assertEqual(observed, expected)

    def test_selected_c_and_honest_remaining_assembly(self):
        counts = load_spanish_overlay_inventories(ROOT)
        for module in self.modules:
            layout = ROOT / module["layout"]
            slot, base = int(self.instances[module["name"]]["slot"]), int(module["load_address"], 0)
            expected = [{"address": f"0x{base+offset:X}", "size": f"0x{size:X}",
                         "profile": "gcc_2_8_1_g0_split",
                         "source": f"src/overlays/french_model_variant/variant338_{label}" +
                         ("_slot1" if slot else "") + ".c"}
                        for offset, size, label in ((0x16D8, 1208, "rings"), (0x1B90, 836, "strand"))]
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching["functions"], expected)
            self.assertEqual([r["source"] for r in c_segments(ROOT, layout)], [r["source"] for r in expected])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0)-base, int(r["size"], 0)) for r in rows],
                             [(start, end-start) for start, end in self.spans])
            self.assertEqual([r["status"] for r in rows],
                             ["unmatched_asm", "unmatched_asm", "matching_c", "matching_c", "unmatched_asm"])
            self.assertIn("direct-entry reachable", rows[2]["notes"])
            self.assertIn("no direct entry-call path", rows[3]["notes"])
            self.assertEqual(counts[layout.stem]["function_count"], 5)
            self.assertEqual(counts[layout.stem]["matching_c_function_count"], 2)
            self.assertEqual(counts[layout.stem]["matching_c_bytes"], 2044)

    def test_real_raw_extents_and_complete_resident_bindings(self):
        for module in self.modules:
            base = int(module["load_address"], 0)
            layout = ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x270C, 10484)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0x270C, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())
            bindings = (ROOT / module["linker_symbols"]).read_text()
            self.assertEqual(len(re.findall(r"^\w+ =", bindings, re.M)), 35)
            self.assertNotRegex(bindings, r"=\s*0x801[37]")
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_terminal_records_identify_all_four_wrappers(self):
        with (ROOT / "notes/overlays/spanish-model-variant338-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 4)
        self.assertEqual({(r["function_offset"], r["slot"]) for r in rows},
                         {(o, s) for o in ("0x16D8", "0x1B90") for s in ("0", "1")})
        for row in rows:
            label, size = ("rings", 1208) if row["function_offset"] == "0x16D8" else ("strand", 836)
            suffix = "_slot1" if row["slot"] == "1" else ""
            source = ROOT / f"src/overlays/french_model_variant/variant338_{label}{suffix}.c"
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
                self.assertEqual(struct.unpack_from("<I", data)[0], 338 + slot * 150)
                for offset, word in reference.FrenchModelVariant338Tests.entry_anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                config = base + 0x2808
                self.assertEqual(struct.unpack_from("<II", data, 0x78),
                                 (0x3C030000 | ((config+0x8000) >> 16 & 0xFFFF),
                                  0x24630000 | (config & 0xFFFF)))
                archive.seek((int(row["record"])*276+275)*2048+0x110+(int(row["stage"])-7)//2*4)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, int(row["command_word"]))
                self.assertGreaterEqual(request, 0)
                offset = 0x2808 + request % 1000 * 52
                self.assertTrue(0x270C <= offset and offset+52 <= len(data))
                start, end, _, fade_start, fade_end = struct.unpack_from("<5I", data, offset+32)
                self.assertLess(start, end)
                self.assertLess(fade_start, fade_end)
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
                self.assertEqual(entry_calls, {0xBA0, 0x16D8, 0x1ED4})
                self.assertNotIn(0x1B90, entry_calls)


if __name__ == "__main__":
    unittest.main()
