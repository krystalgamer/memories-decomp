from __future__ import annotations

import unittest
from pathlib import Path

from integrate_verified_match import (
    IntegrationError,
    declared_symbol,
    load_tracked_symbol_names,
    uses_disallowed_psyq_gte_asm,
    uses_disallowed_psyq_rtps_asm,
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

    def test_preprocessed_psyq_rtps_macros_can_be_allowed(self) -> None:
        source = (
            '__asm__ volatile ( "lwc2 $0, 0( %0 );" '
            '"lwc2 $1, 4( %0 )" : : "r"( scratch ) ) ;\n'
            '__asm__ volatile ( "nop;" "nop;" '
            '".word 0x0000007f" ) ;\n'
            '__asm__ volatile ( "swc2 $14, 0( %0 )" '
            ': : "r"( output ) : "memory" ) ;\n'
        )
        self.assertEqual(
            source_violations(
                source,
                set(),
                allow_psyq_inline_macros=True,
            ),
            [],
        )

    def test_unrelated_expanded_assembly_is_rejected_with_rtps(self) -> None:
        source = (
            '__asm__ volatile ( "nop;" "nop;" '
            '".word 0x0000007f" ) ;\n'
            'asm("nop");\n'
        )
        self.assertEqual(
            source_violations(
                source,
                set(),
                allow_psyq_inline_macros=True,
            ),
            ["contains statement-level assembly or an untracked assembler alias"],
        )
        self.assertTrue(uses_disallowed_psyq_rtps_asm(source))

    def test_rtps_allowance_requires_the_placeholder(self) -> None:
        source = (
            '__asm__ volatile ( "lwc2 $0, 0( %0 );" '
            '"lwc2 $1, 4( %0 )" : : "r"( scratch ) ) ;\n'
        )
        self.assertTrue(uses_disallowed_psyq_rtps_asm(source))

    def test_stopz_requires_its_explicit_macro_family(self) -> None:
        source = '__asm__ volatile ( "swc2 $24, 0( %0 )" : : "r"( p ) : "memory" ) ;\n'
        self.assertTrue(source_violations(source, set()))
        self.assertTrue(source_violations(source, set(), allow_psyq_inline_macros=True))
        self.assertEqual(source_violations(
            source, set(), allow_psyq_inline_macros=True, psyq_inline_macro="stopz",
        ), [])

    def test_stopz_rejects_other_registers_and_assembly(self) -> None:
        source = '__asm__ volatile ( "swc2 $24, 0( %0 )" : : "r"( p ) : "memory" ) ;\n'
        alternatives = [
            source.replace("$24", "$7"),
            source.replace("$24", "$14"),
            source.replace("volatile ", ""),
            source.replace(': "memory"', ': "cc"'),
            source.replace('"r"( p )', '"r"( p + 1 )'),
            source + 'asm("nop");\n',
            'asm("nop"); ' + source,
            "",
        ]
        for candidate in alternatives:
            with self.subTest(candidate=candidate):
                self.assertTrue(source_violations(
                    candidate, set(), allow_psyq_inline_macros=True, psyq_inline_macro="stopz",
                ))

    def test_stopz_does_not_allow_register_bindings(self) -> None:
        source = (
            'register int *p asm("$2");\n'
            '__asm__ volatile ( "swc2 $24, 0( %0 )" : : "r"( p ) : "memory" ) ;\n'
        )
        self.assertIn("contains a hard-register variable", source_violations(
            source, set(), allow_psyq_inline_macros=True, psyq_inline_macro="stopz",
        ))

    def test_gte_macro_family_accepts_only_its_official_expansions(self) -> None:
        source = (
            '__asm__ volatile ( "lwc2 $0, 0( %0 );" '
            '"lwc2 $1, 4( %0 )" : : "r"( vertex ) ) ;\n'
            '__asm__ volatile ( "nop;" "nop;" ".word 0x0000007f" ) ;\n'
            '__asm__ volatile ( "swc2 $14, 0( %0 )" '
            ': : "r"( xy ) : "memory" ) ;\n'
            '__asm__ volatile ( "cfc2 $12, $31;" "nop;" '
            '"sw $12, 0( %0 )" : : "r"( flag ) : "$12", "memory" ) ;\n'
            '__asm__ volatile ( "lwc2 $6, 0( %0 )" : : "r"( rgb ) ) ;\n'
            '__asm__ volatile ( "nop;" "nop;" ".word 0x00000fff" ) ;\n'
            '__asm__ volatile ( "swc2 $22, 0( %0 )" '
            ': : "r"( color ) : "memory" ) ;\n'
            '__asm__ volatile ( "mfc2 $12, $19;" "nop;" '
            '"sra $12, $12, 2;" "sw $12, 0( %0 )" '
            ': : "r"( depth ) : "$12", "memory" ) ;\n'
            '__asm__ volatile ( "mtc2 %0, $12;" "mtc2 %2, $14;" '
            '"mtc2 %1, $13" : : "r"( xy0 ), "r"( xy1 ), "r"( xy2 ) ) ;\n'
            '__asm__ volatile ( "nop;" "nop;" ".word 0x0000117f" ) ;\n'
            '__asm__ volatile ( "swc2 $24, 0( %0 )" '
            ': : "r"( &z ) : "memory" ) ;\n'
        )
        self.assertFalse(uses_disallowed_psyq_gte_asm(source))
        self.assertEqual(source_violations(
            source, set(), allow_psyq_inline_macros=True, psyq_inline_macro="gte",
        ), [])
        for candidate in (
            source.replace("$19", "$18", 1),
            source.replace(
                '__asm__ volatile ( "nop;" "nop;" '
                '".word 0x00000fff" ) ;\n',
                "",
            ),
            source + 'asm("nop");\n',
        ):
            with self.subTest(candidate=candidate):
                self.assertTrue(uses_disallowed_psyq_gte_asm(candidate))

    def test_macro_profile_requires_known_explicit_allowance(self) -> None:
        validate_effective_profile({
            "compiler_flags": ["-G8"], "maspsx_flags": ["-G8"],
            "psyq_inline_macro": "gte", "allow_psyq_inline_macros": True,
        }, "test")
        for family, enabled in (("other", True), ("stopz", False), (["stopz"], True)):
            with self.subTest(family=family, enabled=enabled):
                with self.assertRaises(IntegrationError):
                    validate_effective_profile({
                        "compiler_flags": ["-G8"], "maspsx_flags": ["-G8"],
                        "psyq_inline_macro": family, "allow_psyq_inline_macros": enabled,
                    }, "test")

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
