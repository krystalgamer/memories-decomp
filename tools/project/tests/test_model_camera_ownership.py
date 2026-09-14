"""Real old-GCC controls for the camera controller's existing owners."""

from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


@unittest.skipUnless(COMPILER.is_file(), "requires the GCC 2.8.1 toolchain")
class ModelCameraOwnershipTests(unittest.TestCase):
    def setUp(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        scratch = ROOT / "tmp/test-model-camera-ownership"
        scratch.mkdir(parents=True, exist_ok=True)
        self.directory = Path(tempfile.mkdtemp(dir=scratch))
        self.addCleanup(shutil.rmtree, self.directory)
        self.environment = dict(os.environ, TMPDIR=str(self.directory))

    def compile(self, path):
        return subprocess.run(
            [
                str(COMPILER), "-S", "-O2", "-G8", "-Werror",
                "-Wimplicit-function-declaration", "-D_LANGUAGE_C",
                "-I", str(ROOT / "src/game"), str(path),
                "-o", str(self.directory / "probe.s"),
            ],
            cwd=ROOT, env=self.environment, capture_output=True, text=True, timeout=60,
        )

    def probe(self, text, accepted=True, diagnostic=""):
        path = self.directory / "probe.c"
        path.write_text(text)
        result = self.compile(path)
        if accepted:
            self.assertEqual(result.returncode, 0, result.stderr)
        else:
            self.assertNotEqual(result.returncode, 0, result.stderr)
            self.assertRegex(result.stderr, diagnostic)

    def test_camera_owner_is_self_contained(self):
        self.probe(
            '#include "camera_view.h"\n'
            "void (*controller)(void) = func_80051A48;\n"
            "GsRVIEW2 *view = &D_800F56F0;\n"
            "typedef char size[sizeof(D_800F56F0) == 32 ? 1 : -1];\n"
            "void tick(void) { func_80051A48(); }\n"
        )

    def test_controller_owner_rejects_wrong_return_and_arity(self):
        for body, diagnostic in (
            ("s32 (*controller)(void) = func_80051A48;", "incompatible pointer type"),
            ("void tick(void) { func_80051A48(0); }", "too many arguments"),
        ):
            with self.subTest(body=body):
                self.probe('#include "camera_view.h"\n' + body, False, diagnostic)

    def test_umbrella_no_longer_supplies_controller(self):
        self.probe(
            '#include "../unmatched.h"\nvoid tick(void) { func_80051A48(); }\n',
            False, "implicit declaration of function `func_80051A48'",
        )
        self.assertNotIn("func_80051A48", (ROOT / "src/unmatched.h").read_text())

    def test_actual_controller_and_frame_caller_use_owned_prototypes(self):
        for relative in ("src/game/func_80051A48.c", "src/game/model_scene_states.c"):
            with self.subTest(source=relative):
                source = ROOT / relative
                self.assertIn('#include "camera_view.h"', source.read_text())
                result = self.compile(source)
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_model_and_key_layouts_are_measured_not_new_allocations(self):
        self.probe(
            '#include "model.h"\n#include "model_effect_key.h"\n'
            "#define OFFSET(t, m) ((u32)&(((t *)0)->m))\n"
            "typedef char move_size[sizeof(ModelCameraMove) == 44 ? 1 : -1];\n"
            "typedef char eye_index[OFFSET(ModelCameraMove, eye.pair_slot) == 18 ? 1 : -1];\n"
            "typedef char eye_end[OFFSET(ModelCameraMove, eye.slot) == 26 ? 1 : -1];\n"
            "typedef char target_index[OFFSET(ModelCameraMove, target.pair_slot) == 34 ? 1 : -1];\n"
            "typedef char target_end[OFFSET(ModelCameraMove, target.slot) == 42 ? 1 : -1];\n"
            "typedef char slot_position[OFFSET(ModelSlot, field_DD0) == 0xDD0 ? 1 : -1];\n"
            "typedef char slots_size[sizeof(D_800F2C40) == 3 * 0xE20 ? 1 : -1];\n"
            "typedef char first_kind[OFFSET(Key, requested[0].kind) == 6 ? 1 : -1];\n"
            "typedef char second_kind[OFFSET(Key, requested[1].kind) == 14 ? 1 : -1];\n"
            "ModelCameraMove *move = &D_800F2B20;\n"
            "s16 *signed_index = &D_800F2B20.eye.slot;\n"
            "u16 *duration = &D_800F2B20.duration;\n"
            "s16 *kind(Key *key) { return &key->requested[0].kind; }\n"
        )

    def test_owned_data_types_reject_incompatible_objects(self):
        for body in (
            "extern u8 D_800F2B20;",
            "extern ModelSlot D_800F2C40[4];",
            "extern u8 D_800F56F0;",
        ):
            with self.subTest(body=body):
                self.probe(
                    '#include "model.h"\n#include "camera_view.h"\n' + body,
                    False, "conflicting types",
                )

    def test_unused_fifth_orbit_argument_is_an_explicit_caller_view(self):
        prefix = '#define FUNC_80058434_CALL_WITH_UNUSED_ARG\n#include "func_80058434.h"\n'
        self.probe(prefix + "void orbit(void) { func_80058434(1, 2, 0, 0, 0); }\n")
        self.probe(
            prefix + "void orbit(void) { func_80058434(1, 2, 0, 0); }\n",
            False, "too few arguments",
        )
        self.probe(
            '#include "func_80058434.h"\n'
            "void orbit(void) { func_80058434(1, 2, 0, 0); }\n"
        )
        self.probe(
            '#include "func_80058434.h"\n'
            "void orbit(void) { func_80058434(1, 2, 0, 0, 0); }\n",
            False, "too many arguments",
        )

    def test_transfer_getters_keep_the_existing_integer_abi(self):
        prefix = '#include "model_transfer_state.h"\n'
        self.probe(
            prefix + "s32 (*gate)(void) = func_8005FB08;\n"
            "s32 (*address)(void) = func_8005FB14;\n"
            "Key *key(void) { return (Key *)func_8005FB14(); }\n"
        )
        self.probe(
            prefix + "Key *(*address)(void) = func_8005FB14;\n",
            False, "incompatible pointer type",
        )


if __name__ == "__main__":
    unittest.main()
