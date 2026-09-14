from __future__ import annotations

import csv
from pathlib import Path
import shutil
import sys
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import psyq_declaration_contracts


class PsyqDeclarationContractTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.root = REPOSITORY / "tmp/test-psyq-declaration-contracts"
        shutil.rmtree(self.root, ignore_errors=True)
        (self.root / "config/slus_01411").mkdir(parents=True)
        (self.root / "src/psyq").mkdir(parents=True)
        (self.root / "src/game").mkdir(parents=True)
        with (self.root / "config/slus_01411/functions.csv").open(
            "w", newline="", encoding="utf-8"
        ) as handle:
            writer = csv.DictWriter(
                handle,
                fieldnames=["address", "size", "name", "status", "module"],
            )
            writer.writeheader()
            writer.writerow(
                {
                    "address": "0x80070000",
                    "size": "0x10",
                    "name": "SdkCall",
                    "status": "sdk_asm",
                    "module": "psyq/sdk",
                }
            )
            writer.writerow(
                {
                    "address": "0x80020000",
                    "size": "0x10",
                    "name": "GameCall",
                    "status": "matching_c",
                    "module": "game",
                }
            )

    def tearDown(self) -> None:
        shutil.rmtree(self.root, ignore_errors=True)

    def test_psyq_header_owns_sdk_declaration(self) -> None:
        (self.root / "src/psyq/libtest.h").write_text(
            "void SdkCall(void);\n", encoding="utf-8"
        )

        errors, stats = psyq_declaration_contracts.validate(self.root)

        self.assertEqual(errors, [])
        self.assertEqual(stats["header_symbols"], 1)

    def test_game_header_cannot_redeclare_sdk_function(self) -> None:
        (self.root / "src/game/test.h").write_text(
            "void SdkCall(void);\n", encoding="utf-8"
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("must be declared by a header" in error for error in errors))

    def test_game_function_declaration_is_out_of_scope(self) -> None:
        (self.root / "src/game/test.h").write_text(
            "void GameCall(void);\n", encoding="utf-8"
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertEqual(errors, [])

    def test_object_macro_declaration_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#define SDK_NAME SdkCall\n"
            "extern int SDK_NAME(int mode);\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_function_macro_declaration_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#define SDK_DECL(name) extern int name(int mode)\n"
            "SDK_DECL(SdkCall);\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_spliced_declaration_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "extern int Sdk\\\n"
            "Call(int mode);\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_local_declaration_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "int call(void) {\n"
            "    extern int SdkCall(int mode);\n"
            "    return SdkCall(0);\n"
            "}\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_bare_local_declaration_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "int call(void) {\n"
            "    void SdkCall(void);\n"
            "    SdkCall();\n"
            "    return 0;\n"
            "}\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_macro_expanded_bare_local_declaration_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#define SDK_NAME SdkCall\n"
            "int call(void) {\n"
            "    void SDK_NAME(void);\n"
            "    SDK_NAME();\n"
            "    return 0;\n"
            "}\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_parenthesized_declarator_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "void (SdkCall)(void);\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_function_pointer_return_declarator_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "void (*SdkCall(void))(void);\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_typedef_function_declarator_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "typedef void SdkFunction(void);\n"
            "SdkFunction SdkCall;\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_later_comma_separated_declarator_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "void GameCall(void), SdkCall(void);\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_inactive_declaration_is_ignored(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#if 0\n"
            "extern int SdkCall(int mode);\n"
            "#endif\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertEqual(errors, [])

    def test_inactive_else_and_elif_declarations_are_ignored(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#define ENABLED 1\n"
            "#if ENABLED\n"
            "void GameCall(void);\n"
            "#elif 1\n"
            "void SdkCall(void);\n"
            "#else\n"
            "void SdkCall(void);\n"
            "#endif\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertEqual(errors, [])

    def test_link_alias_uses_sdk_identity(self) -> None:
        (self.root / "src/game/test.c").write_text(
            'extern long LocalName(void *) asm("SdkCall");\n',
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_macro_does_not_rewrite_link_alias_string(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#define SdkCall GameCall\n"
            'extern long LocalName(void *) asm("SdkCall");\n',
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_undef_stops_macro_expansion(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#define SDK_NAME SdkCall\n"
            "#undef SDK_NAME\n"
            "void SDK_NAME(void);\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertEqual(errors, [])

    def test_link_alias_is_valid_in_psyq_owner(self) -> None:
        (self.root / "src/psyq/libtest.h").write_text(
            'extern long LocalName(void *) asm("SdkCall");\n',
            encoding="utf-8",
        )

        errors, stats = psyq_declaration_contracts.validate(self.root)

        self.assertEqual(errors, [])
        self.assertEqual(stats["header_symbols"], 1)

    def test_reachable_non_psyq_header_is_checked(self) -> None:
        (self.root / "src/legacy_sdk_bridge.h").write_text(
            "extern int SdkCall(int mode);\n",
            encoding="utf-8",
        )
        (self.root / "src/game/test.c").write_text(
            '#include "../legacy_sdk_bridge.h"\n',
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(
            any("src/legacy_sdk_bridge.h" in error for error in errors)
        )

    def test_reachable_header_macro_expands_in_consumer(self) -> None:
        (self.root / "src/legacy_sdk_bridge.h").write_text(
            "#define SDK_DECL(name) void name(void)\n",
            encoding="utf-8",
        )
        (self.root / "src/game/test.c").write_text(
            '#include "../legacy_sdk_bridge.h"\n'
            "SDK_DECL(SdkCall);\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_zero_argument_declaration_macro_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#define SDK_DECL() void SdkCall(void)\n"
            "SDK_DECL();\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_nested_argument_declaration_macro_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#define SDK_DECL(name, args) void name args\n"
            "SDK_DECL(SdkCall, (void));\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_token_pasted_declaration_macro_is_rejected(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#define SDK_NAME(left, right) left ## right\n"
            "void SDK_NAME(Sdk, Call)(void);\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_consumer_macro_expands_inside_reachable_header(self) -> None:
        (self.root / "src/legacy_sdk_bridge.h").write_text(
            "SDK_DECL(SdkCall);\n",
            encoding="utf-8",
        )
        (self.root / "src/game/test.c").write_text(
            "#define SDK_DECL(name) void name(void)\n"
            '#include "../legacy_sdk_bridge.h"\n',
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("Psy-Q function SdkCall" in error for error in errors))

    def test_negated_enabled_expression_is_inactive(self) -> None:
        (self.root / "src/game/test.c").write_text(
            "#define ENABLED 1\n"
            "#if !ENABLED\n"
            "void SdkCall(void);\n"
            "#endif\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertEqual(errors, [])

    def test_unreachable_non_psyq_header_is_out_of_scope(self) -> None:
        (self.root / "src/legacy_sdk_bridge.h").write_text(
            "extern int SdkCall(int mode);\n",
            encoding="utf-8",
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertEqual(errors, [])


if __name__ == "__main__":
    unittest.main()
