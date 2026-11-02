from __future__ import annotations

from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import overlay_build
import overlay_objects


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


if __name__ == "__main__":
    unittest.main()
