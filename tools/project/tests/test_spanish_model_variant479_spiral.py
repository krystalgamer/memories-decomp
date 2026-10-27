import csv
import hashlib
import json
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments

SOURCE = "src/overlays/spanish_model_variant/variant479_spiral"
IMAGES = [(region, stem) for region in ("sles_03948", "sles_03951")
          for stem in ("model_variant_379_stage7_slot0", "model_variant_379_stage8_slot1")]


class SpanishModelVariant479SpiralTests(unittest.TestCase):
    def test_french_and_spanish_images_share_the_spiral_source(self):
        for region, stem in IMAGES:
            with self.subTest(region=region, module=stem):
                layout = ROOT / "config" / region / "overlays" / f"{stem}.yaml"
                base = 0x8017B000 if stem.endswith("slot1") else 0x8013B000
                source = SOURCE + ("_slot1" if base == 0x8017B000 else "") + ".c"
                functions = json.loads(layout.with_name(f"{stem}_matching_c.json").read_text())["functions"]
                self.assertIn({"address": f"0x{base + 0x14F8:X}", "profile": "gcc_2_8_1_g0_split",
                               "size": "0xA70", "source": source}, functions)
                self.assertIn(source, [s["source"] for s in c_segments(ROOT, layout)])
                with layout.with_name(f"{stem}_functions.csv").open() as handle:
                    row = next(r for r in csv.DictReader(handle) if int(r["address"], 0) == base + 0x14F8)
                self.assertEqual((row["status"], row["size"]), ("matching_c", "0xA70"))

    def test_slot1_wrapper_only_renames_the_symbol(self):
        text = (ROOT / f"{SOURCE}_slot1.c").read_text().splitlines()
        self.assertEqual(text, ['#include "../../types.h"', "",
                                "#define func_8013C4F8 func_8017C4F8",
                                '#include "variant479_spiral.c"'])

    def test_attempt_ledger_ends_matched_for_all_four_images(self):
        with (ROOT / "notes/overlays/spanish-model-variant479-spiral-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertTrue(all(r["result"] == "mismatch" for r in rows[:-4]))
        body = hashlib.sha256((ROOT / f"{SOURCE}.c").read_bytes()).hexdigest()
        wrapper = hashlib.sha256((ROOT / f"{SOURCE}_slot1.c").read_bytes()).hexdigest()
        for row, (region, stem) in zip(rows[-4:], IMAGES):
            prefix = "french_" if region == "sles_03948" else "spanish_"
            slot1 = stem.endswith("slot1")
            self.assertEqual((row["module"], row["slot"], row["result"], row["instruction_bytes"],
                              row["different_words"]),
                             (prefix + stem, "1" if slot1 else "0", "matched", "2672", "0"))
            self.assertEqual((row["fingerprint"], row["dependency_fingerprint"]),
                             (wrapper, body) if slot1 else (body, ""))


if __name__ == "__main__":
    unittest.main()
