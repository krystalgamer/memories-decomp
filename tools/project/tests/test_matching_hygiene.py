from __future__ import annotations

import json
from pathlib import Path
import shutil
import sys
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import check_matching_hygiene as hygiene


PROFILES = {
    "schema": 1,
    "profiles": {
        "same": {"compiler_flags": ["-O2", "-G8"], "maspsx_flags": ["-G8"]},
        "mixed": {"compiler_flags": ["-O2", "-G8"], "maspsx_flags": ["-G0"]},
    },
}


class MatchingHygieneTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.root = REPOSITORY / "tmp/test-matching-hygiene"
        shutil.rmtree(self.root, ignore_errors=True)
        (self.root / "config/slus_01411/overlays").mkdir(parents=True)
        (self.root / "src/game").mkdir(parents=True)
        self.write(
            "config/slus_01411/compiler_profiles.json", json.dumps(PROFILES)
        )

    def tearDown(self) -> None:
        shutil.rmtree(self.root, ignore_errors=True)

    def write(self, relative: str, content: str) -> None:
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")

    def function(self, source: str, profile: str = "same") -> list[str]:
        self.write("src/game/unit.c", source)
        self.write(
            "config/slus_01411/functions.csv",
            "address,size,name,status,module,notes\n"
            "0x80010000,0x10,func_test,matching_c,game,\n",
        )
        self.write(
            "config/slus_01411/matching_c.json",
            json.dumps(
                {
                    "functions": [
                        {
                            "address": "0x80010000",
                            "profile": profile,
                            "size": "0x10",
                            "source": "src/game/unit.c",
                        }
                    ]
                }
            ),
        )
        return [finding.problem for finding in hygiene.validate(self.root)[0]]

    def test_plain_c_passes(self) -> None:
        self.assertEqual(
            self.function("void func_test(void) { register int x = 1; }\n"), []
        )

    def test_register_pin_is_rejected(self) -> None:
        problems = self.function(
            'void func_test(void) { register int x asm("$2") = 1; }\n'
        )
        self.assertEqual(problems, ['register pin `register int x asm("$2")`'])

    def test_inline_asm_statement_is_rejected(self) -> None:
        problems = self.function(
            'void func_test(int x) { __asm__ volatile("" : "+r"(x)); }\n'
        )
        self.assertEqual(problems, ["1 inline asm statement"])

    def test_asm_reached_through_macro_is_rejected(self) -> None:
        self.write(
            "src/game/barrier.h",
            '#define BARRIER(x) \\\n    __asm__ volatile("" : "+r"(x))\n'
            "#define USE_BARRIER(x) BARRIER(x)\n",
        )
        problems = self.function(
            '#include "barrier.h"\nvoid func_test(int x) { USE_BARRIER(x); }\n'
        )
        self.assertEqual(problems, ["asm through macro `USE_BARRIER`"])

    def test_asm_reached_through_inline_helper_is_rejected(self) -> None:
        problems = self.function(
            "static inline int helper(void) {\n"
            '    register int r asm("$3"); __asm__("move %0,$2" : "=r"(r));\n'
            "    return r;\n}\n"
            "void func_test(void) { helper(); }\n"
        )
        self.assertEqual(problems, ["asm through inline helper `helper`"])

    def test_mixed_g_profile_is_rejected(self) -> None:
        problems = self.function("void func_test(void) {}\n", profile="mixed")
        self.assertEqual(
            problems, ["mixed -G profile mixed (compiler -G8, MASPSX -G0)"]
        )

    def test_top_level_asm_definition_is_rejected(self) -> None:
        problems = self.function(
            '__asm__(".globl func_test\\nfunc_test:\\n.word 0\\n");\n'
        )
        self.assertEqual(problems, ["no C definition (top-level asm or missing)"])

    def test_comments_strings_and_asm_labels_do_not_count(self) -> None:
        problems = self.function(
            'extern unsigned char alias[] asm("D_80010000");\n'
            "/* register int x asm(\"$2\"); __asm__(\"nop\"); */\n"
            'const char *text = "asm(\\"nop\\")";\n'
            "void func_test(void) { alias[0] = 0; }\n"
        )
        self.assertEqual(problems, [])

    def test_overlay_manifests_are_checked(self) -> None:
        self.function("void func_test(void) {}\n")
        self.write(
            "src/overlays/menu/unit.c",
            'void Menu_Update(void) { register int x asm("$2"); }\n',
        )
        self.write(
            "config/slus_01411/overlays/menu_functions.csv",
            "address,size,name,status,module,notes\n"
            "0x80180000,0x10,Menu_Update,matching_c,overlay/menu,\n",
        )
        self.write(
            "config/slus_01411/overlays/menu_matching_c.json",
            json.dumps(
                {
                    "functions": [
                        {
                            "address": "0x80180000",
                            "profile": "same",
                            "size": "0x10",
                            "source": "src/overlays/menu/unit.c",
                        }
                    ]
                }
            ),
        )
        findings, checked = hygiene.validate(self.root)
        self.assertEqual(checked, 2)
        self.assertEqual(
            [(finding.manifest, finding.name) for finding in findings],
            [("menu", "Menu_Update")],
        )


if __name__ == "__main__":
    unittest.main()
