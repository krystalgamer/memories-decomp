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
            "void (*controller)(void) = Model_UpdateCamera;\n"
            "GsRVIEW2 *view = &gModel_RView;\n"
            "typedef char size[sizeof(gModel_RView) == 32 ? 1 : -1];\n"
            "void tick(void) { Model_UpdateCamera(); }\n"
        )

    def test_controller_owner_rejects_wrong_return_and_arity(self):
        for body, diagnostic in (
            ("s32 (*controller)(void) = Model_UpdateCamera;", "incompatible pointer type"),
            ("void tick(void) { Model_UpdateCamera(0); }", "too many arguments"),
        ):
            with self.subTest(body=body):
                self.probe('#include "camera_view.h"\n' + body, False, diagnostic)

    def test_umbrella_no_longer_supplies_controller(self):
        self.probe(
            '#include "../unmatched.h"\nvoid tick(void) { Model_UpdateCamera(); }\n',
            False, "implicit declaration of function `Model_UpdateCamera'",
        )

    def test_actual_controller_and_frame_caller_use_owned_prototypes(self):
        for relative in ("src/game/model_scene_controllers.c", "src/game/model_scene_states.c"):
            with self.subTest(source=relative):
                source = ROOT / relative
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
            "typedef char slot_position[OFFSET(ModelSlot, cached_position) == 0xDD0 ? 1 : -1];\n"
            "typedef char slots_size[sizeof(gModel_aSlots) == 3 * 0xE20 ? 1 : -1];\n"
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
            "extern ModelSlot gModel_aSlots[4];",
            "extern u8 gModel_RView;",
        ):
            with self.subTest(body=body):
                self.probe(
                    '#include "model.h"\n#include "camera_view.h"\n' + body,
                    False, "conflicting types",
                )

    def test_unused_fifth_orbit_argument_is_an_explicit_caller_view(self):
        prefix = '#define FUNC_80058434_CALL_WITH_UNUSED_ARG\n#include "model_step_view_orbit.h"\n'
        self.probe(prefix + "void orbit(void) { Model_StepViewOrbit(1, 2, 0, 0, 0); }\n")
        self.probe(
            prefix + "void orbit(void) { Model_StepViewOrbit(1, 2, 0, 0); }\n",
            False, "too few arguments",
        )
        self.probe(
            '#include "model_step_view_orbit.h"\n'
            "void orbit(void) { Model_StepViewOrbit(1, 2, 0, 0); }\n"
        )
        self.probe(
            '#include "model_step_view_orbit.h"\n'
            "void orbit(void) { Model_StepViewOrbit(1, 2, 0, 0, 0); }\n",
            False, "too many arguments",
        )

    def test_transfer_getters_keep_the_existing_integer_abi(self):
        prefix = '#include "model_transfer_state.h"\n'
        self.probe(
            prefix + "s32 (*gate)(void) = Model_IsEffectKeyQueueEmpty;\n"
            "s32 (*address)(void) = Model_GetCurrentEffectKeyAddress;\n"
            "Key *key(void) { return (Key *)Model_GetCurrentEffectKeyAddress(); }\n"
        )
        self.probe(
            prefix + "Key *(*address)(void) = Model_GetCurrentEffectKeyAddress;\n",
            False, "incompatible pointer type",
        )

    def test_queue_command_signature_and_nullable_endpoints(self):
        prefix = '#include "model_effect_state.h"\n'
        self.probe(
            prefix
            + "void (*command)(s32, ModelEffectEndpoint *, ModelEffectEndpoint *, s32)"
            " = Model_ApplyEffectKeyQueueCommand;\n"
            "void commands(ModelEffectEndpoint *first, ModelEffectEndpoint *second) {\n"
            "    Model_ApplyEffectKeyQueueCommand(0, first, second, 123);\n"
            "    Model_ApplyEffectKeyQueueCommand(1, 0, second, 0x4000);\n"
            "    Model_ApplyEffectKeyQueueCommand(2, 0, 0, 0);\n"
            "}\n"
        )
        for declaration, diagnostic in (
            (
                "s32 (*command)(s32, ModelEffectEndpoint *, ModelEffectEndpoint *, s32)"
                " = Model_ApplyEffectKeyQueueCommand;\n",
                "incompatible pointer type",
            ),
            (
                "void (*command)(s32, void *, ModelEffectEndpoint *, s32)"
                " = Model_ApplyEffectKeyQueueCommand;\n",
                "incompatible pointer type",
            ),
            (
                "void command(void) { Model_ApplyEffectKeyQueueCommand(0, 0, 0); }\n",
                "too few arguments",
            ),
        ):
            with self.subTest(declaration=declaration):
                self.probe(prefix + declaration, False, diagnostic)

    def test_target_slot_setter_keeps_full_width_request_arguments(self):
        prefix = '#include "model_effect_requests.h"\n'
        self.probe(
            prefix
            + "void (*setter)(s32, s32) = Model_SetCameraTargetSlots;\n"
            "void request(s32 first, s32 second) {\n"
            "    Model_SetCameraTargetSlots(first, second);\n"
            "    Model_SetCameraTargetSlots(-1, 1);\n"
            "}\n"
        )
        for declaration, diagnostic in (
            (
                "s32 (*setter)(s32, s32) = Model_SetCameraTargetSlots;\n",
                "incompatible pointer type",
            ),
            (
                "void (*setter)(s16, s32) = Model_SetCameraTargetSlots;\n",
                "incompatible pointer type",
            ),
            (
                "void request(void) { Model_SetCameraTargetSlots(1); }\n",
                "too few arguments",
            ),
        ):
            with self.subTest(declaration=declaration):
                self.probe(prefix + declaration, False, diagnostic)


    def test_unused_duration_definition_keeps_full_width_request(self):
        prefix = '#include "model_effect_requests.c"\n'
        self.probe(
            prefix
            + "void (*timing)(s32) = Model_SetCameraMoveDurationFromMagnitude;\n"
        )
        for declaration, diagnostic in (
            (
                "s32 (*timing)(s32) = Model_SetCameraMoveDurationFromMagnitude;\n",
                "incompatible pointer type",
            ),
            (
                "void (*timing)(s16) = Model_SetCameraMoveDurationFromMagnitude;\n",
                "incompatible pointer type",
            ),
            (
                "void timing(void) { Model_SetCameraMoveDurationFromMagnitude(); }\n",
                "too few arguments",
            ),
        ):
            with self.subTest(declaration=declaration):
                self.probe(prefix + declaration, False, diagnostic)

    def test_follow_countdown_request_keeps_the_signed_word_contract(self):
        prefix = '#include "model_effect_requests.h"\n'
        self.probe(
            prefix
            + "void (*request)(s32) = Model_SetCameraFollowCountdown;\n"
            "void call(s32 value) { Model_SetCameraFollowCountdown(value); }\n"
        )
        for declaration, diagnostic in (
            (
                "s32 (*request)(s32) = Model_SetCameraFollowCountdown;\n",
                "incompatible pointer type",
            ),
            (
                "void (*request)(u16) = Model_SetCameraFollowCountdown;\n",
                "incompatible pointer type",
            ),
            (
                "void call(void) { Model_SetCameraFollowCountdown(); }\n",
                "too few arguments",
            ),
        ):
            with self.subTest(declaration=declaration):
                self.probe(prefix + declaration, False, diagnostic)


    def test_orbit_step_definition_preserves_compact_region_contract(self):
        for source, extent in (
            ("model_effect_requests.c", "0x2C"),
            ("japanese/model_effect_requests.c", "0x2A"),
        ):
            prefix = f'#include "{source}"\n'
            with self.subTest(source=source):
                self.probe(
                    prefix
                    + "void (*step)(s32) = Model_SetCameraOrbitStep;\n"
                    f"typedef char layout[sizeof(ModelCameraMove) == {extent} ? 1 : -1];\n"
                )
                self.probe(
                    prefix
                    + "void (*step)(s16) = Model_SetCameraOrbitStep;\n",
                    False, "incompatible pointer type",
                )
                self.probe(
                    prefix
                    + "s32 (*step)(s32) = Model_SetCameraOrbitStep;\n",
                    False, "incompatible pointer type",
                )


    def test_slot_move_request_keeps_four_signed_words(self):
        prefix = '#include "model_effect_requests.h"\n'
        self.probe(
            prefix
            + "void (*request)(s32, s32, s32, s32) = Model_RequestCameraMoveToSlots;\n"
            "void call(void) { Model_RequestCameraMoveToSlots(1, -1, 0, -30); }\n"
        )
        for declaration, diagnostic in (
            (
                "s32 (*request)(s32, s32, s32, s32) = Model_RequestCameraMoveToSlots;\n",
                "incompatible pointer type",
            ),
            (
                "void (*request)(s32, s16, s32, s32) = Model_RequestCameraMoveToSlots;\n",
                "incompatible pointer type",
            ),
            (
                "void call(void) { Model_RequestCameraMoveToSlots(1, -1, 0); }\n",
                "too few arguments",
            ),
        ):
            with self.subTest(declaration=declaration):
                self.probe(prefix + declaration, False, diagnostic)


    def test_camera_completion_owner_preserves_int_argument_and_result(self):
        prefix = '#include "model_is_camera_move_complete.h"\n'
        self.probe(
            prefix
            + "int (*complete)(int) = Model_IsCameraMoveComplete;\n"
            "int query(void) { return Model_IsCameraMoveComplete(1)"
            " + Model_IsCameraMoveComplete(0); }\n"
        )
        for declaration, diagnostic in (
            (
                "void (*complete)(int) = Model_IsCameraMoveComplete;\n",
                "incompatible pointer type",
            ),
            (
                "int (*complete)(u8) = Model_IsCameraMoveComplete;\n",
                "incompatible pointer type",
            ),
            (
                "int query(void) { return Model_IsCameraMoveComplete(); }\n",
                "too few arguments",
            ),
        ):
            with self.subTest(declaration=declaration):
                self.probe(prefix + declaration, False, diagnostic)


if __name__ == "__main__":
    unittest.main()
