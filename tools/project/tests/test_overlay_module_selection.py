from __future__ import annotations

import contextlib
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import overlay_build as build
import overlay_extract as extract


class OverlayModuleSelectionTests(unittest.TestCase):
    def setUp(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        self.bank = {
            "name": "japanese_duel_effects",
            "layout": "config/duel_effects.yaml",
            "duplicate_sector_offsets": [6192, 6431],
        }
        self.other = {"name": "japanese_model_image", "layout": "config/model.yaml"}
        self.modules = [self.other, self.bank]

    def test_default_selection_returns_the_original_complete_list(self):
        self.assertIs(extract.select_modules(self.modules, None), self.modules)

    def test_exact_selection_preserves_the_complete_module_record(self):
        selected = extract.select_modules(self.modules, "japanese_duel_effects")
        self.assertEqual(selected, [self.bank])
        self.assertIs(selected[0], self.bank)
        self.assertEqual(selected[0]["duplicate_sector_offsets"], [6192, 6431])
        self.assertEqual(self.modules, [self.other, self.bank])

    def test_unknown_partial_glob_and_path_names_are_rejected(self):
        for name in ("missing", "", "duel_effects", "*duel_effects",
                     "../japanese_duel_effects", "european_duel_effects"):
            with self.subTest(name=name):
                with self.assertRaisesRegex(extract.OverlayError, "unknown overlay module"):
                    extract.select_modules(self.modules, name)

    def test_duplicate_exact_names_are_rejected(self):
        with self.assertRaisesRegex(extract.OverlayError, "duplicate overlay module"):
            extract.select_modules([self.bank, dict(self.bank)], self.bank["name"])

    def test_extraction_commands_dispatch_only_the_selected_module(self):
        for command in ("extract", "verify"):
            with self.subTest(command=command):
                argv = ["overlay_extract.py", command, "--region", "japan",
                        "--module", self.bank["name"]]
                with patch.object(sys, "argv", argv):
                    with patch.object(extract, "require_workspace_root", return_value=ROOT):
                        with patch.object(extract, "load_manifest", return_value=(2048, self.modules)) as loader:
                            with patch.object(extract, command) as action:
                                self.assertEqual(extract.main(), 0)
                                loader.assert_called_once_with(ROOT, "japan")
                                action.assert_called_once_with(ROOT, 2048, [self.bank])

    def test_default_extraction_still_dispatches_every_module(self):
        with patch.object(sys, "argv", ["overlay_extract.py", "extract"]):
            with patch.object(extract, "require_workspace_root", return_value=ROOT):
                with patch.object(extract, "load_manifest", return_value=(2048, self.modules)):
                    with patch.object(extract, "extract") as action:
                        self.assertEqual(extract.main(), 0)
                        self.assertIs(action.call_args.args[2], self.modules)

    def test_bad_extraction_selection_cannot_perform_io(self):
        argv = ["overlay_extract.py", "extract", "--module", "missing"]
        with patch.object(sys, "argv", argv):
            with patch.object(extract, "require_workspace_root", return_value=ROOT):
                with patch.object(extract, "load_manifest", return_value=(2048, self.modules)):
                    with patch.object(extract, "extract") as action:
                        with contextlib.redirect_stderr(io.StringIO()) as errors:
                            self.assertEqual(extract.main(), 1)
                        self.assertIn("unknown overlay module", errors.getvalue())
                        action.assert_not_called()

    def test_metadata_verification_cannot_silently_ignore_selection(self):
        argv = ["overlay_extract.py", "verify-metadata", "--module", self.bank["name"]]
        with patch.object(sys, "argv", argv):
            with patch.object(extract, "verify_metadata") as action:
                with contextlib.redirect_stderr(io.StringIO()) as errors:
                    self.assertEqual(extract.main(), 1)
                self.assertIn("--module cannot be used", errors.getvalue())
                action.assert_not_called()

    def test_build_commands_dispatch_only_the_selected_module(self):
        for command in ("build", "verify"):
            with self.subTest(command=command):
                argv = ["overlay_build.py", command, "--region", "japan",
                        "--module", self.bank["name"]]
                with patch.object(sys, "argv", argv):
                    with patch.object(build, "require_workspace_root", return_value=ROOT):
                        with patch.object(build, "load_modules", return_value=self.modules) as loader:
                            with patch.object(build, "job_count", return_value=4):
                                with patch.object(build, "build_modules") as builder:
                                    with patch.object(build, "verify_module") as verifier:
                                        self.assertEqual(build.main(), 0)
                                        loader.assert_called_once_with(ROOT, "japan")
                                        if command == "build":
                                            builder.assert_called_once_with(ROOT, [self.bank], 4)
                                            verifier.assert_not_called()
                                        else:
                                            verifier.assert_called_once_with(ROOT, self.bank)
                                            builder.assert_not_called()

    def test_default_build_still_dispatches_every_build_ready_module(self):
        with patch.object(sys, "argv", ["overlay_build.py", "build"]):
            with patch.object(build, "require_workspace_root", return_value=ROOT):
                with patch.object(build, "load_modules", return_value=self.modules):
                    with patch.object(build, "job_count", return_value=4):
                        with patch.object(build, "build_modules") as action:
                            self.assertEqual(build.main(), 0)
                            self.assertIs(action.call_args.args[1], self.modules)

    def test_bad_build_selection_cannot_start_workers(self):
        argv = ["overlay_build.py", "build", "--module", "missing"]
        with patch.object(sys, "argv", argv):
            with patch.object(build, "require_workspace_root", return_value=ROOT):
                with patch.object(build, "load_modules", return_value=self.modules):
                    with patch.object(build, "build_modules") as action:
                        with contextlib.redirect_stderr(io.StringIO()) as errors:
                            self.assertEqual(build.main(), 1)
                        self.assertIn("unknown overlay module", errors.getvalue())
                        action.assert_not_called()

    def test_unbuildable_module_cannot_turn_into_a_full_region_build(self):
        with tempfile.TemporaryDirectory(prefix="overlay-selection-", dir=ROOT / "tmp") as name:
            root = Path(name)
            path = root / "modules.json"
            path.write_text(json.dumps({
                "schema": 1,
                "modules": [self.bank, {"name": "unbuildable"}],
            }), encoding="utf-8")
            with patch.dict(build.OVERLAY_MANIFESTS, {"japan": "modules.json"}):
                ready = build.load_modules(root, "japan")
                self.assertEqual(ready, [self.bank])
                with self.assertRaisesRegex(extract.OverlayError, "unknown overlay module"):
                    extract.select_modules(ready, "unbuildable")


if __name__ == "__main__":
    unittest.main()
