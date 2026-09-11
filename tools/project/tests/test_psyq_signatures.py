from __future__ import annotations

import json
from pathlib import Path
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from psyq_signatures import (
    SignatureError,
    classify,
    find_matches,
    parse_signature,
    scan,
)


class PsyqSignatureTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="psyq-signatures-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.signatures = Path(temporary.name)

    def test_parse_signature_marks_wildcards(self) -> None:
        pattern, mask = parse_signature("AA ?? 0f")

        self.assertEqual(pattern, b"\xaa\x00\x0f")
        self.assertEqual(mask, b"\xff\x00\xff")

    def test_parse_signature_rejects_empty_and_malformed_tokens(self) -> None:
        cases = [
            ("", "signature is empty"),
            ("AA B CC", "token 1 'B' is not"),
            ("AA 0xBB CC", "token 1 '0xBB' is not"),
            ("AA 100 CC", "token 1 '100' is not"),
            ("AA GG CC", "token 1 'GG' is not"),
        ]
        for signature, message in cases:
            with self.subTest(signature=signature):
                with self.assertRaisesRegex(SignatureError, message):
                    parse_signature(signature)

    def test_find_matches_requires_alignment_and_full_mask(self) -> None:
        pattern, mask = parse_signature("AA BB CC DD ?? 11 22 33")
        payload = bytearray(40)
        payload[0:8] = bytes.fromhex("AA BB CC DD 99 11 22 33")
        payload[9:17] = bytes.fromhex("AA BB CC DD 88 11 22 33")
        payload[24:32] = bytes.fromhex("AA BB CC DD 77 11 22 44")

        self.assertEqual(find_matches(bytes(payload), pattern, mask), [0])

    def test_find_matches_rejects_unanchorable_signatures(self) -> None:
        pattern, mask = parse_signature("AA ?? BB ?? CC")

        self.assertIsNone(find_matches(b"\xaa\x00\xbb\x00\xcc", pattern, mask))

    def test_scan_rejects_directory_without_json_files(self) -> None:
        with self.assertRaisesRegex(
            SignatureError, "no JSON signature files"
        ):
            scan(self.signatures, 0x80010000, b"")

    def test_scan_reports_catalogue_path_for_invalid_json(self) -> None:
        (self.signatures / "LIBTEST.LIB.json").write_text(
            "[", encoding="utf-8"
        )

        with self.assertRaisesRegex(
            SignatureError,
            "LIBTEST[.]LIB[.]json: invalid JSON at line 1, column 2: "
            "Expecting value",
        ):
            scan(self.signatures, 0x80010000, b"")

    def test_scan_reports_catalogue_path_for_invalid_utf8(self) -> None:
        (self.signatures / "LIBTEST.LIB.json").write_bytes(b"[\xff]")

        with self.assertRaisesRegex(
            SignatureError,
            "LIBTEST[.]LIB[.]json: invalid UTF-8 at byte 1: "
            "invalid start byte",
        ):
            scan(self.signatures, 0x80010000, b"")

    def test_scan_rejects_non_array_catalogue(self) -> None:
        (self.signatures / "LIBTEST.LIB.json").write_text(
            "{}", encoding="utf-8"
        )

        with self.assertRaisesRegex(SignatureError, "expected a JSON array"):
            scan(self.signatures, 0x80010000, b"")

    def test_scan_rejects_non_object_catalogue_entry(self) -> None:
        (self.signatures / "LIBTEST.LIB.json").write_text(
            "[42]", encoding="utf-8"
        )

        with self.assertRaisesRegex(
            SignatureError, "entry 0 is not a JSON object"
        ):
            scan(self.signatures, 0x80010000, b"")

    def test_scan_rejects_catalogue_without_signatures(self) -> None:
        (self.signatures / "LIBTEST.LIB.json").write_text(
            '[{"name": "NO_SIGNATURE.OBJ"}]', encoding="utf-8"
        )

        with self.assertRaisesRegex(SignatureError, "no signature entries"):
            scan(self.signatures, 0x80010000, b"")

    def test_scan_rejects_non_string_entry_fields(self) -> None:
        cases = [
            (
                {"name": 42, "sig": "AA BB CC DD"},
                "entry 0 name is not a string",
            ),
            (
                {"name": "BAD.OBJ", "sig": 42},
                "entry 0 sig is not a string",
            ),
        ]
        for entry, message in cases:
            with self.subTest(message=message):
                (self.signatures / "LIBTEST.LIB.json").write_text(
                    json.dumps([entry]), encoding="utf-8"
                )
                with self.assertRaisesRegex(SignatureError, message):
                    scan(self.signatures, 0x80010000, b"")

    def test_scan_rejects_empty_entry_and_label_names(self) -> None:
        cases = [
            (
                {"name": "", "sig": "AA BB CC DD"},
                "entry 0 name is empty",
            ),
            (
                {
                    "name": "BAD.OBJ",
                    "sig": "AA BB CC DD",
                    "labels": [{"name": " ", "offset": 0}],
                },
                "entry 0 label 0 name is empty",
            ),
        ]
        for entry, message in cases:
            with self.subTest(message=message):
                (self.signatures / "LIBTEST.LIB.json").write_text(
                    json.dumps([entry]), encoding="utf-8"
                )
                with self.assertRaisesRegex(SignatureError, message):
                    scan(self.signatures, 0x80010000, b"")

    def test_scan_reports_catalogue_context_for_invalid_signature(self) -> None:
        cases = [
            ("", "entry 0 sig: signature is empty"),
            (
                "AA INVALID CC DD",
                "entry 0 sig: token 1 'INVALID' is not",
            ),
        ]
        for signature, message in cases:
            with self.subTest(signature=signature):
                (self.signatures / "LIBTEST.LIB.json").write_text(
                    json.dumps(
                        [{"name": "BAD.OBJ", "sig": signature}]
                    ),
                    encoding="utf-8",
                )
                with self.assertRaisesRegex(SignatureError, message):
                    scan(self.signatures, 0x80010000, b"")

    def test_scan_rejects_non_array_labels(self) -> None:
        (self.signatures / "LIBTEST.LIB.json").write_text(
            json.dumps(
                [{"name": "BAD.OBJ", "sig": "AA BB CC DD", "labels": {}}]
            ),
            encoding="utf-8",
        )

        with self.assertRaisesRegex(
            SignatureError, "entry 0 labels is not an array"
        ):
            scan(self.signatures, 0x80010000, b"")

    def test_scan_rejects_invalid_label_fields(self) -> None:
        cases = [
            (42, "label 0 is not an object"),
            (
                {"name": 42, "offset": 0},
                "label 0 name is not a string",
            ),
            (
                {"name": "BadLabel", "offset": "0"},
                "label 0 offset is not an integer",
            ),
        ]
        for label, message in cases:
            with self.subTest(message=message):
                entry = {
                    "name": "BAD.OBJ",
                    "sig": "AA BB CC DD",
                    "labels": [label],
                }
                (self.signatures / "LIBTEST.LIB.json").write_text(
                    json.dumps([entry]), encoding="utf-8"
                )
                with self.assertRaisesRegex(SignatureError, message):
                    scan(self.signatures, 0x80010000, b"")

    def test_scan_rejects_label_offsets_outside_signature(self) -> None:
        cases = [
            (-1, "offset -1 is outside the 4-byte signature"),
            (4, "offset 4 is outside the 4-byte signature"),
        ]
        for offset, message in cases:
            with self.subTest(offset=offset):
                entry = {
                    "name": "BAD.OBJ",
                    "sig": "AA BB CC DD",
                    "labels": [{"name": "BadLabel", "offset": offset}],
                }
                (self.signatures / "LIBTEST.LIB.json").write_text(
                    json.dumps([entry]), encoding="utf-8"
                )
                with self.assertRaisesRegex(SignatureError, message):
                    scan(self.signatures, 0x80010000, b"")

    def test_scan_rejects_duplicates_and_placeholder_labels(self) -> None:
        entries = [
            {
                "name": "UNIQUE.OBJ",
                "sig": "AA BB CC DD 11 22 33 44",
                "labels": [
                    {"name": "RealName", "offset": 0},
                    {"name": "text_4", "offset": 4},
                ],
            },
            {
                "name": "DUPLICATE.OBJ",
                "sig": "10 20 30 40",
                "labels": [{"name": "DuplicateName", "offset": 0}],
            },
            {
                "name": "ABSENT.OBJ",
                "sig": "DE AD BE EF",
                "labels": [{"name": "AbsentName", "offset": 0}],
            },
            {
                "name": "UNANCHORED.OBJ",
                "sig": "AA ?? BB",
                "labels": [{"name": "UnanchoredName", "offset": 0}],
            },
        ]
        (self.signatures / "LIBTEST.LIB.json").write_text(
            json.dumps(entries), encoding="utf-8"
        )
        payload = bytearray(24)
        payload[0:8] = bytes.fromhex("AA BB CC DD 11 22 33 44")
        payload[8:12] = bytes.fromhex("10 20 30 40")
        payload[16:20] = bytes.fromhex("10 20 30 40")

        result = scan(self.signatures, 0x80010000, bytes(payload))

        self.assertEqual(result["unique"], 1)
        self.assertEqual(result["multiple"], 1)
        self.assertEqual(result["absent"], 1)
        self.assertEqual(result["unanchored"], 1)
        self.assertEqual(
            result["proposals"],
            {
                0x80010000: {
                    "RealName": ["LIBTEST.LIB/UNIQUE.OBJ+0x0"]
                }
            },
        )

    def test_classify_keeps_ambiguous_and_off_start_names_out(self) -> None:
        proposals = {
            0x80010000: {"KnownName": ["LIB/KNOWN.OBJ+0x0"]},
            0x80010010: {"NewName": ["LIB/NEW.OBJ+0x0"]},
            0x80010020: {"CorpusName": ["LIB/DIFFERENT.OBJ+0x0"]},
            0x80010030: {
                "AliasA": ["LIBA/SHARED.OBJ+0x0"],
                "AliasB": ["LIBB/SHARED.OBJ+0x0"],
            },
            0x80010040: {"OffStart": ["LIB/OFFSTART.OBJ+0x4"]},
            0x80010050: {
                "GameCollision": ["LIB/GAME_COLLISION.OBJ+0x0"]
            },
        }
        inventory = {
            0x80010000: {
                "name": "KnownName",
                "status": "sdk_asm",
                "module": "psyq/sdk",
            },
            0x80010010: {
                "name": "func_80010010",
                "size": "0x10",
                "status": "sdk_asm",
                "module": "psyq/sdk",
            },
            0x80010020: {
                "name": "ExistingName",
                "status": "sdk_asm",
                "module": "psyq/sdk",
            },
            0x80010050: {
                "name": "func_80010050",
                "size": "0x20",
                "status": "matching_c",
                "module": "game",
            },
        }

        result = classify(proposals, inventory)

        self.assertEqual(result["agreed"], [(0x80010000, "KnownName")])
        self.assertEqual(
            [(address, name) for address, name, _row, _providers in result["new"]],
            [(0x80010010, "NewName")],
        )
        self.assertEqual(
            result["disagreed"],
            [
                (
                    0x80010020,
                    "CorpusName",
                    "ExistingName",
                    ["LIB/DIFFERENT.OBJ+0x0"],
                )
            ],
        )
        self.assertEqual(
            result["ambiguous"], [(0x80010030, ["AliasA", "AliasB"])]
        )
        self.assertEqual(
            result["outside_psyq"],
            [
                (
                    0x80010050,
                    "GameCollision",
                    "func_80010050",
                    "matching_c",
                    "game",
                    ["LIB/GAME_COLLISION.OBJ+0x0"],
                )
            ],
        )
        self.assertEqual(result["off_start"], 1)


if __name__ == "__main__":
    unittest.main()
