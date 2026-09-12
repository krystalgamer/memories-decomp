from __future__ import annotations

from pathlib import Path
import sys
import tempfile
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

    def test_contract_symbols_follow_used_header_asm_aliases(self) -> None:
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as directory:
            root = Path(directory)
            header = root / "aliases.h"
            source = root / "candidate.c"
            header.write_text(
                'extern long LocalName(void *) asm("CanonicalName");\n'
                'extern long UnusedName(void *) asm("UnusedCanonical");\n',
                encoding="utf-8",
            )
            source.write_text(
                '#include "aliases.h"\n'
                "long candidate(void *value) { return LocalName(value); }\n",
                encoding="utf-8",
            )

            self.assertEqual(
                candidate_builds.candidate_contract_symbols(source, source.read_text()),
                ["CanonicalName"],
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

    def test_recorded_key_survives_centralisation_and_tracks_the_header(
        self,
    ) -> None:
        """A declaration moved into a header keeps its coverage.

        This is the regression for the contract's own blind spot: before the
        retention rule, moving `extern s16 value;` out of a candidate and into
        a header dropped `value` from the contract entirely, which took the
        header's declaration out from under the gate. The key has to survive
        AND its hash has to still move when that declaration changes.
        """
        root = REPOSITORY / "tmp/test-candidate-contract-retained"
        root.mkdir(parents=True, exist_ok=True)
        try:
            # the source no longer declares it, but still uses it
            source = "#include \"value.h\"\n\nvoid f(void) { value = 1; }\n"
            own = candidate_builds.candidate_extern_symbols(source)
            self.assertEqual(own, [])

            recorded = {"value": "whatever-the-old-hash-was"}
            retained = candidate_builds.retained_contract_symbols(
                recorded, own, source
            )
            self.assertEqual(retained, ["value"])

            (root / "value.h").write_text("extern s16 value;\n", encoding="utf-8")
            index = candidate_builds.canonical_declaration_index(
                set(retained), root
            )
            self.assertEqual(
                candidate_builds.canonical_contract_sites(retained, index),
                {"value": ["value.h"]},
            )
            first = candidate_builds.canonical_contract_hashes(retained, index)

            # the declaration changes -> the retained key's hash must move
            (root / "value.h").write_text("extern s32 value;\n", encoding="utf-8")
            second = candidate_builds.canonical_contract_hashes(
                retained,
                candidate_builds.canonical_declaration_index(set(retained), root),
            )
            self.assertEqual(
                candidate_builds.canonical_contract_difference(first, second),
                ([], [], ["value"]),
            )
        finally:
            for path in sorted(root.rglob("*"), reverse=True):
                path.unlink() if path.is_file() else path.rmdir()
            root.rmdir()

    def test_retention_is_scoped_to_recorded_and_used_symbols(self) -> None:
        """The rule must not widen the contract, which is why it is scoped.

        Tracking every symbol a source references and a header declares was
        measured and changes all 115 candidate contracts. These three cases are
        what keep it from doing that.
        """
        source = "void f(void) { used = 1; }\n"
        # not recorded -> not retained, however used it is
        self.assertEqual(
            candidate_builds.retained_contract_symbols({}, [], source), []
        )
        # recorded but no longer named by the source -> not retained
        self.assertEqual(
            candidate_builds.retained_contract_symbols(
                {"gone": "h"}, [], source
            ),
            [],
        )
        # still declared by the source itself -> already a key, not a retention
        self.assertEqual(
            candidate_builds.retained_contract_symbols(
                {"used": "h"}, ["used"], source
            ),
            [],
        )
        # recorded, dropped from the source, still used -> retained
        self.assertEqual(
            candidate_builds.retained_contract_symbols(
                {"used": "h"}, [], source
            ),
            ["used"],
        )

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

    def test_overlay_header_index_sees_only_its_own_module(self) -> None:
        root = REPOSITORY / "tmp/test-candidate-contract-overlay-module"
        for directory in ("game", "overlays/password", "overlays/main_menu"):
            (root / directory).mkdir(parents=True, exist_ok=True)
        try:
            (root / "game/state.h").write_text(
                "extern s16 value;\n", encoding="utf-8"
            )
            (root / "overlays/password/state.h").write_text(
                "extern s32 value;\n", encoding="utf-8"
            )
            (root / "overlays/main_menu/state.h").write_text(
                "extern u8 value;\n", encoding="utf-8"
            )

            index = candidate_builds.canonical_declaration_index(
                {"value"}, root, overlay_module="password"
            )

            self.assertEqual(
                index["value"],
                [
                    ("game/state.h", "extern s16 value;"),
                    ("overlays/password/state.h", "extern s32 value;"),
                ],
            )
        finally:
            for path in (
                "game/state.h",
                "overlays/password/state.h",
                "overlays/main_menu/state.h",
            ):
                (root / path).unlink(missing_ok=True)
            for directory in (
                "overlays/password",
                "overlays/main_menu",
                "overlays",
                "game",
            ):
                (root / directory).rmdir()
            root.rmdir()

    def test_candidate_module_is_optional_and_checked(self) -> None:
        self.assertIsNone(candidate_builds.candidate_module({}))
        self.assertEqual(
            candidate_builds.candidate_module({"module": "password"}),
            "password",
        )
        with self.assertRaises(candidate_builds.CandidateBuildError):
            candidate_builds.candidate_module({"module": "../game"})
        self.assertEqual(
            candidate_builds.candidate_directory(Path("/x"), "password"),
            Path("/x/password"),
        )
        self.assertEqual(
            candidate_builds.candidate_directory(Path("/x"), None), Path("/x")
        )

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
