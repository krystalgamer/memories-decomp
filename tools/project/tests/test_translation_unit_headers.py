from __future__ import annotations

import csv
import json
from pathlib import Path
import shutil
import unittest

import translation_unit_headers


REPOSITORY = Path(__file__).resolve().parents[3]


class TranslationUnitHeaderTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.root = REPOSITORY / "tmp/test-translation-unit-headers"
        shutil.rmtree(self.root, ignore_errors=True)
        (self.root / "config/slus_01411/overlays").mkdir(parents=True)
        (self.root / "src/game").mkdir(parents=True)
        self.write_inventory(
            [
                ("func_local", "matching_c"),
                ("func_foreign", "matching_c"),
                ("func_unmatched", "unmatched_asm"),
            ]
        )
        self.write(
            "config/slus_01411/matching_c.json",
            json.dumps(
                {
                    "schema": 1,
                    "functions": [
                        {
                            "address": "0x80010000",
                            "size": "0x10",
                            "source": "src/game/example.c",
                            "profile": "test",
                        }
                    ],
                }
            ),
        )

    def tearDown(self) -> None:
        shutil.rmtree(self.root, ignore_errors=True)

    def write(self, relative: str, text: str) -> None:
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")

    def write_inventory(self, rows: list[tuple[str, str]]) -> None:
        path = self.root / "config/slus_01411/functions.csv"
        with path.open("w", encoding="utf-8", newline="") as handle:
            writer = csv.writer(handle)
            writer.writerow(("address", "size", "name", "status", "module", "notes"))
            for index, (name, status) in enumerate(rows):
                writer.writerow(
                    (f"0x{0x80010000 + index * 0x10:08X}", "0x10", name, status, "game", "")
                )

    def problems(self, source: str) -> list[str]:
        self.write("src/game/example.c", source)
        return translation_unit_headers.audit(self.root)[0]

    def test_same_unit_forward_is_accepted(self) -> None:
        self.assertEqual(
            self.problems(
                "void func_local(void);\n"
                "void func_local(void) {}\n"
            ),
            [],
        )

    def test_foreign_matching_declaration_is_rejected(self) -> None:
        problems = self.problems(
            "void func_foreign(void);\n"
            "void func_local(void) { func_foreign(); }\n"
        )
        self.assertTrue(any("func_foreign belongs in a header" in p for p in problems))

    def test_unknown_external_declaration_is_rejected(self) -> None:
        problems = self.problems(
            "void external_callback(void);\n"
            "void func_local(void) { external_callback(); }\n"
        )
        self.assertTrue(
            any("external_callback belongs in a header" in p for p in problems)
        )

    def test_unmatched_declaration_is_delegated(self) -> None:
        self.assertEqual(
            self.problems(
                "void func_unmatched(void);\n"
                "void func_local(void) { func_unmatched(); }\n"
            ),
            [],
        )

    def test_function_pointer_object_is_not_a_prototype(self) -> None:
        self.assertEqual(
            self.problems(
                "extern void (*callbacks[])(void);\n"
                "void func_local(void) {}\n"
            ),
            [],
        )

    def test_function_returning_function_pointer_is_rejected(self) -> None:
        problems = self.problems(
            "extern s32 (*func_foreign())();\n"
            "void func_local(void) { func_foreign(); }\n"
        )
        self.assertTrue(any("func_foreign belongs in a header" in p for p in problems))

    def test_callback_parameter_does_not_hide_foreign_function(self) -> None:
        problems = self.problems(
            "extern void func_foreign(void (*callback)(void));\n"
            "void func_local(void) { func_foreign(0); }\n"
        )
        self.assertTrue(any("func_foreign belongs in a header" in p for p in problems))

    def test_strings_and_macros_do_not_impersonate_definitions(self) -> None:
        problems = self.problems(
            "void func_foreign(void);\n"
            '#define UNUSED func_foreign(void) {\n'
            'const char *description = "func_foreign(void) {";\n'
            "void func_local(void) {}\n"
        )
        self.assertTrue(any("func_foreign belongs in a header" in p for p in problems))

    def test_each_function_in_multiple_declarators_is_checked(self) -> None:
        problems = self.problems(
            "void func_local(void), func_foreign(void);\n"
            "void func_local(void) {}\n"
        )
        self.assertTrue(any("func_foreign belongs in a header" in p for p in problems))

    def test_initialized_object_does_not_hide_sibling_function(self) -> None:
        problems = self.problems(
            "int state = 0, func_foreign(void);\n"
            "void func_local(void) {}\n"
        )
        self.assertTrue(any("func_foreign belongs in a header" in p for p in problems))

    def test_inactive_definition_does_not_claim_manifest_owned_function(self) -> None:
        self.write(
            "config/slus_01411/matching_c.json",
            json.dumps(
                {
                    "schema": 1,
                    "functions": [
                        {
                            "address": "0x80010000",
                            "size": "0x10",
                            "source": "src/game/example.c",
                            "profile": "test",
                        },
                        {
                            "address": "0x80010010",
                            "size": "0x10",
                            "source": "src/game/foreign.c",
                            "profile": "test",
                        },
                    ],
                }
            ),
        )
        self.write("src/game/foreign.c", "void func_foreign(void) {}\n")
        problems = self.problems(
            "int func_foreign(void);\n"
            "#if 0\n"
            "int func_foreign(void) { return 0; }\n"
            "#endif\n"
            "void func_local(void) {}\n"
        )
        self.assertTrue(any("func_foreign belongs in a header" in p for p in problems))

    def test_alias_of_same_unit_definition_is_accepted(self) -> None:
        self.assertEqual(
            self.problems(
                'void local_alias(void) asm("func_local");\n'
                "void func_local(void) {}\n"
            ),
            [],
        )

    def test_overlay_unmatched_declaration_is_not_delegated(self) -> None:
        self.write(
            "config/slus_01411/overlays/example_functions.csv",
            "address,size,name,status,module,notes\n"
            "0x80160000,0x10,func_overlay_unmatched,unmatched_asm,overlay/example,\n"
            "0x80160010,0x10,func_overlay_local,matching_c,overlay/example,\n",
        )
        self.write(
            "config/slus_01411/overlays/example_matching_c.json",
            json.dumps(
                {
                    "functions": [
                        {
                            "address": "0x80160010",
                            "source": "src/overlays/example.c",
                            "profile": "test",
                        }
                    ]
                }
            ),
        )
        self.write(
            "src/overlays/example.c",
            "void func_overlay_unmatched(void);\n"
            "void func_overlay_local(void) { func_overlay_unmatched(); }\n",
        )
        problems = translation_unit_headers.audit(self.root)[0]
        self.assertTrue(
            any("overlay unmatched function declaration" in p for p in problems)
        )

    def test_inactive_definition_does_not_claim_overlay_unmatched_function(self) -> None:
        self.write(
            "config/slus_01411/overlays/example_functions.csv",
            "address,size,name,status,module,notes\n"
            "0x80160000,0x10,func_overlay_unmatched,unmatched_asm,overlay/example,\n"
            "0x80160010,0x10,func_overlay_local,matching_c,overlay/example,\n",
        )
        self.write(
            "config/slus_01411/overlays/example_matching_c.json",
            json.dumps(
                {
                    "functions": [
                        {
                            "address": "0x80160010",
                            "source": "src/overlays/example.c",
                            "profile": "test",
                        }
                    ]
                }
            ),
        )
        self.write(
            "src/overlays/example.c",
            "void func_overlay_unmatched(void);\n"
            "#if 0\n"
            "void func_overlay_unmatched(void) {}\n"
            "#endif\n"
            "void func_overlay_local(void) { func_overlay_unmatched(); }\n",
        )
        problems = translation_unit_headers.audit(self.root)[0]
        self.assertTrue(
            any("overlay unmatched function declaration" in p for p in problems)
        )

    def test_assembler_alias_declaration_is_rejected(self) -> None:
        problems = self.problems(
            'extern void alias(void) asm("func_foreign");\n'
            "void func_local(void) { alias(); }\n"
        )
        self.assertTrue(any("alias belongs in a header" in p for p in problems))


if __name__ == "__main__":
    unittest.main()
