from __future__ import annotations

import contextlib
import io
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import overlay_build
import overlay_objects

REAL_PIPELINE_FINGERPRINT = overlay_objects.ObjectStore.pipeline_fingerprint


class OverlayObjectStoreTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="overlay-objects-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name).resolve()
        for name in ("a", "b"):
            path = self.root / f"src/{name}.c"
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(f"int {name};\n")
        self.preprocessed = {"src/a.c": b"a", "src/b.c": b"b"}
        self.compiled: list[str] = []
        profiles = {"g0": {"compiler_flags": []}, "g8": {"compiler_flags": ["-G8"]}}
        for target, value in (
            ("load_compiler_profiles", profiles),
            ("tool", Path("as")),
        ):
            patcher = patch.object(overlay_objects, target, return_value=value)
            patcher.start()
            self.addCleanup(patcher.stop)
        for target, replacement in (
            ("pipeline_fingerprint", lambda store: "pipeline"),
            ("profile_fingerprint", lambda store, name: name),
            ("_preprocess", lambda store, source, profile: self.preprocessed[
                str(source.relative_to(self.root))
            ] + b"".join(f.encode() for f in profile["compiler_flags"])),
        ):
            patcher = patch.object(overlay_objects.ObjectStore, target, replacement)
            patcher.start()
            self.addCleanup(patcher.stop)
        patcher = patch.object(overlay_objects, "compile_c", self.fake_compile)
        patcher.start()
        self.addCleanup(patcher.stop)

    def fake_compile(self, root, assembler, segment, profiles, *, object_directory, asm_directory):
        self.compiled.append(f"{segment['source']}:{segment['profile']}")
        output = root / object_directory / segment["object"]
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(f"{segment['source']}:{segment['profile']}")
        return output

    @staticmethod
    def segment(source: str, profile: str = "g0") -> dict[str, str]:
        return {"kind": "text", "source": source, "profile": profile,
                "object": source.replace(".c", ".o")}

    def test_each_distinct_unit_compiles_once_across_modules_and_runs(self) -> None:
        segments = [self.segment("src/a.c"), self.segment("src/a.c"),
                    self.segment("src/a.c", "g8"), self.segment("src/b.c")]
        store = overlay_objects.ObjectStore(self.root)
        self.assertEqual(store.ensure(segments, 4), {"units": 3, "compiled": 3})
        self.assertEqual(sorted(self.compiled),
                         ["src/a.c:g0", "src/a.c:g8", "src/b.c:g0"])
        again = overlay_objects.ObjectStore(self.root)
        self.assertEqual(again.ensure(segments, 4), {"units": 3, "compiled": 0})
        copy = again.materialize(segments[2], self.root / "module/build/a.o")
        self.assertEqual(copy.read_text(), "src/a.c:g8")
        work = self.root / overlay_objects.STORE_DIRECTORY / "work"
        self.assertEqual(list(work.iterdir()), [])

    def test_preprocessed_change_such_as_a_header_edit_recompiles(self) -> None:
        segment = self.segment("src/a.c")
        overlay_objects.ObjectStore(self.root).ensure([segment], 1)
        self.preprocessed["src/a.c"] = b"a with edited header"
        store = overlay_objects.ObjectStore(self.root)
        self.assertEqual(store.ensure([segment], 1)["compiled"], 1)

    def test_compile_driver_changes_recompile_units(self) -> None:
        for relative in overlay_objects.PIPELINE_FILES:
            path = self.root / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(relative)
        for relative in overlay_objects.PIPELINE_DIRECTORIES:
            (self.root / relative).mkdir(parents=True, exist_ok=True)
            (self.root / relative / "__init__.py").write_text("")
        binary = self.root / "usr/bin/mips-linux-gnu-as"
        binary.parent.mkdir(parents=True)
        binary.write_bytes(b"assembler 1")
        wrapper = self.root / "wrapper"
        wrapper.write_text(f'#!/bin/sh\nset -eu\nexec {binary} "$@"\n')
        segments = [self.segment("src/a.c")]

        def ensure() -> dict[str, int]:
            with patch.object(overlay_objects, "tool", return_value=wrapper), \
                    patch.object(overlay_objects.ObjectStore, "pipeline_fingerprint",
                                 REAL_PIPELINE_FINGERPRINT):
                return overlay_objects.ObjectStore(self.root).ensure(segments, 1)

        self.assertEqual(ensure()["compiled"], 1)
        self.assertEqual(ensure()["compiled"], 0)
        (self.root / "tools/vendor/maspsx/maspsx.py").write_text("changed driver")
        self.assertEqual(ensure()["compiled"], 1)
        binary.write_bytes(b"assembler 2")
        self.assertEqual(ensure()["compiled"], 1)
        self.assertEqual(ensure()["compiled"], 0)

    def test_prune_keeps_only_requested_objects(self) -> None:
        store = overlay_objects.ObjectStore(self.root)
        a, b = self.segment("src/a.c"), self.segment("src/b.c")
        store.ensure([a, b], 2)
        self.assertEqual(store.prune([store.key(a)]), 1)
        self.assertTrue(store.path(store.key(a)).is_file())
        self.assertFalse(store.path(store.key(b)).exists())

    def test_active_store_replaces_per_module_compilation(self) -> None:
        store = overlay_objects.ObjectStore(self.root)
        segment = self.segment("src/a.c")
        store.ensure([segment], 1)
        module_root = self.root / "tmp/overlays/example"
        with patch.object(overlay_build, "_OBJECT_STORE", store), \
                patch.object(overlay_build, "compile_c") as compile_c:
            objects = overlay_build.compile_sources(self.root, module_root, [segment])
        compile_c.assert_not_called()
        self.assertEqual(objects, [module_root / "build/src/a.o"])
        self.assertEqual(objects[0].read_text(), "src/a.c:g0")


