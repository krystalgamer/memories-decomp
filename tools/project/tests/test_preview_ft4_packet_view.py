"""Check only actual packet-header/colour stores, never the renderer or GPU."""

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
#include "src/psyq/libgte.h"
#include "src/psyq/libgpu.h"
#include "src/game/display_object.h"
#include "src/game/sprite_primitive.h"
typedef char ilp32[sizeof(void *) == 4 && sizeof(POLY_FT4) == 0x28 ? 1 : -1];
typedef struct {
    u8 before[16];
    POLY_FT4 packet;
    u8 gap_0[16];
    SpritePrim sprite;
    u8 gap_1[16];
    DisplayObject object;
    DisplayObject window;
    u8 after[16];
} GuardedPacket;
extern GuardedPacket guarded;
"""
WITNESS = r"""
GuardedPacket guarded, expected;
void prepare(void);
void blend(void);
void quarter(void);
void restore(void);
static s32 same(void)
{
    u32 i;
    for (i = 0; i < sizeof(guarded); i++)
        if (((u8 *)&guarded)[i] != ((u8 *)&expected)[i]) return 0;
    return 1;
}
int main(void)
{
    static const u32 tags[] = {0, 0xFFFFFF, 0x800000, 0x123456};
    u32 h, low, i, rgb, cases = 0;
    u8 *packet = (u8 *)&expected.packet;
    for (h = 0; h < 4; h++)
        for (low = 0; low < 65536; low++) {
            for (i = 0; i < sizeof(guarded); i++) ((u8 *)&guarded)[i] = i * 37 + low;
            rgb = (low & 255) | ((low >> 8) << 8) |
                  (((low & 255) ^ (low >> 8)) << 16) | (h * 0x55U << 24);
            guarded.object.rgb = rgb;
            guarded.window.rgb = rgb ^ 0xA55A3322;
            guarded.packet.tag = ((low & 255) << 24) | (tags[h] ^ low);
            for (i = 0; i < sizeof(guarded); i++) ((u8 *)&expected)[i] = ((u8 *)&guarded)[i];
            packet[3] = 9;
            for (i = 0; i < 3; i++) packet[4 + i] = ((u8 *)&guarded.window.rgb)[i];
            packet[7] = 0x2C;
            prepare();
            if (!same()) return 1;
            guarded.packet.code = expected.packet.code = (u8)low;
            packet[7] |= 2;
            blend();
            if (!same()) return 1;
            for (i = 0; i < 3; i++) {
                u32 scaled = ((u32)((u8 *)&guarded.object.rgb)[i] * 3) / 4;
                packet[4 + i] = scaled;
                ((u8 *)&expected.sprite.rgb)[i] = scaled;
            }
            quarter();
            if (!same()) return 1;
            for (i = 0; i < 3; i++) packet[4 + i] = ((u8 *)&guarded.object.rgb)[i];
            expected.sprite.rgb = guarded.window.rgb;
            expected.sprite.attribute = (expected.sprite.attribute & 0x8FFFFFFF) | 0x60000000;
            restore();
            if (!same()) return 1;
            cases++;
        }
    return cases == 262144 ? 0 : 2;
}
"""


class PreviewFt4PacketViewTests(unittest.TestCase):

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_sdk_packet_layout_and_rejected_prefix_and_byte_contracts(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="preview-ft4-type-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for packet, accepted in (("POLY_FT4", True), ("Func80028B08Ctx", False), ("u8", False)):
                with self.subTest(packet=packet):
                    source = directory / "probe.c"
                    source.write_text(
                        "#define DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST\n"
                        '#include "../../src/game/display_object_packet_submit.h"\n'
                        "#define OFFSET(member) ((u32)&(((POLY_FT4 *)0)->member))\n"
                        "typedef char size[sizeof(POLY_FT4) == 0x28 ? 1 : -1];\n"
                        "typedef char header[OFFSET(tag) == 0 && OFFSET(r0) == 4 && OFFSET(g0) == 5 && OFFSET(b0) == 6 && OFFSET(code) == 7 ? 1 : -1];\n"
                        "typedef char tail[OFFSET(x0) == 8 && OFFSET(clut) == 0xE && OFFSET(tpage) == 0x16 && OFFSET(u3) == 0x24 ? 1 : -1];\n"
                        f"void (*checked)(SpritePrim *, {packet} *, GsOT *, s32, DisplayObjectPacketOrigin *) = DisplayObject_SubmitPacket;\n"
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
    def test_actual_header_and_european_colour_fragments_with_negatives(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/card_preview_draw_detail_panel.c").read_text()
        patterns = (
            r"        \*\(u32 \*\)&packet->r0 = win->rgb;\n        setlen\(packet, 9\);\n        packet->code = 0x2C;",
            r"    packet->code \|= 2;",
            r"    \(\(u8 \*\)&PRM->rgb\)\[0\] = packet->r0 =.*?\n    \(\(u8 \*\)&PRM->rgb\)\[1\] = packet->g0 =.*?\n    \(\(u8 \*\)&PRM->rgb\)\[2\] = packet->b0 =[^;\n]*;",
            r"    packet->r0 = \(\(u8 \*\)&obj->rgb\)\[0\];\n    packet->g0 =.*?\n    packet->b0 =.*?\n    PRM->rgb = win->rgb;\n    PRM->attribute =[^;\n]*;",
        )
        fragments = []
        for pattern in patterns:
            match = re.search(pattern, source)
            self.assertIsNotNone(match, pattern)
            fragments.append(match.group())
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wrong-length"),
            ("-O2", "wrong-code-offset"), ("-O2", "wrong-colour-scale"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = list(fragments)
                if mutation == "wrong-length":
                    code[0] = code[0].replace("setlen(packet, 9)", "setlen(packet, 8)")
                elif mutation == "wrong-code-offset":
                    code[1] = code[1].replace("packet->code", "packet->r0")
                elif mutation == "wrong-colour-scale":
                    code[2] = code[2].replace("* 3u >> 2", ">> 1")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, fragments)
                operations_text = HEADERS
                for name, fragment in zip(("prepare", "blend", "quarter", "restore"), code):
                    operations_text += (
                        f"\nvoid {name}(void) {{\nPOLY_FT4 *packet = &guarded.packet;\n"
                        "SpritePrim *PRM = &guarded.sprite;\nDisplayObject *obj = &guarded.object;\n"
                        "DisplayObject *win = &guarded.window;\n" + fragment + "\n}\n"
                    )
                with tempfile.TemporaryDirectory(prefix="preview-ft4-bytes-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(operations_text)
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
