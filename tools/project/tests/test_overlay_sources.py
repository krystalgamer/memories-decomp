from __future__ import annotations

import contextlib
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import overlay_build
import overlay_extract
import overlay_sources
from overlay_sources import OverlaySourceError
from workspace import WorkspaceError


class OverlaySourceTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="overlay-sources-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name) / "workspace"
        self.config = "config/slus_01411/overlays"
        self.text = "src/overlays/example/runtime.c"
        self.data = "src/overlays/example/header.c"
        self.write(self.text, "void Example_Run(void) {}\n")
        self.write(self.data, "int D_80168000 = 19;\n")
        self.write_json(
            "config/slus_01411/compiler_profiles.json",
            {"schema": 1, "profiles": {"g0": {}, "g8": {}}},
        )
        self.manifest("matching_c", [self.entry(self.text)])
        self.manifest("data_c", [self.entry(self.data)])
        self.layout = self.write(
            f"{self.config}/example.yaml",
            "segments:\n"
            "  - name: header\n"
            "    type: code\n"
            "    subsegments:\n"
            "      - [0, .data, overlays/example/header]\n"
            "  - name: module\n"
            "    type: code\n"
            "    subsegments:\n"
            "      - [4, c, overlays/example/runtime]\n"
            "      - [16, data, overlays/example/raw]\n",
        )

    def write(self, relative: str, text: str) -> Path:
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
        return path

    def write_json(self, relative: str, value: object) -> Path:
        return self.write(relative, json.dumps(value) + "\n")

    def entry(self, source: str, profile: str = "g0") -> dict[str, str]:
        return {"source": source, "profile": profile}

    def manifest(self, kind: str, entries: list[dict[str, str]]) -> Path:
        return self.write_json(
            f"{self.config}/example_{kind}.json",
            {"schema": 1, "functions" if kind == "matching_c" else "units": entries},
        )

    def collect(self) -> list[dict[str, str]]:
        return overlay_sources.c_segments(self.root, self.layout)

    def append_section(self, kind: str, source: str) -> None:
        with self.layout.open("a", encoding="utf-8") as handle:
            handle.write(f"      - [32, {kind}, {source}]\n")

    def test_data_and_text_are_built_in_layout_order(self) -> None:
        self.assertEqual(
            self.collect(),
            [
                {**self.entry(self.data), "object": "src/overlays/example/header.o"},
                {**self.entry(self.text), "object": "src/overlays/example/runtime.o"},
            ],
        )

    def test_every_owned_section_type_compiles_data_once(self) -> None:
        for kind in (".rodata", ".sdata", ".sbss", ".bss"):
            self.append_section(kind, "overlays/example/header")
        self.assertEqual(len(self.collect()), 2)

    def test_text_owned_sections_do_not_need_a_data_entry(self) -> None:
        for kind in (".rodata", ".data", ".sdata", ".sbss", ".bss"):
            self.append_section(kind, "overlays/example/runtime")
        self.assertEqual(len(self.collect()), 2)

    def test_grouped_functions_compile_once(self) -> None:
        self.manifest("matching_c", [self.entry(self.text), self.entry(self.text)])
        self.assertEqual(len(self.collect()), 2)

    def test_conflicting_grouped_profiles_are_rejected(self) -> None:
        self.manifest(
            "matching_c", [self.entry(self.text), self.entry(self.text, "g8")]
        )
        with self.assertRaisesRegex(OverlaySourceError, "conflicting profiles"):
            self.collect()

    def test_duplicate_data_entry_is_rejected(self) -> None:
        self.manifest("data_c", [self.entry(self.data), self.entry(self.data)])
        with self.assertRaisesRegex(OverlaySourceError, "duplicate data unit"):
            self.collect()

    def test_duplicate_section_is_rejected(self) -> None:
        self.append_section(".data", "overlays/example/header")
        with self.assertRaisesRegex(OverlaySourceError, "duplicate .data subsegment"):
            self.collect()

    def test_text_unit_cannot_be_declared_again_as_data(self) -> None:
        self.manifest("data_c", [self.entry(self.data), self.entry(self.text)])
        with self.assertRaisesRegex(OverlaySourceError, "must not also"):
            self.collect()

    def test_missing_and_unmapped_entries_are_rejected(self) -> None:
        for kind, source in (("matching_c", self.text), ("data_c", self.data)):
            with self.subTest(kind=kind, error="missing"):
                self.manifest(kind, [])
                with self.assertRaisesRegex(OverlaySourceError, "missing units: src/"):
                    self.collect()
                self.manifest(kind, [self.entry(source)])
            with self.subTest(kind=kind, error="unmapped"):
                other = "src/overlays/example/unused.c"
                self.write(other, "/* fixture */\n")
                self.manifest(kind, [self.entry(source), self.entry(other)])
                with self.assertRaisesRegex(OverlaySourceError, "unmapped units: src/"):
                    self.collect()
                self.manifest(kind, [self.entry(source)])

    def test_no_data_manifest_is_required_for_assembly_data(self) -> None:
        (self.root / f"{self.config}/example_data_c.json").unlink()
        self.layout.write_text(
            self.layout.read_text().replace("0, .data,", "0, data,")
        )
        self.assertEqual([unit["source"] for unit in self.collect()], [self.text])

    def test_absent_data_manifest_does_not_hide_an_owned_section(self) -> None:
        (self.root / f"{self.config}/example_data_c.json").unlink()
        with self.assertRaisesRegex(OverlaySourceError, "data_c.json wiring mismatch"):
            self.collect()

    def test_absent_matching_manifest_does_not_hide_c(self) -> None:
        (self.root / f"{self.config}/example_matching_c.json").unlink()
        with self.assertRaisesRegex(OverlaySourceError, "matching_c.json wiring mismatch"):
            self.collect()

    def test_bad_manifests_are_rejected(self) -> None:
        for value in ([], None, {"schema": 2, "units": []}, {"schema": 1, "units": {}}):
            with self.subTest(value=value):
                self.write_json(f"{self.config}/example_data_c.json", value)
                with self.assertRaisesRegex(OverlaySourceError, "expected schema 1"):
                    self.collect()

    def test_non_object_entry_is_rejected(self) -> None:
        self.write_json(
            f"{self.config}/example_data_c.json", {"schema": 1, "units": [None]}
        )
        with self.assertRaisesRegex(OverlaySourceError, "entries must be objects"):
            self.collect()

    def test_unknown_profile_is_rejected(self) -> None:
        self.manifest("data_c", [self.entry(self.data, "typo")])
        with self.assertRaisesRegex(OverlaySourceError, "unknown profile"):
            self.collect()

    def test_missing_source_is_rejected(self) -> None:
        (self.root / self.data).unlink()
        with self.assertRaisesRegex(WorkspaceError, "path does not exist"):
            self.collect()

    def test_noncanonical_and_nonoverlay_paths_are_rejected(self) -> None:
        for source in (
            "src/game/header.c",
            "/src/overlays/header.c",
            "src/overlays/../../header.c",
            "src/overlays//example/header.c",
            "src/overlays/example/./header.c",
            "src/overlays/example/header.h",
        ):
            with self.subTest(source=source):
                self.manifest("data_c", [self.entry(source)])
                with self.assertRaisesRegex(OverlaySourceError, "canonical C path"):
                    self.collect()

    def test_symlink_escape_is_rejected(self) -> None:
        outside = self.root.parent / "outside.c"
        outside.write_text("/* still inside repository tmp */\n")
        source = self.root / self.data
        source.unlink()
        source.symlink_to(outside)
        with self.assertRaisesRegex(WorkspaceError, "path leaves the workspace"):
            self.collect()

    def test_manifest_symlink_escape_is_rejected(self) -> None:
        outside = self.root.parent / "outside.json"
        outside.write_text("{}\n")
        manifest = self.root / f"{self.config}/example_data_c.json"
        manifest.unlink()
        manifest.symlink_to(outside)
        with self.assertRaisesRegex(WorkspaceError, "path leaves the workspace"):
            self.collect()

    def test_shared_data_is_resolved_in_each_overlay(self) -> None:
        second = self.write(f"{self.config}/second.yaml", self.layout.read_text())
        for kind in ("matching_c", "data_c"):
            self.write(
                f"{self.config}/second_{kind}.json",
                (self.root / f"{self.config}/example_{kind}.json").read_text(),
            )
        self.assertEqual(self.collect(), overlay_sources.c_segments(self.root, second))

    def test_metadata_checks_data_only_wiring_without_retail_inputs(self) -> None:
        with contextlib.redirect_stdout(io.StringIO()):
            overlay_extract.verify_sources_wired(self.root)
        self.manifest("data_c", [])
        with self.assertRaisesRegex(overlay_extract.OverlayError, "missing units"):
            overlay_extract.verify_sources_wired(self.root)

    def test_metadata_rejects_orphan_sources(self) -> None:
        self.write("src/overlays/example/unwired.c", "/* fixture */\n")
        with self.assertRaisesRegex(overlay_extract.OverlayError, "not wired"):
            overlay_extract.verify_sources_wired(self.root)

    def test_compiler_receives_data_object_paths_and_profiles(self) -> None:
        module_root = self.root / "tmp/overlays/example"
        with (
            patch.object(overlay_build, "tool", return_value=Path("as")),
            patch.object(overlay_build, "load_compiler_profiles", return_value={"g0": {}}),
            patch.object(overlay_build, "compile_c", return_value=Path("unit.o")) as compile_c,
        ):
            overlay_build.compile_sources(self.root, module_root, self.collect())
        self.assertEqual(compile_c.call_count, 2)
        first = compile_c.call_args_list[0]
        self.assertEqual(first.args[2]["source"], self.data)
        self.assertEqual(first.args[2]["profile"], "g0")
        self.assertEqual(first.kwargs["object_directory"], "tmp/overlays/example/build")
        self.assertEqual(first.kwargs["asm_directory"], "tmp/overlays/example/build/asm")

    def build_fixture(self) -> dict[str, str]:
        self.write("tmp/overlays/example/module.bin", "fixture")
        self.write("tools/environments/python/bin/splat", "")
        for name in ("example.ld", "undefined_funcs_auto.txt", "undefined_syms_auto.txt"):
            self.write(f"tmp/overlays/example/{name}", "")
        return {
            "name": "example",
            "layout": f"{self.config}/example.yaml",
            "output": "tmp/overlays/example/module.bin",
        }

    def test_build_compiles_data_even_without_generated_assembly(self) -> None:
        module = self.build_fixture()
        with (
            patch.object(overlay_build, "run"),
            patch.object(overlay_build, "tool", return_value=Path("tool")),
            patch.object(
                overlay_build, "compile_sources", return_value=[Path("unit.o")]
            ) as compile_sources,
            contextlib.redirect_stdout(io.StringIO()),
        ):
            overlay_build.build_module(self.root, module)
        self.assertEqual(compile_sources.call_args.args[2], self.collect())

    def test_build_rejects_an_empty_object_set(self) -> None:
        module = self.build_fixture()
        with (
            patch.object(overlay_build, "run"),
            patch.object(overlay_build, "compile_sources", return_value=[]),
        ):
            with self.assertRaisesRegex(overlay_build.OverlayBuildError, "no C or"):
                overlay_build.build_module(self.root, module)

    def test_build_checks_wiring_before_splat_or_compilation(self) -> None:
        module = self.build_fixture()
        self.manifest("data_c", [])
        with patch.object(overlay_build, "run") as run:
            with self.assertRaisesRegex(OverlaySourceError, "missing units"):
                overlay_build.build_module(self.root, module)
        run.assert_not_called()


if __name__ == "__main__":
    unittest.main()
