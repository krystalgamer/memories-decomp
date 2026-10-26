from __future__ import annotations

import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from handwritten_asm_contract import in_handwritten_range
from integrate_verified_match import (
    IntegrationError,
    uses_disallowed_handwritten_asm,
    validate_effective_profile,
)
from matching_source_contracts import source_violations

PROFILE = {
    "compiler_flags": ["-G0"],
    "maspsx_flags": ["-G0"],
    "handwritten_asm_macros": True,
}


class HandwrittenAsmContractTests(unittest.TestCase):
    def test_allowlisted_macro_expansions_are_accepted(self) -> None:
        source = (
            'register unsigned int a asm("$16");\n'
            "void f(void) {\n"
            '  register int *p asm("$4");\n'
            '  __asm__ volatile("sw $16, 0x20(%0);" "sw $17, 0x24(%0)"'
            ' : : "r"(p) : "memory");\n'
            '  __asm__ volatile(".word 0x4A280030;" "cfc2 %0, $31;" "nop"'
            ' : "=r"(a));\n'
            '  __asm__ volatile("lwc2 $0, 0(%0);" "lwc2 $1, 4(%0)" : : "r"(p));\n'
            "}\n"
        )
        self.assertFalse(uses_disallowed_handwritten_asm(source))
        self.assertEqual(
            source_violations(source, set(), allow_handwritten_asm_macros=True),
            [],
        )

    def test_lwr_and_branch_delay_slot_forms_are_accepted(self) -> None:
        source = (
            "unsigned int *f(unsigned char *p, unsigned int w, unsigned int z) {\n"
            '  __asm__ volatile("lwr %0, 10(%1)" : "=r"(w) : "r"(p), "0"(w));\n'
            '  __asm__ volatile(".set\\tnoreorder\\n" "beq %0, %1, %2\\n"'
            ' "mtc2 %0, $17\\n" ".set\\treorder"'
            ' : : "r"(z), "r"(w), "i"(&&next));\n'
            '  __asm__ volatile(".set\\tnoreorder\\n" "beqz %1, %2\\n"'
            ' "lw %0, 16(%3)\\n" ".set\\treorder"'
            ' : "=r"(w) : "r"(z), "i"(&&next), "r"(p));\n'
            "next:\n"
            "  return 0;\n"
            "}\n"
        )
        self.assertFalse(uses_disallowed_handwritten_asm(source))

    def test_branch_blocks_outside_the_narrow_form_are_rejected(self) -> None:
        for template, operands in (
            ('".set\\tnoreorder\\n" "beq %0, %1, %2\\n" ".set\\treorder"',
             ': : "r"(z), "r"(z), "i"(&&next)'),
            ('".set\\tnoreorder\\n" "beq %0, %1, %2\\n" "addiu $2, $2, 1\\n"'
             ' ".set\\treorder"', ': : "r"(z), "r"(z), "i"(&&next)'),
            ('".set\\tnoreorder\\n" "j %2\\n" "nop\\n" ".set\\treorder"',
             ': : "r"(z), "r"(z), "i"(&&next)'),
            ('".set\\tnoreorder\\n" "beq %0, %1, %2\\n" "mtc2 %0, $17"',
             ': : "r"(z), "r"(z), "i"(&&next)'),
            ('"beq %0, %1, %2"', ': : "r"(z), "r"(z), "i"(&&next)'),
            ('".set\\tnoreorder\\n" "beq %0, %1, %2\\n" "mtc2 %0, $17\\n"'
             ' ".set\\treorder"', ': : "r"(z), "r"(z), "i"(4)'),
            ('"lwr %0, 2(%1)"', ': "=r"(z) : "r"(z), "m"(z)'),
        ):
            with self.subTest(template=template, operands=operands):
                source = (
                    "void f(int z) {\n"
                    f"  __asm__ volatile({template} {operands});\n"
                    "next:\n"
                    "  return;\n"
                    "}\n"
                )
                self.assertTrue(uses_disallowed_handwritten_asm(source))

    def test_special_form_operand_signatures_are_enforced(self) -> None:
        beq = '".set\\tnoreorder\\n" "beq %0, %1, %2\\n" "mtc2 %0, $17\\n" ".set\\treorder"'
        beqz = '".set\\tnoreorder\\n" "beqz %1, %2\\n" "lw %0, 16(%3)\\n" ".set\\treorder"'
        for template, operands in (
            # lwr: missing, mismatched or misplaced tie; extra clobber.
            ('"lwr %0, 10(%1)"', ': "=r"(w) : "r"(p)'),
            ('"lwr %0, 10(%1)"', ': "=r"(w) : "r"(p), "0"(z)'),
            ('"lwr %0, 10(%1)"', ': "=r"(w) : "0"(w), "r"(p)'),
            ('"lwr %0, 10(%1)"', ': "=r"(w) : "r"(p), "r"(w)'),
            ('"lwr %0, 10(%1)"', ': "=r"(w) : "r"(p), "0"(w) : "memory"'),
            ('"lwr %0, 10(%1);" "nop"', ': "=r"(w) : "r"(p), "0"(w)'),
            # beq/mtc2: label in the wrong position or not a label; extra output.
            (beq, ': : "r"(z), "i"(&&next), "r"(w)'),
            (beq, ': : "r"(z), "r"(w), "r"(&&next)'),
            (beq, ': : "r"(z), "r"(w), "i"(&next_word)'),
            (beq, ': "=r"(w) : "r"(z), "r"(w), "i"(&&next)'),
            (beq, ': : "r"(z), "r"(w)'),
            (beq, ': : "r"(z), "r"(w), "i"(&&next) : "memory"'),
            # beqz/lw: wrong label slot, missing output, wrong arity.
            (beqz, ': "=r"(w) : "r"(z), "r"(p), "i"(&&next)'),
            (beqz, ': : "r"(z), "i"(&&next), "r"(p)'),
            (beqz, ': "=r"(w) : "r"(z), "i"(&&next)'),
            # Swapped branch/delay-slot pairs.
            ('".set\\tnoreorder\\n" "beq %0, %1, %2\\n" "lw %0, 16(%3)\\n"'
             ' ".set\\treorder"', ': : "r"(z), "r"(w), "i"(&&next)'),
            ('".set\\tnoreorder\\n" "beqz %1, %2\\n" "mtc2 %0, $17\\n"'
             ' ".set\\treorder"', ': "=r"(w) : "r"(z), "i"(&&next), "r"(p)'),
            # Tied and label constraints outside the special forms.
            ('"nop"', ': "=r"(w) : "0"(w)'),
            ('"nop"', ': : "i"(&&next)'),
        ):
            with self.subTest(template=template, operands=operands):
                source = (
                    "int next_word;\n"
                    "void f(int *p, int w, int z) {\n"
                    f"  __asm__ volatile({template} {operands});\n"
                    "next:\n"
                    "  return;\n"
                    "}\n"
                )
                self.assertTrue(uses_disallowed_handwritten_asm(source))

    def test_unlisted_instruction_is_rejected(self) -> None:
        for template in (
            "jr $31",
            "sw $4, 0(%0)",
            "lw $29, 0(%0)",
            ".word 0x0000007f",
            "addiu $29, $29, -8",
            "li $1, 1",
            "li %0, -1",
            "",
            ";",
            " ; ; ",
            "\\n\\t;",
        ):
            with self.subTest(template=template):
                source = f'void f(int *p) {{ __asm__ volatile("{template}" : : "r"(p)); }}\n'
                self.assertTrue(uses_disallowed_handwritten_asm(source))

    def test_instruction_free_barriers_are_rejected(self) -> None:
        for template in ("", ";", " ; ; ", "\\n;\\t"):
            with self.subTest(template=template):
                source = (
                    f'void f(void) {{ __asm__ volatile("{template}" : : : "memory"); }}\n'
                )
                self.assertTrue(uses_disallowed_handwritten_asm(source))
                self.assertNotEqual(
                    source_violations(source, set(), allow_handwritten_asm_macros=True),
                    [],
                )

    def test_unlisted_constraints_and_clobbers_are_rejected(self) -> None:
        for operands in (': : "m"(*p)', ': : "r"(p) : "$2"'):
            with self.subTest(operands=operands):
                source = f'void f(int *p) {{ __asm__ volatile("nop" {operands}); }}\n'
                self.assertTrue(uses_disallowed_handwritten_asm(source))

    def test_pins_still_rejected_without_the_profile(self) -> None:
        source = 'void f(void) { register int v asm("$2"); }\n'
        self.assertEqual(
            source_violations(source, set()),
            ["contains a hard-register variable"],
        )

    def test_profile_validation(self) -> None:
        validate_effective_profile(PROFILE, "hw")
        with self.assertRaises(IntegrationError):
            validate_effective_profile({**PROFILE, "handwritten_asm_macros": 1}, "hw")
        with self.assertRaises(IntegrationError):
            validate_effective_profile(
                {**PROFILE, "allow_psyq_inline_macros": True}, "hw"
            )

    def test_address_range(self) -> None:
        self.assertTrue(in_handwritten_range(0x800612C0))
        self.assertTrue(in_handwritten_range(0x8006ADE8))
        self.assertFalse(in_handwritten_range(0x8006AF74))
        self.assertFalse(in_handwritten_range(0x80012B50))


if __name__ == "__main__":
    unittest.main()
