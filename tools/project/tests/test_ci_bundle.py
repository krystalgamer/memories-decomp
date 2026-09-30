from __future__ import annotations

import contextlib
import io
import json
import os
from pathlib import Path
import shutil
import sys
import unittest
from unittest import mock

from tools.project.tests import test_stage_ci_inputs as fixtures

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import ci_bundle
from stage_ci_inputs import REGIONS
from verify_inputs import VerificationError
from workspace import WorkspaceError


class CiBundleTests(unittest.TestCase):
    def setUp(self) -> None:
        self.fixture = fixtures.StageCiInputsTests()
        self.fixture.setUp()
        self.addCleanup(self.fixture.doCleanups)
        self.root = self.fixture.root
        self.payloads = {}
        for region in REGIONS:
            self.payloads.update(self.fixture.fixture(region))
        self.fixture.bundle(self.payloads)
        credentials = mock.patch.dict(os.environ, {
            "YGOFM_CI_FILES": "https://example.invalid/private.zip",
            "YGOFM_CI_FILES_USERNAME": "synthetic-user",
            "YGOFM_CI_FILES_PASSWORD": "synthetic-password-with-$-and-\"-and-\n-newline",
        })
        credentials.start()
        self.addCleanup(credentials.stop)

    def encrypt(self) -> None:
        if shutil.which("gpg") is None:
            self.skipTest("GnuPG is required for the encrypted bundle integration test")
        with contextlib.redirect_stdout(io.StringIO()):
            ci_bundle.encrypt_bundle(self.root, "tmp/inputs.zip")

    def stage(self, **kwargs) -> None:
        with contextlib.redirect_stdout(io.StringIO()):
            ci_bundle.stage_cached_bundle(self.root, "france", **kwargs)

    def assert_no_plaintext_scratch(self) -> None:
        self.assertFalse(list((self.root / "tmp").glob("ci-encrypt-*")))
        self.assertFalse(list((self.root / "tmp").glob("ci-decrypt-*")))
        self.assertFalse(list((self.root / "tmp").glob("ci-inputs-*")))

    def test_key_is_stable_without_exposing_credentials(self) -> None:
        key = ci_bundle.cache_key(self.root)
        self.assertEqual(ci_bundle.cache_key(self.root), key)
        self.assertRegex(key, r"^retail-encrypted-v1-[0-9a-f]{64}$")
        for name in ci_bundle.CREDENTIALS:
            self.assertNotIn(os.environ[name], key)

    def test_checksum_and_credentials_invalidate_cache(self) -> None:
        original = ci_bundle.cache_key(self.root)
        path = self.root / "config/slus_01411/files.sha256"
        contents = path.read_text()
        path.write_text(contents + "\n")
        self.assertNotEqual(original, ci_bundle.cache_key(self.root))
        path.write_text(contents)
        for name in ci_bundle.CREDENTIALS:
            with self.subTest(secret=name), mock.patch.dict(os.environ, {name: "rotated-value"}):
                self.assertNotEqual(original, ci_bundle.cache_key(self.root))

    def test_public_key_fingerprint_uses_slow_derivation(self) -> None:
        with mock.patch("ci_bundle.hashlib.pbkdf2_hmac",
                        wraps=ci_bundle.hashlib.pbkdf2_hmac) as derive:
            ci_bundle.cache_key(self.root)
        self.assertEqual(derive.call_count, 1)
        self.assertEqual(derive.call_args.args[0], "sha256")
        self.assertGreaterEqual(derive.call_args.args[3], 600_000)

    def test_more_modules_in_same_archive_do_not_invalidate_cache(self) -> None:
        original = ci_bundle.cache_key(self.root)
        path = self.root / "config/sles_03948/overlays.json"
        manifest = json.loads(path.read_text())
        manifest["modules"][0]["sector_offset"] = 3
        path.write_text(json.dumps(manifest))
        self.assertEqual(original, ci_bundle.cache_key(self.root))

    def test_new_selected_archive_changes_cache_key(self) -> None:
        original = ci_bundle.cache_key(self.root)
        path = self.root / "config/sles_03948/overlays.json"
        manifest = json.loads(path.read_text())
        manifest["modules"][0]["archive"] = "game/france/DATA/EXTRA.MRG"
        path.write_text(json.dumps(manifest))
        self.assertNotEqual(original, ci_bundle.cache_key(self.root))

    def test_missing_credentials_fail_before_any_external_command(self) -> None:
        for name in ci_bundle.CREDENTIALS:
            with self.subTest(secret=name), mock.patch.dict(os.environ, {name: ""}), \
                    mock.patch("ci_bundle.subprocess.run") as run:
                with self.assertRaisesRegex(ci_bundle.BundleError, name):
                    ci_bundle.cache_key(self.root)
                run.assert_not_called()

    def test_round_trip_verifies_all_regions_but_installs_only_selected_region(self) -> None:
        self.encrypt()
        self.assertFalse((self.root / "game").exists())
        ciphertext = (self.root / ci_bundle.CACHE_PATH).read_bytes()
        self.assertNotIn(b"synthetic executable", ciphertext)
        self.stage()
        self.assertEqual((self.root / "game/france/SLES_039.48").read_bytes(), b"synthetic executable")
        self.assertEqual(sorted(path.name for path in (self.root / "game").iterdir()), ["france"])
        self.assert_no_plaintext_scratch()

    def test_executable_only_restore_preserves_selection(self) -> None:
        self.encrypt()
        self.stage(executable_only=True)
        self.assertTrue((self.root / "game/france/SLES_039.48").is_file())
        self.assertFalse((self.root / "game/france/DATA").exists())
        self.assert_no_plaintext_scratch()

    def test_bad_region_member_is_not_cached(self) -> None:
        self.payloads["ci_files/ita/MODEL.MRG"] = b"corrupted"
        self.fixture.bundle(self.payloads)
        with mock.patch("ci_bundle.crypt") as crypt, contextlib.redirect_stdout(io.StringIO()):
            with self.assertRaisesRegex(VerificationError, "SHA-256 mismatch"):
                ci_bundle.encrypt_bundle(self.root, "tmp/inputs.zip")
            crypt.assert_not_called()
        self.assertFalse((self.root / ci_bundle.CACHE_PATH).exists())
        self.assertFalse((self.root / "game").exists())
        self.assert_no_plaintext_scratch()

    def test_wrong_credentials_never_stage_plaintext(self) -> None:
        self.encrypt()
        with mock.patch.dict(os.environ, {"YGOFM_CI_FILES_PASSWORD": "wrong-password"}):
            with self.assertRaisesRegex(ci_bundle.BundleError, "Cannot decrypt"):
                self.stage()
        self.assertFalse((self.root / "game").exists())
        self.assert_no_plaintext_scratch()

    def test_tampered_ciphertext_never_stages_plaintext(self) -> None:
        self.encrypt()
        path = self.root / ci_bundle.CACHE_PATH
        data = bytearray(path.read_bytes())
        data[-1] ^= 1
        path.write_bytes(data)
        with self.assertRaisesRegex(ci_bundle.BundleError, "Cannot decrypt"):
            self.stage()
        self.assertFalse((self.root / "game").exists())
        self.assert_no_plaintext_scratch()

    def test_missing_cache_does_not_download_from_origin(self) -> None:
        with mock.patch("ci_bundle.subprocess.run") as run:
            with self.assertRaisesRegex(ci_bundle.BundleError, "Rerun all jobs"):
                self.stage()
            run.assert_not_called()
        self.assertFalse((self.root / "game").exists())

    def test_archive_path_cannot_escape_repository(self) -> None:
        with self.assertRaises(WorkspaceError):
            ci_bundle.encrypt_bundle(self.root, "../outside.zip")

    def test_decrypt_requires_authenticated_encryption_status(self) -> None:
        source = self.root / "tmp/plain"
        source.write_bytes(b"not encrypted")
        with mock.patch("ci_bundle.subprocess.run") as run:
            run.return_value.returncode = 0
            run.return_value.stdout = b"[GNUPG:] PLAINTEXT\n"
            with self.assertRaisesRegex(ci_bundle.BundleError, "Cannot decrypt"):
                ci_bundle.crypt(source, self.root / "tmp/output", self.root / "tmp/gpg", decrypt=True)
            command = run.call_args.args[0]
            self.assertIn("--no-autostart", command)
            for name in ci_bundle.CREDENTIALS:
                self.assertNotIn(os.environ[name], command)

    def test_status_words_inside_a_plaintext_filename_are_not_authentication(self) -> None:
        source = self.root / "tmp/plain"
        source.write_bytes(b"not encrypted")
        with mock.patch("ci_bundle.subprocess.run") as run:
            run.return_value.returncode = 0
            run.return_value.stdout = b"[GNUPG:] PLAINTEXT 62 0 [GNUPG:] DECRYPTION_OKAY [GNUPG:] GOODMDC\n"
            with self.assertRaisesRegex(ci_bundle.BundleError, "Cannot decrypt"):
                ci_bundle.crypt(source, self.root / "tmp/output", self.root / "tmp/gpg", decrypt=True)


if __name__ == "__main__":
    unittest.main()
