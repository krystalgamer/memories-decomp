from __future__ import annotations

import csv
import json
from pathlib import Path
import shutil
import sys
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import unmatched_contracts


class UnmatchedContractTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.root = REPOSITORY / "tmp/test-unmatched-contracts"
        shutil.rmtree(self.root, ignore_errors=True)
        (self.root / "config/slus_01411").mkdir(parents=True)
        (self.root / "src/game").mkdir(parents=True)
        self.write_inventory("func_test", "unmatched_asm")
        self.write_matching(["src/game/caller.c"])
        self.write("src/unmatched.h", "#include \"types.h\"\n")
        self.write_exceptions([])
        self.write_data_exceptions([])
        self.write_linker_symbols("")

    def tearDown(self) -> None:
        shutil.rmtree(self.root, ignore_errors=True)

    def write(self, relative: str, content: str) -> None:
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")

    def write_inventory(self, name: str, status: str) -> None:
        path = self.root / "config/slus_01411/functions.csv"
        with path.open("w", newline="", encoding="utf-8") as handle:
            writer = csv.DictWriter(
                handle,
                fieldnames=["address", "size", "name", "status", "module", "notes"],
            )
            writer.writeheader()
            writer.writerow(
                {
                    "address": "0x80010000",
                    "size": "0x10",
                    "name": name,
                    "status": status,
                    "module": "game",
                    "notes": "",
                }
            )

    def write_linker_symbols(self, content: str) -> None:
        self.write("config/slus_01411/c_symbols.ld", content)

    def write_matching(self, sources: list[str]) -> None:
        self.write(
            "config/slus_01411/matching_c.json",
            json.dumps(
                {
                    "functions": [
                        {
                            "address": f"0x{0x80020000 + index * 0x10:08X}",
                            "source": source,
                        }
                        for index, source in enumerate(sources)
                    ]
                }
            ),
        )

    def write_exceptions(self, exceptions: list[dict[str, str]]) -> None:
        self.write(
            "config/slus_01411/unmatched_contract_exceptions.json",
            json.dumps({"schema": 1, "exceptions": exceptions}),
        )

    def write_data_exceptions(self, exceptions: list[dict[str, str]]) -> None:
        self.write(
            "config/slus_01411/unmatched_data_contract_exceptions.json",
            json.dumps({"schema": 1, "exceptions": exceptions}),
        )

    def errors(self) -> list[str]:
        return unmatched_contracts.validate(self.root)[0]

    def test_central_declaration_covers_call_and_callback_reference(self) -> None:
        self.write("src/unmatched.h", "void func_test(s32 value);\n")
        self.write(
            "src/game/caller.c",
            "void caller(void) { func_test(1); use_callback(func_test); }\n",
        )

        self.assertEqual(self.errors(), [])

    def test_implicit_reference_requires_central_declaration(self) -> None:
        self.write("src/game/caller.c", "void caller(void) { func_test(1); }\n")

        self.assertIn(
            "src/game/caller.c: unmatched function func_test is referenced "
            "without a declaration in src/unmatched.h",
            self.errors(),
        )

    def test_unapproved_local_declaration_is_rejected(self) -> None:
        self.write(
            "src/game/caller.c",
            "void func_test(s32 value);\nvoid caller(void) { func_test(1); }\n",
        )

        self.assertTrue(
            any("local declaration of unmatched function func_test" in error
                for error in self.errors())
        )

    def test_local_exception_is_rejected_in_favour_of_guarded_header(self) -> None:
        declaration = "void func_test(s32 value);"
        self.write(
            "src/game/caller.c",
            declaration + "\nvoid caller(void) { func_test(1); }\n",
        )
        self.write_exceptions(
            [
                {
                    "symbol": "func_test",
                    "source": "src/game/caller.c",
                    "declaration": declaration,
                    "reason": "The caller-visible spelling is load-bearing.",
                }
            ]
        )

        errors = self.errors()
        self.assertTrue(any("exceptions are no longer supported" in error
                            for error in errors))
        self.assertTrue(any("local declaration of unmatched function func_test"
                            in error for error in errors))

    def test_local_exception_drift_remains_actionable(self) -> None:
        self.write(
            "src/game/caller.c",
            "void func_test(u32 value);\nvoid caller(void) { func_test(1); }\n",
        )
        self.write_exceptions(
            [
                {
                    "symbol": "func_test",
                    "source": "src/game/caller.c",
                    "declaration": "void func_test(s32 value);",
                    "reason": "The caller-visible spelling is load-bearing.",
                }
            ]
        )

        errors = self.errors()
        self.assertTrue(any("exceptions are no longer supported" in error
                            for error in errors))
        self.assertTrue(any("local declaration of unmatched function func_test"
                            in error for error in errors))

    def test_stale_central_declaration_is_rejected(self) -> None:
        self.write_inventory("func_test", "matching_c")
        self.write("src/unmatched.h", "void func_test(s32 value);\n")
        self.write("src/game/caller.c", "void caller(void) {}\n")

        self.assertTrue(
            any("functions.csv status is matching_c" in error
                for error in self.errors())
        )

    def test_removed_central_declaration_is_rejected(self) -> None:
        self.write("src/unmatched.h", "void func_removed(s32 value);\n")
        self.write("src/game/caller.c", "void caller(void) {}\n")

        self.assertTrue(
            any("func_removed: functions.csv status is absent" in error
                for error in self.errors())
        )

    def test_exact_legacy_handwritten_alias_is_recognized(self) -> None:
        self.write_inventory("renderer", "handwritten_asm")
        self.write(
            "config/slus_01411/link_symbols.ld",
            "func_80010000 = renderer;\n",
        )
        self.assertEqual(
            unmatched_contracts.legacy_handwritten_declarations(self.root),
            {"func_80010000"},
        )

    def test_legacy_alias_requires_a_terminating_semicolon(self) -> None:
        self.write_inventory("renderer", "handwritten_asm")
        for source in (
            "func_80010000 = renderer",
            "func_80010000 = renderer\n",
            "func_80010000 = renderer /* no delimiter */\n",
            "unrelated = renderer;\nfunc_80010000 = renderer\n",
        ):
            with self.subTest(source=source):
                self.write("config/slus_01411/link_symbols.ld", source)
                self.assertEqual(
                    unmatched_contracts.legacy_handwritten_declarations(
                        self.root,
                    ),
                    set(),
                )

    def test_legacy_alias_requires_the_complete_linker_lhs(self) -> None:
        self.write_inventory("renderer", "handwritten_asm")
        for name in (
            "prefix.func_80010000", ".func_80010000",
            "prefix$func_80010000", "prefix_func_80010000",
            '"prefix.func_80010000"',
        ):
            with self.subTest(name=name):
                self.write(
                    "config/slus_01411/link_symbols.ld",
                    f"{name} = renderer;\n",
                )
                self.assertEqual(
                    unmatched_contracts.legacy_handwritten_declarations(
                        self.root,
                    ),
                    set(),
                )
        self.write(
            "config/slus_01411/link_symbols.ld",
            "func_80010000 = renderer;\nprefix.func_80010000 = renderer;\n",
        )
        self.assertEqual(
            unmatched_contracts.legacy_handwritten_declarations(self.root),
            {"func_80010000"},
        )

    def test_legacy_alias_uses_quoted_linker_symbol_boundaries(self) -> None:
        self.write_inventory("renderer", "handwritten_asm")
        valid = "func_80010000 = renderer;\n"
        quoted = '"prefix;func_80010000 = renderer;tail" = 0;\n'
        cases = (
            (quoted, set()),
            (valid + quoted, {"func_80010000"}),
            (valid + '"func_80010000" = renderer;\n', set()),
            (valid + '"func_80010000" = absent;\n', set()),
            (valid + valid, set()),
            (
                '"prefix\\\";func_80010000 = renderer;tail" = 0;\n',
                set(),
            ),
            (valid + '"prefix\\\";unrelated;tail" = 0;\n', set()),
            (valid + '"unterminated;func_80010000 = renderer;\n', set()),
        )
        for source, expected in cases:
            with self.subTest(source=source):
                self.write("config/slus_01411/link_symbols.ld", source)
                self.assertEqual(
                    unmatched_contracts.legacy_handwritten_declarations(
                        self.root,
                    ),
                    expected,
                )

    def test_legacy_alias_counts_hyphenated_linker_names_whole(self) -> None:
        self.write_inventory("renderer", "handwritten_asm")
        valid = "func_80010000 = renderer;\n"
        for prefix in ("prefix-", "prefix.", "prefix$", "prefix_"):
            with self.subTest(prefix=prefix):
                self.write(
                    "config/slus_01411/link_symbols.ld",
                    valid + f"{prefix}func_80010000 = 0;\n",
                )
                self.assertEqual(
                    unmatched_contracts.legacy_handwritten_declarations(
                        self.root,
                    ),
                    {"func_80010000"},
                )
        self.write(
            "config/slus_01411/link_symbols.ld",
            valid + "prefix-func_80010000 = func_80010000;\n",
        )
        self.assertEqual(
            unmatched_contracts.legacy_handwritten_declarations(self.root),
            set(),
        )

    def test_invalid_legacy_alias_metadata_is_rejected(self) -> None:
        valid = "func_80010000 = renderer;\n"
        cases = {
            "missing": "",
            "missing-target": "func_80010000 = absent;\n",
            "wrong-address": "func_80010004 = renderer;\n",
            "chain": (
                "func_80010000 = intermediate;\n"
                "intermediate = renderer;\n"
            ),
            "duplicate": valid + valid,
            "malformed": "func_80010000 = renderer + 0;\n",
            "compound": valid + "func_80010000 += 4;\n",
            "wrapped": "HIDDEN(func_80010000 = renderer);\n",
            "wrapped-override": valid + "HIDDEN(func_80010000 = absent);\n",
            "provide": "PROVIDE(func_80010000 = renderer);\n",
            "provide-override": valid + "PROVIDE(func_80010000 = absent);\n",
        }
        for label, source in cases.items():
            with self.subTest(case=label):
                self.write_inventory("renderer", "handwritten_asm")
                self.write("config/slus_01411/link_symbols.ld", source)
                self.assertEqual(
                    unmatched_contracts.legacy_handwritten_declarations(
                        self.root,
                    ),
                    set(),
                )
        for status in ("matching_c", "unmatched_asm", "sdk_asm"):
            with self.subTest(status=status):
                self.write_inventory("renderer", status)
                self.write("config/slus_01411/link_symbols.ld", valid)
                self.assertEqual(
                    unmatched_contracts.legacy_handwritten_declarations(
                        self.root,
                    ),
                    set(),
                )
        self.write_inventory("renderer", "handwritten_asm")
        path = self.root / "config/slus_01411/functions.csv"
        with path.open(newline="", encoding="utf-8") as handle:
            reader = csv.DictReader(handle)
            fields = reader.fieldnames
            rows = list(reader)
        assert fields is not None
        rows[0]["module"] = "psyq"
        with path.open("w", newline="", encoding="utf-8") as handle:
            writer = csv.DictWriter(handle, fieldnames=fields)
            writer.writeheader()
            writer.writerows(rows)
        self.write("config/slus_01411/link_symbols.ld", valid)
        self.assertEqual(
            unmatched_contracts.legacy_handwritten_declarations(self.root),
            set(),
        )

    def test_data_attributes_are_not_function_declarations(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write(
            "src/unmatched.h",
            'extern s32 data __attribute__((section(".data")));\n',
        )
        self.write("src/game/caller.c", "void caller(void) {}\n")

        self.assertEqual(self.errors(), [])

    def test_comments_strings_and_inline_assembly_do_not_count(self) -> None:
        self.write(
            "src/game/caller.c",
            """
/* func_test(1); */
const char *text = "func_test";
void caller(void) { asm volatile("jal func_test"); }
""",
        )

        self.assertEqual(self.errors(), [])

    def test_unreferenced_assembly_function_needs_no_invented_signature(self) -> None:
        self.write("src/game/caller.c", "void caller(void) {}\n")

        self.assertEqual(self.errors(), [])

    def test_object_parser_finds_every_supported_declarator(self) -> None:
        symbols = {
            "data",
            "array",
            "pointer",
            "qualified",
            "real_alias",
            "hook",
            "callback",
        }
        text = r'''
/* extern s32 fake_comment; */
extern s32 data, array[];
extern u8 *pointer;
extern volatile s16 qualified __attribute__((section(".data")));
extern u8 *local_alias asm("real_alias");
extern void (*hook)(void);
extern void callback(s32);
const char *text = "data array pointer";
'''

        self.assertEqual(
            unmatched_contracts.extern_object_declarations(text, symbols),
            [
                ("array", "extern s32 data, array[];"),
                ("data", "extern s32 data, array[];"),
                ("pointer", "extern u8 *pointer;"),
                (
                    "qualified",
                    'extern volatile s16 qualified '
                    '__attribute__((section(".data")));',
                ),
                ("real_alias", 'extern u8 *local_alias asm("real_alias");'),
                ("hook", "extern void (*hook)(void);"),
            ],
        )

    def test_linker_symbol_parser_ignores_non_assignments(self) -> None:
        self.write_linker_symbols(
            """
data = 0x80010000;
alias = other;
func = 0x80020000; // still an assignment
"""
        )

        self.assertEqual(
            unmatched_contracts.linker_symbols(self.root),
            {"data", "func"},
        )

    def test_central_data_rejects_unapproved_local_declaration(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write("src/unmatched.h", "extern s32 data;\n")
        self.write(
            "src/game/caller.c",
            "extern s32 data;\nvoid caller(void) { data = 1; }\n",
        )

        self.assertTrue(
            any("local declaration of central unmatched data data" in error
                for error in self.errors())
        )

    def test_central_data_rejects_removed_linker_assignment(self) -> None:
        self.write("src/unmatched.h", "extern s32 data;\n")
        self.write("src/game/caller.c", "void caller(void) {}\n")

        self.assertTrue(
            any("stale data declaration data" in error for error in self.errors())
        )

    def test_central_data_rejects_subsystem_header_takeover(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write("src/unmatched.h", "extern s32 data;\n")
        self.write("src/game/state.h", "extern s32 data;\n")
        self.write("src/game/caller.c", "void caller(void) {}\n")

        self.assertTrue(
            any("resident headers ['game/state.h']" in error
                for error in self.errors())
        )

    def test_exact_data_exception_allows_local_codegen_view(self) -> None:
        declaration = (
            'extern s32 data __attribute__((section(".data")));'
        )
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write("src/unmatched.h", "extern s32 data;\n")
        self.write(
            "src/game/caller.c",
            declaration + "\nvoid caller(void) { data = 1; }\n",
        )
        self.write_data_exceptions(
            [
                {
                    "symbol": "data",
                    "source": "src/game/caller.c",
                    "declaration": declaration,
                    "reason": "Absolute addressing is load-bearing.",
                }
            ]
        )

        self.assertEqual(self.errors(), [])

    def test_data_exception_may_reference_a_resident_owner_header(self) -> None:
        declaration = "extern u8 data[];"
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write("src/game/data.h", "extern s32 data;\n")
        self.write(
            "src/game/caller.c",
            declaration + "\nvoid caller(void) { data[0] = 1; }\n",
        )
        self.write_data_exceptions(
            [
                {
                    "symbol": "data",
                    "source": "src/game/caller.c",
                    "declaration": declaration,
                    "reason": "The byte-oriented codegen view is load-bearing.",
                }
            ]
        )

        self.assertEqual(self.errors(), [])

    def test_data_exception_drift_is_actionable(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write("src/unmatched.h", "extern s32 data;\n")
        self.write(
            "src/game/caller.c",
            "extern volatile s32 data;\nvoid caller(void) { data = 1; }\n",
        )
        self.write_data_exceptions(
            [
                {
                    "symbol": "data",
                    "source": "src/game/caller.c",
                    "declaration": "extern s32 data;",
                    "reason": "The local view is load-bearing.",
                }
            ]
        )

        errors = self.errors()
        self.assertTrue(any("is not approved" in error for error in errors))
        self.assertTrue(any("not found exactly" in error for error in errors))

    def test_distinct_guarded_central_data_views_are_allowed(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write(
            "src/unmatched.h",
            """
#ifdef DATA_POINTER
extern u8 *data;
#else
extern s32 data;
#endif
""",
        )
        self.write("src/game/caller.c", "void caller(void) {}\n")

        self.assertEqual(self.errors(), [])

    def test_function_pointer_data_is_not_a_function_prototype(self) -> None:
        self.write_linker_symbols("D_80010000 = 0x80010000;\n")
        self.write(
            "src/unmatched.h",
            "extern void (*D_80010000)(void);\n",
        )
        self.write("src/game/caller.c", "void caller(void) {}\n")

        self.assertEqual(self.errors(), [])

    def test_duplicate_central_data_declaration_is_rejected(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write(
            "src/unmatched.h",
            "extern s32 data;\nextern s32 data;\n",
        )
        self.write("src/game/caller.c", "void caller(void) {}\n")

        self.assertTrue(
            any("duplicate data declarations for data" in error
                for error in self.errors())
        )

    def test_headerless_data_is_rejected_without_guessing_a_contract(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write(
            "src/game/caller.c",
            "extern s32 data;\nvoid caller(void) { data = 1; }\n",
        )

        errors, stats = unmatched_contracts.validate(self.root)

        self.assertIn(
            "src/game/caller.c: local declaration of unmatched data data "
            "has no owner header: extern s32 data;",
            errors,
        )
        self.assertEqual(stats["headerless_data"], 1)
        self.assertEqual(stats["headerless_data_sites"], 1)

    def test_local_data_with_owner_header_is_allowed(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write("src/game/data.h", "extern s32 data;\n")
        self.write(
            "src/game/caller.c",
            '#include "data.h"\n'
            "extern s32 data;\n"
            "void caller(void) { data = 1; }\n",
        )

        errors, stats = unmatched_contracts.validate(self.root)

        self.assertEqual(errors, [])
        self.assertEqual(stats["headerless_data"], 0)
        self.assertEqual(stats["headerless_data_sites"], 0)

    def test_unincluded_owner_header_does_not_cover_local_data(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write("src/game/data.h", "extern s32 data;\n")
        self.write(
            "src/game/caller.c",
            "extern s32 data;\n"
            "void caller(void) { data = 1; }\n",
        )

        errors, _ = unmatched_contracts.validate(self.root)

        self.assertTrue(
            any("local declaration of unmatched data data has no owner header"
                in error for error in errors)
        )

    def test_inactive_guarded_owner_does_not_cover_local_data(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write(
            "src/game/data.h",
            "#ifdef ENABLED\n"
            "extern s32 data;\n"
            "#endif\n",
        )
        self.write(
            "src/game/caller.c",
            '#include "data.h"\n'
            "extern s32 data;\n"
            "void caller(void) { data = 1; }\n",
        )

        errors, _ = unmatched_contracts.validate(self.root)

        self.assertTrue(
            any("local declaration of unmatched data data has no owner header"
                in error for error in errors)
        )

    def test_composite_inactive_guards_do_not_cover_local_data(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write(
            "src/game/caller.c",
            '#include "data.h"\n'
            "extern s32 data;\n"
            "void caller(void) { data = 1; }\n",
        )
        for condition in (
            "!defined(DISABLED) && defined(ENABLED)",
            "defined(ENABLED) || defined(FALLBACK)",
            "(defined(ENABLED) && !defined(DISABLED))",
        ):
            with self.subTest(condition=condition):
                self.write(
                    "src/game/data.h",
                    f"#if {condition}\nextern s32 data;\n#endif\n",
                )
                errors, _ = unmatched_contracts.validate(self.root)
                self.assertTrue(
                    any(
                        "local declaration of unmatched data data has no owner header"
                        in error
                        for error in errors
                    )
                )

    def test_condition_evaluator_handles_boolean_precedence_and_parentheses(self) -> None:
        enabled = {"ENABLED"}
        self.assertTrue(unmatched_contracts.condition_enabled(
            "!defined(DISABLED) && defined(ENABLED)", enabled
        ))
        self.assertTrue(unmatched_contracts.condition_enabled(
            "defined(DISABLED) || (defined(ENABLED) && !defined(OTHER))",
            enabled,
        ))
        self.assertFalse(unmatched_contracts.condition_enabled(
            "defined(DISABLED) || defined(OTHER) && defined(ENABLED)", enabled
        ))
        self.assertFalse(unmatched_contracts.condition_enabled(
            "(!defined(DISABLED) && defined(ENABLED)) == 0", enabled
        ))
        macros = {
            "ZERO": "0",
            "FOUR": "2 + 2",
            "ALIAS": "FOUR",
            "FUNCTION": None,
        }
        self.assertFalse(unmatched_contracts.condition_enabled("ZERO", macros))
        self.assertTrue(unmatched_contracts.condition_enabled(
            "ALIAS == 4 && defined(FUNCTION) && !FUNCTION", macros
        ))
        self.assertTrue(unmatched_contracts.condition_enabled(
            "(-3 % 2) == -1", macros
        ))
        self.assertFalse(unmatched_contracts.condition_enabled(
            "(-3 % 2) == 1", macros
        ))

    def test_zero_valued_macro_does_not_activate_an_owner(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write(
            "src/game/data.h",
            "#define ENABLED 0\n"
            "#if ENABLED\n"
            "extern s32 data;\n"
            "#endif\n",
        )
        self.write(
            "src/game/caller.c",
            '#include "data.h"\n'
            "extern s32 data;\n"
            "void caller(void) { data = 1; }\n",
        )

        errors, _ = unmatched_contracts.validate(self.root)

        self.assertTrue(
            any(
                "local declaration of unmatched data data has no owner header"
                in error
                for error in errors
            )
        )

    def test_unsupported_condition_fails_closed(self) -> None:
        with self.assertRaisesRegex(
            unmatched_contracts.ContractError,
            "unsupported preprocessor condition",
        ):
            unmatched_contracts.condition_enabled("ENABLED ? 1 : 0", {"ENABLED"})

    def test_every_compiler_profile_requires_an_active_owner(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write(
            "src/game/data.h",
            "#ifdef ENABLED\n"
            "extern s32 data;\n"
            "#endif\n",
        )
        self.write(
            "src/game/caller.c",
            '#include "data.h"\n'
            "extern s32 data;\n"
            "void caller(void) { data = 1; }\n",
        )
        self.write(
            "config/slus_01411/matching_c.json",
            json.dumps(
                {
                    "functions": [
                        {
                            "address": "0x80020000",
                            "source": "src/game/caller.c",
                            "profile": "enabled",
                        },
                        {
                            "address": "0x80020010",
                            "source": "src/game/caller.c",
                            "profile": "disabled",
                        },
                    ]
                }
            ),
        )
        self.write(
            "config/slus_01411/compiler_profiles.json",
            json.dumps(
                {
                    "profiles": {
                        "enabled": {"compiler_flags": ["-DENABLED"]},
                        "disabled": {"compiler_flags": []},
                    }
                }
            ),
        )

        errors, stats = unmatched_contracts.validate(self.root)

        self.assertTrue(any("under profile disabled" in error for error in errors))
        self.assertFalse(any("under profile enabled" in error for error in errors))
        self.assertEqual(stats["headerless_data"], 1)
        self.assertEqual(stats["headerless_data_sites"], 1)

    def test_resident_header_does_not_cover_a_non_candidate(self) -> None:
        self.write("src/game/test.h", "void func_test(s32 value);\n")
        self.write("src/game/caller.c", "void caller(void) { func_test(1); }\n")

        self.assertIn(
            "src/game/caller.c: unmatched function func_test is referenced "
            "without a declaration in src/unmatched.h",
            self.errors(),
        )

    def test_shared_abi_dispatcher_may_own_an_unmatched_function(self) -> None:
        self.write_inventory("DisplayObject_SubmitPacket", "unmatched_asm")
        self.write(
            "src/game/display_object_packet_submit.h",
            "void DisplayObject_SubmitPacket(s32 value);\n",
        )
        self.write(
            "src/game/caller.c",
            '#include "display_object_packet_submit.h"\n'
            "void caller(void) { DisplayObject_SubmitPacket(1); }\n",
        )

        self.assertEqual(self.errors(), [])

    def test_model_slot_header_may_own_an_unmatched_function(self) -> None:
        self.write_inventory("Model_InitSlot", "unmatched_asm")
        self.write(
            "src/game/model_slot_control.h",
            "void Model_InitSlot(s32, s32, s32, s32);\n",
        )
        self.write(
            "src/game/caller.c",
            '#include "model_slot_control.h"\n'
            "void caller(void) { Model_InitSlot(0, 0, 0, 0); }\n",
        )

        self.assertEqual(self.errors(), [])



if __name__ == "__main__":
    unittest.main()
