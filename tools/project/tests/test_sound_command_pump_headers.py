"""Check command-pump owner declarations with the real GCC 2.8.1 frontend.

These are declaration controls, not byte-match tests. In particular, both
func_80049AF4 views must be prototypes, not permissive empty parameter lists.
"""

from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess
import unittest
import uuid


REPOSITORY = Path(__file__).resolve().parents[3]
COMPILER = REPOSITORY / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
AMBIENT = "#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG\n"

# owner, symbol, canonical pointer declaration, incompatible control
DECLARATIONS = [
    (
        "sound.h",
        "func_80045514",
        "void (*checked)(void)",
        "s32 (*checked)(void)",
    ),
    (
        "sound_pending_entries.h",
        "func_800476B4",
        "void (*checked)(SDSeqBlock *, u32)",
        "void (*checked)(u8 *, s32)",
    ),
    (
        "sound_output_state.h",
        "SD_ArmBusyCallback",
        "void (*checked)(void)",
        "s32 (*checked)(void)",
    ),
    (
        "sound.h",
        "func_80045484",
        "s32 (*checked)(void)",
        "u8 (*checked)(void)",
    ),
    (
        "sound.h",
        "func_80049A64",
        "s32 (*checked)(void *, s16)",
        "s16 (*checked)(u8 *, s16)",
    ),
    (
        "sound_sequence_timing.h",
        "func_80049AF4",
        "void (*checked)(s32)",
        "void (*checked)(s32, s32)",
    ),
]

CALL_ARGUMENTS = {
    "func_80045514": "",
    "func_800476B4": "0, 0",
    "SD_ArmBusyCallback": "",
    "func_80045484": "",
    "func_80049A64": "0, 0",
    "func_80049AF4": "1",
}


