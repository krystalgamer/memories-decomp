from __future__ import annotations

from pathlib import Path
import subprocess
import sys
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
FILTER = REPOSITORY / "tools/project/normalize_psyq_rtps.py"


class NormalizePsyqRtpsTests(unittest.TestCase):
    def run_filter(self, source: str) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [sys.executable, str(FILTER)],
            cwd=REPOSITORY,
            input=source,
            capture_output=True,
            text=True,
        )

    def test_expands_rtps_placeholder(self) -> None:
        completed = self.run_filter(
            "\tnop\n\t.word 0x0000007f\n\t.word 0x0000117f\n"
        )
        self.assertEqual(completed.returncode, 0, completed.stderr)
        self.assertEqual(
            completed.stdout,
            "\tnop\n\t.word 0x4A180001\n\t.word 0x0000117f\n",
        )

    def test_rejects_input_without_rtps_placeholder(self) -> None:
        completed = self.run_filter("\tnop\n")
        self.assertNotEqual(completed.returncode, 0)
        self.assertIn("missing Psy-Q RTPS placeholder", completed.stderr)


if __name__ == "__main__":
    unittest.main()
