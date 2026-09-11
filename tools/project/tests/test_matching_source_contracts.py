from __future__ import annotations

import unittest

from integrate_verified_match import declared_symbol
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

    def test_statement_assembly_is_rejected(self) -> None:
        source = 'void f(void) { asm("nop"); }\n'
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

    def test_profile_requires_one_g_value(self) -> None:
        self.assertEqual(profile_g_value(["-O2", "-G8"], "flags"), 8)
        self.assertEqual(profile_g_value(["-O2", "-G", "0"], "flags"), 0)
        with self.assertRaisesRegex(Exception, "exactly one"):
            profile_g_value(["-O2"], "flags")
        with self.assertRaisesRegex(Exception, "exactly one"):
            profile_g_value(["-G0", "-G8"], "flags")

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


if __name__ == "__main__":
    unittest.main()
