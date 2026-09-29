from __future__ import annotations

import contextlib
import hashlib
import io
import os
from pathlib import Path
import shutil
import stat
import subprocess
import sys
import tempfile
import textwrap
import unittest
from unittest import mock
import warnings
import zipfile

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from stage_ci_inputs import REGIONS, stage_inputs
from verify_inputs import VerificationError
from workspace import WorkspaceError


class StageCiInputsTests(unittest.TestCase):
    def setUp(self) -> None:
        scratch = REPOSITORY / "tmp"
        scratch.mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix="test-ci-inputs-", dir=scratch)
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        (self.root / "tmp").mkdir()

    def fixture(self, region: str = "france") -> dict[str, bytes]:
        code, config, executable = REGIONS[region]
        directory = "game" if region == "usa" else f"game/{region}"
        payloads = {
            f"ci_files/{code}/{executable}": b"synthetic executable",
            f"ci_files/{code}/SU.MRG": b"synthetic SU",
            f"ci_files/{code}/WA_MRG.MRG": b"synthetic WA",
        }
        manifest = self.root / f"config/{config}/files.sha256"
        manifest.parent.mkdir(parents=True, exist_ok=True)
        manifest.write_text("".join(
            f"{hashlib.sha256(data).hexdigest()}  {directory}/"
            f"{'DATA/' if name.endswith('.MRG') else ''}{Path(name).name}\n"
            for name, data in payloads.items()
        ))
        return payloads

    def bundle(self, payloads: dict[str, bytes]) -> None:
        with zipfile.ZipFile(self.root / "tmp/inputs.zip", "w") as bundle:
            for name, data in payloads.items():
                bundle.writestr(name, data)

    def stage(
        self, region: str = "france", *,
        archives_only: bool = False, executable_only: bool = False,
    ) -> None:
        with contextlib.redirect_stdout(io.StringIO()):
            stage_inputs(
                self.root, "tmp/inputs.zip", region,
                archives_only=archives_only, executable_only=executable_only,
            )

    def assert_no_install(self) -> None:
        self.assertFalse((self.root / "game").exists())
        self.assertEqual(list((self.root / "tmp").iterdir()), [self.root / "tmp/inputs.zip"])

    def test_each_region_stages_three_verified_files_idempotently(self) -> None:
        for region in REGIONS:
            with self.subTest(region=region):
                payloads = self.fixture(region)
                self.bundle(payloads)
                self.stage(region)
                directory = self.root / ("game" if region == "usa" else f"game/{region}")
                files = sorted(directory.rglob("*"))
                before = {path: path.stat().st_mtime_ns for path in files if path.is_file()}
                self.stage(region)
                self.assertEqual(len(before), 3)
                for path, modified in before.items():
                    self.assertEqual(path.stat().st_mtime_ns, modified)
                    code = REGIONS[region][0]
                    self.assertEqual(path.read_bytes(), payloads[f"ci_files/{code}/{path.name}"])
                self.assertEqual(
                    list((self.root / "tmp").iterdir()), [self.root / "tmp/inputs.zip"]
                )

    def test_archives_only_does_not_require_an_executable_member(self) -> None:
        payloads = self.fixture()
        del payloads["ci_files/fra/SLES_039.48"]
        self.bundle(payloads)
        self.stage(archives_only=True)
        self.assertFalse((self.root / "game/france/SLES_039.48").exists())
        self.assertEqual(len(list((self.root / "game/france/DATA").iterdir())), 2)

    def test_usa_executable_only_does_not_require_archive_members(self) -> None:
        payloads = self.fixture("usa")
        del payloads["ci_files/usa/SU.MRG"]
        del payloads["ci_files/usa/WA_MRG.MRG"]
        self.bundle(payloads)
        self.stage("usa", executable_only=True)
        self.assertTrue((self.root / "game/SLUS_014.11").is_file())
        self.assertFalse((self.root / "game/DATA").exists())

    def test_usa_does_not_require_unrelated_disc_files(self) -> None:
        self.bundle(self.fixture("usa"))
        manifest = self.root / "config/slus_01411/files.sha256"
        with manifest.open("a") as output:
            output.write(f"{'0' * 64}  game/DATA/MODEL.MRG\n")
            output.write(f"{'0' * 64}  game/rpg-yfm.bin\n")
        self.stage("usa")
        self.assertTrue((self.root / "game/DATA/WA_MRG.MRG").is_file())
        self.assertFalse((self.root / "game/DATA/MODEL.MRG").exists())

    def test_missing_selected_checksum_is_rejected(self) -> None:
        self.bundle(self.fixture("usa"))
        manifest = self.root / "config/slus_01411/files.sha256"
        manifest.write_text(manifest.read_text().splitlines()[0] + "\n")
        with self.assertRaisesRegex(VerificationError, "missing input checksum"):
            self.stage("usa")
        self.assert_no_install()

    def test_known_patched_input_remains_rejected(self) -> None:
        payloads = self.fixture("usa")
        self.bundle(payloads)
        digest = hashlib.sha256(payloads["ci_files/usa/WA_MRG.MRG"]).hexdigest()
        with (
            mock.patch("stage_ci_inputs.KNOWN_PATCHED_INPUTS", {("game/DATA/WA_MRG.MRG", digest)}),
            self.assertRaisesRegex(VerificationError, "known patched retail input"),
        ):
            self.stage("usa")
        self.assert_no_install()

    def test_input_selections_are_mutually_exclusive(self) -> None:
        self.bundle(self.fixture())
        with self.assertRaisesRegex(VerificationError, "mutually exclusive"):
            self.stage(archives_only=True, executable_only=True)
        self.assert_no_install()

    def test_corrupt_final_member_does_not_install_earlier_members(self) -> None:
        payloads = self.fixture()
        payloads["ci_files/fra/WA_MRG.MRG"] = b"wrong region or modified input"
        self.bundle(payloads)
        with self.assertRaisesRegex(VerificationError, "SHA-256 mismatch"):
            self.stage()
        self.assert_no_install()

    def test_missing_member_is_rejected(self) -> None:
        payloads = self.fixture()
        del payloads["ci_files/fra/SU.MRG"]
        self.bundle(payloads)
        with self.assertRaisesRegex(VerificationError, "exactly one ZIP member"):
            self.stage()
        self.assert_no_install()

    def test_duplicate_member_is_rejected(self) -> None:
        payloads = self.fixture()
        self.bundle(payloads)
        with warnings.catch_warnings():
            warnings.simplefilter("ignore", UserWarning)
            with zipfile.ZipFile(self.root / "tmp/inputs.zip", "a") as bundle:
                bundle.writestr("ci_files/fra/SU.MRG", b"duplicate")
        with self.assertRaisesRegex(VerificationError, "exactly one ZIP member"):
            self.stage()
        self.assert_no_install()

    def test_symlink_member_is_rejected(self) -> None:
        payloads = self.fixture()
        with zipfile.ZipFile(self.root / "tmp/inputs.zip", "w") as bundle:
            for name, data in payloads.items():
                info = zipfile.ZipInfo(name)
                info.create_system = 3
                info.external_attr = (stat.S_IFLNK | 0o777) << 16
                bundle.writestr(info, data)
        with self.assertRaisesRegex(VerificationError, "not a regular input"):
            self.stage()
        self.assert_no_install()

    def test_unselected_members_are_never_extracted(self) -> None:
        payloads = self.fixture()
        payloads.update({
            "../escaped": b"unexpected",
            "/absolute": b"unexpected",
            "ci_files/fra/../../escaped": b"unexpected",
            "ci_files/ger/SU.MRG": b"other region",
        })
        self.bundle(payloads)
        self.stage()
        self.assertFalse((self.root / "escaped").exists())
        self.assertFalse((self.root / "game/germany").exists())
        self.assertEqual(len([p for p in (self.root / "game").rglob("*") if p.is_file()]), 3)

    def test_existing_different_input_is_not_overwritten(self) -> None:
        self.bundle(self.fixture())
        destination = self.root / "game/france/DATA/WA_MRG.MRG"
        destination.parent.mkdir(parents=True)
        destination.write_bytes(b"preserve user input")
        with self.assertRaisesRegex(VerificationError, "refusing to replace"):
            self.stage()
        self.assertEqual(destination.read_bytes(), b"preserve user input")
        self.assertFalse((self.root / "game/france/SLES_039.48").exists())
        self.assertFalse((self.root / "game/france/DATA/SU.MRG").exists())

    def test_destination_cannot_escape_workspace_via_symlink(self) -> None:
        self.bundle(self.fixture())
        (self.root / "game").symlink_to(self.root.parent, target_is_directory=True)
        with self.assertRaisesRegex(WorkspaceError, "leaves the workspace"):
            self.stage()

    def test_archive_cannot_escape_workspace(self) -> None:
        self.fixture()
        with self.assertRaises(WorkspaceError):
            stage_inputs(self.root, "../inputs.zip", "france")

    def test_invalid_zip_is_rejected(self) -> None:
        self.fixture()
        (self.root / "tmp/inputs.zip").write_bytes(b"not a ZIP")
        with self.assertRaises(zipfile.BadZipFile):
            self.stage()
        self.assert_no_install()

    def run_action(
        self, *, password: str = "test-password", curl_status: int = 0,
        region: str = "france", executable_only: bool = False,
    ):
        action = (REPOSITORY / ".github/actions/retail-inputs/action.yml").read_text()
        script = textwrap.dedent(action.split("      run: |\n", 1)[1])
        tools = self.root / "tools/project"
        tools.mkdir(parents=True)
        for name in ("stage_ci_inputs.py", "workspace.py", "hashing.py", "verify_inputs.py"):
            shutil.copyfile(REPOSITORY / "tools/project" / name, tools / name)
        for marker in (".git", "Makefile", "config/slus_01411/target.yaml"):
            path = self.root / marker
            path.parent.mkdir(parents=True, exist_ok=True)
            path.touch()
        bin_dir = self.root / "tmp/bin"
        bin_dir.mkdir()
        curl = bin_dir / "curl"
        curl.write_text(
            "#!/bin/bash\n"
            "set -euo pipefail\n"
            'printf invoked > "$PWD/tmp/curl-called"\n'
            f"if [ {curl_status} -ne 0 ]; then exit {curl_status}; fi\n"
            'while [ "$#" -gt 0 ]; do\n'
            '  case "$1" in\n'
            '    --user) test "$2" = "test-user:test-password"; shift ;;\n'
            '    --output) cp tmp/inputs.zip "$2"; shift ;;\n'
            "  esac\n"
            "  shift\n"
            "done\n"
        )
        curl.chmod(0o755)
        return subprocess.run(
            ["bash", "-c", script], cwd=self.root, text=True, capture_output=True,
            env={
                **os.environ,
                "PATH": f"{bin_dir}:{os.environ['PATH']}",
                "YGOFM_CI_FILES": "https://example.invalid/private.zip",
                "YGOFM_CI_FILES_USERNAME": "test-user",
                "YGOFM_CI_FILES_PASSWORD": password,
                "INPUT_REGION": region,
                "ARCHIVES_ONLY": "false",
                "EXECUTABLE_ONLY": str(executable_only).lower(),
            },
        )

    def test_action_downloads_with_auth_and_stages_verified_inputs(self) -> None:
        self.bundle(self.fixture())
        result = self.run_action()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertTrue((self.root / "tmp/curl-called").exists())
        self.assertTrue((self.root / "game/france/SLES_039.48").exists())
        self.assertFalse(list((self.root / "tmp").glob("ci-files-*")))
        self.assertNotIn("test-password", result.stdout + result.stderr)

    def test_action_requires_credentials_before_downloading(self) -> None:
        result = self.run_action(password="")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Configure the YGOFM_CI_FILES_PASSWORD", result.stdout)
        self.assertFalse((self.root / "tmp/curl-called").exists())
        self.assertFalse(list((self.root / "tmp").glob("ci-files-*")))

    def test_action_supports_usa_executable_only(self) -> None:
        payloads = self.fixture("usa")
        self.bundle({"ci_files/usa/SLUS_014.11": payloads["ci_files/usa/SLUS_014.11"]})
        result = self.run_action(region="usa", executable_only=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertTrue((self.root / "game/SLUS_014.11").exists())
        self.assertFalse((self.root / "game/DATA").exists())

    def test_action_download_failure_cleans_up_and_does_not_stage(self) -> None:
        result = self.run_action(curl_status=22)
        self.assertEqual(result.returncode, 22)
        self.assertFalse((self.root / "game").exists())
        self.assertFalse(list((self.root / "tmp").glob("ci-files-*")))


if __name__ == "__main__":
    unittest.main()