class OverlayFingerprintTests(unittest.TestCase):
    def setUp(self) -> None:
        temporary = tempfile.TemporaryDirectory(
            prefix="overlay-fingerprint-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name).resolve()
        self.files = {
            "config/r/a.yaml": "symbol_addrs_path:\n  - config/r/a_symbols.txt\n",
            "config/r/a_symbols.txt": "f = 0x80100000;\n",
            "config/r/b.yaml": "segments: []\n",
        }
        for relative, text in self.files.items():
            (self.root / relative).parent.mkdir(parents=True, exist_ok=True)
            (self.root / relative).write_text(text)
        self.modules = [
            {"name": "a", "layout": "config/r/a.yaml", "sha256": "1"},
            {"name": "b", "layout": "config/r/b.yaml", "sha256": "2"},
        ]
        self.object_key = "k1"
        store = patch.object(overlay_build, "ObjectStore")
        store.start().return_value.key.side_effect = lambda segment: self.object_key
        self.addCleanup(store.stop)
        for target, value in (
            ("pipeline_fingerprint", lambda root: "tools"),
            ("c_segments", lambda root, layout: [
                {"kind": "text", "source": "src/x.c", "profile": "g0", "object": "x.o"}
            ] if layout.name == "a.yaml" else []),
        ):
            patcher = patch.object(overlay_build, target, value)
            patcher.start()
            self.addCleanup(patcher.stop)

    def fingerprints(self) -> dict[str, str]:
        return overlay_build.module_fingerprints(self.root, self.modules, 1)

    def test_only_modules_with_changed_inputs_get_new_fingerprints(self) -> None:
        before = self.fingerprints()
        (self.root / "config/r/a_symbols.txt").write_text("f = 0x80100004;\n")
        after = self.fingerprints()
        self.assertNotEqual(before["a"], after["a"])
        self.assertEqual(before["b"], after["b"])
        self.object_key = "k2"
        self.assertNotEqual(after["a"], self.fingerprints()["a"])
        self.assertEqual(before["b"], self.fingerprints()["b"])

    def test_skip_verified_checks_only_unrecorded_modules(self) -> None:
        record = self.root / "verified.txt"
        with contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(
                overlay_build.unverified_modules(self.root, self.modules, record, 1),
                self.modules,
            )
            record.write_text(self.fingerprints()["b"] + "\n")
            self.assertEqual(
                overlay_build.unverified_modules(self.root, self.modules, record, 1),
                [self.modules[0]],
            )

    def test_record_is_written_only_when_every_module_matches(self) -> None:
        output = self.root / "verified.txt"
        with patch.object(
            overlay_build, "verify_module",
            side_effect=overlay_build.OverlayBuildError("mismatch"),
        ):
            with self.assertRaises(overlay_build.OverlayBuildError):
                overlay_build.write_record(self.root, self.modules, output)
        self.assertFalse(output.exists())
        with patch.object(overlay_build, "verify_module"), \
                contextlib.redirect_stdout(io.StringIO()):
            overlay_build.write_record(self.root, self.modules, output)
        self.assertEqual(sorted(output.read_text().split()),
                         sorted(self.fingerprints().values()))


if __name__ == "__main__":
    unittest.main()
