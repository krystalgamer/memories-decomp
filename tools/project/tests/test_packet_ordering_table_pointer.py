"""Check real SDK-call/table-depth expressions without executing the dispatcher."""

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
#include "src/game/display_object_packet_submit.h"
typedef char ilp32[sizeof(void *) == 4 && sizeof(GsOT) == 0x14 ? 1 : -1];
typedef struct {
    u8 before[16];
    GsOT tables[4];
    u8 gap[16];
    SpritePrim sprite;
    u8 after[16];
} GuardedTables;
extern GuardedTables guarded;
"""
WITNESS = r"""
GuardedTables guarded, expected;
static u32 depth_words[65544];
static GsOT *selected;
static s32 input_mode;
static u32 calls, error;
void fast(SpritePrim *sprite, GsOT *ot, s32 mode);
void flip(SpritePrim *sprite, GsOT *ot, s32 mode);
void normal(SpritePrim *sprite, GsOT *ot, s32 mode);
u32 *resolve_depth(GsOT *ot, s32 mode);
static s32 same(void)
{
    u32 i;
    for (i = 0; i < sizeof(guarded); i++)
        if (((u8 *)&guarded)[i] != ((u8 *)&expected)[i]) return 0;
    return 1;
}
static void observe(u32 kind, GsSPRITE *sprite, GsOT *ot, u16 depth)
{
    if (kind != calls || (void *)sprite != (void *)&guarded.sprite ||
        ot != selected || depth != ((u32)input_mode & 65535) || !same())
        error = 1;
    calls++;
}
void GsSortFastSprite(GsSPRITE *sprite, GsOT *ot, u16 depth) { observe(0, sprite, ot, depth); }
void GsSortFlipSprite(GsSPRITE *sprite, GsOT *ot, u16 depth) { observe(1, sprite, ot, depth); }
void GsSortSprite(GsSPRITE *sprite, GsOT *ot, u16 depth) { observe(2, sprite, ot, depth); }
int main(void)
{
    static const u32 high[] = {0, 0x10000, 0xFFFF0000, 0x80000000};
    u32 slot, h, low, i, cases = 0;
    s32 depth;
    for (i = 0; i < 65544; i++) depth_words[i] = i * 4099U ^ 0xA55AA55A;
    for (slot = 0; slot < 4; slot++)
        for (h = 0; h < 4; h++)
            for (low = 0; low < 65536; low++) {
                for (i = 0; i < sizeof(guarded); i++) ((u8 *)&guarded)[i] = i * 37 + low;
                for (i = 0; i < 4; i++) {
                    guarded.tables[i].org = (GsOT_TAG *)&depth_words[32768 + i];
                    guarded.tables[i].tag = (GsOT_TAG *)&depth_words[32769 + i];
                }
                selected = &guarded.tables[slot];
                input_mode = (s32)(high[h] | low);
                depth = low < 32768 ? (s32)low : (s32)low - 65536;
                for (i = 0; i < sizeof(guarded); i++) ((u8 *)&expected)[i] = ((u8 *)&guarded)[i];
                calls = error = 0;
                fast(&guarded.sprite, selected, input_mode);
                flip(&guarded.sprite, selected, input_mode);
                normal(&guarded.sprite, selected, input_mode);
                if (error || calls != 3 || !same()) return 1;
                if (resolve_depth(selected, input_mode) != &depth_words[32768 + slot + depth] ||
                    !same()) return 1;
                cases++;
            }
    for (i = 0; i < 65544; i++)
        if (depth_words[i] != (i * 4099U ^ 0xA55AA55A)) return 1;
    return cases == 1048576 ? 0 : 2;
}
"""


class PacketOrderingTablePointerTests(unittest.TestCase):

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_all_views_accept_descriptor_and_reject_legacy_integer_carrier(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="packet-ot-type-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for selector, symbol, first, packet, extra in (
                ("", "DisplayObject_SubmitPacket", "SpritePrim *", "u8 *", "u8 *"),
                ("DISPLAY_OBJECT_PACKET_SUBMIT_ATTRIBUTE_VIEW", "DisplayObject_SubmitAttributePacket", "s32", "u8 *", "u8 *"),
                ("DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST", "DisplayObject_SubmitPacket", "SpritePrim *", "POLY_FT4 *", "DisplayObjectPacketOrigin *"),
            ):
                for ot, accepted in (("GsOT *", True), ("s32", False), ("GsOT_TAG *", False)):
                    with self.subTest(selector=selector, ot=ot):
                        source = directory / "probe.c"
                        source.write_text(
                            (f"#define {selector}\n" if selector else "")
                            + '#include "../../src/game/display_object_packet_submit.h"\n'
                            "typedef char descriptor_size[sizeof(GsOT) == 0x14 ? 1 : -1];\n"
                            "typedef char org_offset[(u32)&(((GsOT *)0)->org) == 4 ? 1 : -1];\n"
                            f"void (*checked)({first}, {packet}, {ot}, s32, {extra}) = {symbol};\n"
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
    def test_actual_sdk_boundaries_and_org_depth_expression_with_negatives(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/display_object_projection_checks.c").read_text()
        macros = []
        for name in ("GS_SPRITE_VIEW", "GS_OT_VIEW"):
            match = re.search(rf"(?m)^#define {name}[^\n]*", source)
            self.assertIsNotNone(match)
            macros.append(match.group())
        statements = []
        for name in ("GsSortFastSprite", "GsSortFlipSprite", "GsSortSprite"):
            statement = f"{name}(GS_SPRITE_VIEW(sprite), GS_OT_VIEW(ot), pri);"
            self.assertEqual(source.count(statement), 1)
            statements.append(statement)
        expression = "(u32 *)GS_OT_VIEW(ot)->org + pri"
        self.assertEqual(source.count(expression), 1)
        priority = "s32 pri = (s16)mode;"
        self.assertIn(priority, source)
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wrong-descriptor"),
            ("-O2", "unsigned-depth"), ("-O2", "wrong-table-field"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                calls = list(statements)
                offset, cast = expression, priority
                if mutation == "wrong-descriptor":
                    calls[0] = calls[0].replace("GS_OT_VIEW(ot)", "GS_OT_VIEW(ot + 1)")
                elif mutation == "unsigned-depth":
                    cast = cast.replace("(s16)mode", "(u16)mode")
                elif mutation == "wrong-table-field":
                    offset = offset.replace("->org", "->tag")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual((calls, offset, cast), (statements, expression, priority))
                code = HEADERS + "\n" + "\n".join(macros) + "\n"
                for name, statement in zip(("fast", "flip", "normal"), calls):
                    code += f"void {name}(SpritePrim *sprite, GsOT *ot, s32 mode) {{ {priority} {statement} }}\n"
                code += f"u32 *resolve_depth(GsOT *ot, s32 mode) {{ {cast} return {offset}; }}\n"
                with tempfile.TemporaryDirectory(prefix="packet-ot-boundary-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(code)
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
