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

    def write_candidate(self) -> None:
        self.write(
            "config/slus_01411/candidates.json",
            json.dumps(
                {"schema": 2, "candidates": [{"address": "0x80010000"}]}
            ),
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

    def func_80042188_variant(self) -> str:
        return (
            "#ifdef FUNC_80042188_CANDIDATE_SPRITE_VIEW\n"
            "void func_80042188(\n"
            "    SpritePrim *, Func80028B08Ctx *, s32, s32,\n"
            "    Func80028B08Extra *\n"
            ");\n"
            "#elif defined(FUNC_80042188_SPRITE_VIEW)\n"
            "void func_80042188(SpritePrim *, u8 *, s32, s32, u8 *);\n"
            "#else\n"
            "void func_80042188(s32, u8 *, s32, s32, u8 *);\n"
            "#endif\n"
        )

    def configure_func_80042188_variant(self, declaration: str) -> None:
        self.write_inventory("func_80042188", "unmatched_asm")
        self.write("src/unmatched.h", declaration)
        self.write(
            "src/game/caller.c",
            "#include \"../unmatched.h\"\n"
            "void caller(void) { func_80042188(0, 0, 0, 0, 0); }\n",
        )

    def test_approved_central_variant_abis_are_required(self) -> None:
        self.configure_func_80042188_variant(
            "void func_80042188();\n"
            "void func_80042188();\n"
            "void func_80042188();\n"
        )
        self.assertTrue(
            any("approved selector and ABI arms" in error for error in self.errors())
        )

    def test_repeated_central_variant_arm_is_rejected(self) -> None:
        declaration = self.func_80042188_variant().replace(
            "void func_80042188(SpritePrim *, u8 *, s32, s32, u8 *);",
            "void func_80042188(s32, u8 *, s32, s32, u8 *);",
        )
        self.configure_func_80042188_variant(declaration)
        self.assertTrue(
            any("approved selector and ABI arms" in error for error in self.errors())
        )

    def test_inactive_central_variant_block_is_rejected(self) -> None:
        wrappers = (
            ("#if 0\n", ""),
            ("#if (0)\n", ""),
            ("#if(0)\n", ""),
            ("# if 0\n", ""),
            ("#if \\\n(0)\n", ""),
            ("#if 0\n", "#elif 0\n"),
            ("#if 1\n", "#elif 1\n"),
        )
        for opening, selected_arm in wrappers:
            with self.subTest(opening=opening, selected_arm=selected_arm):
                self.configure_func_80042188_variant(
                    opening
                    + selected_arm
                    + self.func_80042188_variant()
                    + "#endif\n"
                )
                self.assertTrue(
                    any(
                        "extra enclosing preprocessor arm" in error
                        for error in self.errors()
                    )
                )

    def test_include_guard_else_arm_cannot_hold_central_variant_block(self) -> None:
        self.configure_func_80042188_variant(
            "#ifndef MEMORIES_DECOMP_UNMATCHED_H\n"
            "#else\n"
            + self.func_80042188_variant()
            + "#endif\n"
        )
        self.assertTrue(
            any(
                "extra enclosing preprocessor arm" in error
                for error in self.errors()
            )
        )

    def test_header_guard_may_enclose_central_variant_block(self) -> None:
        self.configure_func_80042188_variant(
            "#ifndef MEMORIES_DECOMP_UNMATCHED_H\n"
            "#define MEMORIES_DECOMP_UNMATCHED_H\n"
            + self.func_80042188_variant()
            + "#endif\n"
        )
        self.assertEqual(self.errors(), [])

    def test_misselected_central_variant_arm_is_rejected(self) -> None:
        declaration = self.func_80042188_variant().replace(
            "FUNC_80042188_SPRITE_VIEW",
            "FUNC_80042188_WRONG_VIEW",
        )
        self.configure_func_80042188_variant(declaration)
        self.assertTrue(
            any("approved selector and ABI arms" in error for error in self.errors())
        )

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

    def test_headerless_data_is_reported_without_guessing_a_contract(self) -> None:
        self.write_linker_symbols("data = 0x80010000;\n")
        self.write(
            "src/game/caller.c",
            "extern s32 data;\nvoid caller(void) { data = 1; }\n",
        )

        errors, stats = unmatched_contracts.validate(self.root)

        self.assertEqual(errors, [])
        self.assertEqual(stats["headerless_data"], 1)
        self.assertEqual(stats["headerless_data_sites"], 1)

    def test_candidate_may_keep_its_resident_header_declaration(self) -> None:
        self.write_candidate()
        self.write("src/game/test.h", "void func_test(s32 value);\n")
        self.write(
            "src/game/caller.c",
            '#include "test.h"\nvoid caller(void) { func_test(1); }\n',
        )

        self.assertEqual(self.errors(), [])

    def test_resident_header_does_not_cover_a_non_candidate(self) -> None:
        self.write("src/game/test.h", "void func_test(s32 value);\n")
        self.write("src/game/caller.c", "void caller(void) { func_test(1); }\n")

        self.assertIn(
            "src/game/caller.c: unmatched function func_test is referenced "
            "without a declaration in src/unmatched.h",
            self.errors(),
        )

    def test_candidate_cannot_be_declared_centrally_and_in_a_header(
        self,
    ) -> None:
        self.write_candidate()
        self.write("src/unmatched.h", "void func_test(s32 value);\n")
        self.write("src/game/test.h", "void func_test(s32 value);\n")
        self.write("src/game/caller.c", "void caller(void) { func_test(1); }\n")

        self.assertIn(
            "src/unmatched.h: candidate func_test is also declared by "
            "resident headers ['src/game/test.h']",
            self.errors(),
        )

    def test_candidate_needs_a_single_home_header(self) -> None:
        self.write_candidate()
        self.write("src/game/one.h", "void func_test(s32 value);\n")
        self.write("src/game/two.h", "void func_test(s32 value);\n")
        self.write("src/game/caller.c", "void caller(void) { func_test(1); }\n")

        self.assertIn(
            "candidate func_test is declared by several resident headers: "
            "['src/game/one.h', 'src/game/two.h']",
            self.errors(),
        )

    def test_candidate_trees_are_not_home_headers(self) -> None:
        self.write_candidate()
        self.write("src/candidates/test.h", "void func_test(s32 value);\n")
        self.write("src/game/caller.c", "void caller(void) { func_test(1); }\n")

        self.assertIn(
            "src/game/caller.c: unmatched function func_test is referenced "
            "without a declaration in src/unmatched.h",
            self.errors(),
        )


if __name__ == "__main__":
    unittest.main()
