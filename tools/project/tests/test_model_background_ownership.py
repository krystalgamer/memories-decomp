"""Real GCC contracts and native ILP32 backing for the model background."""

from __future__ import annotations

import contextlib
import io
import json
import os
from pathlib import Path
import platform
import shutil
import struct
import subprocess
import sys
import unittest
import uuid


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))
from build_baseline import compile_c, tool


GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
CC = shutil.which("cc")
SDK = (
    '#include "../psyq/libgte.h"\n#include "../psyq/libgpu.h"\n'
    '#include "../psyq/libgs.h"\n#include "../psyq/libhmd.h"\n'
)
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


class ModelBackgroundOwnershipTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        self.scratch = ROOT / "tmp" / f"test-background-ownership-{uuid.uuid4().hex}"
        self.scratch.mkdir(parents=True)
        self.addCleanup(shutil.rmtree, self.scratch)
        self.environment = os.environ.copy()
        self.environment["TMPDIR"] = str(self.scratch)

    def command(self, arguments: list[str]) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            arguments, cwd=ROOT, env=self.environment, capture_output=True,
            text=True, errors="replace", timeout=60,
        )

    def probe(self, source: str, *, accepted: bool, diagnostic: str = "") -> None:
        path = self.scratch / "probe.c"
        path.write_text(source)
        compilers = []
        if GCC.is_file():
            compilers.append([str(GCC), "-S", "-O2", "-G8", "-Werror"])
        if CC:
            compilers.append([
                CC, "-S", "-m32", "-std=gnu99", "-D_LANGUAGE_C",
                "-Werror=incompatible-pointer-types", "-Werror=int-conversion",
                "-Werror=implicit-function-declaration",
            ])
        if not compilers:
            self.skipTest("needs GCC 2.8.1 or a native C compiler")
        for compiler in compilers:
            with self.subTest(compiler=compiler[0], source=source):
                result = self.command(compiler + [
                    "-Wimplicit-function-declaration", "-I", str(ROOT / "src/game"),
                    "-o", str(self.scratch / "probe.s"), str(path),
                ])
                if accepted:
                    self.assertEqual(result.returncode, 0, result.stderr)
                else:
                    self.assertNotEqual(result.returncode, 0, result.stderr)
                    self.assertRegex(result.stderr, diagnostic)

    def test_renderer_and_matrix_getter_have_owned_prototypes(self) -> None:
        header = SDK + '#include "model_background.h"\n#include "screen_projection.h"\n'
        self.probe(
            header + "void (*render)(void) = func_8004DE24;\n"
            "void *(*matrix)(void) = func_80059220;\n"
            "void set(void) { GsSetLsMatrix(func_80059220()); }",
            accepted=True,
        )
        for declaration in (
            "s32 (*render)(void) = func_8004DE24;",
            "MATRIX *(*matrix)(void) = func_80059220;",
        ):
            self.probe(
                header + declaration, accepted=False,
                diagnostic="incompatible pointer|incompatible-pointer",
            )
        for call in ("func_8004DE24(1)", "func_80059220(1)"):
            self.probe(
                header + f"void call(void) {{ {call}; }}",
                accepted=False, diagnostic="too many arguments",
            )
        self.probe(
            "void (*render)(void) = func_8004DE24;", accepted=False,
            diagnostic="undeclared",
        )

    def test_normal_pair_has_a_self_contained_owner(self) -> None:
        self.probe(
            '#include "model_background.h"\n'
            "typedef char normal_pair_size[sizeof(BackgroundNormals) == 16 ? 1 : -1];\n"
            "SVECTOR *second_normal(BackgroundNormals *pair) { return &pair->values[1]; }",
            accepted=True,
        )

    def test_sdk_calls_enforce_real_argument_counts(self) -> None:
        self.probe(
            SDK + "void (*color)(SVECTOR *, CVECTOR *, CVECTOR *) = NormalColorCol;\n"
            "void (*light)(MATRIX *) = GsSetLightMatrix;\n"
            "void call(SVECTOR *n, CVECTOR *c, MATRIX *m) {\n"
            " NormalColorCol(n, c, c); GsSetLightMatrix(m);\n}",
            accepted=True,
        )
        for call in (
            "NormalColorCol((SVECTOR *)0, (CVECTOR *)0)",
            "GsSetLightMatrix((MATRIX *)0, 0, 0, 0)",
        ):
            self.probe(
                SDK + f"void call(void) {{ {call}; }}", accepted=False,
                diagnostic="too (few|many) arguments",
            )

    def test_model_and_sdk_extents_agree_without_duplicate_objects(self) -> None:
        self.probe(
            SDK + '#include "model.h"\n#include "camera_view.h"\n'
            '#include "screen_projection.h"\n'
            "ModelSlot (*slots)[3] = &D_800F2C40;\n"
            "ModelSlot *third = &D_800F2C40[2];\n"
            "MATRIX *camera_matrix = &D_800F56A0.matrix;\n"
            "typedef char slots_size[sizeof(D_800F2C40) == 3 * 0xE20 ? 1 : -1];\n"
            "typedef char coord_size[sizeof(GsCOORDUNIT) == 80 ? 1 : -1];\n"
            "typedef char coord_y[(u32)&((GsCOORDUNIT *)0)->matrix.t[1] == 0x1C ? 1 : -1];\n"
            "typedef char light_size[sizeof(ModelSlotLightEntry) == sizeof(GsF_LIGHT) ? 1 : -1];\n"
            "typedef char matrix_size[sizeof(D_800FE148) == 32 ? 1 : -1];\n"
            "typedef char vertex_size[sizeof(SVECTOR) == 8 ? 1 : -1];\n"
            "typedef char color_size[sizeof(CVECTOR) == 4 ? 1 : -1];\n"
            "typedef char sprite_size[sizeof(GsSPRITE) == 36 ? 1 : -1];\n"
            "typedef char polygon_size[sizeof(POLY_G4) == 36 ? 1 : -1];\n"
            "typedef char output_size[sizeof(long) == 4 ? 1 : -1];",
            accepted=True,
        )
        self.probe(
            '#include "model.h"\nModelSlot D_800F2C40[2];',
            accepted=False, diagnostic="conflicting types",
        )
        self.probe(
            SDK + '#include "camera_view.h"\nMATRIX D_800F56A0;',
            accepted=False, diagnostic="conflicting types",
        )

    @unittest.skipUnless(GCC.is_file(), "needs GCC 2.8.1")
    def test_third_slot_link_value_is_the_retail_interior_address(self) -> None:
        path = self.scratch / "slot.c"
        path.write_text(
            f'#include "{ROOT / "src/game/model.h"}"\n'
            "ModelSlot *owned = &D_800F2C40[2];\n"
        )
        profiles = json.loads(
            (ROOT / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            obj = compile_c(
                ROOT, tool(ROOT, "as"),
                {"source": str(path.relative_to(ROOT)),
                 "profile": "gcc_2_8_1_g8_split", "object": "slot.o"}, profiles,
                object_directory=str(self.scratch.relative_to(ROOT)),
                asm_directory=str(self.scratch.relative_to(ROOT)),
            )
        script = self.scratch / "slot.ld"
        script.write_text(
            "D_800F2C40 = 0x800F2C40;\n"
            "SECTIONS { .data 0x80080000 : { *(.sdata .data) } }\n"
        )
        elf = self.scratch / "slot.elf"
        result = self.command([
            str(tool(ROOT, "ld")), "-EL", "-T", str(script),
            "-o", str(elf), str(obj),
        ])
        self.assertEqual(result.returncode, 0, result.stderr)
        binary = self.scratch / "slot.bin"
        result = self.command([
            str(tool(ROOT, "objcopy")), "-O", "binary", "--only-section=.data",
            str(elf), str(binary),
        ])
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(struct.unpack_from("<I", binary.read_bytes())[0], 0x800F4880)

    @unittest.skipUnless(GCC.is_file(), "needs GCC 2.8.1")
    def test_existing_definition_and_caller_include_their_owners(self) -> None:
        for name, header in (
            ("model_buffer_getters.c", "screen_projection.h"),
            ("model_scene_states.c", "model_background.h"),
        ):
            with self.subTest(name=name):
                path = ROOT / "src/game" / name
                self.assertIn(f'#include "{header}"', path.read_text())
                result = self.command([
                    str(GCC), "-S", "-O2", "-G8", "-Werror",
                    "-Wimplicit-function-declaration", "-o",
                    str(self.scratch / "definition.s"), str(path),
                ])
                self.assertEqual(result.returncode, 0, result.stderr)
        self.assertNotIn(
            "void func_8004DE24(void);", (ROOT / "src/unmatched.h").read_text(),
        )

    def native_owners(self, corrupt_record: bool) -> int:
        if not (
            CC and platform.system() == "Linux"
            and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
        ):
            self.skipTest("needs native Linux x86 ILP32 execution")
        source = self.scratch / "owners.c"
        source.write_text(
            SDK + '#include "model.h"\n#include "model_graphics_state.h"\n'
            '#include "model_record_tables.h"\n#include "camera_view.h"\n'
            '#include "screen_projection.h"\n'
            "ModelSlot D_800F2C40[MODEL_SLOT_COUNT];\n"
            "GsCOORDUNIT D_800F56A0;\nMATRIX D_800FE148;\nu8 *D_8009AF88;\n"
            "static GsCOORDUNIT placement;\n"
            "static union { u16 aligned; u8 bytes[0xB2]; } record;\n"
            "static MATRIX *observed_matrix;\n"
            "void GsSetLsMatrix(MATRIX *matrix) { observed_matrix = matrix; }\n"
            "static s32 supported(u8 *bytes) {\n"
            " u16 width = *(u16 *)(bytes + 0xA6);\n"
            " u16 depth = (*(u16 *)(bytes + 0xA8) >> 13) & 3;\n"
            " return depth < 3 && (u16)(width / 10u) != 0;\n}\n"
            "int main(void) {\n"
            " s32 i;\nModelSlot *slot = &D_800F2C40[2];\n"
            " if (sizeof(void *) != 4 || sizeof(long) != 4) return 1;\n"
            " if ((u32)slot - (u32)D_800F2C40 != 0x1C40) return 2;\n"
            " slot->field_D18 = &placement;\n"
            " placement.matrix.t[1] = 123;\nslot->field_E1F = 0;\n"
            " if (slot->field_D18->matrix.t[1] + 300 != 423) return 3;\n"
            " if ((u32)&D_800F56A0.matrix - (u32)&D_800F56A0 != 4) return 4;\n"
            " GsSetLsMatrix(func_80059220());\n"
            " if (observed_matrix != &D_800FE148) return 5;\n"
            " for (i = 0; i < 7; i++) if (!supported(D_80091008 + i * 0xB2)) return 6;\n"
            " for (i = 0; i < 0xB2; i++) record.bytes[i] = D_80091008[i];\n"
            " D_8009AF88 = record.bytes;\n"
            + (" *(u16 *)(D_8009AF88 + 0xA8) |= 0x6000;\n" if corrupt_record else "")
            + " if (!supported(D_8009AF88)) return 7;\n"
            " *(u16 *)(D_8009AF88 + 0xB0) = 0x1234;\n"
            " if (record.bytes[0xB0] != 0x34 || record.bytes[0xB1] != 0x12) return 8;\n"
            " return 0;\n}\n"
        )
        startup = self.scratch / "start.S"
        startup.write_text(START)
        executable = self.scratch / "owners"
        result = self.command([
            CC, "-m32", "-std=gnu99", "-D_LANGUAGE_C", "-O2",
            "-nostdlib", "-fno-pie", "-no-pie", "-ffreestanding",
            "-fno-builtin", "-fno-stack-protector", "-fno-strict-aliasing",
            "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
            "-Werror=incompatible-pointer-types", "-I", str(ROOT / "src/game"),
            str(source), str(ROOT / "src/game/model_buffer_getters.c"),
            str(ROOT / "src/game/model_record_tables.c"), str(startup),
            "-o", str(executable),
        ])
        self.assertEqual(result.returncode, 0, result.stderr)
        return self.command([str(executable)]).returncode

    def test_native_actual_record_table_getter_and_bounded_third_slot(self) -> None:
        self.assertEqual(self.native_owners(corrupt_record=False), 0)

    def test_native_fixture_rejects_depth_three_before_negative_shift(self) -> None:
        self.assertEqual(self.native_owners(corrupt_record=True), 7)


if __name__ == "__main__":
    unittest.main()
