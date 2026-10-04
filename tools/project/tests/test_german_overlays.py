from __future__ import annotations

import csv
import contextlib
import hashlib
import io
import json
from pathlib import Path
import sys
import unittest
from unittest import mock

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import overlay_extract
import progress
from function_inventory import load_inventory


class GermanOverlayTests(unittest.TestCase):
    def test_report_json_and_renderer_receive_full_german_metrics(self) -> None:
        metrics = {
            "target": "SLES-03949",
            "target_sha256": "german-hash",
            **progress.summarize_functions(
                load_inventory(REPOSITORY / "config/sles_03949/functions.csv"),
                progress.load_text_size(REPOSITORY, "config/sles_03949"),
            ),
            "overlays": progress.load_german_overlay_inventories(REPOSITORY),
        }
        calculations = (
            "calculate", "calculate_japanese", "calculate_european",
            "calculate_spanish", "calculate_italian", "calculate_german",
        )
        with (
            mock.patch.multiple(
                progress,
                **{name: mock.Mock(return_value=metrics) for name in calculations},
            ),
            mock.patch.object(progress, "parse_arguments", return_value=mock.Mock(check=True)),
            mock.patch.object(progress, "atomic_write_json") as write,
            mock.patch.object(progress, "sync_readme", return_value="current") as sync,
            contextlib.redirect_stdout(io.StringIO()),
        ):
            self.assertEqual(progress.main(), 0)
        self.assertEqual(write.call_args.args[1]["german"], metrics)
        self.assertEqual(sync.call_args.args[-1], metrics)
        self.assertEqual(metrics["matching_c_function_count"], 1140)
        self.assertEqual(len(metrics["overlays"]), 8)

    def test_accepted_resident_inventory_is_complete_and_unchanged(self) -> None:
        spanish = load_inventory(REPOSITORY / "config/sles_03951/functions.csv")
        german = load_inventory(REPOSITORY / "config/sles_03949/functions.csv")
        self.assertEqual(
            [(f.address, f.size, f.status, f.module) for f in german],
            [(f.address, f.size, f.status, f.module) for f in spanish],
        )
        metrics = progress.summarize_functions(
            german, progress.load_text_size(REPOSITORY, "config/sles_03949")
        )
        self.assertEqual(metrics["matching_c_function_count"], 1140)
        self.assertEqual(metrics["matching_c_bytes"], 357700)
        self.assertEqual(metrics["assembly_function_count"], 0)
        self.assertEqual(metrics["handwritten_function_count"], 61)
        self.assertEqual(metrics["sdk_function_count"], 623)
        progress.validate_regional_inventory(
            REPOSITORY, german,
            progress.load_image_map(REPOSITORY, "config/sles_03949"),
            config="config/sles_03949", region_name="German",
        )

    def test_all_instances_reuse_verified_sources(self) -> None:
        counts = progress.load_german_overlay_inventories(REPOSITORY)
        self.assertEqual(len(counts), 8)
        self.assertEqual(sum(row["matching_c_function_count"] for row in counts.values()), 132)
        self.assertEqual(sum(row["matching_c_bytes"] for row in counts.values()), 69292)
        _, spanish = overlay_extract.load_manifest(REPOSITORY, "spain")
        sector_size, german = overlay_extract.load_manifest(REPOSITORY, "germany")
        self.assertEqual(sector_size, 2048)
        self.assertEqual(len(german), 8)
        spanish_by_name = {
            module["name"].removeprefix("spanish_"): module for module in spanish
        }
        for current in german:
            name = current["name"].removeprefix("german_")
            self.assertIn(name, spanish_by_name)
            original = spanish_by_name[name]
            for field in ("sector_offset", "sector_count", "load_address", "sha256"):
                self.assertEqual(current[field], original[field])
            self.assertTrue(current["archive"].startswith("game/germany/"))
            old = REPOSITORY / f"config/sles_03951/overlays/{name}_matching_c.json"
            new = REPOSITORY / f"config/sles_03949/overlays/{name}_matching_c.json"
            current_mapping = json.loads(new.read_text())
            if name.startswith("model_variant_174_"):
                self.assertEqual(len(current_mapping["functions"]), 4)
                self.assertEqual(
                    [row["source"] for row in current_mapping["functions"]],
                    [
                        "src/overlays/french_model_variant/variant450_ribbons"
                        + ("_slot1.c" if name.endswith("slot1") else ".c"),
                        "src/overlays/french_model_variant/variant450_bands"
                        + ("_slot1.c" if name.endswith("slot1") else ".c"),
                        "src/overlays/spanish_model_variant/variant450_quads"
                        + ("_slot1.c" if name.endswith("slot1") else ".c"),
                        "src/overlays/spanish_model_variant/variant450_lines"
                        + ("_slot1.c" if name.endswith("slot1") else ".c"),
                    ],
                )
            else:
                self.assertEqual(current_mapping, json.loads(old.read_text()))

    def test_distinct_german_archive_hash_is_not_copied_from_spanish(self) -> None:
        _, modules = overlay_extract.load_manifest(REPOSITORY, "germany")
        wa = [module for module in modules if module["archive"].endswith("/WA_MRG.MRG")]
        self.assertEqual(len(wa), 5)
        for module in wa:
            self.assertEqual(
                module["archive_sha256"],
                "fbe294274a0c88fd70f1ea94a85ef6b2a6b5e4e9b5fd0c98eabdf687614d6fc6",
            )

    def test_inventory_disagreement_is_rejected(self) -> None:
        with (
            mock.patch.object(progress, "load_matching_ranges", return_value=[]),
            self.assertRaisesRegex(
                progress.ProgressError, "does not agree with the matching manifest"
            ),
        ):
            progress.load_german_overlay_inventories(REPOSITORY)

    def test_model450_instances_and_terminal_attempts(self) -> None:
        with (
            REPOSITORY / "notes/overlays/german-model-variant450-instances.csv"
        ).open() as handle:
            instances = list(csv.DictReader(handle))
        self.assertEqual(
            [(row["module"], row["sector_offset"], row["sha256"]) for row in instances],
            [
                (
                    "german_model_variant_174_stage9_slot0",
                    "48224",
                    "b2c0697e759ecfe1e5d746ca7ad4bc29bcd3719c05893056acb877274dc6eac8",
                ),
                (
                    "german_model_variant_174_stage10_slot1",
                    "48234",
                    "770befcc07901cfa9da9573421c5713062488ea83f6df218a25fe302d5c091b7",
                ),
            ],
        )
        with (
            REPOSITORY / "notes/overlays/german-model-variant450-attempts.csv"
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
            source = REPOSITORY / sources[(row["function_offset"], row["slot"])]
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["profile"], row["result"], row["different_words"]),
                             ("gcc_2_8_1_g0_split", "matched", "0"))

    def test_ci_does_not_depend_on_unmerged_resident_build(self) -> None:
        workflow = (REPOSITORY / ".github/workflows/german-overlay-build.yml").read_text()
        self.assertIn("uses: ./.github/actions/retail-inputs", workflow)
        self.assertIn("region: germany", workflow)
        self.assertIn('archives-only: "true"', workflow)
        for command in ("german-match-overlays", "german-verify-overlays"):
            self.assertIn(f"make {command}\n", workflow)
        self.assertNotIn("make german-match\n", workflow)
        self.assertNotIn("YGOFM_SLES_03949_URL", workflow)
        self.assertNotIn("YGOFM_ESP_", workflow)


if __name__ == "__main__":
    unittest.main()
