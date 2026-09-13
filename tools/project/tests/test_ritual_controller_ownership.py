"""Bounded ritual caller views, checked with native ILP32 and real GCC 2.8.1."""

from __future__ import annotations

import contextlib
import io
import json
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys
import unittest
import uuid


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))
from build_baseline import compile_c, tool


GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
CC = shutil.which("cc")
RITUAL_VIEW = "#define DISPLAY_OBJECT_WORK_RITUAL_VIEW\n"
CALLBACK_VIEW = "#define DISPLAY_OBJECT_MOTION_BYTE_CALLBACK\n"
START = """
.text
.globl _start
_start:
    andl $-16, %esp
    call main
    movl %eax, %ebx
    movl $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""


class RitualControllerOwnershipTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        self.scratch = ROOT / "tmp" / f"test-ritual-ownership-{uuid.uuid4().hex}"
        self.scratch.mkdir(parents=True)
        self.addCleanup(shutil.rmtree, self.scratch)
        self.environment = os.environ.copy()
        self.environment["TMPDIR"] = str(self.scratch)

    def run_command(self, arguments: list[str]) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            arguments, cwd=ROOT, env=self.environment, capture_output=True,
            text=True, errors="replace", timeout=60,
        )

    def probe(self, body: str, *, accepted: bool, diagnostic: str = "") -> None:
        source = self.scratch / "declaration.c"
        source.write_text(body)
        compilers = []
        if GCC.is_file():
            compilers.append([str(GCC), "-S", "-O2", "-G8", "-Werror"])
        if CC:
            compilers.append([
                CC, "-S", "-m32", "-std=gnu99",
                "-Werror=incompatible-pointer-types", "-Werror=int-conversion",
                "-Werror=implicit-function-declaration",
            ])
        if not compilers:
            self.skipTest("needs GCC 2.8.1 or a native C compiler")
        for compiler in compilers:
            with self.subTest(compiler=compiler[0], source=body):
                result = self.run_command(
                    compiler + [
                        "-Wimplicit-function-declaration",
                        "-I", str(ROOT / "src/game"),
                        "-o", str(self.scratch / "declaration.s"), str(source),
                    ]
                )
                if accepted:
                    self.assertEqual(result.returncode, 0, result.stderr)
                else:
                    self.assertNotEqual(result.returncode, 0, result.stderr)
                    self.assertRegex(result.stderr, diagnostic)

    def test_function_owners_and_exact_argument_counts(self) -> None:
        for symbol, pointer, wrong, call, bad_calls in (
            ("func_800262D4", "void (*p)(void)", "s32 (*p)(void)", "", ("0",)),
            ("func_80019CC8", "void (*p)(void *)", "void (*p)(s32)",
             "(void *)(s32)-1", ("", "0, 0")),
        ):
            owner = '#include "duel_ritual_controller.h"\n'
            with self.subTest(symbol=symbol):
                self.probe(owner + f"{pointer} = {symbol};\n", accepted=True)
                self.probe(
                    owner + f"{wrong} = {symbol};\n", accepted=False,
                    diagnostic="incompatible pointer|incompatible-pointer",
                )
                self.probe(
                    owner + f"void call(void) {{ {symbol}({call}); }}\n",
                    accepted=True,
                )
                for arguments in bad_calls:
                    self.probe(
                        owner + f"void call(void) {{ {symbol}({arguments}); }}\n",
                        accepted=False, diagnostic="too (few|many) arguments",
                    )
                self.probe(
                    '#include "../types.h"\n' + f"{pointer} = {symbol};\n",
                    accepted=False, diagnostic="undeclared",
                )

    def test_owned_data_types_and_measured_rectangle_extent(self) -> None:
        header = '#include "duel_ritual_controller.h"\n'
        for symbol, correct, wrong in (
            ("D_8009B1A0", "s16 *p", "s32 *p"),
            ("D_8009B1C0", "DisplayObject **p", "u8 **p"),
            ("D_800EA128", "u16 (*p)[22]", "u16 (*p)[21]"),
        ):
            with self.subTest(symbol=symbol):
                self.probe(header + f"{correct} = &{symbol};\n", accepted=True)
                self.probe(
                    header + f"{wrong} = &{symbol};\n", accepted=False,
                    diagnostic="incompatible pointer|incompatible-pointer",
                )
        self.probe(
            header + "typedef char extent[sizeof(D_800EA128) == 0x2C ? 1 : -1];\n",
            accepted=True,
        )
        self.probe(
            header + "extern u16 D_800EA128[21];\n",
            accepted=False, diagnostic="conflicting types",
        )

    def test_callback_guard_preserves_both_prototypes(self) -> None:
        for prefix, parameter, other in (
            ("", "DisplayObject *", "u8 *"),
            (CALLBACK_VIEW, "u8 *", "DisplayObject *"),
        ):
            header = prefix + '#include "display_object_motion.h"\n'
            self.probe(
                header + f"void (*p)({parameter}) = func_8001EC70;\n", accepted=True,
            )
            self.probe(
                header + f"void (*p)({other}) = func_8001EC70;\n",
                accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
            )
        body = (
            '#include "display_object_motion.h"\n'
            "DisplayObjectCallback callback = func_8001EC70;\n"
        )
        self.probe(CALLBACK_VIEW + body, accepted=True)
        self.probe(body, accepted=False, diagnostic="incompatible pointer|incompatible-pointer")

    def test_ritual_work_view_contains_the_entire_result(self) -> None:
        header = RITUAL_VIEW + '#include "display_object_work_slots.h"\n'
        self.probe(
            header
            + "typedef char size[sizeof(D_800E9EF0) == 24 ? 1 : -1];\n"
            "typedef char slots[sizeof(D_800E9EF0.slots) == 20 ? 1 : -1];\n"
            "DuelRitualResult *out = &D_800E9EF0.ritual.result;\n"
            "DisplayObjectRitualWorkArea D_800E9EF0;\n",
            accepted=True,
        )
        self.probe(
            header + "DisplayObject *D_800E9EF0[5];\n",
            accepted=False, diagnostic="conflicting types",
        )
        self.probe(
            header + "DisplayObject *(*p)[6] = &D_800E9EF0.slots;\n",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )
        self.probe(
            '#include "display_object_work_slots.h"\n'
            "DisplayObject **slot = &D_800E9EF0[4];\n"
            "DisplayObject *D_800E9EF0[5];\n",
            accepted=True,
        )
        self.probe(
            '#include "display_object_work_slots.h"\n'
            "void *out = &D_800E9EF0.ritual.result;\n",
            accepted=False, diagnostic="not a structure|not structure|request for member",
        )

    def test_scalar_absolute_arms_remain_scalar_and_compatible(self) -> None:
        self.probe(
            "#define GDUEL_WSELECTEDCARDID_IN_DATA\n"
            "#define GINPUT_PAD1_PRESSED_IN_DATA\n"
            '#include "duel_effect.h"\n#include "input.h"\n'
            "u16 *card = &gDuel_wSelectedCardID;\n"
            "u16 *input = &gInput_wPad1Pressed;\n",
            accepted=True,
        )
        self.probe(
            "#define GINPUT_PAD1_PRESSED_IN_DATA\n"
            '#include "input.h"\n'
            "u16 (*input)[4] = &gInput_wPad1Pressed;\n",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )

    def test_both_workspaces_fit_the_existing_image_backing(self) -> None:
        import yaml

        layout = yaml.safe_load((ROOT / "config/slus_01411/split.yaml").read_text())
        segments = layout["segments"]
        index = next(
            i for i, segment in enumerate(segments)
            if isinstance(segment, dict)
            and segment.get("name") == "bss_image_after_viewport"
        )
        backing = segments[index]
        following = segments[index + 1]
        self.assertEqual(backing["type"], "bin")
        self.assertEqual(backing["vram"], 0x8009B14A)
        self.assertEqual(following["vram"], 0x800FE728)
        self.assertEqual(
            following["start"] - backing["start"],
            following["vram"] - backing["vram"],
        )
        for start, extent, next_label in (
            (0x800E9EF0, 0x18, 0x800E9F10),
            (0x800EA128, 0x2C, 0x800EA1E8),
        ):
            with self.subTest(start=hex(start)):
                self.assertGreaterEqual(start, backing["vram"])
                self.assertLessEqual(start + extent, next_label)
                self.assertLessEqual(next_label, following["vram"])

    @unittest.skipUnless(GCC.is_file(), "needs GCC 2.8.1")
    def test_existing_definitions_and_callers_include_owners(self) -> None:
        for name in ("func_80019CC8.c", "func_8002622C.c", "duel_effect_tables.c"):
            path = ROOT / "src/game" / name
            self.assertIn('#include "duel_ritual_controller.h"', path.read_text())
            result = self.run_command([
                str(GCC), "-S", "-O2", "-G8", "-Werror",
                "-Wimplicit-function-declaration", str(path),
                "-o", str(self.scratch / "existing.s"),
            ])
            self.assertEqual(result.returncode, 0, result.stderr)
        unmatched = (ROOT / "src/unmatched.h").read_text()
        self.assertNotIn("extern s16 D_8009B1A0;", unmatched)
        self.assertNotIn("void func_800262D4(void);", unmatched)

    @unittest.skipUnless(GCC.is_file(), "needs GCC 2.8.1")
    def test_absolute_and_gp_relocations_survive_owned_views(self) -> None:
        source = self.scratch / "addresses.c"
        source.write_text(
            "#define GDUEL_WSELECTEDCARDID_IN_DATA\n"
            "#define GINPUT_PAD1_PRESSED_IN_DATA\n"
            + RITUAL_VIEW
            + f'#include "{ROOT / "src/game/duel_ritual_controller.h"}"\n'
            + f'#include "{ROOT / "src/game/display_object_work_slots.h"}"\n'
            + f'#include "{ROOT / "src/game/duel_effect.h"}"\n'
            + f'#include "{ROOT / "src/game/input.h"}"\n'
            + "s16 id(void) { return D_8009B1A0; }\n"
            "DisplayObject *object(void) { return D_8009B1C0; }\n"
            "u16 coordinate(void) { return D_800EA128[21]; }\n"
            "DuelRitualResult *result(void) { return &D_800E9EF0.ritual.result; }\n"
            "u16 selected(void) { return gDuel_wSelectedCardID; }\n"
            "u16 pressed(void) { return gInput_wPad1Pressed; }\n"
        )
        profiles = json.loads(
            (ROOT / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            obj = compile_c(
                ROOT, tool(ROOT, "as"),
                {"source": str(source.relative_to(ROOT)),
                 "profile": "gcc_2_8_1_g8_split", "object": "addresses.o"},
                profiles, object_directory=str(self.scratch.relative_to(ROOT)),
                asm_directory=str(self.scratch.relative_to(ROOT)),
            )
        result = self.run_command([str(tool(ROOT, "objdump")), "-r", str(obj)])
        self.assertEqual(result.returncode, 0, result.stderr)
        for symbol in ("D_8009B1A0", "D_8009B1C0"):
            self.assertRegex(result.stdout, r"R_MIPS_GPREL16\s+" + symbol)
        for symbol in ("D_800EA128", "D_800E9EF0",
                       "gDuel_wSelectedCardID", "gInput_wPad1Pressed"):
            self.assertRegex(result.stdout, r"R_MIPS_HI16\s+" + symbol)
            self.assertRegex(result.stdout, r"R_MIPS_LO16\s+" + symbol)
            self.assertNotRegex(result.stdout, r"R_MIPS_GPREL16\s+" + symbol)

    def native_storage_witness(self, five_slot_control: bool) -> int:
        if not (
            CC and platform.system() == "Linux"
            and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
        ):
            self.skipTest("needs native Linux x86 ILP32 execution")
        source = self.scratch / "storage.c"
        source.write_text(
            RITUAL_VIEW
            + '#include "display_object_work_slots.h"\n'
            '#include "duel_ritual_controller.h"\n'
            '#include "duel_card.h"\n'
            '#include "duel_side_state.h"\n'
            '#include "duel_card_layout.h"\n'
            "u16 gDuel_awRitualData[] = {100, 101, 102, 103, 104, 0};\n"
            "DuelCardRecord D_801A7AD8[40];\n"
            "u8 D_8009B1D5;\n"
            "static DisplayObject objects[5];\n"
            "static struct {\n"
            " u32 before;\n"
            " DisplayObjectRitualWorkArea work;\n"
            " u32 after;\n"
            "} allocation;\n"
            "u16 D_800EA128[DUEL_RITUAL_IMAGE_PREFIX_HALFWORDS];\n"
            "int main(void) {\n"
            " s32 i;\n"
            " if (sizeof(void *) != 4) return 1;\n"
            " allocation.before = 0x12345678;\n"
            " allocation.after = 0x76543210;\n"
            " allocation.work.slots[0] = &objects[0];\n"
            " allocation.work.slots[1] = &objects[1];\n"
            " allocation.work.ritual.result.field_0C = 0x11223344;\n"
            " for (i = 0; i < 3; i++) {\n"
            "   D_801A7AD8[5 + i].card_id = 101 + i;\n"
            "   D_801A7AD8[5 + i].object = &objects[2 + i];\n"
            "   D_801A7AD8[5 + i].flags = DUEL_CARD_FLAG_OCCUPIED;\n"
            " }\n"
            " if (Duel_CheckRitual(&allocation.work.ritual.result, 100) != 104) return 2;\n"
            " if (allocation.work.slots[0] != &objects[0] ||\n"
            "     allocation.work.slots[1] != &objects[1]) return 3;\n"
            " for (i = 0; i < 3; i++)\n"
            "   if (allocation.work.slots[2 + i] != &objects[2 + i]) return 4;\n"
            " if (allocation.before != 0x12345678 || allocation.after != 0x76543210) return 5;\n"
            " if (allocation.work.ritual.result.field_0C != "
            + ("0x11223344" if five_slot_control else "0")
            + ") return 7;\n"
            " D_800EA128[4] = 56; D_800EA128[5] = 42;\n"
            " D_800EA128[6] = 8; D_800EA128[7] = 88;\n"
            " D_800EA128[20] = 100; D_800EA128[21] = 200;\n"
            " if ((u8 *)&D_800EA128[22] - (u8 *)D_800EA128 != 44) return 8;\n"
            " return 0;\n}\n"
        )
        startup = self.scratch / "start.S"
        startup.write_text(START)
        binary = self.scratch / "storage"
        compilation = self.run_command([
            CC, "-m32", "-std=gnu99", "-O2", "-nostdlib", "-fno-pie", "-no-pie",
            "-ffreestanding", "-fno-builtin", "-fno-stack-protector",
            "-fno-strict-aliasing", "-Werror=incompatible-pointer-types",
            "-I", str(ROOT / "src/game"), str(source),
            str(ROOT / "src/game/duel_check_ritual.c"), str(startup),
            "-o", str(binary),
        ])
        self.assertEqual(compilation.returncode, 0, compilation.stderr)
        result = self.run_command([str(binary)])
        return result.returncode

    def test_native_result_writes_fit_full_work_area(self) -> None:
        self.assertEqual(self.native_storage_witness(five_slot_control=False), 0)

    def test_native_five_slots_do_not_preserve_the_following_word(self) -> None:
        # The control treats the sixth word as a sentinel but still allocates
        # the full union, so detecting that misconception never runs OOB.
        self.assertEqual(self.native_storage_witness(five_slot_control=True), 7)


if __name__ == "__main__":
    unittest.main()
