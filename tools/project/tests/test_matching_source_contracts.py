from __future__ import annotations

import unittest
from pathlib import Path

from integrate_verified_match import (
    IntegrationError,
    declared_symbol,
    load_tracked_symbol_names,
    validate_effective_profile,
)
from matching_source_contracts import profile_g_value, source_violations


class MatchingSourceContractTests(unittest.TestCase):
    def test_plain_c_is_accepted(self) -> None:
        self.assertEqual(source_violations("int f(void) { return 1; }\n", set()), [])

    def test_hard_register_is_rejected_without_double_reporting(self) -> None:
        source = 'int f(void) { register int value asm("$2"); return value; }\n'
        self.assertEqual(
            source_violations(source, set()),
            ["contains a hard-register variable"],
        )

    def test_register_binding_text_in_adjacent_literals_is_accepted(self) -> None:
        source = 'const char *s = "register int r asm(" ")";\n'
        self.assertEqual(source_violations(source, set()), [])

    def test_initializer_assembly_is_not_removed_as_a_register_pin(self) -> None:
        source = (
            'void f(void) { const char *a = "register", '
            '*b = ({ asm("nop"); "x"; }); }\n'
        )
        self.assertEqual(
            source_violations(source, set()),
            ["contains statement-level assembly or an untracked assembler alias"],
        )

    def test_statement_assembly_is_rejected(self) -> None:
        source = 'void f(void) { asm("nop"); }\n'
        self.assertEqual(
            source_violations(source, set()),
            ["contains statement-level assembly or an untracked assembler alias"],
        )

    def test_comment_markers_inside_literals_do_not_hide_assembly(self) -> None:
        source = 'void f(void) { const char *s = "//"; asm("nop"); }\n'
        self.assertEqual(
            source_violations(source, set()),
            ["contains statement-level assembly or an untracked assembler alias"],
        )

    def test_assembly_word_inside_literal_is_accepted(self) -> None:
        source = 'const char *f(void) { return "asm"; }\n'
        self.assertEqual(source_violations(source, set()), [])

    def test_line_spliced_assembly_keyword_is_rejected(self) -> None:
        source = 'void f(void) { a\\\nsm("nop"); }\n'
        self.assertEqual(
            source_violations(source, set()),
            ["contains statement-level assembly or an untracked assembler alias"],
        )

    def test_tracked_symbol_alias_with_attribute_is_accepted(self) -> None:
        source = (
            'extern short viewport asm("gViewport")\n'
            '    __attribute__((section(".data")));\n'
        )
        self.assertEqual(source_violations(source, {"gViewport"}), [])

    def test_untracked_symbol_alias_is_rejected(self) -> None:
        source = 'extern short viewport asm("gViewport");\n'
        self.assertEqual(
            source_violations(source, set()),
            ["contains statement-level assembly or an untracked assembler alias"],
        )

    def test_inventory_symbol_alias_with_attribute_is_accepted(self) -> None:
        source = (
            'extern void replacement(void) asm("func_80014B30")\n'
            "    __attribute__((unused));\n"
        )
        self.assertEqual(source_violations(source, {"func_80014B30"}), [])

    def test_header_alias_does_not_invent_asm_symbol(self) -> None:
        root = Path(__file__).resolve().parents[3]
        names = load_tracked_symbol_names(root)
        self.assertNotIn("asm", names)
        self.assertEqual(
            source_violations('extern int x asm("asm");\n', names),
            ["contains statement-level assembly or an untracked assembler alias"],
        )

    def test_profile_requires_one_g_value(self) -> None:
        self.assertEqual(profile_g_value(["-O2", "-G8"], "flags"), 8)
        self.assertEqual(profile_g_value(["-O2", "-G", "0"], "flags"), 0)
        with self.assertRaisesRegex(Exception, "exactly one"):
            profile_g_value(["-O2"], "flags")
        with self.assertRaisesRegex(Exception, "exactly one"):
            profile_g_value(["-G0", "-G8"], "flags")

    def test_effective_profile_rejects_mixed_g_values(self) -> None:
        validate_effective_profile(
            {
                "compiler_flags": ["-O2", "-G8"],
                "maspsx_flags": ["-G8"],
            },
            "uniform",
        )
        with self.assertRaisesRegex(IntegrationError, "mixes compiler -G8"):
            validate_effective_profile(
                {
                    "compiler_flags": ["-O2", "-G8"],
                    "maspsx_flags": ["-G0"],
                },
                "mixed",
            )

    def test_canonical_declaration_symbol_shapes(self) -> None:
        self.assertEqual(declared_symbol(" void callback(void)"), "callback")
        self.assertEqual(declared_symbol(" void (*callback)(void)"), "callback")
        self.assertEqual(declared_symbol(" unsigned values[4]"), "values")
        self.assertEqual(
            declared_symbol(
                ' short viewport __attribute__((section(".data")))'
            ),
            "viewport",
        )
        self.assertEqual(
            declared_symbol(' int x asm("tracked_target")'),
            "x",
        )


if __name__ == "__main__":
    unittest.main()
