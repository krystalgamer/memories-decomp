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
LEDGER = ROOT / "notes/overlays/french-model-usa-new-entry-reuse.csv"
CLUT = {"752", "754", "756", "766", "779"}
HEIGHT = {"119", "754"}


class FrenchModelUsaNewEntryReuseTests(unittest.TestCase):
    def setUp(self):
        self.modules = {m["name"]: m for m in json.loads((FRENCH / "overlays.json").read_text())["modules"]}
        with LEDGER.open() as handle:
            self.rows = list(csv.DictReader(handle))

    def test_ledger_matches_the_manifests(self):
        self.assertEqual(len(self.rows), 433)
        for row in self.rows:
            with self.subTest(module=row["module"]):
                module = self.modules[row["module"]]
                layout = ROOT / module["layout"]
                self.assertEqual(int(row["address"], 0), int(module["load_address"], 0) + 4)
                entry = {"address": row["address"], "profile": "gcc_2_8_1_g0_split",
                         "size": row["size"], "source": row["french_source"]}
                functions = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())["functions"]
                self.assertEqual([entry], functions)
                self.assertIn(row["french_source"], [s["source"] for s in c_segments(ROOT, layout)])
                family = re.search(r"variant(\d+)_entry", row["french_source"]).group(1)
                self.assertEqual(module["linker_symbols"],
                                 f"config/sles_03948/overlays/model_variant{family}_linker_symbols.txt")
                self.assertEqual(row["fingerprint"],
                                 hashlib.sha256((ROOT / row["french_source"]).read_bytes()).hexdigest())
                self.assertEqual(row["dependency_fingerprint"],
                                 hashlib.sha256((ROOT / row["north_american_source"]).read_bytes()).hexdigest())

    def test_wrappers_only_set_regional_constants_and_slot_renames(self):
        for path in sorted({row["french_source"] for row in self.rows}):
            with self.subTest(path=path):
                text = (ROOT / path).read_text().splitlines()
                body = next(r["north_american_source"] for r in self.rows if r["french_source"] == path)
                header = re.search(r"variant(\d+)_entry", body).group(1)
                expected = ['#include "../../types.h"']
                if header in HEIGHT:
                    expected.append("#define GRAPHICS_DEFAULT_HEIGHT 256")
                if header in CLUT:
                    expected.append(f"#define MODEL_VARIANT{header}_CLUT_X 640")
                self.assertEqual(text[:len(expected)], expected)
                self.assertEqual(text[-1], f'#include "../{body[len("src/overlays/"):]}"')
                renames = text[len(expected):-1]
                if path.endswith("_slot1.c"):
                    north_american = (ROOT / body.replace(".c", "_slot1.c")).read_text().splitlines()
                    self.assertEqual(renames, [l for l in north_american if l.startswith("#define ")])
                else:
                    self.assertEqual(renames, [])
                source = (ROOT / body).read_text()
                if header in CLUT:
                    self.assertIn(f"#ifndef MODEL_VARIANT{header}_CLUT_X\n#define MODEL_VARIANT{header}_CLUT_X 512\n#endif\n",
                                  source)
                    self.assertNotIn("0x3D20", source)
                if header in HEIGHT:
                    self.assertIn('#include "../../game/graphics_constants.h"', source)
                    self.assertNotIn(", 240", source)

    def test_bindings_are_french_resident_functions(self):
        with (FRENCH / "functions.csv").open() as handle:
            resident = {int(r["address"], 0) for r in csv.DictReader(handle)}
        for link in sorted({self.modules[row["module"]]["linker_symbols"] for row in self.rows}):
            bindings = re.findall(r"^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);", (ROOT / link).read_text(), re.M)
            self.assertTrue(bindings, link)
            for name, address in bindings:
                self.assertIn(int(address, 0), resident, (link, name))

    def test_built_images_are_exact(self):
        for row in self.rows:
            name = row["module"]
            binary = ROOT / "tmp/overlays" / name / f"build/{name}.bin"
            if not binary.exists():
                self.skipTest("Build French overlays before checking linked images")
            with self.subTest(module=name):
                self.assertEqual(hashlib.sha256(binary.read_bytes()).hexdigest(), self.modules[name]["sha256"])


if __name__ == "__main__":
    unittest.main()
