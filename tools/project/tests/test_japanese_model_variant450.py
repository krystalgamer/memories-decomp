import csv
import hashlib
import json
from pathlib import Path
import re
import sys
import unittest

import yaml

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from tools.project.overlay_sources import c_segments


CONFIG = ROOT / "config/slpm_86398"
SPANS = (
    (0x4, 0xDDC),
    (0xDDC, 0x1770),
    (0x1770, 0x1E7C),
    (0x1E7C, 0x2754),
    (0x2754, 0x2D20),
    (0x2D20, 0x30A8),
    (0x30A8, 0x38DC),
)
IMAGES = (
    (9, 0, 48224, "149e9a9dff9244d944c39458502bd31bea4ae6bd075fd8d800f3d60a3cbd3513"),
    (10, 1, 48234, "1a9e31b18c52c63353ef0eec15a8bb21c0458fe943628512c9b77f1a769640a3"),
)
HELPERS = (
    (0x2754, 0x5CC, "quads"),
    (0x2D20, 0x388, "lines"),
)
IMPORTS = {
    0x8005BD40,
    0x80082D20,
    0x80082F10,
    0x800841C0,
    0x80084EC0,
    0x80085F10,
    0x80086260,
    0x800863A0,
    0x80086500,
    0x800865C0,
    0x80086920,
    0x80088590,
}


class JapaneseModelVariant450Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        with (ROOT / "notes/overlays/japanese-model-variant450-instances.csv").open() as handle:
            cls.instances = {row["module"]: row for row in csv.DictReader(handle)}
        modules = json.loads((CONFIG / "overlays.json").read_text())["modules"]
        cls.modules = {row["name"]: row for row in modules if row["name"] in cls.instances}

    def test_two_registered_images_and_exact_inventory(self):
        expected = {
            f"japanese_model_variant_174_stage{stage}_slot{slot}"
            for stage, slot, *_ in IMAGES
        }
        self.assertEqual(set(self.instances), expected)
        self.assertEqual(set(self.modules), expected)
        for stage, slot, sector, digest in IMAGES:
            name = f"japanese_model_variant_174_stage{stage}_slot{slot}"
            module, instance = self.modules[name], self.instances[name]
            base = 0x8013B000 + slot * 0x40000
            self.assertEqual((module["sector_offset"], module["sector_count"]), (sector, 10))
            self.assertEqual(module["load_address"], f"0x{base:08X}")
            self.assertEqual((module["sha256"], instance["sha256"]), (digest, digest))
            self.assertEqual(module["linker_symbols"],
                             "config/slpm_86398/overlays/model_variant_linker_symbols.txt")
            layout = ROOT / module["layout"]
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual(
                [(int(row["address"], 0) - base, int(row["size"], 0), row["status"]) for row in rows],
                [(start, end - start, "matching_c" if start in (0x2754, 0x2D20)
                  else "unmatched_asm") for start, end in SPANS],
            )
            self.assertTrue(all("complete matching 20 KiB image" in row["notes"]
                                for row in rows if row["status"] == "matching_c"))

    def test_two_c_owners_use_cdk_profile_and_exact_extents(self):
        for stage, slot, *_ in IMAGES:
            name = f"japanese_model_variant_174_stage{stage}_slot{slot}"
            layout = ROOT / self.modules[name]["layout"]
            base = 0x8013B000 + slot * 0x40000
            mapping = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            expected = []
            for offset, size, label in HELPERS:
                suffix = "_slot1" if slot else ""
                expected.append({
                    "address": f"0x{base + offset:08X}",
                    "profile": "gcc_2_7_2_cdk_g0",
                    "size": f"0x{size:X}",
                    "source": f"src/overlays/model_variant/variant450_japanese_{label}{suffix}.c",
                })
            self.assertEqual(mapping, {"functions": expected, "schema": 1})
            self.assertEqual([row["source"] for row in c_segments(ROOT, layout)],
                             [row["source"] for row in expected])
            segments = yaml.safe_load(layout.read_text())["segments"]
            self.assertEqual(segments[-1], [0x5000])
            self.assertEqual(
                [(row["start"], row["vram"], row["subsegments"][0][1]) for row in segments[:-1]],
                [
                    (0, base, "data"),
                    (4, base + 4, "asm"),
                    (0x2754, base + 0x2754, "c"),
                    (0x2D20, base + 0x2D20, "c"),
                    (0x30A8, base + 0x30A8, "asm"),
                    (0x38DC, base + 0x38DC, "data"),
                ],
            )

    def test_attempts_and_source_fingerprints(self):
        with (ROOT / "notes/overlays/japanese-model-variant450-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([row["result"] for row in rows], ["text_exact"] * 4 + ["matched"] * 4)
        self.assertEqual(
            [(int(row["instruction_bytes"]), int(row["different_words"])) for row in rows],
            [(1484, 0), (1484, 0), (904, 0), (904, 0)] * 2,
        )
        for row in rows:
            slot = int(row["slot"])
            label = "quads" if row["function_offset"] == "0x2754" else "lines"
            suffix = "_slot1" if slot else ""
            source = ROOT / f"src/overlays/model_variant/variant450_japanese_{label}{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["profile"], "gcc_2_7_2_cdk_g0")

    def test_external_bindings_are_resident_function_starts(self):
        with (CONFIG / "functions.csv").open() as handle:
            starts = {int(row["address"], 0) for row in csv.DictReader(handle)}
        self.assertTrue(IMPORTS <= starts)
        linker = (CONFIG / "overlays/model_variant_linker_symbols.txt").read_text()
        linker_bindings = {
            int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", linker)
        }
        self.assertEqual(linker_bindings, IMPORTS)
        for module in self.modules.values():
            layout = ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = {
                int(value, 16)
                for value in re.findall(
                    r"= (0x[0-9A-F]+); // type:func absolute:true", symbols
                )
            }
            self.assertEqual(bindings, IMPORTS)


if __name__ == "__main__":
    unittest.main()
