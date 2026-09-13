"""Ring-prefix, preset and local-frame ownership with real SDK/ILP32 types."""

from __future__ import annotations

import contextlib
import csv
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
SDK = (
    '#include "../psyq/libgte.h"\n#include "../psyq/libgpu.h"\n'
    '#include "../psyq/libgs.h"\n'
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


class ModelRingEffectOwnershipTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        self.scratch = ROOT / "tmp" / f"test-ring-ownership-{uuid.uuid4().hex}"
        self.scratch.mkdir(parents=True)
        self.addCleanup(shutil.rmtree, self.scratch)
        self.environment = os.environ.copy()
        self.environment["TMPDIR"] = str(self.scratch)

    def command(self, arguments: list[str]) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            arguments, cwd=ROOT, env=self.environment, capture_output=True,
            text=True, errors="replace", timeout=60,
        )

    def probe(self, text: str, *, accepted: bool, diagnostic: str = "") -> None:
        path = self.scratch / "probe.c"
        path.write_text(text)
        compilers = []
        if GCC.is_file():
            compilers.append([str(GCC), "-S", "-O2", "-G0", "-Werror"])
        if CC:
            compilers.append([
                CC, "-S", "-m32", "-std=gnu99", "-D_LANGUAGE_C",
                "-Werror=incompatible-pointer-types", "-Werror=int-conversion",
                "-Werror=implicit-function-declaration",
            ])
        if not compilers:
            self.skipTest("needs GCC 2.8.1 or a native C compiler")
        for compiler in compilers:
            with self.subTest(compiler=compiler[0], source=text):
                result = self.command(compiler + [
                    "-Wimplicit-function-declaration", "-I", str(ROOT / "src/game"),
                    "-o", str(self.scratch / "probe.s"), str(path),
                ])
                if accepted:
                    self.assertEqual(result.returncode, 0, result.stderr)
                else:
                    self.assertNotEqual(result.returncode, 0, result.stderr)
                    self.assertRegex(result.stderr, diagnostic)

    def test_self_contained_ring_owner_and_bounded_layout(self) -> None:
        header = '#include "model_ring_effect.h"\n'
        self.probe(
            header + "s32 (*effect)(RingWork *, s32) = func_8006C37C;\n"
            "typedef char preset[sizeof(RingSettings) == 12 ? 1 : -1];\n"
            "typedef char prefix[sizeof(RingWork) == 0x230 ? 1 : -1];\n"
            "typedef char vectors[sizeof(((RingWork *)0)->vertices) == 67 * 8 ? 1 : -1];\n"
            "typedef char query[sizeof(((RingQueryFrameReservation *)0)->value) == 8 ? 1 : -1];\n"
            "typedef char frame[sizeof(RingQueryFrameReservation) == 32 ? 1 : -1];\n"
            "SVECTOR (*vertices)[67] = &((RingWork *)0)->vertices;",
            accepted=True,
        )
        for declaration in (
            "void (*effect)(RingWork *, s32) = func_8006C37C;",
            "s32 (*effect)(RingWork *, s16) = func_8006C37C;",
            "SVECTOR (*vertices)[66] = &((RingWork *)0)->vertices;",
        ):
            self.probe(
                header + declaration, accepted=False,
                diagnostic="incompatible pointer|incompatible-pointer",
            )
        self.probe(
            header + "void call(RingWork *work) { func_8006C37C(work); }",
            accepted=False, diagnostic="too few arguments",
        )

    def test_dimension_output_is_not_the_frame_envelope(self) -> None:
        header = '#include "model_ring_effect.h"\n'
        self.probe(
            header + "void query(RingQueryFrameReservation *frame) {\n"
            " func_80057E20(1, &frame->value);\n}",
            accepted=True,
        )
        self.probe(
            header + "void query(RingQueryFrameReservation *frame) {\n"
            " func_80057E20(1, frame);\n}",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )

    def test_getter_and_sdk_import_prototypes_remain_measured_views(self) -> None:
        for header, symbol, result, wrong in (
            ("model_graphics_state.h", "func_80058DCC", "s32", "u8"),
            ("screen_projection.h", "func_80059220", "void *", "MATRIX *"),
            ("ordering_tables.h", "func_80058F10", "void *", "GsOT *"),
        ):
            with self.subTest(symbol=symbol):
                self.probe(
                    f'#include "{header}"\n{result} (*getter)(void) = {symbol};',
                    accepted=True,
                )
                self.probe(
                    f'#include "{header}"\n{wrong} (*getter)(void) = {symbol};',
                    accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
                )
                self.probe(
                    f'#include "{header}"\nvoid call(void) {{ {symbol}(1); }}',
                    accepted=False, diagnostic="too many arguments",
                )

    def test_sdk_projection_and_packet_call_types(self) -> None:
        self.probe(
            SDK + '#include "gpu_packets.h"\n'
            "void (*project)(SVECTOR *, DVECTOR *, u16 *, u16 *, u16 *, long) = RotTransPersN;\n"
            "void (*triangle)(POLY_G3 *) = SetPolyG3;\n"
            "void (*quad)(POLY_G4 *) = SetPolyG4;\n"
            "void (*submit)(u32 *, GsOT *, s32, s32) = func_8005B260;\n"
            "typedef char matrix_translation[sizeof(((MATRIX *)0)->t) == 12 ? 1 : -1];\n"
            "typedef char vector[sizeof(VECTOR) == 16 ? 1 : -1];\n"
            "typedef char triangle_size[sizeof(POLY_G3) == 28 ? 1 : -1];\n"
            "typedef char quad_size[sizeof(POLY_G4) == 36 ? 1 : -1];",
            accepted=True,
        )
        self.probe(
            SDK
            + "void (*project)(SVECTOR *, DVECTOR *, long *, long *, long *, long) = RotTransPersN;",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )
        self.probe(
            '#include "gpu_packets.h"\n'
            "void (*submit)(u32 *, GsOT *, u16, s32) = func_8005B260;",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )

    def test_address_named_import_remains_sdk_assembly(self) -> None:
        with (ROOT / "config/slus_01411/functions.csv").open() as stream:
            entry = next(row for row in csv.DictReader(stream)
                         if int(row["address"], 0) == 0x80058F10)
        self.assertEqual(entry["name"], "func_80058F10")
        self.assertEqual(entry["status"], "sdk_asm")
        self.assertEqual(int(entry["size"], 0), 16)
        header = (ROOT / "src/game/ordering_tables.h").read_text()
        self.assertIn("void *func_80058F10(void);", header)

    @unittest.skipUnless(GCC.is_file(), "needs GCC 2.8.1")
    def test_existing_getter_definitions_include_compatible_owners(self) -> None:
        for source, header in (
            ("model_state_getters.c", "model_graphics_state.h"),
            ("model_buffer_getters.c", "screen_projection.h"),
        ):
            path = ROOT / "src/game" / source
            self.assertIn(f'#include "{header}"', path.read_text())
            result = self.command([
                str(GCC), "-S", "-O2", "-G0", "-Werror",
                "-Wimplicit-function-declaration", str(path),
                "-o", str(self.scratch / "definition.s"),
            ])
            self.assertEqual(result.returncode, 0, result.stderr)

    @unittest.skipUnless(GCC.is_file(), "needs GCC 2.8.1")
    def test_real_preset_definition_has_exact_twelve_byte_storage(self) -> None:
        profiles = json.loads(
            (ROOT / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            obj = compile_c(
                ROOT, tool(ROOT, "as"),
                {"source": "src/game/model_geometry_tables.c",
                 "profile": "gcc_2_8_1_g0", "object": "presets.o"}, profiles,
                object_directory=str(self.scratch.relative_to(ROOT)),
                asm_directory=str(self.scratch.relative_to(ROOT)),
            )
        script = self.scratch / "presets.ld"
        script.write_text("SECTIONS { .data 0x800915E8 : { *(.data) } }\n")
        elf = self.scratch / "presets.elf"
        result = self.command([
            str(tool(ROOT, "ld")), "-EL", "-T", str(script),
            "-o", str(elf), str(obj),
        ])
        self.assertEqual(result.returncode, 0, result.stderr)
        binary = self.scratch / "presets.bin"
        result = self.command([
            str(tool(ROOT, "objcopy")), "-O", "binary", "--only-section=.data",
            str(elf), str(binary),
        ])
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(
            binary.read_bytes()[0x1C:0x28],
            bytes.fromhex("202020002c0114000a000000"),
        )
        names = self.command([str(tool(ROOT, "nm")), "-n", str(elf)])
        self.assertEqual(names.returncode, 0, names.stderr)
        self.assertRegex(names.stdout, r"80091604 [A-Za-z] D_80091604")
        self.assertRegex(names.stdout, r"80091610 [A-Za-z] D_80091610")

    def native_helpers(self, assume_large_query_output: bool) -> int:
        if not (
            CC and platform.system() == "Linux"
            and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
        ):
            self.skipTest("needs native Linux x86 ILP32 execution")
        source = self.scratch / "owners.c"
        source.write_text(
            '#include "model_ring_effect.h"\n#include "model_geometry_tables.h"\n'
            '#include "model.h"\n#include "model_graphics_state.h"\n'
            '#include "func_80058E1C.h"\n'
            "ModelSlot D_800F2C40[MODEL_SLOT_COUNT];\n"
            "u8 D_8009AFA0;\nvolatile u8 D_8009AFA3;\n"
            "static struct { u32 before; RingWork work; u32 after; } borrowed;\n"
            "static struct { u32 before; RingQueryFrameReservation query; u32 after; } frame;\n"
            "int main(void) {\n"
            " s32 i;\nRingSettings *settings = (RingSettings *)D_80091604;\n"
            " if (sizeof(void *) != 4 || sizeof(long) != 4) return 1;\n"
            " if (settings->color.r != 32 || settings->color.g != 32 ||\n"
            "     settings->color.b != 32 || settings->color.cd != 0) return 2;\n"
            " if (settings->minimum_radius != 300 || settings->fade_divisor != 20 ||\n"
            "     settings->growth_divisor != 10 || settings->pad_0A != 0) return 3;\n"
            " borrowed.before = 0x12345678; borrowed.after = 0x76543210;\n"
            " borrowed.work.settings = settings;\n"
            " for (i = 0; i < MODEL_RING_VERTEX_COUNT; i++) {\n"
            "   borrowed.work.vertices[i].vx = i;\n"
            "   borrowed.work.vertices[i].vy = -i;\n"
            "   borrowed.work.vertices[i].vz = 2 * i;\n"
            " }\n"
            " borrowed.work.color.r = 17; borrowed.work.fade = 20;\n"
            " borrowed.work.growth = 10; borrowed.work.phase = 255;\n"
            " if (borrowed.before != 0x12345678 || borrowed.after != 0x76543210 ||\n"
            "     borrowed.work.vertices[66].vz != 132) return 4;\n"
            " frame.before = 0xA1B2C3D4; frame.after = 0xD4C3B2A1;\n"
            " for (i = 0; i < 32; i++) frame.query.frame_bytes[i] = 0xA5;\n"
            " D_8009AFA0 = 1;\n"
            " D_800F2C40[1].field_DC8[0] = 100;\n"
            " D_800F2C40[1].field_DC8[1] = 200;\n"
            " D_800F2C40[1].field_DC8[2] = 300;\n"
            " func_80057E20(func_80058DCC(), &frame.query.value);\n"
            " if (frame.query.value.x != 100 || frame.query.value.y != 200 ||\n"
            "     frame.query.value.z != 300 || frame.query.value.max != 300) return 5;\n"
            " if (frame.before != 0xA1B2C3D4 || frame.after != 0xD4C3B2A1) return 6;\n"
            " for (i = 8; i < 32; i++) if (frame.query.frame_bytes[i] != "
            + ("0" if assume_large_query_output else "0xA5")
            + ") return 7;\n"
            " for (i = 0; i < 256; i++) {\n"
            "   D_8009AFA3 = i;\n"
            "   if (func_80058E1C() != (i < 6 ? i : 6)) return 8;\n"
            " }\n"
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
            str(source), str(ROOT / "src/game/model_geometry_tables.c"),
            str(ROOT / "src/game/func_80057E20.c"),
            str(ROOT / "src/game/model_state_getters.c"),
            str(ROOT / "src/game/func_80058E1C.c"), str(startup),
            "-o", str(executable),
        ])
        self.assertEqual(result.returncode, 0, result.stderr)
        return self.command([str(executable)]).returncode

    def test_native_real_settings_helpers_and_borrowed_prefix_guards(self) -> None:
        self.assertEqual(self.native_helpers(assume_large_query_output=False), 0)

    def test_native_helper_does_not_initialize_the_unused_frame_tail(self) -> None:
        self.assertEqual(self.native_helpers(assume_large_query_output=True), 7)


if __name__ == "__main__":
    unittest.main()
