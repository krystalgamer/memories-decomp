"""Old-GCC owner controls and a bounded link of the split small-data run."""

import contextlib
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))
from build_baseline import compile_c, tool

COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


@unittest.skipUnless(COMPILER.is_file(), "requires the GCC 2.8.1 toolchain")
class ModelDebugOwnershipTests(unittest.TestCase):
    def setUp(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        scratch = ROOT / "tmp/test-model-debug-ownership"
        scratch.mkdir(parents=True, exist_ok=True)
        self.directory = Path(tempfile.mkdtemp(dir=scratch))
        self.addCleanup(shutil.rmtree, self.directory)
        self.environment = dict(os.environ, TMPDIR=str(self.directory))

    def command(self, arguments):
        return subprocess.run(
            arguments, cwd=ROOT, env=self.environment,
            capture_output=True, text=True, timeout=60,
        )

    def compile(self, path):
        return self.command([
            str(COMPILER), "-S", "-O2", "-G8", "-Werror",
            "-Wimplicit-function-declaration", "-D_LANGUAGE_C",
            "-I", str(ROOT / "src/game"), str(path),
            "-o", str(self.directory / "probe.s"),
        ])

    def probe(self, source, accepted=True, diagnostic=""):
        path = self.directory / "probe.c"
        path.write_text(source)
        result = self.compile(path)
        if accepted:
            self.assertEqual(result.returncode, 0, result.stderr)
        else:
            self.assertNotEqual(result.returncode, 0, result.stderr)
            self.assertRegex(result.stderr, diagnostic)

    def test_controller_owner_return_and_arity(self):
        prefix = '#include "model_debug_controller.h"\n'
        self.probe(prefix + "s32 (*tick)(void) = func_800534B8;\n")
        self.probe(
            prefix + "void (*tick)(void) = func_800534B8;\n",
            False, "incompatible pointer type",
        )
        self.probe(
            prefix + "s32 tick(void) { return func_800534B8(1); }\n",
            False, "too many arguments",
        )
        self.probe(
            '#include "../unmatched.h"\ns32 tick(void) { return func_800534B8(); }\n',
            False, "implicit declaration of function `func_800534B8'",
        )

    def test_actual_definition_and_callers_keep_visible_owners(self):
        for relative in (
            "src/game/func_800534B8.c", "src/game/main_run_animated_battle.c",
            "src/game/func_80052D2C.c", "src/game/model_intro_controller.c",
        ):
            with self.subTest(source=relative):
                result = self.compile(ROOT / relative)
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_property_cache_extents_and_signedness(self):
        prefix = '#include "model_scene_setup.h"\n'
        self.probe(
            prefix + "s16 (*ids)[3] = &D_8009B488;\n"
            "u8 (*first)[2] = &D_8009B48E;\nu8 (*second)[2] = &D_8009B490;\n"
            "typedef char ids_size[sizeof(D_8009B488) == 6 ? 1 : -1];\n"
            "void configure(void) {\n"
            " Model_SetSlotProperties(0, 1, -1, -1, 0, 0);\n"
            " Model_SetSlotProperties(2, 0);\n}\n"
        )
        for wrong in ("extern u16 D_8009B488[3];", "extern u8 D_8009B48E[3];"):
            with self.subTest(wrong=wrong):
                self.probe(prefix + wrong, False, "conflicting types")

    def test_input_views_keep_true_halfword_extents(self):
        prefix = (
            "#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE\n"
            "#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE\n"
            "#define GINPUT_PAD2_PRESSED_IN_DATA\n"
            '#include "input.h"\n'
        )
        self.probe(
            prefix + "volatile u16 *repeat = &gInput_wPad1Repeat;\n"
            "volatile u16 *pressed = &gInput_wPad1Pressed;\n"
            "u16 *second = &gInput_wPad2Pressed;\n"
            "typedef char size[(sizeof(gInput_wPad1Repeat) == 2 &&\n"
            " sizeof(gInput_wPad1Pressed) == 2 && sizeof(gInput_wPad2Pressed) == 2) ? 1 : -1];\n"
        )
        self.probe(
            prefix + "void bad(void) { gInput_wPad2Pressed[1] = 0; }\n",
            False, "subscripted value",
        )

    def test_diagnostic_views_do_not_enlarge_backing(self):
        header = '#include "model_handler_state.h"\n'
        self.probe(
            header + "typedef char width[sizeof(D_8009B004) == 4 ? 1 : -1];\n"
            "typedef char normal[sizeof(D_8009B00C) == 8 ? 1 : -1];\n"
            "typedef char newline[sizeof(D_8009B02C) == 4 ? 1 : -1];\n"
        )
        self.probe(header + "extern u16 D_8009B004;\n", False, "conflicting types")
        self.probe(
            "#define MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY\n" + header
            + "char *normal = D_8009B00C;\nchar *newline = D_8009B02C;\n",
        )
        self.probe(
            "#define MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY\n" + header
            + "u32 bad(void) { return sizeof(D_8009B00C); }\n",
            False, "incomplete type",
        )

    def test_live_model_pointer_and_height_layout(self):
        self.probe(
            '#include "model.h"\n#include "camera_view.h"\n'
            "#define OFFSET(t, m) ((u32)&(((t *)0)->m))\n"
            "typedef char placement[OFFSET(ModelSlot, field_D18) == 0xD18 ? 1 : -1];\n"
            "typedef char rotation[OFFSET(GsCOORDUNIT, rot) == 0x44 ? 1 : -1];\n"
            "typedef char height[OFFSET(GsCOORDUNIT, matrix.t[1]) == 0x1C ? 1 : -1];\n"
            "GsCOORDUNIT **unit = &D_800F2C40[2].field_D18;\n"
        )

    @unittest.skipUnless((ROOT / "game/SLUS_014.11").is_file(), "requires the retail input")
    def test_split_small_data_has_one_real_word_and_identical_neighbors(self):
        profiles = json.loads((ROOT / "config/slus_01411/compiler_profiles.json").read_text())["profiles"]
        objects = []
        for name, source, profile in (
            ("prefix", "src/game/model_handler_state.c", "gcc_2_8_1_g8"),
            ("controller", "src/game/func_800534B8.c", "gcc_2_8_1_g8_split"),
            ("suffix", "src/game/model_handler_diagnostics.c", "gcc_2_8_1_g8"),
        ):
            with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
                obj = compile_c(
                    ROOT, tool(ROOT, "as"),
                    {"source": source, "profile": profile, "object": name + ".o"},
                    profiles, object_directory=str(self.directory.relative_to(ROOT)),
                    asm_directory=str(self.directory.relative_to(ROOT)),
                )
            if name == "controller":
                symbols = self.command([str(tool(ROOT, "nm")), "-S", "--defined-only", str(obj)])
                self.assertEqual(symbols.returncode, 0, symbols.stderr)
                self.assertRegex(symbols.stdout, r"(?m)^[0-9a-f]+(?: 00000004)? D D_8009B004$")
                self.assertNotRegex(symbols.stdout, r" C D_8009B004$")
            data = self.directory / (name + ".data.o")
            copied = self.command([str(tool(ROOT, "objcopy")), "--only-section=.sdata", str(obj), str(data)])
            self.assertEqual(copied.returncode, 0, copied.stderr)
            raw = self.directory / (name + ".bin")
            copied = self.command([str(tool(ROOT, "objcopy")), "-O", "binary", str(data), str(raw)])
            self.assertEqual(copied.returncode, 0, copied.stderr)
            self.assertEqual(len(raw.read_bytes()), {"prefix": 0x20, "controller": 4, "suffix": 0x50}[name])
            objects.append(data)
        script = self.directory / "layout.ld"
        script.write_text(
            "SECTIONS { .sdata 0x8009AFE4 : SUBALIGN(2) { *(.sdata) } }\n"
            'ASSERT(SIZEOF(.sdata) == 0x74, "wrong state-run extent")\n'
            'ASSERT(D_8009B004 == 0x8009B004, "wrong owned word address")\n'
            'ASSERT(D_8009B008 == 0x8009B008, "wrong suffix address")\n'
        )
        elf = self.directory / "state.elf"
        linked = self.command([
            str(tool(ROOT, "ld")), "-EL", "-T", str(script),
            *map(str, objects), "-o", str(elf),
        ])
        self.assertEqual(linked.returncode, 0, linked.stderr)
        binary = self.directory / "state.bin"
        copied = self.command([str(tool(ROOT, "objcopy")), "-O", "binary", str(elf), str(binary)])
        self.assertEqual(copied.returncode, 0, copied.stderr)
        with (ROOT / "game/SLUS_014.11").open("rb") as retail:
            retail.seek(0x8B7E4)
            self.assertEqual(binary.read_bytes(), retail.read(0x74))
        missing = self.command([
            str(tool(ROOT, "ld")), "-EL", "-T", str(script),
            str(objects[0]), str(objects[2]), "-o", str(self.directory / "missing.elf"),
        ])
        self.assertNotEqual(missing.returncode, 0, missing.stderr)
        self.assertRegex(missing.stderr, r"D_8009B004|wrong state-run extent|wrong suffix address")


if __name__ == "__main__":
    unittest.main()
