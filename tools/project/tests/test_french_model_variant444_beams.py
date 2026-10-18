import csv
import hashlib
import json
from pathlib import Path
import re
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments

FRENCH = ROOT / "config/sles_03948"
STEM = "src/overlays/french_model_variant/variant444_beams"
BODY = ROOT / "src/overlays/spanish_model_variant/variant411_beams.c"
MODULES = {"french_model_variant_175_stage9_slot0", "french_model_variant_175_stage10_slot1"}


class FrenchModelVariant444BeamsTests(unittest.TestCase):
    def setUp(self):
        manifest = json.loads((FRENCH / "overlays.json").read_text())["modules"]
        self.modules = [m for m in manifest if m["name"] in MODULES]

    def test_both_images_use_the_shared_body(self):
        self.assertEqual({m["name"] for m in self.modules}, MODULES)
        for module in self.modules:
            with self.subTest(module=module["name"]):
                base = int(module["load_address"], 0)
                slot = (base - 0x8013B000) // 0x40000
                source = STEM + ("_slot1" if slot else "") + ".c"
                layout = ROOT / module["layout"]
                functions = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())["functions"]
                self.assertIn({"address": f"0x{base + 0x14E8:X}", "profile": "gcc_2_8_1_g0_split",
                               "size": "0x8DC", "source": source}, functions)
                self.assertIn(source, [s["source"] for s in c_segments(ROOT, layout)])
                with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                    row = next(r for r in csv.DictReader(handle) if int(r["address"], 0) == base + 0x14E8)
                self.assertEqual((row["status"], row["size"]), ("matching_c", "0x8DC"))
                self.assertEqual(module["linker_symbols"], "config/sles_03948/overlays/model_variant444_linker_symbols.txt")


    def test_binding_is_a_french_resident_function(self):
        with (FRENCH / "functions.csv").open() as handle:
            resident = {int(r["address"], 0) for r in csv.DictReader(handle)}
        bindings = dict(re.findall(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);",
                                   (FRENCH / "overlays/model_variant444_linker_symbols.txt").read_text(), re.M))
        self.assertEqual(bindings["RotTransPers"], "0x80087868")
        for address in bindings.values():
            self.assertIn(int(address, 0), resident)


    def test_built_images_are_exact(self):
        for module in self.modules:
            name = module["name"]
            binary = ROOT / "tmp/overlays" / name / f"build/{name}.bin"
            if not binary.exists():
                self.skipTest("Build French overlays before checking linked images")
            with self.subTest(module=name):
                self.assertEqual(hashlib.sha256(binary.read_bytes()).hexdigest(), module["sha256"])


if __name__ == "__main__":
    unittest.main()
