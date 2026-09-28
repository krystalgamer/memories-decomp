from __future__ import annotations

import json
from pathlib import Path
import sys
import unittest
from unittest import mock

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import overlay_extract
import progress


class GermanOverlayTests(unittest.TestCase):
    def test_all_instances_reuse_verified_sources(self) -> None:
        counts = progress.load_german_overlay_inventories(REPOSITORY)
        self.assertEqual(len(counts), 6)
        self.assertEqual(sum(row["matching_c_function_count"] for row in counts.values()), 124)
        self.assertEqual(sum(row["matching_c_bytes"] for row in counts.values()), 55852)
        _, spanish = overlay_extract.load_manifest(REPOSITORY, "spain")
        sector_size, german = overlay_extract.load_manifest(REPOSITORY, "germany")
        self.assertEqual(sector_size, 2048)
        self.assertEqual(len(german), 6)
        for original, current in zip(spanish, german):
            for field in ("sector_offset", "sector_count", "load_address", "sha256"):
                self.assertEqual(current[field], original[field])
            self.assertTrue(current["archive"].startswith("game/germany/"))
            name = current["name"].removeprefix("german_")
            old = REPOSITORY / f"config/sles_03951/overlays/{name}_matching_c.json"
            new = REPOSITORY / f"config/sles_03949/overlays/{name}_matching_c.json"
            self.assertEqual(json.loads(new.read_text()), json.loads(old.read_text()))

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

    def test_ci_does_not_depend_on_unmerged_resident_build(self) -> None:
        workflow = (REPOSITORY / ".github/workflows/german-overlay-build.yml").read_text()
        for secret in ("GER_SU_MRG", "GER_WA_MRG"):
            self.assertIn(f"secrets.YGOFM_{secret}_URL", workflow)
        for command in ("german-match-overlays", "german-verify-overlays"):
            self.assertIn(f"make {command}\n", workflow)
        self.assertNotIn("make german-match\n", workflow)
        self.assertNotIn("YGOFM_SLES_03949_URL", workflow)
        self.assertNotIn("YGOFM_ESP_", workflow)


if __name__ == "__main__":
    unittest.main()
