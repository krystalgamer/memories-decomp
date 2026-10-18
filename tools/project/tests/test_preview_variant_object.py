"""Exercise the actual variant callback with canonical objects and bounded stubs."""

import os
from pathlib import Path
import platform
import shutil
import subprocess
import tempfile
import unittest

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
FIXTURE = r"""
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct {
    u8 before[16];
    DisplayObject object;
    u8 gap[16];
    s32 rerun;
    u8 after[16];
} GuardedState;
extern GuardedState state;
#define D_8009B424 state.rerun
"""
WITNESS = r"""
GuardedState state, expected;
static u32 events, error, wanted, original_selector;
static s32 old_rerun;
void DisplayObject_SetResourceVariant(DisplayObjectConfig *object, s32 value)
{
    if ((void *)object != (void *)&state.object || events != 0 ||
        value != wanted || object->field_69 != original_selector ||
        state.rerun != old_rerun) error = 1;
    object->field_69 = value;
    events = events * 10 + 1;
}
void DisplayObject_UpdateCommandStream(DisplayObject *object)
{
    if (object != &state.object || events != 1 || object->field_69 != wanted ||
        state.rerun != old_rerun) error = 2;
    events = events * 10 + 2;
}
int main(void)
{
    static const s32 arguments[4] = {(-2147483647 - 1), -1, 0, 2147483647};
    u32 a, variant, selector, i, changed, cases = 0;
    for (a = 0; a < 4; a++)
        for (variant = 0; variant < 256; variant++)
            for (selector = 0; selector < 256; selector++) {
                for (i = 0; i < sizeof(state); i++)
                    ((u8 *)&state)[i] = i * 37 + variant + selector;
                state.object.field_6A = variant;
                state.object.field_69 = selector;
                state.rerun = old_rerun = (s32)(0x80000000U ^ (variant << 16) ^ (selector * 257));
                wanted = a < 2 ? 2 : variant;
                original_selector = selector;
                changed = wanted != selector;
                for (i = 0; i < sizeof(state); i++)
                    ((u8 *)&expected)[i] = ((u8 *)&state)[i];
                if (changed) {
                    ((u8 *)&expected.object)[0x69] = wanted;
                    expected.rerun = 1;
                }
                events = error = 0;
                CardPreview_UpdateVariant(&state.object, arguments[a]);
                if (error || events != (changed ? 12 : 0)) return 1;
                for (i = 0; i < sizeof(state); i++)
                    if (((u8 *)&state)[i] != ((u8 *)&expected)[i]) return 1;
                cases++;
            }
    return cases == 262144 ? 0 : 2;
}
"""


class PreviewVariantObjectTests(unittest.TestCase):
    def test_facing_area_comes_from_reference_triangle_mac0_not_depth(self):
        projection = (ROOT / "src/game/display_object_projection_checks.c").read_text()
        facing = projection[projection.index("s32 DisplayObject_ProjectRotationFacing("):]
        self.assertIn("v308[1].vx = 0x200;", facing)
        self.assertIn("v308[2].vy = 0x200;", facing)
        self.assertLess(facing.index("RotAverageNclip3_nom("), facing.index("gte_stopz(otzp);"))
        callback = "((void (*)(DisplayObject *, s32))cb)(obj, otz);"
        self.assertLess(facing.index("gte_stopz(otzp);"), facing.index(callback))
        self.assertLess(facing.index(callback), facing.index("if (otz >= 0)"))
        for name in ("inline_c.h", "inline_o.h"):
            header = (ROOT / "src/psyq" / name).read_text()
            for macro, register in (("gte_stopz", 24), ("gte_stotz", 7)):
                start = header.index("#define " + macro)
                end = header.index("\n#define ", start + 1)
                self.assertRegex(header[start:end], rf"swc2\s+\${register}\b")

    def test_canonical_producer_facing_call_and_unchanged_narrow_helper(self):
        source = (ROOT / "src/game/card_preview_update_variant.c").read_text()
        self.assertIn("CardPreview_UpdateVariant(DisplayObject *source, s32 facing_area)", source)
        self.assertIn("requested_variant = source->field_6A;", source)
        self.assertIn("DisplayObjectConfig *object = (DisplayObjectConfig *)source;", source)
        self.assertIn("#define DISPLAY_OBJECT_UPDATE_COMMAND_STREAM_AMBIENT_ARGS", source)
        self.assertIn("DisplayObject_UpdateCommandStream((DisplayObject *)object);", source)
        header = (ROOT / "src/game/card_preview_callbacks.h").read_text()
        self.assertNotIn("CardVariantSource", header)
        self.assertIn("CardPreview_UpdateVariant(DisplayObject *source, s32 facing_area)", header)
        facing = (ROOT / "src/game/display_object_projection_checks.c").read_text()
        self.assertIn("((void (*)(DisplayObject *, s32))cb)(obj, otz);", facing)
        producer = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        self.assertIn("object->field_10 = (u32)CardPreview_UpdateVariant;", producer)

    @unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
    def test_exact_callback_type_and_canonical_byte_layout(self):
        with tempfile.TemporaryDirectory(prefix="variant-object-contract-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for parameters, accepted in (("DisplayObject *, s32", True), ("u8 *, s32", False)):
                with self.subTest(parameters=parameters):
                    probe = directory / "probe.c"
                    probe.write_text(
                        '#include "../../src/game/card_preview_callbacks.h"\n'
                        f"void (*checked)({parameters}) = CardPreview_UpdateVariant;\n"
                        "typedef char extent[sizeof(DisplayObject) == 0x70 ? 1 : -1];\n"
                        "typedef char byte_offset[(u32)&((DisplayObject *)0)->field_6A == 0x6A ? 1 : -1];\n"
                        "typedef char byte_width[sizeof(((DisplayObject *)0)->field_6A) == 1 ? 1 : -1];\n"
                        "typedef char byte_unsigned[((__typeof__(((DisplayObject *)0)->field_6A))-1) > 0 ? 1 : -1];\n"
                    )
                    result = subprocess.run(
                        [str(COMPILER), "-S", "-O2", "-G0", "-Werror", "-o",
                         str(directory / "probe.s"), str(probe)],
                        cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)),
                        text=True, capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn("incompatible pointer type", result.stderr)

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_complete_callback_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/card_preview_update_variant.c").read_text()
        start = source.index("void CardPreview_UpdateVariant(")
        prefix, original = source[:start], source[start:]
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wrong-fallback"),
            ("-O2", "wrong-source-byte"), ("-O2", "zero-is-negative"),
            ("-O2", "early-rerun"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = original
                if mutation == "wrong-fallback":
                    code = code.replace("int requested_variant = 2;", "int requested_variant = 3;")
                elif mutation == "wrong-source-byte":
                    code = code.replace("source->field_6A", "source->field_68")
                elif mutation == "zero-is-negative":
                    code = code.replace("facing_area >= 0", "facing_area > 0")
                elif mutation == "early-rerun":
                    code = code.replace("        D_8009B424 = 1;\n", "")
                    code = code.replace(
                        "DisplayObject_UpdateCommandStream((DisplayObject *)object);",
                        "D_8009B424 = 1;\n        DisplayObject_UpdateCommandStream((DisplayObject *)object);",
                    )
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, original)
                with tempfile.TemporaryDirectory(prefix="preview-variant-object-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(prefix + FIXTURE + code)
                    witness = directory / "witness.c"
                    witness.write_text(prefix + FIXTURE + WITNESS)
                    start_file = directory / "start.S"
                    start_file.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT / "src/game"),
                         str(operations), str(witness), str(start_file), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
