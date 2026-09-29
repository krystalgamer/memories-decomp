import csv
import hashlib
import json
from pathlib import Path
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_spanish_overlay_inventories
from verify_inputs import load_checksum_manifest


class SpanishModelReturnTwoTests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [row for row in manifest["modules"]
                        if row["name"].startswith("spanish_model_return_two_")]
        with (ROOT / "notes/overlays/spanish-model-return-two-instances.csv").open() as handle:
            self.instances = list(csv.DictReader(handle))

    def test_representatives_follow_primary_loader_slices(self):
        self.assertEqual({row["name"] for row in self.modules},
                         {"spanish_model_return_two_slot0", "spanish_model_return_two_slot1"})
        checksums = load_checksum_manifest(self.config / "files.sha256")
        first = next(row for row in self.instances if row["model_id"] == "0")
        for module in self.modules:
            slot = int(module["name"][-1])
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sector_offset"], 220 + slot * 2)
            self.assertEqual(module["sector_count"], 2)
            self.assertEqual(int(module["load_address"], 0), 0x8013A000 + slot * 0x40000)
            self.assertEqual(module["sha256"], first[f"slot{slot}_sha256"])
            self.assertNotIn("duplicate_sector_offsets", module)

    def test_one_real_compiler_entry_and_no_callee_aliases(self):
        for module in self.modules:
            layout = ROOT / module["layout"]
            slot = int(module["name"][-1])
            source = "src/overlays/spanish_model_primary/return_two" + ("_slot1" if slot else "") + ".c"
            functions = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(functions["functions"], [{
                "address": f"0x{int(module['load_address'], 0) + 4:X}",
                "profile": "gcc_2_8_1_g0_split", "size": "0x8", "source": source,
            }])
            segments = c_segments(ROOT, layout)
            self.assertEqual(len(segments), 1)
            self.assertEqual(segments[0]["source"], source)
            self.assertNotIn("=", (ROOT / module["linker_symbols"]).read_text())

    def test_complete_raw_suffix_stays_owned_and_uncounted(self):
        counts = load_spanish_overlay_inventories(ROOT)
        for module in self.modules:
            layout = ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base:X} = 0x{base:X}; // type:u8 size:0x4 defined:true", symbols)
            self.assertIn(f"D_{base + 12:X} = 0x{base + 12:X}; // type:u8 size:0xFF4 defined:true", symbols)
            self.assertIn(f"[0xC, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())
            self.assertIn("- [0x1000]", layout.read_text())
            self.assertEqual(counts[layout.stem]["matching_c_function_count"], 1)
            self.assertEqual(counts[layout.stem]["matching_c_bytes"], 8)

    def test_ledger_covers_all_other_compact_records_without_inflating_manifest(self):
        nontrivial = {8, 116, 141, 150, 167, 320, 344, 366, 607, 615}
        self.assertEqual(len(self.instances), 611)
        self.assertEqual(len({row["model_id"] for row in self.instances}), 611)
        self.assertEqual({int(row["record_index"]) for row in self.instances},
                         set(range(621)) - nontrivial)
        for row in self.instances:
            for slot in (0, 1):
                self.assertEqual(int(row[f"slot{slot}_sector"]),
                                 int(row["record_index"]) * 276 + 220 + slot * 2)
                self.assertRegex(row[f"slot{slot}_sha256"], r"^[0-9a-f]{64}$")
        self.assertEqual(len(self.modules), 2)

    def test_source_keeps_the_two_argument_callback_and_real_slot_name(self):
        directory = ROOT / "src/overlays/spanish_model_primary"
        text = (directory / "return_two.c").read_text()
        self.assertIn('#include "../../types.h"', text)
        self.assertRegex(text, r"s32 func_8013A004\(u8 \*context, s32 command\)\s*\{\s*return 2;\s*\}")
        self.assertNotRegex(text, r"\b(?:asm|__asm__|register)\b")
        wrapper = (directory / "return_two_slot1.c").read_text()
        self.assertIn("#define func_8013A004 func_8017A004", wrapper)
        self.assertIn('#include "return_two.c"', wrapper)

    @unittest.skipUnless((ROOT / "game/spain/DATA/MODEL.MRG").exists(), "legal Spanish MODEL input required")
    def test_all_ledger_images_match_legal_archive_and_entry_bytes(self):
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as archive:
            for row in self.instances:
                for slot in (0, 1):
                    with self.subTest(model=row["model_id"], slot=slot):
                        archive.seek(int(row[f"slot{slot}_sector"]) * 2048)
                        payload = archive.read(4096)
                        self.assertEqual(len(payload), 4096)
                        self.assertEqual(hashlib.sha256(payload).hexdigest(), row[f"slot{slot}_sha256"])
                        self.assertEqual(struct.unpack_from("<I", payload)[0], 54 + slot * 3)
                        self.assertEqual(payload[4:12], struct.pack("<II", 0x03E00008, 0x24020002))
