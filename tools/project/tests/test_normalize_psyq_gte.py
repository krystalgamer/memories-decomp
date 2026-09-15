from __future__ import annotations

from pathlib import Path
import subprocess
import sys
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
FILTER = REPOSITORY / "tools/project/normalize_psyq_gte.py"


class NormalizePsyqGteTests(unittest.TestCase):
    def run_filter(self, source: str) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [sys.executable, str(FILTER)],
            cwd=REPOSITORY,
            input=source,
            capture_output=True,
            text=True,
        )

    def test_expands_rtps_ncds_and_nclip_placeholders(self) -> None:
        completed = self.run_filter(
            "\tnop\n\t.word 0x0000007f\n\t.word 0x00000fff\n\t.word 0x0000117f\n"
        )
        self.assertEqual(completed.returncode, 0, completed.stderr)
        self.assertEqual(
            completed.stdout,
            "\tnop\n\t.word 0x4A180001\n\t.word 0x4AE80413\n\t.word 0x4B400006\n",
        )

    def test_rejects_input_without_placeholder(self) -> None:
        completed = self.run_filter("\tnop\n")
        self.assertNotEqual(completed.returncode, 0)
        self.assertIn("missing Psy-Q GTE placeholder", completed.stderr)

    def test_rejects_other_command_placeholders(self) -> None:
        for marker in ("0x0000107f", "0x000011bf", "0x0000003f"):
            with self.subTest(marker=marker):
                completed = self.run_filter(
                    f"\t.word 0x0000007f\n\t.word {marker}\n"
                )
                self.assertNotEqual(completed.returncode, 0)
                self.assertIn("unsupported Psy-Q GTE placeholder", completed.stderr)

    def test_leaves_ordinary_data_words(self) -> None:
        completed = self.run_filter("\t.word 0x0000007f\n\t.word 0x00001000\n")
        self.assertEqual(completed.returncode, 0, completed.stderr)
        self.assertEqual(completed.stdout, "\t.word 0x4A180001\n\t.word 0x00001000\n")


if __name__ == "__main__":
    unittest.main()
