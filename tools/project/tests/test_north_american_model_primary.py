import csv
import json
from pathlib import Path
import re
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from progress import load_overlay_inventories
from verify_inputs import load_checksum_manifest


class NorthAmericanModelPrimaryTests(unittest.TestCase):
    config = ROOT / "config/slus_01411"
    spanish = ROOT / "config/sles_03951"
    records = {8: 8, 116: 116, 141: 141, 150: 150, 167: 167,
               370: 320, 394: 344, 416: 366, 707: 607, 715: 615}

    def modules(self):
        manifest = json.loads((self.config / "overlays.json").read_text())
        return [m for m in manifest["modules"] if m["name"].startswith("model_primary_")]

    def spanish_modules(self):
        manifest = json.loads((self.spanish / "overlays.json").read_text())
        return {m["name"].removeprefix("spanish_"): m for m in manifest["modules"]
                if m["name"].startswith("spanish_model_primary_")}

    def test_loader_slices_cover_both_slots_of_registered_models(self):
        modules = self.modules()
        self.assertEqual({m["name"] for m in modules}, {
            f"model_primary_{model}_slot{slot}" for model in self.records for slot in (0, 1)
        })
        checksums = load_checksum_manifest(self.config / "files.sha256")
        for module in modules:
            model, slot = map(int, re.fullmatch(
                r"model_primary_(\d+)_slot(\d)", module["name"]).groups())
            self.assertEqual(module["archive"], "game/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sector_offset"], self.records[model] * 276 + 220 + slot * 2)
            self.assertEqual(module["sector_count"], 2)
            self.assertEqual(int(module["load_address"], 0), 0x8013A000 + slot * 0x40000)

    def test_images_reuse_the_spanish_sources_and_layouts_unchanged(self):
        spanish = self.spanish_modules()
        self.assertEqual(set(spanish), {m["name"] for m in self.modules()})
        for module in self.modules():
            with self.subTest(module=module["name"]):
                original = spanish[module["name"]]
                for key in ("sector_offset", "sector_count", "load_address"):
                    self.assertEqual(module[key], original[key])
                self.assertNotEqual(module["archive_sha256"], original["archive_sha256"])
                layout = ROOT / module["layout"]
                source = ROOT / original["layout"]
                for suffix in ("_matching_c.json", "_functions.csv", "_symbols.txt"):
                    self.assertEqual(
                        layout.with_name(layout.stem + suffix).read_text(),
                        source.with_name(source.stem + suffix).read_text(),
                    )
                ours = layout.read_text()
                theirs = re.sub(r"spanish_model_primary_(\d)", r"model_primary_\1",
                                source.read_text()).replace("config/sles_03951", "config/slus_01411")
                strip = lambda text: re.sub(r"^sha1: .*$", "", text, flags=re.M)
                self.assertEqual(strip(ours), strip(theirs))
                self.assertIn("overlays/spanish_model_primary/", ours)

    def test_resident_bindings_are_the_executable_owners(self):
        with (self.config / "functions.csv").open() as handle:
            resident = {int(row["address"], 16): row for row in csv.DictReader(handle)}
        seen = set()
        for module in self.modules():
            path = ROOT / module["linker_symbols"]
            if path in seen:
                continue
            seen.add(path)
            for name, address in re.findall(r"^(\w+) = (0x[0-9A-F]+);", path.read_text(), re.M):
                self.assertEqual(resident[int(address, 16)]["name"], name, path.name)
        self.assertEqual(len(seen), 5)
        effect = (self.config / "overlays/model_primary_effect_linker_symbols.txt").read_text()
        self.assertEqual(dict(re.findall(r"^(\w+) = (0x[0-9A-F]+);", effect, re.M)), {
            "Model_GetActiveSlotIndex": "0x80058DCC",
            "Model_GetSlotAnimationIndex": "0x80058E68",
            "func_8005D994": "0x8005D994",
        })

    def test_reporting_counts_the_images(self):
        inventories = load_overlay_inventories(ROOT)
        rows = [counts for name, counts in inventories.items()
                if name.startswith("model_primary_")]
        self.assertEqual(len(rows), 20)
        self.assertEqual(sum(r["function_count"] for r in rows), 20)
        self.assertEqual(sum(r["matching_c_function_count"] for r in rows), 20)
        self.assertEqual(sum(r["matching_c_bytes"] for r in rows), 16432)


if __name__ == "__main__":
    unittest.main()
