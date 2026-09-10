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

    def test_exact_local_exception_is_accepted(self) -> None:
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

        self.assertEqual(self.errors(), [])

    def test_exception_declaration_drift_is_actionable(self) -> None:
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
        self.assertTrue(any("is not approved" in error for error in errors))
        self.assertTrue(any("not found exactly" in error for error in errors))

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


if __name__ == "__main__":
    unittest.main()
