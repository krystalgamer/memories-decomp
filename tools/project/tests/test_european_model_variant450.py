from __future__ import annotations

import csv
import hashlib
import json
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import overlay_extract
import progress


class EuropeanModelVariant450Tests(unittest.TestCase):
    def test_two_images_and_four_c_owners_per_image(self) -> None:
        counts = progress.load_european_overlay_inventories(ROOT)
        _, modules = overlay_extract.load_manifest(ROOT, "europe")
        selected = [m for m in modules if "model_variant_174_" in m["name"]]
        self.assertEqual(len(selected), 2)
        expected = {
            48224: "8f1c82c9f1bf89dc89021c300d5d8dde429676f2d60018fb192240f39f5daab8",
            48234: "a00ce80c35979acbd237737a3f32559c55d30b0c87255b7b154869765ad6e043",
        }
        for module in selected:
            self.assertEqual(module["archive"], "game/europe/DATA/MODEL.MRG")
            self.assertEqual(module["sha256"], expected[module["sector_offset"]])
            name = Path(module["layout"]).stem
            self.assertEqual(
                (
                    counts[name]["function_count"],
                    counts[name]["matching_c_function_count"],
                    counts[name]["matching_c_bytes"],
                ),
                (7, 4, 6720),
            )
            mapping = json.loads(
                (ROOT / module["layout"]).with_name(name + "_matching_c.json").read_text()
            )
            self.assertEqual(
                [row["size"] for row in mapping["functions"]],
                ["0x9BC", "0x738", "0x5C8", "0x384"],
            )

    def test_terminal_attempts_use_the_selected_sources(self) -> None:
        with (
            ROOT / "notes/overlays/european-model-variant450-attempts.csv"
        ).open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 8)
        sources = {
            ("0xDD8", "0"): "src/overlays/french_model_variant/variant450_ribbons.c",
            ("0xDD8", "1"): "src/overlays/french_model_variant/variant450_ribbons_slot1.c",
            ("0x1794", "0"): "src/overlays/french_model_variant/variant450_bands.c",
            ("0x1794", "1"): "src/overlays/french_model_variant/variant450_bands_slot1.c",
            ("0x27C0", "0"): "src/overlays/spanish_model_variant/variant450_quads.c",
            ("0x27C0", "1"): "src/overlays/spanish_model_variant/variant450_quads_slot1.c",
            ("0x2D88", "0"): "src/overlays/spanish_model_variant/variant450_lines.c",
            ("0x2D88", "1"): "src/overlays/spanish_model_variant/variant450_lines_slot1.c",
        }
        for row in attempts:
            source = ROOT / sources[(row["function_offset"], row["slot"])]
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["profile"], row["result"], row["different_words"]),
                             ("gcc_2_8_1_g0_split", "matched", "0"))

    def test_all_external_call_bindings_are_real_resident_starts(self) -> None:
        with (ROOT / "config/sles_03947/functions.csv").open() as handle:
            starts = {int(row["address"], 0) for row in csv.DictReader(handle)}
        bindings = ROOT / "config/sles_03947/overlays/model_variant450_linker_symbols.txt"
        addresses = {
            int(line.split(" = ", 1)[1].rstrip(";"), 0)
            for line in bindings.read_text().splitlines()
        }
        self.assertEqual(len(addresses), 35)
        self.assertTrue(addresses <= starts)


if __name__ == "__main__":
    unittest.main()
