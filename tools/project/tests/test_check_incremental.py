from __future__ import annotations

from pathlib import Path
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from check_incremental import IncrementalCheckError, changed_objects, edited_input, object_counts


class IncrementalCheckTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory(
            prefix="incremental-check-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.path = self.root / "input.c"
        self.path.write_bytes(b"original")

    def test_probe_changes_contents_without_relying_on_timestamps(self) -> None:
        before = self.path.stat()
        with edited_input(self.root, "input.c", b"modified"):
            self.assertEqual(self.path.read_bytes(), b"modified")
            self.assertEqual(self.path.stat().st_mtime_ns, before.st_mtime_ns)
        self.assertEqual(self.path.read_bytes(), b"original")
        self.assertEqual(self.path.stat().st_mtime_ns, before.st_mtime_ns)

    def test_probe_restores_input_after_failure(self) -> None:
        with self.assertRaisesRegex(RuntimeError, "failed build"):
            with edited_input(self.root, "input.c", b"modified"):
                raise RuntimeError("failed build")
        self.assertEqual(self.path.read_bytes(), b"original")

    def test_probe_does_not_overwrite_a_concurrent_change(self) -> None:
        with self.assertRaisesRegex(IncrementalCheckError, "changed concurrently"):
            with edited_input(self.root, "input.c", b"modified"):
                self.path.write_bytes(b"another writer")
        self.assertEqual(self.path.read_bytes(), b"another writer")
        backup = self.root / "tmp/incremental-check/backups/input.c"
        self.assertEqual(backup.read_bytes(), b"original")

    def test_probe_must_actually_change_the_input(self) -> None:
        with self.assertRaisesRegex(IncrementalCheckError, "does not change"):
            with edited_input(self.root, "input.c", b"original"):
                self.fail("unchanged probe ran")

    def test_object_replacement_measurement(self) -> None:
        before = {"first.o": (1, 2, 3), "second.o": (4, 5, 6)}
        self.assertEqual(changed_objects(before, dict(before)), set())
        after = {**before, "first.o": (7, 2, 3)}
        self.assertEqual(changed_objects(before, after), {"first.o"})
        with self.assertRaisesRegex(IncrementalCheckError, "object set"):
            changed_objects(before, {})

    def test_incremental_counters_are_captured(self) -> None:
        output = (
            "incremental split: reused 20 generated files\n"
            "incremental build: rebuilt=1 reused=9 retained=8 materialized=1 output=target\n"
        )
        self.assertEqual(object_counts(output), {
            "rebuilt": 1, "reused": 9, "retained": 8, "materialized": 1,
        })

    def test_missing_or_inconsistent_counters_are_reported(self) -> None:
        for output in (
            "no counters",
            "incremental build: rebuilt=1 reused=9 retained=9 materialized=1 output=target",
        ):
            with self.subTest(output=output):
                with self.assertRaises(IncrementalCheckError):
                    object_counts(output)


if __name__ == "__main__":
    unittest.main()
