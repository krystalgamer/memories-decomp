from __future__ import annotations

from pathlib import Path
import sys
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import candidate_builds


class CandidateFingerprintTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")

    def test_text_only_payload_preserves_v1_format(self) -> None:
        content = bytes.fromhex("01020304")
        relocations = ["00000000 R_MIPS_26 target"]

        payload = candidate_builds.candidate_fingerprint_payload(
            [(".text", content, relocations)]
        )

        self.assertEqual(
            payload,
            b"candidate-build-v1\0"
            + content
            + b"\0"
            + b"00000000 R_MIPS_26 target\n",
        )

    def test_read_only_data_uses_framed_v2_payload(self) -> None:
        sections = [
            (".text", bytes.fromhex("01020304"), []),
            (
                ".rodata",
                bytes.fromhex("00000000"),
                ["00000000 R_MIPS_32 .text"],
            ),
        ]

        payload = candidate_builds.candidate_fingerprint_payload(sections)
        changed = candidate_builds.candidate_fingerprint_payload(
            [
                sections[0],
                (".rodata", bytes.fromhex("01000000"), sections[1][2]),
            ]
        )
        text_relocations = b"\n"
        rodata_relocations = b"00000000 R_MIPS_32 .text\n"
        expected = (
            b"candidate-build-v2\0"
            + len(b".text").to_bytes(2, "big")
            + b".text"
            + len(sections[0][1]).to_bytes(8, "big")
            + sections[0][1]
            + len(text_relocations).to_bytes(8, "big")
            + text_relocations
            + len(b".rodata").to_bytes(2, "big")
            + b".rodata"
            + len(sections[1][1]).to_bytes(8, "big")
            + sections[1][1]
            + len(rodata_relocations).to_bytes(8, "big")
            + rodata_relocations
        )

        self.assertEqual(payload, expected)
        self.assertNotEqual(payload, changed)


if __name__ == "__main__":
    unittest.main()
