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


class CandidateContractTests(unittest.TestCase):
    def test_extern_parser_handles_supported_forms(self) -> None:
        text = r'''
/* extern void fake_comment(void); */
extern void callback(s32, void *);
extern u8 data[];
extern s32 value __attribute__((section(".data")));
extern void (*hook)(void);
extern u8 *alias asm("real_symbol");
// extern s32 fake_line;
'''

        self.assertEqual(
            candidate_builds.candidate_extern_symbols(text),
            ["callback", "data", "hook", "real_symbol", "value"],
        )

    def test_contract_hash_is_deterministic(self) -> None:
        declarations = {
            "alpha": [("b.h", "extern s32 alpha;")],
            "beta": [
                ("a.h", "void beta(s32 value);"),
                ("z.h", "void beta(void);"),
            ],
        }
        first = candidate_builds.canonical_contract_hashes(
            ["beta", "alpha"],
            declarations,
        )
        second = candidate_builds.canonical_contract_hashes(
            ["alpha", "beta"],
            declarations,
        )

        self.assertEqual(first, second)
        self.assertEqual(
            candidate_builds.canonical_contract_hash(first),
            candidate_builds.canonical_contract_hash(second),
        )

    def test_contract_hash_changes_for_type_and_presence(self) -> None:
        absent = candidate_builds.canonical_symbol_contract_hash("value", [])
        present = candidate_builds.canonical_symbol_contract_hash(
            "value",
            [("state.h", "extern s16 value;")],
        )
        changed = candidate_builds.canonical_symbol_contract_hash(
            "value",
            [("state.h", "extern s32 value;")],
        )

        self.assertNotEqual(absent, present)
        self.assertNotEqual(present, changed)

    def test_contract_pipeline_detects_source_and_header_drift(self) -> None:
        root = REPOSITORY / "tmp/test-candidate-contract-drift"
        root.mkdir(parents=True, exist_ok=True)
        try:
            source = "extern s16 value;\n"
            symbols = candidate_builds.candidate_extern_symbols(source)
            absent = candidate_builds.canonical_contract_hashes(
                symbols,
                candidate_builds.canonical_declaration_index(
                    set(symbols),
                    root,
                ),
            )

            (root / "value.h").write_text(
                "extern s16 value;\n",
                encoding="utf-8",
            )
            present = candidate_builds.canonical_contract_hashes(
                symbols,
                candidate_builds.canonical_declaration_index(
                    set(symbols),
                    root,
                ),
            )
            self.assertEqual(
                candidate_builds.canonical_contract_difference(
                    absent,
                    present,
                ),
                ([], [], ["value"]),
            )

            (root / "value.h").write_text(
                "extern s32 value;\n",
                encoding="utf-8",
            )
            retyped = candidate_builds.canonical_contract_hashes(
                symbols,
                candidate_builds.canonical_declaration_index(
                    set(symbols),
                    root,
                ),
            )
            self.assertEqual(
                candidate_builds.canonical_contract_difference(
                    present,
                    retyped,
                ),
                ([], [], ["value"]),
            )

            expanded_symbols = candidate_builds.candidate_extern_symbols(
                source + "extern void added(void);\n"
            )
            expanded = candidate_builds.canonical_contract_hashes(
                expanded_symbols,
                candidate_builds.canonical_declaration_index(
                    set(expanded_symbols),
                    root,
                ),
            )
            self.assertEqual(
                candidate_builds.canonical_contract_difference(
                    retyped,
                    expanded,
                ),
                (["added"], [], []),
            )
            self.assertEqual(
                candidate_builds.canonical_contract_difference(
                    expanded,
                    retyped,
                ),
                ([], ["added"], []),
            )
        finally:
            (root / "value.h").unlink(missing_ok=True)
            root.rmdir()

    def test_header_index_ignores_comments_and_function_bodies(self) -> None:
        root = REPOSITORY / "tmp/test-candidate-contract-headers"
        root.mkdir(parents=True, exist_ok=True)
        try:
            (root / "state.h").write_text(
                """
/* extern s16 ignored; */
extern s16 tracked;
static void helper(void) {
    tracked = 1;
}
""",
                encoding="utf-8",
            )

            index = candidate_builds.canonical_declaration_index(
                {"ignored", "tracked"},
                root,
            )

            self.assertEqual(index["ignored"], [])
            self.assertEqual(
                index["tracked"],
                [("state.h", "extern s16 tracked;")],
            )
        finally:
            (root / "state.h").unlink(missing_ok=True)
            root.rmdir()

    def test_header_index_reads_extern_c_linkage_block(self) -> None:
        root = REPOSITORY / "tmp/test-candidate-contract-extern-c"
        root.mkdir(parents=True, exist_ok=True)
        try:
            (root / "sdk.h").write_text(
                """
#ifdef __cplusplus
extern "C" {
#endif
extern int sdk_call(int value);
#ifdef __cplusplus
}
#endif
""",
                encoding="utf-8",
            )

            index = candidate_builds.canonical_declaration_index(
                {"sdk_call"},
                root,
            )

            self.assertEqual(
                index["sdk_call"],
                [("sdk.h", "extern int sdk_call(int value);")],
            )
        finally:
            (root / "sdk.h").unlink(missing_ok=True)
            root.rmdir()

    def test_header_index_excludes_overlay_declarations(self) -> None:
        root = REPOSITORY / "tmp/test-candidate-contract-overlay"
        (root / "game").mkdir(parents=True, exist_ok=True)
        (root / "overlays").mkdir(parents=True, exist_ok=True)
        try:
            (root / "game/state.h").write_text(
                "extern s16 value;\n",
                encoding="utf-8",
            )
            (root / "overlays/state.h").write_text(
                "extern s32 value;\n",
                encoding="utf-8",
            )

            index = candidate_builds.canonical_declaration_index(
                {"value"},
                root,
            )

            self.assertEqual(
                index["value"],
                [("game/state.h", "extern s16 value;")],
            )
        finally:
            (root / "game/state.h").unlink(missing_ok=True)
            (root / "overlays/state.h").unlink(missing_ok=True)
            (root / "game").rmdir()
            (root / "overlays").rmdir()
            root.rmdir()

    def test_header_index_orders_paths_deterministically(self) -> None:
        root = REPOSITORY / "tmp/test-candidate-contract-order"
        root.mkdir(parents=True, exist_ok=True)
        try:
            (root / "z.h").write_text(
                "extern s32 value;\n",
                encoding="utf-8",
            )
            (root / "a.h").write_text(
                "extern s16 value;\n",
                encoding="utf-8",
            )

            index = candidate_builds.canonical_declaration_index(
                {"value"},
                root,
            )

            self.assertEqual(
                index["value"],
                [
                    ("a.h", "extern s16 value;"),
                    ("z.h", "extern s32 value;"),
                ],
            )
        finally:
            (root / "a.h").unlink(missing_ok=True)
            (root / "z.h").unlink(missing_ok=True)
            root.rmdir()

    def test_header_index_preserves_non_utf8_files(self) -> None:
        root = REPOSITORY / "tmp/test-candidate-contract-non-utf8"
        root.mkdir(parents=True, exist_ok=True)
        try:
            (root / "legacy.h").write_bytes(
                b"/* legacy byte: \\x89 */\nextern s32 value;\n"
            )

            index = candidate_builds.canonical_declaration_index(
                {"value"},
                root,
            )

            self.assertEqual(
                index["value"],
                [("legacy.h", "extern s32 value;")],
            )
        finally:
            (root / "legacy.h").unlink(missing_ok=True)
            root.rmdir()

    def test_contract_difference_names_each_change_kind(self) -> None:
        self.assertEqual(
            candidate_builds.canonical_contract_difference(
                {"removed": "a", "changed": "a"},
                {"added": "b", "changed": "b"},
            ),
            (["added"], ["removed"], ["changed"]),
        )

    def test_contract_validation_reports_changed_dependencies(self) -> None:
        digest = "a" * 64
        with self.assertRaisesRegex(
            candidate_builds.CandidateBuildError,
            (
                r"0x80012345: canonical contracts differ: "
                r"added=\['added'\], removed=\['removed'\], "
                r"changed=\['changed'\]"
            ),
        ):
            candidate_builds.validate_canonical_contract_metadata(
                0x80012345,
                "b" * 64,
                {"removed": digest, "changed": digest},
                {"added": digest, "changed": "c" * 64},
                {
                    "added": ["game/added.h"],
                    "changed": ["game/changed.h"],
                },
            )

    def test_contract_validation_checks_aggregate_hash(self) -> None:
        contracts = {"value": "a" * 64}
        with self.assertRaisesRegex(
            candidate_builds.CandidateBuildError,
            r"0x80012345: canonical contract hash differs:",
        ):
            candidate_builds.validate_canonical_contract_metadata(
                0x80012345,
                "b" * 64,
                contracts,
                contracts,
                {"value": ["game/value.h"]},
            )


if __name__ == "__main__":
    unittest.main()
