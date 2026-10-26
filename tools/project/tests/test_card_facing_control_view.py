"""Check the scoped raw-word/callback overlay, not generic projection execution."""

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
HEADERS = r"""
#include "src/types.h"
#include "src/game/duel_card_display_state.h"
#include "src/game/display_object.h"
#include "src/game/display_object_set_resource_variant_from_sign.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef union { DisplayObject full; DuelCardDisplayObject card; } ObjectViews;
typedef struct { u8 before[16]; ObjectViews views; u8 after[16]; } GuardedObject;
typedef struct { u8 before[16]; GuardedObject objects[2]; u8 after[16]; } HookBacking;
"""
WITNESS = r"""
HookBacking backing, expected;
static void write32(u8 *p, u32 offset, u32 value)
{
    p[offset] = value; p[offset + 1] = value >> 8;
    p[offset + 2] = value >> 16; p[offset + 3] = value >> 24;
}
static s32 same(void)
{
    u32 i;
    for (i = 0; i < sizeof(backing); i++)
        if (((u8 *)&backing)[i] != ((u8 *)&expected)[i]) return 0;
    return 1;
}
int main(void)
{
    static const u32 modes[8] = {0, 1, 7, 0x7FFFFFFF, 0x80000000, 0x80016778, 0xFFFFFFFE, 0xFFFFFFFF};
    static const u32 upper[4] = {0, 0x7FFF0000, 0x80000000, 0xFFFF0000};
    u32 which, high, low, i, value, cases = 0;
    DuelCardDisplayObject *object;
    for (i = 0; i < sizeof(backing); i++) ((u8 *)&backing)[i] = i * 37 + 0x5A;
    for (which = 0; which < 2; which++) {
        object = &backing.objects[which].views.card;
        for (i = 0; i < sizeof(backing); i++) ((u8 *)&expected)[i] = ((u8 *)&backing)[i];
        for (high = 0; high < 8; high++) {
            object->field_10.word = modes[high];
            write32((u8 *)&expected.objects[which].views.full, 0x10, modes[high]);
            if (backing.objects[which].views.full.field_10 != modes[high] || !same()) return 1;
        }
        object->field_10.callback = 0;
        write32((u8 *)&expected.objects[which].views.full, 0x10, 0);
        if (object->field_10.word != 0 || !same()) return 1;
        for (high = 0; high < 4; high++)
            for (low = 0; low < 65536; low++) {
                for (i = 0; i < sizeof(backing.objects); i++)
                    ((u8 *)backing.objects)[i] = i * 37 + low + which;
                object->field_10.callback = DisplayObject_SetResourceVariantFromSign;
                if (backing.objects[which].views.full.field_10 != (u32)DisplayObject_SetResourceVariantFromSign ||
                    object->field_10.word != (u32)DisplayObject_SetResourceVariantFromSign) return 1;
                for (i = 0; i < sizeof(backing); i++) ((u8 *)&expected)[i] = ((u8 *)&backing)[i];
                value = upper[high] | low;
                ((u8 *)&expected.objects[which].views.full)[0x69] = value >= 0x80000000U;
                CALL32(void (*)(DisplayObjectConfig *, u32), object->field_10.callback)(
                    (DisplayObjectConfig *)object, value);
                if (!same()) return 1;
                cases++;
            }
    }
    return cases == 524288 ? 0 : 2;
}
"""


class CardFacingControlViewTests(unittest.TestCase):

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_real_callback_type_and_original_word_layout(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="card-facing-type-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for parameters, return_type, accepted in (
                ("DisplayObjectConfig *, u32", "void", True),
                ("DuelCardDisplayObject *, u32", "void", False),
                ("DisplayObject *, u32", "void", False),
                ("DisplayObjectConfig *, s32", "void", False),
                ("DisplayObjectConfig *, u8", "void", False),
                ("DisplayObjectConfig *", "void", False),
                ("DisplayObjectConfig *, u32", "s32", False),
            ):
                with self.subTest(parameters=parameters, return_type=return_type):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/game/duel_card_display_state.h"\n'
                        '#include "../../src/game/display_object.h"\n'
                        '#include "../../src/game/display_object_set_resource_variant_from_sign.h"\n'
                        '#define OFF(member) ((u32)&((DuelCardDisplayObject *)0)->member)\n'
                        'typedef char word_offset[OFF(field_10.word) == 0x10 ? 1 : -1];\n'
                        'typedef char hook_offset[OFF(field_10.callback) == 0x10 ? 1 : -1];\n'
                        'typedef char word_width[sizeof(((DuelCardDisplayObject *)0)->field_10.word) == 4 ? 1 : -1];\n'
                        'typedef char hook_width[sizeof(((DuelCardDisplayObject *)0)->field_10.callback) == 4 ? 1 : -1];\n'
                        'typedef char view_size[sizeof(DuelCardDisplayObject) == 0x6C ? 1 : -1];\n'
                        'typedef char output_offset[OFF(out_x) == 0x30 ? 1 : -1];\n'
                        'typedef char secondary_offset[OFF(field_4C) == 0x4C ? 1 : -1];\n'
                        'typedef char index_offset[OFF(card_index) == 0x6A ? 1 : -1];\n'
                        'void verify(DuelCardDisplayObject *object) {\n'
                        f'    {return_type} (*checked)({parameters}) = object->field_10.callback;\n'
                        '    object->field_10.callback = DisplayObject_SetResourceVariantFromSign;\n'
                        '    object->field_10.word = 1;\n'
                        '    (void)checked;\n}\n'
                    )
                    result = subprocess.run(
                        [str(GCC), "-S", "-O2", "-G8", "-Werror", "-o",
                         str(directory / "probe.s"), str(source)],
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
        and shutil.which("cc"), "requires Linux x86 compiler and ILP32 execution",
    )
    def test_actual_callback_through_stored_view_and_independent_word_roundtrips(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/display_object_set_resource_variant_from_sign.c").read_text()
        match = re.search(r"void DisplayObject_SetResourceVariantFromSign\([^)]*\)\n\{.*?\n\}", source, re.S)
        self.assertIsNotNone(match)
        callback = match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wrong-sign-bit"),
            ("-O2", "inverted-sign"), ("-O2", "wrong-selector"),
            ("-O2", "lost-flags"), ("-O2", "guard-store"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                candidate = callback
                if mutation == "wrong-sign-bit":
                    candidate = candidate.replace("value >> 31", "value >> 30")
                elif mutation == "inverted-sign":
                    candidate = candidate.replace("value >> 31", "(value >> 31) ^ 1")
                elif mutation == "wrong-selector":
                    candidate = candidate.replace("object->resource_variant", "object->resource_branch_index")
                elif mutation == "lost-flags":
                    candidate = candidate[:-1] + "    object->flags = 0;\n}"
                elif mutation == "guard-store":
                    candidate = candidate[:-1] + "    ((u8 *)object)[sizeof(DisplayObject)] ^= 0x80;\n}"
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(candidate, callback)
                with tempfile.TemporaryDirectory(prefix="card-facing-call-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "callback.c"
                    operations.write_text(HEADERS + candidate + "\n")
                    witness = directory / "witness.c"
                    witness.write_text(HEADERS + WITNESS)
                    startup = directory / "start.S"
                    startup.write_text(START)
                    binary = directory / "witness"
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT),
                         str(operations), str(witness), str(startup), "-o", str(binary)],
                        cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)),
                        text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run(
                        [str(binary)], cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)), timeout=180,
                    )
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
