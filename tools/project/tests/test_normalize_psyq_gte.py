from __future__ import annotations

from itertools import product
from pathlib import Path
import subprocess
import sys
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
FILTER = REPOSITORY / "tools/project/normalize_psyq_gte.py"
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import normalize_psyq_gte  # noqa: E402


def mvmva_marker(sf: int, mx: int, v: int, cv: int, lm: int) -> int:
    # The gte_mvmva() expansion in src/psyq/inline_c.h.
    return 0x000013BF | (sf << 25) | (mx << 23) | (v << 21) | (cv << 19) | (lm << 18)


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
            "\tnop;nop;.word 0x0000007f\n\t.word 0x00000fff\n\t.word 0x0000117f\n"
        )
        self.assertEqual(completed.returncode, 0, completed.stderr)
        self.assertEqual(
            completed.stdout,
            "\tnop;nop;.word 0x4A180001\n\t.word 0x4AE80413\n\t.word 0x4B400006\n",
        )

    def test_rejects_input_without_placeholder(self) -> None:
        completed = self.run_filter("\tnop\n")
        self.assertNotEqual(completed.returncode, 0)
        self.assertIn("missing Psy-Q GTE placeholder", completed.stderr)

    def test_rejects_other_command_placeholders(self) -> None:
        for marker in ("0x0000107f", "0x000011bf", "0x0000003f", "0x000013bf"):
            with self.subTest(marker=marker):
                completed = self.run_filter(
                    f"\t.word 0x0000007f\n\t.word {marker}\n"
                )
                self.assertNotEqual(completed.returncode, 0)
                self.assertIn("unsupported Psy-Q GTE placeholder", completed.stderr)

    def test_classifies_every_parameterized_mvmva_marker(self) -> None:
        markers = {mvmva_marker(*fields) for fields in product((0, 1), range(4), range(4), range(4), (0, 1))}
        self.assertEqual(len(markers), 256)
        for marker in markers:
            with self.subTest(marker=hex(marker)):
                self.assertTrue(normalize_psyq_gte.is_marker(marker))

    def test_rejects_parameterized_mvmva_markers_as_gcc_prints_them(self) -> None:
        for fields in ((1, 0, 0, 0, 0), (0, 1, 0, 0, 0), (1, 3, 3, 3, 1), (0, 0, 0, 0, 1)):
            marker = mvmva_marker(*fields)
            for spelling in (str(marker), f"0x{marker:08x}"):
                with self.subTest(fields=fields, spelling=spelling):
                    completed = self.run_filter(
                        f"\tnop;nop;.word 0x0000007f\n\tnop;nop;.word {spelling}\n"
                    )
                    self.assertNotEqual(completed.returncode, 0)
                    self.assertIn("unsupported Psy-Q GTE placeholder", completed.stderr)

    def test_classifies_every_literal_marker_in_the_inline_headers(self) -> None:
        import re

        translated = {0x7F, 0xFFF, 0x117F}
        for header in ("inline_c.h", "inline_o.h", "inline_s.h"):
            text = (REPOSITORY / "src/psyq" / header).read_text(encoding="utf-8")
            markers = {int(value, 16) for value in re.findall(r"\.word\s+(0x[0-9A-Fa-f]+)", text)}
            self.assertTrue(markers, header)
            for marker in sorted(markers - translated):
                with self.subTest(header=header, marker=hex(marker)):
                    self.assertTrue(normalize_psyq_gte.is_marker(marker))

    def test_leaves_ordinary_data_words(self) -> None:
        source = "\t.word 0x0000007f\n\t.word 0x00001000\n\t.word 305419896\n\t.word L8003487C\n"
        completed = self.run_filter(source)
        self.assertEqual(completed.returncode, 0, completed.stderr)
        self.assertEqual(completed.stdout, source.replace("0x0000007f", "0x4A180001"))
        for word in (0x4A180001, 0x4AE80413, 0x4B400006, 0x00001000, 0x12345678):
            with self.subTest(word=hex(word)):
                self.assertFalse(normalize_psyq_gte.is_marker(word))


if __name__ == "__main__":
    unittest.main()
