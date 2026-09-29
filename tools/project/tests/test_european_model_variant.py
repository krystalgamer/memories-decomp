import csv
import json
from pathlib import Path
import re
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_french_overlay_inventories, load_spanish_overlay_inventories
from verify_inputs import load_checksum_manifest


class SpanishModelVariantTests(unittest.TestCase):
    config = ROOT / "config/sles_03951"
    region = "spanish"
    archive = "game/spain/DATA/MODEL.MRG"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    functions = ((4, 2360, "entry"), (0x93C, 1776, "update"), (0x102C, 1064, "draw"))
    parts = (
        (0x1454, 28, "unknown_prefix", False),
        (0x1470, 28, "image0", True),
        (0x148C, 84, "unknown_records1_3", False),
        (0x14E0, 28, "image4", True),
        (0x14FC, 28, "unknown_record5", False),
        (0x1518, 28, "image6", True),
        (0x1534, 28, "image7", True),
        (0x1550, 36, "config0", True),
        (0x1574, 0x3A8C, "unclassified_tail", False),
    )

    def modules(self):
        manifest = json.loads((self.config / "overlays.json").read_text())
        return [m for m in manifest["modules"] if m["name"].startswith(f"{self.region}_model_variant_54_")]

    def test_loader_slices_are_distinct_second_variants(self):
        modules = self.modules()
        self.assertEqual({m["name"] for m in modules}, {
            f"{self.region}_model_variant_54_stage9_slot0",
            f"{self.region}_model_variant_54_stage10_slot1",
        })
        checksums = load_checksum_manifest(self.config / "files.sha256")
        for module in modules:
            slot = int(module["name"][-1])
            self.assertEqual(module["archive"], self.archive)
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sector_offset"], 54 * 276 + 200 + slot * 10)
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(int(module["load_address"], 0), 0x8013B000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)
        self.assertEqual(len({m["sha256"] for m in modules}), 2)

    def test_all_three_extents_select_compiler_owned_functions(self):
        inventories = self.load_inventories(ROOT)
        total = 0
        for module in self.modules():
            layout = ROOT / module["layout"]
            base = int(module["load_address"], 0)
            suffix = "_slot1" if module["name"].endswith("slot1") else ""
            expected = [
                {"address": f"0x{base+offset:X}", "profile": "gcc_2_8_1_g0_split",
                 "size": f"0x{size:X}",
                 "source": f"src/overlays/spanish_model_variant/variant408_{role}{suffix}.c"}
                for offset, size, role in self.functions
            ]
            self.assertEqual(json.loads(layout.with_name(
                layout.stem + "_matching_c.json").read_text())["functions"], expected)
            self.assertEqual([s["source"] for s in c_segments(ROOT, layout)],
                             [f["source"] for f in expected])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([r["name"] for r in rows], [f"func_{base+o:X}" for o, _, _ in self.functions])
            self.assertTrue(all(r["status"] == "matching_c" for r in rows))
            counts = inventories[layout.stem]
            self.assertEqual(counts["matching_c_function_count"], 3)
            self.assertEqual(counts["matching_c_bytes"], 5200)
            total += counts["matching_c_bytes"]
        self.assertEqual(total, 10400)

    def test_known_data_and_unknown_intervals_remain_real_storage(self):
        self.assertEqual(sum(size for _, size, _, known in self.parts if known), 148)
        self.assertEqual(sum(size for _, size, _, known in self.parts if not known), 15128)
        self.assertEqual(self.parts[-1][0] + self.parts[-1][1], 20480)
        for module in self.modules():
            layout = ROOT / module["layout"]
            text = layout.read_text()
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (ROOT / module["linker_symbols"]).read_text()
            base = int(module["load_address"], 0)
            for offset, size, label, _ in self.parts:
                self.assertIn(f"[0x{offset:X}, data, overlays/{module['name']}/{label}]", text)
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; "
                              f"// type:u8 size:0x{size:X} defined:true", symbols)
                self.assertNotIn(f"D_{base+offset:X} =", bindings)
            self.assertIn("- [0x5000]", text)

    def test_slot_wrappers_and_canonical_contracts(self):
        directory = ROOT / "src/overlays/spanish_model_variant"
        for offset, _, role in self.functions:
            wrapper = (directory / f"variant408_{role}_slot1.c").read_text()
            self.assertIn(f"#define func_{0x8013B000+offset:X} func_{0x8017B000+offset:X}", wrapper)
            self.assertIn(f'#include "variant408_{role}.c"', wrapper)
        entry = (directory / "variant408_entry_slot1.c").read_text()
        for offset in (0x1470, 0x1550):
            self.assertIn(f"#define D_{0x8013B000+offset:X} D_{0x8017B000+offset:X}", entry)
        header = (directory / "variant408_entry.h").read_text()
        self.assertIn('#include "../../game/model_texture_upload.h"', header)
        self.assertIn("extern GsIMAGE D_8013C470[];", header)
        self.assertNotIn("u32 func_80059A50(", header)
        for source in directory.glob("*.c"):
            self.assertIn('#include "../../types.h"', source.read_text())
            self.assertNotRegex(source.read_text(), r"\b(?:asm|__asm__)\b")

    def test_resident_bindings_are_not_overlay_aliases(self):
        path = self.config / "overlays/model_variant408_linker_symbols.txt"
        bindings = {name: int(address, 16) for name, address in
                    re.findall(r"^(\w+) = (0x[0-9A-F]+);", path.read_text(), re.M)}
        self.assertEqual(len(bindings), 32)
        with (self.config / "functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        self.assertTrue(all(address in resident for address in bindings.values()))
        self.assertTrue(all(address < 0x80100000 for address in bindings.values()))
        self.assertEqual(bindings["func_80059A50"], 0x8005CB58)
        self.assertEqual(bindings["func_80058F10"], 0x8005C018)
        self.assertEqual(bindings["func_8005B260"], 0x8004D5B8)


class FrenchModelVariantTests(SpanishModelVariantTests):
    config = ROOT / "config/sles_03948"
    region = "french"
    archive = "game/france/DATA/MODEL.MRG"
    load_inventories = staticmethod(load_french_overlay_inventories)


if __name__ == "__main__":
    unittest.main()
