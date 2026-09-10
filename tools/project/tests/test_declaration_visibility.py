from __future__ import annotations

import json
from pathlib import Path
import shutil
import sys
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import check_declaration_visibility as visibility


COMPILER = "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


@unittest.skipUnless(
    (REPOSITORY / COMPILER).is_file(), "needs the GCC 2.8.1 toolchain"
)
class DeclarationVisibilityTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.root = REPOSITORY / "tmp/test-declaration-visibility"
        shutil.rmtree(self.root, ignore_errors=True)
        (self.root / "config/slus_01411/overlays").mkdir(parents=True)
        (self.root / "src/game").mkdir(parents=True)
        # The real toolchain, reached through a symlink so the fixture stays
        # small; the check resolves the compiler relative to the fixture root.
        toolchain = Path(COMPILER).parents[1]
        (self.root / toolchain).parent.mkdir(parents=True)
        (self.root / toolchain).symlink_to(REPOSITORY / toolchain)
        self.write(
            "config/slus_01411/compiler_profiles.json",
            json.dumps(
                {
                    "schema": 1,
                    "profiles": {
                        "p": {
                            "compiler": COMPILER,
                            "compiler_flags": ["-O2", "-G8", "-fno-builtin"],
                        }
                    },
                }
            ),
        )
        # The prototype exists in the tree, globally unique and correct.
        self.write("src/game/callee.h", "int callee(int value);\n")

    def tearDown(self) -> None:
        shutil.rmtree(self.root, ignore_errors=True)

    def write(self, relative: str, content: str) -> None:
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")

    def matching(self, *sources: str) -> None:
        self.write(
            "config/slus_01411/matching_c.json",
            json.dumps(
                {
                    "functions": [
                        {"address": f"0x8001{i:04X}", "profile": "p", "source": s}
                        for i, s in enumerate(sources)
                    ]
                }
            ),
        )

    def exceptions(self, *pairs: tuple[str, str]) -> None:
        self.write(
            "config/slus_01411/implicit_declaration_exceptions.json",
            json.dumps(
                {
                    "schema": 1,
                    "exceptions": [
                        {"source": s, "function": f, "reason": "measured"}
                        for s, f in pairs
                    ],
                }
            ),
        )

    def test_prototype_elsewhere_is_not_a_declaration_the_caller_sees(self) -> None:
        # Negative control: callee() is declared in the tree, but not in any
        # header this caller includes, so the call is implicit.
        self.write("src/game/caller.c", "int caller(void) { return callee(1); }\n")
        self.matching("src/game/caller.c")

        errors, checked = visibility.validate(self.root, jobs=1)

        self.assertEqual(checked, 1)
        self.assertEqual(
            errors,
            [
                "src/game/caller.c: calls callee through an implicit "
                "declaration; include the header that declares it"
            ],
        )

    def test_builtin_name_without_prototype_is_implicit_with_profile_flags(self) -> None:
        # GCC anticipates strcmp as a builtin when -fno-builtin is omitted.
        # The production profile disables builtins, so the visibility check
        # must preserve that flag and diagnose this headerless call.
        self.write(
            "src/game/caller.c",
            "int caller(char *a, char *b) { return strcmp(a, b); }\n",
        )
        self.matching("src/game/caller.c")

        errors, checked = visibility.validate(self.root, jobs=1)

        self.assertEqual(checked, 1)
        self.assertEqual(
            errors,
            [
                "src/game/caller.c: calls strcmp through an implicit "
                "declaration; include the header that declares it"
            ],
        )

    def test_included_prototype_passes(self) -> None:
        self.write(
            "src/game/caller.c",
            '#include "callee.h"\nint caller(void) { return callee(1); }\n',
        )
        self.matching("src/game/caller.c")

        self.assertEqual(visibility.validate(self.root, jobs=1)[0], [])

    def test_recorded_exception_is_accepted_and_stale_one_rejected(self) -> None:
        self.write("src/game/caller.c", "int caller(void) { return callee(1); }\n")
        self.write("src/game/clean.c", "int clean(void) { return 0; }\n")
        self.matching("src/game/caller.c", "src/game/clean.c")
        self.exceptions(("src/game/caller.c", "callee"))
        self.assertEqual(visibility.validate(self.root, jobs=1)[0], [])

        self.exceptions(
            ("src/game/caller.c", "callee"), ("src/game/clean.c", "callee")
        )
        self.assertEqual(
            visibility.validate(self.root, jobs=1)[0],
            [
                "config/slus_01411/implicit_declaration_exceptions.json: "
                "stale exception src/game/clean.c: callee is no longer "
                "called implicitly"
            ],
        )

    def test_candidates_are_checked_too(self) -> None:
        self.write("src/game/clean.c", "int clean(void) { return 0; }\n")
        self.matching("src/game/clean.c")
        self.write(
            "src/candidates/func_80010000.c",
            "int func_80010000(void) { return callee(2); }\n",
        )
        self.write(
            "config/slus_01411/candidates.json",
            json.dumps(
                {
                    "schema": 2,
                    "candidates": [
                        {
                            "address": "0x80010000",
                            "profile": "p",
                            "source": "src/candidates/func_80010000.c",
                        }
                    ],
                }
            ),
        )

        errors, checked = visibility.validate(self.root, jobs=1)

        self.assertEqual(checked, 2)
        self.assertEqual(len(errors), 1)
        self.assertIn("src/candidates/func_80010000.c: calls callee", errors[0])


if __name__ == "__main__":
    unittest.main()
