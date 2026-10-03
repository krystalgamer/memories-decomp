import contextlib
import hashlib
import io
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

import overlay_extract
from hashing import sha256_file, sha256_stream


class OverlayExtractionTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix="overlay-extraction-", dir=ROOT / "tmp")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.archive = self.root / "archive.bin"
        self.archive.write_bytes(b"----abcd----abcd")
        self.module = {
            "name": "first", "archive": "archive.bin",
            "archive_sha256": sha256_file(self.archive),
            "sector_offset": 1, "sector_count": 1,
            "duplicate_sector_offsets": [3],
            "sha256": hashlib.sha256(b"abcd").hexdigest(),
            "output": "tmp/first.bin", "load_address": "0x80100000",
        }
        self.second = {**self.module, "name": "second", "output": "tmp/second.bin"}

    def test_hash_stream_matches_file(self):
        self.assertEqual(sha256_stream(io.BytesIO(self.archive.read_bytes())),
                         sha256_file(self.archive))

    def test_each_distinct_archive_is_hashed_once_in_manifest_order(self):
        other = self.root / "other.bin"
        other.write_bytes(self.archive.read_bytes())
        middle = {**self.module, "name": "middle", "archive": "other.bin",
                  "output": "tmp/middle.bin"}
        self.second["archive"] = "./archive.bin"
        with patch("overlay_extract.sha256_stream", wraps=sha256_stream) as hashing:
            payloads = overlay_extract.read_modules(self.root, 4, [self.module, middle, self.second])
        self.assertEqual(hashing.call_count, 2)
        self.assertEqual(payloads, [
            (self.root / f"tmp/{name}.bin", b"abcd") for name in ("first", "middle", "second")
        ])
        self.assertTrue(all(call.args[0].closed for call in hashing.call_args_list))

    def test_each_invocation_verifies_archive_again(self):
        with patch("overlay_extract.sha256_stream", wraps=sha256_stream) as hashing:
            overlay_extract.read_modules(self.root, 4, [self.module, self.second])
            self.archive.write_bytes(b"xxxxabcd----abcd")
            with self.assertRaisesRegex(overlay_extract.OverlayError, "archive SHA-256"):
                overlay_extract.read_modules(self.root, 4, [self.module, self.second])
        self.assertEqual(hashing.call_count, 2)

    def test_each_module_archive_hash_is_checked(self):
        self.second["archive_sha256"] = "0" * 64
        with patch("overlay_extract.sha256_stream", wraps=sha256_stream) as hashing:
            with self.assertRaisesRegex(overlay_extract.OverlayError, "second: archive SHA-256"):
                overlay_extract.read_modules(self.root, 4, [self.module, self.second])
        self.assertEqual(hashing.call_count, 1)
        self.assertTrue(hashing.call_args.args[0].closed)

    def test_each_module_payload_hash_is_checked_before_any_output_is_written(self):
        self.second["sha256"] = "0" * 64
        with self.assertRaisesRegex(overlay_extract.OverlayError, "second: payload SHA-256"):
            overlay_extract.extract(self.root, 4, [self.module, self.second])
        self.assertFalse((self.root / "tmp/first.bin").exists())

    def test_later_modules_duplicate_copies_are_checked(self):
        self.second["duplicate_sector_offsets"] = [2]
        with self.assertRaisesRegex(overlay_extract.OverlayError, "sector 2 differs"):
            overlay_extract.read_modules(self.root, 4, [self.module, self.second])

    def test_later_module_sector_bounds_are_checked(self):
        self.second["sector_offset"] = 4
        with self.assertRaisesRegex(overlay_extract.OverlayError, "sectors exceed"):
            overlay_extract.read_modules(self.root, 4, [self.module, self.second])

    def test_archive_mutation_during_hash_is_rejected(self):
        def mutate(handle):
            digest = sha256_stream(handle)
            self.archive.write_bytes(b"xxxxabcd----abcd")
            return digest

        with patch("overlay_extract.sha256_stream", side_effect=mutate):
            with self.assertRaisesRegex(overlay_extract.OverlayError, "archive changed"):
                overlay_extract.read_modules(self.root, 4, [self.module])

    def test_archive_replacement_after_hash_is_rejected(self):
        def replace(handle):
            digest = sha256_stream(handle)
            replacement = self.root / "replacement.bin"
            replacement.write_bytes(self.archive.read_bytes())
            replacement.replace(self.archive)
            return digest

        with patch("overlay_extract.sha256_stream", side_effect=replace):
            with self.assertRaisesRegex(overlay_extract.OverlayError, "archive changed"):
                overlay_extract.read_modules(self.root, 4, [self.module])

    def test_archive_mutation_after_payload_read_is_rejected_even_with_restored_mtime(self):
        original = overlay_extract._read_module_payload
        before = self.archive.stat()

        def mutate(*args):
            result = original(*args)
            self.archive.write_bytes(b"xxxxabcd----abcd")
            os.utime(self.archive, ns=(before.st_atime_ns, before.st_mtime_ns))
            return result

        with patch("overlay_extract._read_module_payload", side_effect=mutate):
            with self.assertRaisesRegex(overlay_extract.OverlayError, "archive changed"):
                overlay_extract.extract(self.root, 4, [self.module])
        self.assertFalse((self.root / "tmp/first.bin").exists())

    def test_extract_and_verify_both_use_one_fresh_pass_per_archive(self):
        modules = [self.module, self.second]
        with contextlib.redirect_stdout(io.StringIO()):
            with patch("overlay_extract.sha256_stream", wraps=sha256_stream) as hashing:
                overlay_extract.extract(self.root, 4, modules)
                with patch("overlay_extract.verify_metadata") as metadata:
                    overlay_extract.verify(self.root, 4, modules)
                    metadata.assert_called_once_with(self.root)
        self.assertEqual(hashing.call_count, 2)
        self.assertEqual((self.root / "tmp/second.bin").read_bytes(), b"abcd")

    def test_verify_still_rejects_missing_or_changed_output(self):
        with self.assertRaisesRegex(overlay_extract.OverlayError, "missing output"):
            overlay_extract.verify(self.root, 4, [self.module])
        output = self.root / "tmp/first.bin"
        output.parent.mkdir()
        output.write_bytes(b"abce")
        with self.assertRaisesRegex(overlay_extract.OverlayError, "does not match"):
            overlay_extract.verify(self.root, 4, [self.module])


if __name__ == "__main__":
    unittest.main()
