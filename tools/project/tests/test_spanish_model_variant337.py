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
from progress import load_spanish_overlay_inventories, load_french_overlay_inventories
from verify_inputs import load_checksum_manifest


class SpanishModelVariant337Tests(unittest.TestCase):
    region = "spanish"
    config_path = "config/sles_03951"
    archive_path = "game/spain/DATA/MODEL.MRG"
    instances = ((110, 110, 9, 1), (159, 159, 9, 4), (410, 360, 7, 7))
    spans = ((4, 2464), (0x9A4, 2260), (0x1278, 1216), (0x1738, 2084))
    c_helpers = ((0x4, 2464, "french_model_variant/variant337_entry"),
                 (0x9A4, 2260, "french_model_variant/variant337_ribbon"),
                 (0x1278, 1216, "spanish_model_variant/variant337_rings"))

    def setUp(self):
        self.config = ROOT / self.config_path
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = {r["name"]: r for r in manifest["modules"]}

    def selected(self):
        for model, record, first_stage, command in self.instances:
            for slot in (0, 1):
                name = f"{self.region}_model_variant_{model}_stage{first_stage + slot}_slot{slot}"
                yield self.modules[name], record, first_stage, slot, command

    def test_loader_slices_are_independent_and_preserve_stage_selection(self):
        checksums = load_checksum_manifest(self.config / "files.sha256")
        selected = list(self.selected())
        self.assertEqual(len(selected), 6)
        self.assertEqual(len({m["sha256"] for m, *_ in selected}), 6)
        for module, record, stage, slot, _ in selected:
            self.assertEqual(module["archive"], self.archive_path)
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sector_offset"], record * 276 + (180 if stage == 7 else 200) + slot * 10)
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(int(module["load_address"], 0), 0x8013B000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)

    def test_selected_helpers_are_c(self):
        counts = (load_french_overlay_inventories if self.region == "french" else load_spanish_overlay_inventories)(ROOT)
        for module, _, _, slot, _ in self.selected():
            layout = ROOT / module["layout"]
            base = int(module["load_address"], 0)
            expected = [
                {"address": f"0x{base + offset:X}", "size": f"0x{size:X}",
                 "profile": "gcc_2_8_1_g0_split",
                 "source": "src/overlays/" + source + ("_slot1" if slot else "") + ".c"}
                for offset, size, source in self.c_helpers
            ]
            c_offsets = {offset for offset, _, _ in self.c_helpers}
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching["functions"], expected)
            self.assertEqual([r["source"] for r in c_segments(ROOT, layout)], [r["source"] for r in expected])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(r["address"], 0) - base, int(r["size"], 0)) for r in rows], list(self.spans))
            self.assertEqual([r["status"] for r in rows],
                             ["matching_c" if offset in c_offsets else "unmatched_asm" for offset, _ in self.spans])
            self.assertEqual(counts[layout.stem]["function_count"], 4)
            self.assertEqual(counts[layout.stem]["matching_c_function_count"], len(expected))
            self.assertEqual(counts[layout.stem]["matching_c_bytes"], sum(size for _, size, _ in self.c_helpers))
            for offset, _ in self.spans:
                if offset not in c_offsets:
                    self.assertIn(f"[0x{offset:X}, asm,", layout.read_text())

    def test_real_suffix_storage_and_all_fallback_bindings(self):
        for module, *_ in self.selected():
            layout = ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0x1F5C:X} = 0x{base + 0x1F5C:X}; // type:u8 size:0x30A4 defined:true", symbols)
            self.assertIn(f"[0x1F5C, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())
            bindings = (ROOT / module["linker_symbols"]).read_text()
            self.assertNotRegex(bindings, r"=\s*0x801[37]")
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_canonical_state_and_exact_attempt_records(self):
        directory = ROOT / "src/overlays/spanish_model_variant"
        header = (directory / "variant337_rings.h").read_text()
        source = (directory / "variant337_rings.c").read_text()
        self.assertIn("MATRIX transform;", header)
        self.assertIn("SVECTOR points[4][4];", header)
        self.assertIn("work->transform.t[0]", source)
        self.assertIn("work->elapsed - work->config->start", source)
        self.assertNotRegex(source, r"\b(?:asm|__asm__|register|volatile)\b")
        self.assertIn("#define func_8013C278 func_8017C278", (directory / "variant337_rings_slot1.c").read_text())
        with (ROOT / f"notes/overlays/{self.region}-model-variant337-attempts.csv").open() as handle:
            attempts = [row for row in csv.DictReader(handle) if int(row["function_offset"], 0) == 0x1278]
        self.assertEqual([r["result"] for r in attempts], ["matched", "matched"])
        self.assertTrue(all(r["instruction_bytes"] == "1216" and r["different_words"] == "0" for r in attempts))

    def test_legal_images_boundaries_and_normal_config_windows(self):
        path = ROOT / self.archive_path
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module, record, stage, slot, command in self.selected():
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<I", data)[0], 337 + slot * 150)
                self.assertEqual(struct.unpack_from("<I", data, 0xD8)[0], 0x26C50CCC)
                archive.seek((record * 276 + 275) * 2048)
                metadata = archive.read(2048)
                request = struct.unpack_from("<i", metadata, 0x110 + (stage == 9) * 4)[0]
                self.assertGreaterEqual(request, 0)
                self.assertEqual(request % 1000, command)
                self.assertLessEqual(0x2058 + command * 36 + 36, len(data))
                for start, size in self.spans:
                    end = start + size
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
                            self.assertLess(pc + 4, end)
                            visited.add(pc + 4)
                            delay = struct.unpack_from("<I", data, pc + 4)[0]
                            self.assertNotIn(delay >> 26, (1, 2, 3, 4, 5, 6, 7))
                            self.assertFalse(delay >> 26 == 0 and delay & 63 in (8, 9))
                            if word == 0x03E00008:
                                returns.add(pc)
                            elif op in (2, 3):
                                if op == 2:
                                    pending.append((((base + pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)) - base)
                                else:
                                    pending.append(pc + 8)
                            else:
                                imm = (word & 0xFFFF) - (0x10000 if word & 0x8000 else 0)
                                rs, rt = (word >> 21) & 31, (word >> 16) & 31
                                always = op == 4 and rs == rt
                                never = op == 5 and rs == rt
                                if not never:
                                    pending.append(pc + 4 + imm * 4)
                                if not always:
                                    pending.append(pc + 8)
                        else:
                            self.assertFalse(op == 0 and word & 63 in (8, 9))
                            pending.append(pc + 4)
                    self.assertEqual(visited, set(range(start, end, 4)))
                    self.assertEqual(len(returns), 1)


class SpanishModelVariant337RibbonTests(unittest.TestCase):
    def test_ribbon_terminal_records_and_sdk_aliases(self):
        with (ROOT / "notes/overlays/spanish-model-variant337-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if int(row["function_offset"], 0) == 0x9A4]
        self.assertEqual(len(rows), 2)
        for slot, row in enumerate(rows):
            self.assertEqual(set(row), {"function_offset", "tick", "fingerprint", "profile",
                                       "result", "instruction_bytes", "different_words", "reason"})
            self.assertTrue(row["reason"].startswith(f"Slot{slot}:"))
            suffix = "_slot1" if slot else ""
            source = ROOT / f"src/overlays/french_model_variant/variant337_ribbon{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["result"], row["instruction_bytes"], row["different_words"],
                              row["profile"]), ("matched", "2260", "0", "gcc_2_8_1_g0_split"))
        bindings = (ROOT / "config/sles_03951/overlays/model_variant337_linker_symbols.txt").read_text()
        self.assertEqual(len(re.findall(r"^\w+ =", bindings, re.M)), 33)
        for name, address in (("rsin", 0x80086628), ("rcos", 0x800866F8),
                              ("RotTransPers", 0x80087868), ("ratan2", 0x80089928)):
            self.assertIn(f"{name} = 0x{address:X};", bindings)

    def test_actual_spanish_ribbon_anchors_context_and_descriptors(self):
        from tools.project.tests.test_french_model_variant337 import FrenchModelVariant337Tests

        path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        fixture = SpanishModelVariant337Tests()
        fixture.setUp()
        with path.open("rb") as archive:
            for module, record, stage, slot, command in fixture.selected():
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                base = int(module["load_address"], 0)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                for offset, word in FrenchModelVariant337Tests.ribbon_anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(struct.unpack_from("<I", data, 0x60)[0], 0x04A0018D)
                self.assertEqual(struct.unpack_from("<I", data, 0x818)[0],
                                 0x0C000000 | ((base + 0x9A4) >> 2 & 0x3FFFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x1274)[0], 0x27BD0130)
                for start, end, register, expected in (
                    (4, 0x68, 18, [0xC]), (0x698, 0x824, 18, []),
                    (0x818, 0x820, 4, [0x81C]),
                    (0x9A4, 0x1278, 23, [0x9AC, 0x1250]),
                    (0x9A4, 0x1278, 29, [0x9A4, 0x1274]),
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
                archive.seek((record * 276 + 275) * 2048 + 0x110 + (stage == 9) * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 503000 + command)
                descriptor = 0x2058 + command * 36
                self.assertTrue(0x1F5C <= descriptor and descriptor + 36 <= len(data))
                gate, start, end, fade_start, fade_end = struct.unpack_from("<5I", data, descriptor + 16)
                self.assertLessEqual(gate, start)
                self.assertLess(start, end)
                self.assertLess(fade_start, fade_end)

    def test_record_and_packet_extents_from_spanish_instructions(self):
        path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        fixture = SpanishModelVariant337Tests()
        fixture.setUp()
        with path.open("rb") as archive:
            for module, *_ in fixture.selected():
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                stride = struct.unpack_from("<I", data, 0x418)[0] & 0xFFFF
                rings = struct.unpack_from("<I", data, 0x1C)[0] & 0xFFFF
                self.assertEqual((stride, rings), (932, 932))
                packet_start = struct.unpack_from("<I", data, 0x30)[0] & 0xFFFF
                packet_size = struct.unpack_from("<I", data, 0x10C4)[0] & 0xFFFF
                self.assertEqual((packet_start, packet_size, packet_start + 2 * packet_size),
                                 (0xC6C, 40, 0xCBC))
                fields = set()
                for offset in range(0xF94, 0x1100, 4):
                    word, = struct.unpack_from("<I", data, offset)
                    if word >> 26 in (40, 41, 43) and (word >> 21 & 31) == 16:
                        displacement = (word & 0xFFFF) - (0x10000 if word & 0x8000 else 0)
                        fields.add((6 + displacement, {40: 1, 41: 2, 43: 4}[word >> 26]))
                self.assertEqual(fields, {(4, 1), (5, 1), (6, 1), (8, 2), (10, 2),
                                          (16, 2), (18, 2), (24, 2), (26, 2), (32, 2), (34, 2)})
                self.assertLessEqual(max(offset + size for offset, size in fields), packet_size)


if __name__ == "__main__":
    unittest.main()
