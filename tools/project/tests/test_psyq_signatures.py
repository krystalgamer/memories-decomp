from __future__ import annotations

from contextlib import redirect_stderr, redirect_stdout
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from psyq_signatures import (
    SignatureError,
    catalogue_sha256,
    classify,
    evidence,
    find_matches,
    load_resolutions,
    load_resolution_entries,
    main,
    parse_signature,
    report_coverage,
    report,
    scan,
    validate_catalogue_scope,
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

    def test_evidence_uses_selected_catalogue_version(self) -> None:
        providers = ["LIBDS.LIB/DSSYS.OBJ+0x0"]

        self.assertIn("Psy-Q 4.6", evidence(providers))
        self.assertIn("Psy-Q 4.7", evidence(providers, "4.7"))

    def test_catalogue_scope_limits_version_47_to_libds(self) -> None:
        validate_catalogue_scope(self.signatures, "4.6")
        with self.assertRaisesRegex(
            SignatureError,
            "Psy-Q 4[.]7 is permitted only",
        ):
            validate_catalogue_scope(self.signatures, "4.7")

        (self.signatures / "LIBDS.LIB.json").write_text(
            "[]", encoding="utf-8"
        )
        validate_catalogue_scope(self.signatures, "4.7")

        (self.signatures / "LIBGPU.LIB.json").write_text(
            "[]", encoding="utf-8"
        )
        with self.assertRaisesRegex(
            SignatureError,
            "directory containing exactly LIBDS[.]LIB[.]json",
        ):
            validate_catalogue_scope(self.signatures, "4.7")

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

    def test_resolution_ledger_pins_catalogue_and_validates_entries(self) -> None:
        root = self.signatures / "repository"
        catalogue = root / "catalogue"
        config = root / "config/slus_01411"
        catalogue.mkdir(parents=True)
        config.mkdir(parents=True)
        (catalogue / "LIBTEST.LIB.json").write_text(
            "[]", encoding="utf-8"
        )
        document = {
            "schema": 1,
            "catalogues": {
                "4.6": {
                    "sha256": catalogue_sha256(catalogue),
                    "resolutions": [
                        {
                            "address": "0x80010000",
                            "catalogue_names": ["AliasA", "AliasB"],
                            "selected_name": "AliasA",
                            "basis": "evidence",
                            "evidence": "Call graph.",
                        }
                    ],
                }
            },
        }
        (config / "psyq_signature_resolutions.json").write_text(
            json.dumps(document), encoding="utf-8"
        )

        self.assertEqual(
            load_resolutions(root, catalogue, "4.6"),
            {
                0x80010000: {
                    "catalogue_names": ["AliasA", "AliasB"],
                    "selected_name": "AliasA",
                    "basis": "evidence",
                    "evidence": "Call graph.",
                }
            },
        )

        (catalogue / "LIBTEST.LIB.json").write_text(
            "[ ]", encoding="utf-8"
        )
        with self.assertRaisesRegex(SignatureError, "catalogue hash differs"):
            load_resolutions(root, catalogue, "4.6")

    def test_resolution_loader_rejects_malformed_policy_fields(self) -> None:
        root = self.signatures / "repository"
        config = root / "config/slus_01411"
        config.mkdir(parents=True)
        path = config / "psyq_signature_resolutions.json"

        path.write_text("[]", encoding="utf-8")
        with self.assertRaisesRegex(SignatureError, "document is not an object"):
            load_resolution_entries(root, "4.6")

        entry = {
            "address": "0x80010000",
            "catalogue_names": ["AliasA"],
            "selected_name": "AliasA",
            "basis": "evidence",
            "evidence": "Call graph.",
        }
        document = {
            "schema": 1,
            "catalogues": {
                "4.6": {
                    "sha256": "0" * 64,
                    "resolutions": [entry],
                }
            },
        }
        for field, value, message in (
            ("basis", [], "basis must be evidence or naming_policy"),
            ("evidence", " \t", "has invalid evidence"),
        ):
            with self.subTest(field=field):
                entry[field] = value
                path.write_text(json.dumps(document), encoding="utf-8")
                with self.assertRaisesRegex(SignatureError, message):
                    load_resolution_entries(root, "4.6")
                entry[field] = (
                    "evidence" if field == "basis" else "Call graph."
                )

    def test_check_resolutions_cli_reports_policy_errors_as_exit_two(self) -> None:
        root = self.signatures / "repository"
        config = root / "config/slus_01411"
        config.mkdir(parents=True)
        (config / "functions.csv").write_text(
            "address,size,name,status,module,notes\n"
            "0x80010000,0x20,AliasA,sdk_asm,psyq/sdk,\n",
            encoding="utf-8",
        )
        path = config / "psyq_signature_resolutions.json"
        valid_entry = {
            "address": "0x80010000",
            "catalogue_names": ["AliasA"],
            "selected_name": "AliasA",
            "basis": "evidence",
            "evidence": "Call graph.",
        }
        valid_document = {
            "schema": 1,
            "catalogues": {
                version: {
                    "sha256": "0" * 64,
                    "resolutions": [dict(valid_entry)],
                }
                for version in ("4.6", "4.7")
            },
        }
        cases = (
            ([], "document is not an object"),
            (
                {
                    **valid_document,
                    "catalogues": {
                        **valid_document["catalogues"],
                        "4.6": {
                            **valid_document["catalogues"]["4.6"],
                            "resolutions": [
                                {**valid_entry, "basis": ["evidence"]}
                            ],
                        },
                    },
                },
                "basis must be evidence or naming_policy",
            ),
            (
                {
                    **valid_document,
                    "catalogues": {
                        **valid_document["catalogues"],
                        "4.6": {
                            **valid_document["catalogues"]["4.6"],
                            "resolutions": [
                                {**valid_entry, "evidence": "   "}
                            ],
                        },
                    },
                },
                "has invalid evidence",
            ),
        )
        for document, message in cases:
            with self.subTest(message=message):
                path.write_text(json.dumps(document), encoding="utf-8")
                stderr = io.StringIO()
                with (
                    patch("psyq_signatures.ROOT", root),
                    patch.object(
                        sys,
                        "argv",
                        ["psyq_signatures.py", "--check-resolutions"],
                    ),
                    redirect_stderr(stderr),
                ):
                    self.assertEqual(main(), 2)
                self.assertIn(message, stderr.getvalue())

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
            result["objects"],
            [
                {
                    "provider": "LIBTEST.LIB/UNIQUE.OBJ",
                    "start": 0x80010000,
                    "end": 0x80010008,
                }
            ],
        )
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
            0x80010034: {
                "NamedAliasA": ["LIBA/NAMED.OBJ+0x0"],
                "NamedAliasB": ["LIBB/NAMED.OBJ+0x0"],
            },
            0x80010040: {
                "OffStartA": ["LIBA/OFFSTART.OBJ+0x4"],
                "OffStartB": ["LIBB/OFFSTART.OBJ+0x4"],
            },
            0x80010050: {
                "GameCollisionA": ["LIBA/GAME_COLLISION.OBJ+0x0"],
                "GameCollisionB": ["LIBB/GAME_COLLISION.OBJ+0x0"],
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
            0x80010030: {
                "name": "func_80010030",
                "status": "sdk_asm",
                "module": "psyq/sdk",
            },
            0x80010034: {
                "name": "LocallyResolvedName",
                "status": "sdk_asm",
                "module": "psyq/sdk",
            },
            0x80010050: {
                "name": "func_80010050",
                "size": "0x20",
                "status": "matching_c",
                "module": "game",
            },
            0x80010060: {
                "name": "func_80010060",
                "size": "0x30",
                "status": "sdk_asm",
                "module": "psyq/sdk",
            },
        }

        objects = [
            {
                "provider": "LIB/NEW.OBJ",
                "start": 0x80010010,
                "end": 0x80010040,
            },
            {
                "provider": "LIB/UNNAMED.OBJ",
                "start": 0x80010060,
                "end": 0x80010090,
            },
        ]

        result = classify(proposals, inventory, objects)

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
            [
                (address, row["name"], providers)
                for address, row, providers
                in result["object_covered_inventory"]
            ],
            [
                (0x80010010, "func_80010010", ["LIB/NEW.OBJ"]),
                (0x80010030, "func_80010030", ["LIB/NEW.OBJ"]),
                (0x80010060, "func_80010060", ["LIB/UNNAMED.OBJ"]),
            ],
        )
        self.assertEqual(result["object_uncovered_inventory"], [])
        self.assertEqual(
            result["ambiguous"],
            [
                (0x80010030, ["AliasA", "AliasB"]),
                (0x80010034, ["NamedAliasA", "NamedAliasB"]),
            ],
        )
        self.assertEqual(
            result["ambiguous_locally_named"],
            [
                (
                    0x80010034,
                    ["NamedAliasA", "NamedAliasB"],
                    "LocallyResolvedName",
                )
            ],
        )
        self.assertEqual(
            result["ambiguous_unresolved"],
            [
                (
                    0x80010030,
                    ["AliasA", "AliasB"],
                    "func_80010030",
                )
            ],
        )
        self.assertEqual(
            result["outside_psyq"],
            [
                (
                    0x80010050,
                    ["GameCollisionA", "GameCollisionB"],
                    "func_80010050",
                    "matching_c",
                    "game",
                    [
                        "LIBA/GAME_COLLISION.OBJ+0x0",
                        "LIBB/GAME_COLLISION.OBJ+0x0",
                    ],
                )
            ],
        )
        self.assertEqual(result["off_start"], 1)
        self.assertEqual(
            [
                (address, row["name"])
                for address, row in result["address_named_inventory"]
            ],
            [
                (0x80010010, "func_80010010"),
                (0x80010030, "func_80010030"),
                (0x80010060, "func_80010060"),
            ],
        )

    def test_coverage_report_is_machine_readable(self) -> None:
        result = {
            "object_covered_inventory": [
                (
                    0x80010010,
                    {"size": "0x20", "name": "func_80010010"},
                    ["LIBA/A.OBJ", "LIBB/B.OBJ"],
                )
            ]
        }

        from contextlib import redirect_stdout
        from io import StringIO

        output = StringIO()
        with redirect_stdout(output):
            report_coverage(result)

        self.assertEqual(
            output.getvalue(),
            "address,size,name,unique_signature_objects\n"
            "0x80010010,0x20,func_80010010,"
            "LIBA/A.OBJ;LIBB/B.OBJ\n",
        )

    def test_classify_applies_and_checks_local_resolutions(self) -> None:
        proposals = {
            0x80010000: {
                "AliasA": ["LIBA/SHARED.OBJ+0x0"],
                "AliasB": ["LIBB/SHARED.OBJ+0x0"],
            }
        }
        inventory = {
            0x80010000: {
                "name": "LocalName",
                "status": "sdk_asm",
                "module": "psyq/sdk",
            }
        }
        resolutions = {
            0x80010000: {
                "catalogue_names": ["AliasA", "AliasB"],
                "selected_name": "LocalName",
                "basis": "evidence",
                "evidence": "Call graph.",
            }
        }

        result = classify(proposals, inventory, resolutions=resolutions)

        self.assertEqual(
            result["resolved"],
            [
                (
                    0x80010000,
                    "LocalName",
                    ["AliasA", "AliasB"],
                    "evidence",
                    "Call graph.",
                )
            ],
        )
        self.assertEqual(result["ambiguous"], [])

        resolutions[0x80010000]["catalogue_names"] = ["AliasA"]
        with self.assertRaisesRegex(
            SignatureError, "resolution catalogue names differ"
        ):
            classify(proposals, inventory, resolutions=resolutions)

    def test_report_distinguishes_evidence_from_naming_policy(self) -> None:
        output = io.StringIO()
        scanned = {
            "unique": 2,
            "multiple": 0,
            "absent": 0,
            "unanchored": 0,
        }
        result = {
            "agreed": [],
            "disagreed": [],
            "new": [],
            "resolved": [
                (0x80010000, "AliasA", ["AliasA", "AliasB"], "evidence", "Call graph."),
                (
                    0x80010010,
                    "AliasC",
                    ["AliasC", "AliasD"],
                    "naming_policy",
                    "Retained project alias.",
                ),
            ],
            "ambiguous": [],
            "ambiguous_locally_named": [],
            "ambiguous_unresolved": [],
            "address_named_inventory": [],
            "object_covered_inventory": [],
            "object_uncovered_inventory": [],
            "outside_psyq": [],
            "off_start": 0,
        }

        with redirect_stdout(output):
            report(scanned, result)

        self.assertIn(
            "catalogue conflicts resolved by evidence : 1", output.getvalue()
        )
        self.assertIn(
            "catalogue names retained by policy       : 1", output.getvalue()
        )

    def test_classify_rejects_unused_resolution(self) -> None:
        resolutions = {
            0x80010000: {
                "catalogue_names": ["AliasA"],
                "selected_name": "AliasA",
                "basis": "evidence",
                "evidence": "Call graph.",
            }
        }

        with self.assertRaisesRegex(
            SignatureError, "resolutions have no catalogue proposal"
        ):
            classify({}, {}, resolutions=resolutions)


if __name__ == "__main__":
    unittest.main()