@unittest.skipUnless(COMPILER.is_file(), "needs the GCC 2.8.1 toolchain")
class SoundCommandPumpHeaderTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.scratch = REPOSITORY / "tmp" / f"test-sound-headers-{uuid.uuid4().hex}"
        self.scratch.mkdir(parents=True)
        self.addCleanup(shutil.rmtree, self.scratch)

    def compile(self, source: Path) -> subprocess.CompletedProcess[str]:
        environment = os.environ.copy()
        environment["TMPDIR"] = str(self.scratch)
        return subprocess.run(
            [
                str(COMPILER),
                "-S",
                "-O2",
                "-G0",
                "-Werror",
                "-Wimplicit-function-declaration",
                f"-I{REPOSITORY / 'src/game'}",
                "-o",
                str(self.scratch / "probe.s"),
                str(source),
            ],
            cwd=REPOSITORY,
            env=environment,
            capture_output=True,
            text=True,
            errors="replace",
        )

    def probe(self, text: str, *, accepted: bool) -> None:
        source = self.scratch / "probe.c"
        source.write_text(text, encoding="utf-8")
        result = self.compile(source)
        if accepted:
            self.assertEqual(result.returncode, 0, result.stderr)
        else:
            self.assertNotEqual(result.returncode, 0, result.stderr)
            self.assertRegex(
                result.stderr,
                r"incompatible pointer type|conflicting types|"
                r"too (?:few|many) arguments",
            )

    def test_canonical_function_pointers(self) -> None:
        for header, symbol, canonical, _ in DECLARATIONS:
            with self.subTest(symbol=symbol):
                self.probe(
                    f'#include "{header}"\n{canonical} = {symbol};\n',
                    accepted=True,
                )

    def test_incompatible_function_pointers_are_rejected(self) -> None:
        for header, symbol, _, incompatible in DECLARATIONS:
            with self.subTest(symbol=symbol):
                self.probe(
                    f'#include "{header}"\n{incompatible} = {symbol};\n',
                    accepted=False,
                )

    def test_missing_owners_report_implicit_calls(self) -> None:
        for header, symbol, _, _ in DECLARATIONS:
            with self.subTest(symbol=symbol):
                call = f"void checked(void) {{ {symbol}({CALL_ARGUMENTS[symbol]}); }}\n"
                self.probe(f'#include "{header}"\n' + call, accepted=True)
                source = self.scratch / "missing-owner.c"
                source.write_text('#include "../types.h"\n' + call, encoding="utf-8")
                result = self.compile(source)
                self.assertNotEqual(result.returncode, 0, result.stderr)
                self.assertIn(
                    f"implicit declaration of function `{symbol}'", result.stderr
                )

    def test_command_pump_prototype_rejects_arguments(self) -> None:
        self.probe(
            '#include "sound.h"\nvoid checked(void) { func_80045514(1); }\n',
            accepted=False,
        )

    def test_runtime_caller_uses_sound_owner_not_unmatched(self) -> None:
        path = REPOSITORY / "src/game/sound_runtime.c"
        self.assertIn('#include "sound.h"', path.read_text(encoding="utf-8"))
        unmatched = (REPOSITORY / "src/unmatched.h").read_text(encoding="utf-8")
        self.assertNotRegex(unmatched, r"\bfunc_80045514\s*\(")
        result = self.compile(path)
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_secondary_input_pointer_and_return_are_independently_checked(self) -> None:
        for declaration in (
            "s32 (*checked)(u8 *, s16)",
            "s16 (*checked)(void *, s16)",
        ):
            with self.subTest(declaration=declaration):
                self.probe(
                    f'#include "sound.h"\n{declaration} = func_80049A64;\n',
                    accepted=False,
                )

    def test_ambient_function_pointer(self) -> None:
        self.probe(
            AMBIENT + '#include "sound_sequence_timing.h"\n'
            "void (*checked)(s32, s32) = func_80049AF4;\n",
            accepted=True,
        )
        self.probe(
            AMBIENT + '#include "sound_sequence_timing.h"\n'
            "void (*checked)(s32) = func_80049AF4;\n",
            accepted=False,
        )

    def test_both_start_views_enforce_exact_argument_counts(self) -> None:
        for prefix, valid in (("", "1"), (AMBIENT, "1, 1")):
            for arguments in ("", "1", "1, 1", "1, 1, 1"):
                with self.subTest(ambient=bool(prefix), arguments=arguments):
                    self.probe(
                        prefix + '#include "sound_sequence_timing.h"\n'
                        f"void checked(void) {{ func_80049AF4({arguments}); }}\n",
                        accepted=arguments == valid,
                    )

    def test_combined_owners_preserve_existing_ambient_steps(self) -> None:
        for prefix, arguments in (("", ""), (AMBIENT, "1")):
            with self.subTest(ambient=bool(prefix)):
                self.probe(
                    prefix
                    + '#include "sound_pending_entries.h"\n'
                    '#include "sound_output_state.h"\n'
                    '#include "sound_sequence_timing.h"\n'
                    '#include "sound.h"\n'
                    "void checked(void) {\n"
                    "  func_800476B4((SDSeqBlock *)0, (u32)0);\n"
                    "  SD_ArmBusyCallback();\n"
                    "  (void)func_80045484();\n"
                    "  (void)func_80049A64((u8 *)0, (s16)0);\n"
                    f"  func_800498F8({arguments});\n"
                    f"  func_80049C40({arguments});\n"
                    f"  func_80049CB0({arguments});\n"
                    "}\n",
                    accepted=True,
                )

    def test_definitions_include_compatible_owners(self) -> None:
        for source, header in (
            ("src/game/sound_pending_entries.c", "sound_pending_entries.h"),
            ("src/game/sd_arm_busy_callback.c", "sound_output_state.h"),
            ("src/game/func_80045484.c", "sound.h"),
            ("src/game/sound_secondary_playback.c", "sound.h"),
            ("src/game/func_80049AF4.c", "sound_sequence_timing.h"),
        ):
            with self.subTest(source=source):
                path = REPOSITORY / source
                self.assertIn(f'#include "{header}"', path.read_text(encoding="utf-8"))
                result = self.compile(path)
                self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
