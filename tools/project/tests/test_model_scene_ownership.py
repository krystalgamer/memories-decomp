"""Real GCC 2.8.1 declaration controls and a bounded model-state data link.

The miniature data link explicitly supplies the required A6 = A4 + 2 interior
alias. It proves the storage/relocation contract, not the full resident linker
configuration or executable match; those remain integration gates.
"""

from __future__ import annotations

import contextlib
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import unittest
import uuid


REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))
from build_baseline import compile_c, tool


COMPILER = REPOSITORY / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
BYTE_VIEW = "#define MODEL_GRAPHICS_STATE_SCENE_BYTES\n"
STATE_HEADER = f'#include "{REPOSITORY / "src/game/model_graphics_state.h"}"\n'
DECLARATIONS = (
    ("model_scene_states.h", "func_80050584", "void", "s32", "0"),
    ("model_scene_states.h", "func_800507D0", "void", "void", ""),
    ("model_scene_imports.h", "func_801807B0", "void", "void", ""),
    ("model_scene_imports.h", "func_80181C4C", "void", "s32", "0"),
    ("model_scene_imports.h", "func_80180A24", "s32", "void", ""),
)
RETAIL_STATE = bytes.fromhex(
    "081009802c01000000000000000000000001ff0000000000000000000100000001000000"
)


