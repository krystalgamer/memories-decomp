import csv
import json
from pathlib import Path
import re
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_spanish_overlay_inventories
from verify_inputs import load_checksum_manifest


class SpanishModelPrimaryTests(unittest.TestCase):
    def modules(self):
        manifest = json.loads((ROOT / "config/sles_03951/overlays.json").read_text())
        return [m for m in manifest["modules"] if m["name"].startswith("spanish_model_primary_")]

    def test_loader_slices_cover_both_slots_of_seven_models(self):
        records = {116: 116, 150: 150, 167: 167, 370: 320, 394: 344, 707: 607, 715: 615}
        modules = self.modules()
        self.assertEqual({m["name"] for m in modules}, {
            f"spanish_model_primary_{model}_slot{slot}" for model in records for slot in (0, 1)
        })
        checksums = load_checksum_manifest(ROOT / "config/sles_03951/files.sha256")
        for module in modules:
            model, slot = map(int, re.fullmatch(
                r"spanish_model_primary_(\d+)_slot(\d)", module["name"]).groups())
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sector_offset"], records[model] * 276 + 220 + slot * 2)
            self.assertEqual(module["sector_count"], 2)
            self.assertEqual(int(module["load_address"], 0), 0x8013A000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)

    def test_each_layout_selects_one_compiler_owned_entry(self):
        for module in self.modules():
            with self.subTest(module=module["name"]):
                layout = ROOT / module["layout"]
                copy = any(f"_{model}_" in module["name"] for model in (150, 167))
                size = 276 if copy else 216
                address = int(module["load_address"], 0) + 4
                suffix = "_slot1" if module["name"].endswith("slot1") else ""
                source = f"src/overlays/spanish_model_primary/{'copy' if copy else 'effect'}{suffix}.c"
                functions = json.loads(layout.with_name(
                    layout.stem + "_matching_c.json").read_text())["functions"]
                self.assertEqual(functions, [{
                    "address": f"0x{address:X}", "profile": "gcc_2_8_1_g0_split",
                    "size": f"0x{size:X}", "source": source,
                }])
                with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                    rows = list(csv.DictReader(handle))
                self.assertEqual(len(rows), 1)
                self.assertEqual(rows[0]["name"], f"func_{address:X}")
                self.assertEqual(rows[0]["status"], "matching_c")
                segments = c_segments(ROOT, layout)
                self.assertEqual(len(segments), 1)
                self.assertEqual(segments[0]["source"], source)

    def test_descriptor_prefixes_have_real_storage_and_tails_are_uncounted(self):
        inventories = load_spanish_overlay_inventories(ROOT)
        total = 0
        for module in self.modules():
            layout = ROOT / module["layout"]
            copy = any(f"_{model}_" in module["name"] for model in (150, 167))
            data = 0x118 if copy else 0xDC
            tail = data + 48
            symbol = int(module["load_address"], 0) + data
            text = layout.read_text()
            self.assertIn(f"[0x{tail:X}, data, overlays/{module['name']}/opaque_tail]", text)
            self.assertIn("- [0x1000]", text)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{symbol:X} = 0x{symbol:X}; // size:0x30", symbols)
            self.assertNotIn(f"D_{symbol:X} =", (ROOT / module["linker_symbols"]).read_text())
            counts = inventories[layout.stem]
            self.assertEqual(counts["matching_c_function_count"], 1)
            self.assertEqual(counts["matching_c_bytes"], data - 4)
            total += counts["matching_c_bytes"]
        self.assertEqual(total, 3264)

    def test_slot_wrappers_keep_real_slot_specific_symbol_names(self):
        directory = ROOT / "src/overlays/spanish_model_primary"
        for family, data in (("copy", "118"), ("effect", "0DC")):
            wrapper = (directory / f"{family}_slot1.c").read_text()
            self.assertIn("#define func_8013A004 func_8017A004", wrapper)
            self.assertIn(f"#define D_8013A{data} D_8017A{data}", wrapper)
            self.assertIn(f'#include "{family}.c"', wrapper)
        header = (directory / "primary.h").read_text()
        self.assertIn('#include "../../game/model_control.h"', header)
        self.assertIn("u32 tick;", header)
        for path in directory.glob("*.c"):
            self.assertNotRegex(path.read_text(), r"\b(?:asm|__asm__)\b")

    def test_resident_callee_bindings_are_fixed(self):
        configuration = ROOT / "config/sles_03951/overlays"
        for family, expected in (
            ("copy", {"Model_GetActiveSlotIndex": "0x8005BED4", "MoveImage": "0x8007FFD0"}),
            ("effect", {"Model_GetActiveSlotIndex": "0x8005BED4",
                        "Model_GetSlotAnimationIndex": "0x8005BF70",
                        "func_8005D994": "0x8004D9A4"}),
        ):
            text = (configuration / f"model_primary_{family}_linker_symbols.txt").read_text()
            self.assertEqual(dict(re.findall(r"^(\w+) = (0x[0-9A-F]+);", text, re.M)), expected)
