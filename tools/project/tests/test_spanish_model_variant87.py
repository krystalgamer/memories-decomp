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


class SpanishModelVariant87Tests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"]
                        if m.get("linker_symbols", "").endswith("/model_variant87_linker_symbols.txt")]
        with (ROOT / "notes/overlays/spanish-model-variant87-instances.csv").open() as handle:
            self.instances = {r["module"]: r for r in csv.DictReader(handle)}

    def test_loader_slices_and_four_independent_images(self):
        self.assertEqual(len(self.modules), 4)
        self.assertEqual(len({m["sha256"] for m in self.modules}), 4)
        checksums = load_checksum_manifest(self.config / "files.sha256")
        observed = set()
        for module in self.modules:
            row = self.instances[module["name"]]
            model, record, stage, slot = (int(row[k]) for k in ("model", "record", "stage", "slot"))
            observed.add((model, stage, slot))
            self.assertEqual(record, model - (50 if model >= 350 else 0))
            self.assertEqual(module["sector_offset"], record * 276 + 200 + slot * 10)
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sha256"], row["sha256"])
            self.assertEqual(int(module["load_address"], 0), 0x8013B000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)
        self.assertEqual(observed, {(m, 9+s, s) for m in (140, 584) for s in (0, 1)})

    def test_real_c_selection_reuses_the_verified_source(self):
        counts = load_spanish_overlay_inventories(ROOT)
        for module in self.modules:
            layout = ROOT / module["layout"]
            slot = int(self.instances[module["name"]]["slot"])
            base = int(module["load_address"], 0)
            source = "src/overlays/french_model_variant/variant87_entry" + ("_slot1" if slot else "") + ".c"
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching["functions"], [{
                "address": f"0x{base+4:X}", "size": "0xA18",
                "profile": "gcc_2_8_1_g0_split", "source": source}])
            self.assertEqual([r["source"] for r in c_segments(ROOT, layout)], [source])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(r["address"], r["size"], r["status"]) for r in rows],
                             [(f"0x{base+4:X}", "0xA18", "matching_c")])
            self.assertEqual(counts[layout.stem]["function_count"], 1)
            self.assertEqual(counts[layout.stem]["matching_c_function_count"], 1)
            self.assertEqual(counts[layout.stem]["matching_c_bytes"], 2584)

    def test_real_header_suffix_storage_and_complete_bindings(self):
        for module in self.modules:
            base = int(module["load_address"], 0)
            layout = ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0xA1C, 17892)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0xA1C, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())
            bindings = (ROOT / module["linker_symbols"]).read_text()
            self.assertEqual(len(re.findall(r"^\w+ =", bindings, re.M)), 21)
            self.assertNotRegex(bindings, r"=\s*0x801[37]")
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_terminal_attempts_identify_each_compiled_source(self):
        with (ROOT / "notes/overlays/spanish-model-variant87-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual({r["slot"] for r in rows}, {"0", "1"})
        self.assertEqual(len(rows), 2)
        for row in rows:
            suffix = "_slot1" if row["slot"] == "1" else ""
            source = ROOT / f"src/overlays/french_model_variant/variant87_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["result"], row["instruction_bytes"], row["different_words"]),
                             ("matched", "2584", "0"))
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")

    def test_legal_images_config_windows_and_entry_control_flow(self):
        archive_path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<I", data)[0], 87 + int(row["slot"]) * 130)
                self.assertEqual(struct.unpack_from("<I", data, 4)[0], 0x27BDFE10)
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x114)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                self.assertEqual(command, 17001)
                offset = 0xA1C + command % 1000 * 22
                self.assertEqual(offset, 0xA32)
                descriptor = struct.unpack_from("<8B7h", data, offset)
                self.assertEqual(descriptor[7], 16)
                self.assertLessEqual(0xD4 + descriptor[7] * 8, 0x1D4)
                bindings = {int(a, 0) for a in re.findall(r"= (0x[0-9A-F]+);",
                            (ROOT / module["linker_symbols"]).read_text())}
                pending, visited, calls, returns = [4], set(), [], set()
                while pending:
                    pc = pending.pop()
                    self.assertTrue(4 <= pc < 0xA1C and pc % 4 == 0)
                    if pc in visited:
                        continue
                    visited.add(pc)
                    word = struct.unpack_from("<I", data, pc)[0]
                    op = word >> 26
                    if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
                        self.assertLess(pc + 4, 0xA1C)
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
                                calls.append(target)
                                pending.append(pc + 8)
                        else:
                            imm = (word & 0xFFFF) - (0x10000 if word & 0x8000 else 0)
                            pending.extend((pc + 8, pc + 4 + imm * 4))
                    else:
                        self.assertFalse(op == 0 and word & 63 in (8, 9))
                        pending.append(pc + 4)
                self.assertEqual(visited, set(range(4, 0xA1C, 4)))
                self.assertEqual(returns, {0xA14})
                self.assertEqual(len(calls), 29)
                self.assertEqual(set(calls), bindings)


if __name__ == "__main__":
    unittest.main()