@unittest.skipUnless(COMPILER.is_file(), "needs the GCC 2.8.1 toolchain")
class ModelSceneOwnershipTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.scratch = REPOSITORY / "tmp" / f"test-model-ownership-{uuid.uuid4().hex}"
        self.scratch.mkdir(parents=True)
        self.addCleanup(shutil.rmtree, self.scratch)
        self.environment = os.environ.copy()
        self.environment["TMPDIR"] = str(self.scratch)

    def run_command(self, arguments: list[str]) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            arguments, cwd=REPOSITORY, env=self.environment,
            capture_output=True, text=True, errors="replace", timeout=60,
        )

    def compile(self, path: Path) -> subprocess.CompletedProcess[str]:
        return self.run_command([
            str(COMPILER), "-S", "-O2", "-G8", "-Werror",
            "-Wimplicit-function-declaration",
            f"-I{REPOSITORY / 'src/game'}",
            "-o", str(self.scratch / "probe.s"), str(path),
        ])

    def probe(self, source: str, *, accepted: bool, diagnostic: str = "") -> None:
        path = self.scratch / "probe.c"
        path.write_text(source, encoding="utf-8")
        result = self.compile(path)
        if accepted:
            self.assertEqual(result.returncode, 0, result.stderr)
        else:
            self.assertNotEqual(result.returncode, 0, result.stderr)
            self.assertRegex(result.stderr, diagnostic)

    def build_object(self, path: Path, name: str) -> Path:
        profiles = json.loads(
            (REPOSITORY / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            return compile_c(
                REPOSITORY, tool(REPOSITORY, "as"),
                {"source": str(path.relative_to(REPOSITORY)),
                 "profile": "gcc_2_8_1_g8", "object": name + ".o"},
                profiles, object_directory=str(self.scratch.relative_to(REPOSITORY)),
                asm_directory=str(self.scratch.relative_to(REPOSITORY)),
            )

    def test_function_owners_accept_measured_function_pointers(self) -> None:
        for header, symbol, result, arguments, _ in DECLARATIONS:
            with self.subTest(symbol=symbol):
                self.probe(
                    f'#include "{header}"\n'
                    f"{result} (*checked)({arguments}) = {symbol};\n",
                    accepted=True,
                )

    def test_function_owners_reject_wrong_return_types(self) -> None:
        for header, symbol, result, arguments, _ in DECLARATIONS:
            with self.subTest(symbol=symbol):
                wrong_result = "s32" if result == "void" else "void"
                self.probe(
                    f'#include "{header}"\n'
                    f"{wrong_result} (*checked)({arguments}) = {symbol};\n",
                    accepted=False, diagnostic="incompatible pointer type",
                )

    def test_function_owners_reject_wrong_argument_types(self) -> None:
        for header, symbol, result, arguments, _ in DECLARATIONS:
            with self.subTest(symbol=symbol):
                wrong_arguments = "void *" if arguments == "s32" else "s32"
                self.probe(
                    f'#include "{header}"\n'
                    f"{result} (*checked)({wrong_arguments}) = {symbol};\n",
                    accepted=False, diagnostic="incompatible pointer type",
                )

    def test_owner_removal_rejects_implicit_calls(self) -> None:
        for header, symbol, _, _, call in DECLARATIONS:
            with self.subTest(symbol=symbol):
                body = f"void checked(void) {{ {symbol}({call}); }}\n"
                self.probe(f'#include "{header}"\n' + body, accepted=True)
                self.probe(
                    '#include "../types.h"\n' + body, accepted=False,
                    diagnostic=f"implicit declaration of function `{symbol}'",
                )

    def test_function_prototypes_reject_wrong_arity(self) -> None:
        for header, symbol, _, arguments, _ in DECLARATIONS:
            for call in (("", "0, 0") if arguments == "s32" else ("0",)):
                with self.subTest(symbol=symbol, call=call):
                    self.probe(
                        f'#include "{header}"\n'
                        f"void checked(void) {{ {symbol}({call}); }}\n",
                        accepted=False, diagnostic=r"too (?:few|many) arguments",
                    )

    def test_high_memory_words_have_owned_data_types(self) -> None:
        for symbol, correct, incorrect in (
            ("D_80010030", "s32 *", "void **"),
            ("D_80010034", "void **", "s32 *"),
        ):
            for owner, declaration, accepted in (
                (True, correct, True), (True, incorrect, False), (False, correct, False),
            ):
                with self.subTest(symbol=symbol, owner=owner, declaration=declaration):
                    self.probe(
                        ('#include "high_memory_addresses.h"\n' if owner
                         else '#include "../types.h"\n')
                        + f"{declaration}checked = &{symbol};\n",
                        accepted=accepted,
                        diagnostic="incompatible pointer type|undeclared",
                    )
        self.probe(
            '#include "high_memory_addresses.h"\n#include "file_transfer.h"\n'
            '#include "file_names.h"\n#include "sound_init.h"\n'
            "void checked(void) {\n"
            " File_RequestAsyncTransfer(1, D_800114F8, 1223, 16, 0, 0, D_80010030);\n"
            " func_80049394(D_80010034);\n}\n",
            accepted=True,
        )

    def test_default_clamp_and_absolute_views_remain_scalars(self) -> None:
        for prefix in (
            "", "#define MODEL_GRAPHICS_STATE_CLAMP_NONVOLATILE\n",
            "#define MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE\n",
        ):
            with self.subTest(prefix=prefix):
                self.probe(
                    prefix + STATE_HEADER
                    + "typedef char extent[sizeof(D_8009AFA4) == 1 ? 1 : -1];\n"
                    "u8 *first = &D_8009AFA4;\nu8 *interior = &D_8009AFA6;\n"
                    "void checked(void) { D_8009AFA4 = 1; }\n",
                    accepted=True,
                )
                self.probe(
                    prefix + STATE_HEADER
                    + "void checked(void) { D_8009AFA4[3] = 1; }\n",
                    accepted=False, diagnostic="subscripted value",
                )

    def test_scene_view_has_exact_four_byte_extent(self) -> None:
        self.probe(
            BYTE_VIEW + STATE_HEADER
            + "typedef char extent[sizeof(D_8009AFA4) == 4 ? 1 : -1];\n"
            "u8 (*checked)[4] = &D_8009AFA4;\n"
            "void update(void) { D_8009AFA4[3] ^= 1; }\n",
            accepted=True,
        )
        for body, diagnostic in (
            ("u8 (*checked)[1] = &D_8009AFA4;\n", "incompatible pointer type"),
            ("extern u8 D_8009AFA4[1];\n", "conflicting types"),
            ("void checked(void) { D_8009AFA4 = 1; }\n", "incompatible types"),
        ):
            with self.subTest(body=body):
                self.probe(
                    BYTE_VIEW + STATE_HEADER + body,
                    accepted=False, diagnostic=diagnostic,
                )

    def test_scene_and_absolute_addressing_are_not_silently_combined(self) -> None:
        self.probe(
            BYTE_VIEW + "#define MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE\n" + STATE_HEADER,
            accepted=False, diagnostic="scene byte view requires GP-relative addressing",
        )

    def test_scalar_backing_cannot_satisfy_the_array_owner(self) -> None:
        self.probe(
            BYTE_VIEW + STATE_HEADER
            + 'u8 D_8009AFA4 __attribute__((section(".sdata"))) = 1;\n',
            accepted=False, diagnostic="conflicting types",
        )
        self.probe(
            BYTE_VIEW + STATE_HEADER
            + 'u8 D_8009AFA4[4] __attribute__((section(".sdata"))) = {1, 0, 0, 0};\n',
            accepted=True,
        )

    def test_existing_definition_and_caller_include_the_scene_owner(self) -> None:
        for name in ("model_intro_controller.c", "model_scene_states.c"):
            with self.subTest(source=name):
                path = REPOSITORY / "src/game" / name
                self.assertIn('#include "model_scene_states.h"', path.read_text())
                result = self.compile(path)
                self.assertEqual(result.returncode, 0, result.stderr)
        unmatched = (REPOSITORY / "src/unmatched.h").read_text()
        self.assertNotRegex(unmatched, r"\bfunc_800507D0\s*\(")

    def test_gp_byte_addend_and_absolute_high_memory_relocations(self) -> None:
        path = self.scratch / "views.c"
        path.write_text(
            BYTE_VIEW + STATE_HEADER
            + f'#include "{REPOSITORY / "src/game/high_memory_addresses.h"}"\n'
            "u8 read_slot(void) { return D_8009AFA4[3]; }\n"
            "void write_slot(u8 slot) { D_8009AFA4[3] = slot; }\n"
            "s32 transfer_word(void) { return D_80010030; }\n"
            "void *data_word(void) { return D_80010034; }\n"
        )
        obj = self.build_object(path, "views")
        result = self.run_command([str(tool(REPOSITORY, "objdump")), "-dr", str(obj)])
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertRegex(result.stdout, r"lbu\s+v0,3\(gp\)")
        self.assertRegex(result.stdout, r"sb\s+a0,3\(gp\)")
        self.assertEqual(result.stdout.count("R_MIPS_GPREL16\tD_8009AFA4"), 2)
        for symbol in ("D_80010030", "D_80010034"):
            self.assertIn("R_MIPS_HI16\t" + symbol, result.stdout)
            self.assertIn("R_MIPS_LO16\t" + symbol, result.stdout)
            self.assertNotIn("R_MIPS_GPREL16\t" + symbol, result.stdout)

    def test_scalar_addressing_arms_keep_their_relocations(self) -> None:
        for prefix, absolute in (
            ("", False), ("#define MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE\n", True),
        ):
            with self.subTest(absolute=absolute):
                path = self.scratch / "scalar.c"
                path.write_text(
                    prefix + STATE_HEADER
                    + "u8 checked(void) { return D_8009AFA4; }\n"
                )
                obj = self.build_object(path, "scalar")
                result = self.run_command([
                    str(tool(REPOSITORY, "objdump")), "-r", str(obj),
                ])
                self.assertEqual(result.returncode, 0, result.stderr)
                if absolute:
                    self.assertRegex(result.stdout, r"R_MIPS_HI16\s+D_8009AFA4")
                    self.assertRegex(result.stdout, r"R_MIPS_LO16\s+D_8009AFA4")
                    self.assertNotIn("R_MIPS_GPREL16", result.stdout)
                else:
                    self.assertRegex(result.stdout, r"R_MIPS_GPREL16\s+D_8009AFA4")

    def test_real_storage_extent_initial_bytes_and_interior_link_identity(self) -> None:
        owner = REPOSITORY / "src/game/model_graphics_state.c"
        self.assertIn("#define MODEL_GRAPHICS_STATE_SCENE_BYTES", owner.read_text())
        obj = self.build_object(owner, "state")
        symbols = self.run_command([str(tool(REPOSITORY, "objdump")), "-t", str(obj)])
        self.assertEqual(symbols.returncode, 0, symbols.stderr)
        self.assertRegex(symbols.stdout, r"0000001c g\s+\.sdata\s+\w+ D_8009AFA4")
        self.assertRegex(
            symbols.stdout, r"00000020 l\s+\.sdata\s+\w+ sModelGraphicsState_ValueA8",
        )
        self.assertNotRegex(symbols.stdout, r"\.sdata\s+\w+ D_8009AFA6")
        self.assertNotIn("sModelGraphicsState_PadA7", symbols.stdout)
        script = self.scratch / "storage.ld"
        script.write_text(
            "D_80091008 = 0x80091008;\n"
            "D_8009AFA6 = D_8009AFA4 + 2;\n"
            "SECTIONS { .sdata 0x8009AF88 : { *(.sdata) } }\n"
        )
        elf = self.scratch / "storage.elf"
        linked = self.run_command([
            str(tool(REPOSITORY, "ld")), "-EL", "-T", str(script), "-o", str(elf), str(obj),
        ])
        self.assertEqual(linked.returncode, 0, linked.stderr)
        names = self.run_command([str(tool(REPOSITORY, "nm")), "-n", str(elf)])
        self.assertEqual(names.returncode, 0, names.stderr)
        for symbol, address in (("D_8009AFA4", "8009afa4"), ("D_8009AFA6", "8009afa6")):
            self.assertRegex(names.stdout, address + r" [A-Za-z] " + symbol)
        binary = self.scratch / "storage.bin"
        copied = self.run_command([
            str(tool(REPOSITORY, "objcopy")), "-O", "binary", "--only-section=.sdata",
            str(elf), str(binary),
        ])
        self.assertEqual(copied.returncode, 0, copied.stderr)
        self.assertEqual(binary.read_bytes(), RETAIL_STATE)

    def test_interior_reference_requires_a_real_link_mapping(self) -> None:
        owner = self.build_object(REPOSITORY / "src/game/model_graphics_state.c", "owner")
        source = self.scratch / "interior.c"
        source.write_text(STATE_HEADER + "u8 *checked(void) { return &D_8009AFA6; }\n")
        consumer = self.build_object(source, "interior")
        script = self.scratch / "missing-alias.ld"
        script.write_text(
            "D_80091008 = 0x80091008;\n"
            "SECTIONS { .text 0x80050000 : { *(.text) }\n"
            ".sdata 0x8009AF88 : { *(.sdata) } }\n"
        )
        result = self.run_command([
            str(tool(REPOSITORY, "ld")), "-EL", "-T", str(script),
            "-o", str(self.scratch / "missing-alias.elf"), str(owner), str(consumer),
        ])
        self.assertNotEqual(result.returncode, 0, result.stderr)
        self.assertRegex(result.stderr, r"undefined reference to .D_8009AFA6")


if __name__ == "__main__":
    unittest.main()
