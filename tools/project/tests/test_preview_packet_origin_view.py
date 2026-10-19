"""Exercise only real projection-prefix/reset stores, never the full renderer."""

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
#include "src/game/display_object.h"
#include "src/game/display_object_projection.h"
#include "src/game/display_object_packet_submit.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef union {
    struct ProjectionOut projection;
    DisplayObjectPacketOrigin origin;
} OriginViews;
typedef struct {
    u8 before[16];
    OriginViews scratch;
    u8 gap[16];
    DisplayObject object;
    u8 after[16];
} GuardedOrigin;
extern GuardedOrigin guarded;
"""
WITNESS = r"""
GuardedOrigin guarded, expected;
void produce(s32 x, s32 y);
void reset(void);
static void put16(void *p, u32 value)
{
    ((u8 *)p)[0] = value; ((u8 *)p)[1] = value >> 8;
}
static void put32(void *p, u32 value)
{
    put16(p, value); put16((u8 *)p + 2, value >> 16);
}
static s32 same(void)
{
    u32 i;
    for (i = 0; i < sizeof(guarded); i++)
        if (((u8 *)&guarded)[i] != ((u8 *)&expected)[i]) return 0;
    return 1;
}
int main(void)
{
    static const u32 high[] = {0, 0x10000, 0xFFFF0000, 0x80000000};
    u32 h, low, i, x, y, cases = 0;
    for (h = 0; h < 4; h++)
        for (low = 0; low < 65536; low++) {
            for (i = 0; i < sizeof(guarded); i++) ((u8 *)&guarded)[i] = i * 37 + low;
            guarded.object.field_65 = low * 13 + h;
            for (i = 0; i < sizeof(guarded); i++) ((u8 *)&expected)[i] = ((u8 *)&guarded)[i];
            x = high[h] | low;
            y = high[(h + 1) & 3] | (low ^ 0x8000);
            put16((u8 *)&expected.scratch, x);
            put16((u8 *)&expected.scratch + 2, y);
            put32((u8 *)&expected.scratch + 4, guarded.object.field_65);
            produce((s32)x, (s32)y);
            if (!same()) return 1;
            guarded.scratch.origin.divisions = (s32)x;
            put32((u8 *)&expected.scratch + 4, 0);
            reset();
            if (!same()) return 1;
            cases++;
        }
    return cases == 262144 ? 0 : 2;
}
"""


class PreviewPacketOriginViewTests(unittest.TestCase):

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_layout_and_all_measured_submit_argument_views(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="preview-origin-type-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for selector, parameters, accepted in (
                ("DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST",
                 "SpritePrim *, POLY_FT4 *, GsOT *, s32, DisplayObjectPacketOrigin *", True),
                ("DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST",
                 "SpritePrim *, POLY_FT4 *, GsOT *, s32, Func80028B08Extra *", False),
                ("DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST",
                 "SpritePrim *, POLY_FT4 *, GsOT *, s32, u8 *", False),
                ("DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST",
                 "SpritePrim *, POLY_FT4 *, GsOT *, s32", False),
                ("", "SpritePrim *, u8 *, GsOT *, s32, u8 *", True),
                ("DISPLAY_OBJECT_PACKET_SUBMIT_ATTRIBUTE_VIEW", "s32, u8 *, GsOT *, s32, u8 *", True),
            ):
                with self.subTest(selector=selector, parameters=parameters):
                    source = directory / "probe.c"
                    source.write_text(
                        (f"#define {selector}\n" if selector else "")
                        + '#include "../../src/game/display_object_packet_submit.h"\n'
                        '#include "../../src/game/display_object_projection.h"\n'
                        "typedef char sizes[sizeof(DisplayObjectPacketOrigin) == 8 && sizeof(struct ProjectionOut) == 8 ? 1 : -1];\n"
                        "#define OFFSET(member) ((u32)&(((DisplayObjectPacketOrigin *)0)->member))\n"
                        "typedef char offsets[OFFSET(x) == 0 && OFFSET(y) == 2 && OFFSET(divisions) == 4 ? 1 : -1];\n"
                        "typedef char widths[sizeof(((DisplayObjectPacketOrigin *)0)->x) == 2 && sizeof(((DisplayObjectPacketOrigin *)0)->y) == 2 && sizeof(((DisplayObjectPacketOrigin *)0)->divisions) == 4 ? 1 : -1];\n"
                        "typedef char signed_fields[((__typeof__(((DisplayObjectPacketOrigin *)0)->x))-1 < 0) && ((__typeof__(((DisplayObjectPacketOrigin *)0)->y))-1 < 0) && ((__typeof__(((DisplayObjectPacketOrigin *)0)->divisions))-1 < 0) ? 1 : -1];\n"
                        "typedef char projection_offsets[(u32)&(((struct ProjectionOut *)0)->x) == 0 && (u32)&(((struct ProjectionOut *)0)->y) == 2 && (u32)&(((struct ProjectionOut *)0)->divisions) == 4 ? 1 : -1];\n"
                        f"void (*checked)({parameters}) = {'DisplayObject_SubmitAttributePacket' if selector.endswith('ATTRIBUTE_VIEW') else 'DisplayObject_SubmitPacket'};\n"
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
    def test_actual_projection_stores_and_renderer_reset_with_negatives(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        projection = (ROOT / "src/game/display_object_projection_checks.c").read_text()
        projection = projection[projection.index("s32 DisplayObject_ProjectRotationFacing("):]
        match = re.search(r"    out->x = x;\n    out->y = y;\n    out->divisions = obj->field_65;", projection)
        self.assertIsNotNone(match)
        stores = match.group()
        renderer = (ROOT / "src/game/card_preview_draw_detail_panel.c").read_text()
        match = re.search(r"    origin->divisions = 0;", renderer)
        self.assertIsNotNone(match)
        reset = match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wrong-x"),
            ("-O2", "signed-count-byte"), ("-O2", "halfword-reset"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                produce, clear = stores, reset
                if mutation == "wrong-x":
                    produce = produce.replace("out->x = x;", "out->x = y;")
                elif mutation == "signed-count-byte":
                    produce = produce.replace("obj->field_65", "(s8)obj->field_65")
                elif mutation == "halfword-reset":
                    clear = clear.replace("origin->divisions = 0;", "((u16 *)&origin->divisions)[0] = 0;")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual((produce, clear), (stores, reset))
                with tempfile.TemporaryDirectory(prefix="preview-origin-bytes-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        HEADERS + "\nvoid produce(s32 x, s32 y) {\n"
                        "struct ProjectionOut *out = &guarded.scratch.projection;\n"
                        "DisplayObject *obj = &guarded.object;\n" + produce + "\n}\n"
                        "void reset(void) {\nDisplayObjectPacketOrigin *origin = &guarded.scratch.origin;\n"
                        + clear + "\n}\n"
                    )
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
