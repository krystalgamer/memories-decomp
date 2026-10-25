"""Check field-object ownership and isolated consumer stores, not 3D rendering."""

import csv
import json
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
NAME = "Duel_CreateFieldCardObject"
REGIONS = (
    ("slus_01411", 0x80024734, ""),
    ("slpm_86398", 0x80024560, "japanese/"),
    ("sles_03947", 0x800246AC, "european/"),
) + tuple((region, 0x800247F0, "european/") for region in (
    "sles_03948", "sles_03949", "sles_03950", "sles_03951",
))
SIZES = (0xF0, 0xF0, 0x40, 0x28, 0x64, 0x23C, 0x118, 0x94)
HEADERS = r"""
#include "src/types.h"
#include "src/psyq/libgte.h"
#include "src/psyq/libgpu.h"
#include "src/game/duel_card_record_lifecycle.h"
#include "src/game/display_object.h"
#include "src/game/display_object_core.h"
#include "src/game/display_object_layout.h"
#include "src/game/display_object_set_resource_variant_from_sign.h"
#include "src/game/card_constants.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef union { DisplayObject full; DuelCardDisplayObject card; } ObjectViews;
typedef struct { u8 before[16]; ObjectViews views; u8 after[16]; } GuardedObject;
typedef struct {
    u8 before[16];
    s32 stats[32];
    u8 gap_0[16];
    GuardedObject objects[2];
    u8 gap_1[16];
    MATRIX matrix;
    u8 gap_2[16];
    POLY_GT4 prim;
    u8 after[16];
} FieldBacking;
extern FieldBacking backing;
#define gDuel_adwCardStats backing.stats
"""
WITNESS = r"""
FieldBacking backing, expected;
static u32 which, low, phase, error;
void observe_front(DisplayObject *obj, MATRIX *lm, POLY_GT4 *prim);
static void write16(u8 *p, u32 offset, u32 value)
{ p[offset] = value; p[offset + 1] = value >> 8; }
static u32 read32(u8 *p, u32 offset)
{ return p[offset] | p[offset + 1] << 8 | p[offset + 2] << 16 | (u32)p[offset + 3] << 24; }
static void write32(u8 *p, u32 offset, u32 value)
{ write16(p, offset, value); write16(p, offset + 2, value >> 16); }
static s32 sign16(u32 value)
{ value &= 65535; return value < 32768 ? (s32)value : (s32)value - 65536; }
static s32 same(void)
{
    u32 i;
    for (i = 0; i < sizeof(backing); i++)
        if (((u8 *)&backing)[i] != ((u8 *)&expected)[i]) return 0;
    return 1;
}
s32 DisplayObject_FindFreeGeneralSlot(void)
{
    if (phase++ != 0 || !same()) error = 1;
    return 19;
}
void *DisplayObject_AcquireSlot(s32 index, s32 key)
{
    if (phase++ != 1 || index != 19 || key != 0 || !same()) error = 1;
    return &backing.objects[which].views.full;
}
void DisplayObject_SetResourceVariantFromSign(DisplayObjectConfig *object, u32 value)
{ (void)object; (void)value; error = 1; }
int main(void)
{
    u32 i, type, variant, z, cases = 0;
    u8 *object;
    DuelCardDisplayObject *result;
    for (which = 0; which < 2; which++)
        for (low = 0; low < 65536; low++) {
            for (i = 0; i < sizeof(backing); i++) ((u8 *)&backing)[i] = i * 37 + low + which;
            for (i = 0; i < 32; i++)
                backing.stats[i] = i << 26 | 0x123456 | (low & 32 ? 0x80000000U : 0);
            type = low & 31;
            z = low ^ 0x8000;
            for (i = 0; i < sizeof(backing); i++) ((u8 *)&expected)[i] = ((u8 *)&backing)[i];
            phase = error = 0;
            result = FACTORY(type + 1, (s32)(0x12340000U | low), (s32)(0x80000000U | z));
            if (result != &backing.objects[which].views.card || phase != 2 || error) return 1;
            object = (u8 *)&expected.objects[which].views.full;
            variant = type == 20 || type == 23 ? 1 : type == 21 ? 2 : type == 22 ? 3 : 0;
            write16(object, 0x30, low);
            write16(object, 0x32, 0xFFE8);
            write16(object, 0x34, z);
            object[0x67] = object[0x69] = 0;
            write32(object, 4, read32(object, 4) | 0x01000000U);
            write32(object, 0x10, (u32)DisplayObject_SetResourceVariantFromSign);
            object[0x5C] = type < 20 ? 0 : 0x38;
            object[0x5D] = 0xC0;
            object[0x68] = type;
            write16(object, 0x42, variant);
            if (!same()) return 1;
            observe_front(&backing.objects[which].views.full, &backing.matrix, &backing.prim);
            write32((u8 *)&expected.matrix, 0x14, (u32)sign16(low));
            write32((u8 *)&expected.matrix, 0x18, 0xFFFFFFE8U);
            write32((u8 *)&expected.matrix, 0x1C, (u32)sign16(z));
            ((u8 *)&expected.prim)[0x0C] = ((u8 *)&expected.prim)[0x24] = type < 20 ? 0 : 0x38;
            ((u8 *)&expected.prim)[0x0D] = ((u8 *)&expected.prim)[0x19] = 0xC0;
            write16((u8 *)&expected.prim, 0x0E, ((0xF1 + variant) << 6) | 0x10);
            if (!same() || error || phase != 2) return 1;
            cases++;
        }
    return cases == 131072 ? 0 : 2;
}
"""


class FieldCardObjectNameTests(unittest.TestCase):
    def test_all_regions_keep_eight_function_owner_order_profiles_and_extent(self):
        for region, start, tier in REGIONS:
            with self.subTest(region=region):
                directory = ROOT / "config" / region
                address = start + sum(SIZES[:6])
                with (directory / "functions.csv").open(newline="") as handle:
                    entry, = [row for row in csv.DictReader(handle)
                              if int(row["address"], 0) == address]
                self.assertEqual(
                    (entry["name"], entry["status"], entry["module"], int(entry["size"], 0)),
                    (NAME, "matching_c", "game", 0x118),
                )
                owner = "src/game/" + tier + "duel_card_record_lifecycle.c"
                rows = json.loads((directory / "matching_c.json").read_text())["functions"]
                group = [row for row in rows if row["source"] == owner]
                self.assertEqual(
                    [(int(row["address"], 0), int(row["size"], 0)) for row in group],
                    [(start + sum(SIZES[:i]), size) for i, size in enumerate(SIZES)],
                )
                self.assertTrue(all(row["profile"] == "gcc_2_8_1_g8_split" for row in group))
                self.assertIsNotNone(
                    re.search(rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func",
                              (directory / "symbols.txt").read_text()), region,
                )
        japanese = (ROOT / "src/game/japanese/duel_card_record_lifecycle.c").read_text()
        self.assertNotIn("func_80024C1C", japanese)
        self.assertNotIn("func_80024A48", japanese)
        self.assertIn("#define VERSION_JAPAN\n", japanese)
        self.assertIn(
            'extern DuelCardRecord gDuel_aCardRecords[] asm("gJapanese_DuelCardRecords");',
            (ROOT / "src/game/duel_card.h").read_text(),
        )
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entry, = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("function", "0x80024C1C", "high"))

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_actual_owner_rejects_old_names_and_changed_return_or_position_widths(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="field-card-name-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for owner, declaration, accepted, diagnostic in (
                (True, f"DuelCardDisplayObject *(*checked)(s32, s32, s32) = {NAME};", True, ""),
                (True, f"u8 *(*checked)(s32, s32, s32) = {NAME};", False, "incompatible pointer type"),
                (True, f"DuelCardDisplayObject *(*checked)(s32, s16, s16) = {NAME};",
                 False, "incompatible pointer type"),
                (True, "__typeof__(&func_80024C1C) checked = func_80024C1C;", False, "undeclared"),
                (True, "__typeof__(&func_80024A48) checked = func_80024A48;", False, "undeclared"),
                (False, f"__typeof__(&{NAME}) checked = {NAME};", False, "undeclared"),
            ):
                with self.subTest(owner=owner, declaration=declaration):
                    source = directory / "probe.c"
                    header = "duel_card_record_lifecycle.h" if owner else "duel_card_display_state.h"
                    source.write_text(f'#include "../../src/game/{header}"\n' + declaration + "\n")
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
                        self.assertIn(diagnostic, result.stderr)

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires Linux x86 compiler and ILP32 execution",
    )
    def test_actual_factory_and_isolated_world_uv_clut_consumer_stores(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_card_record_lifecycle.c").read_text()
        match = re.search(r"DuelCardDisplayObject \*" + NAME + r"\([^)]*\)\s*\{.*?\n\}", source, re.S)
        self.assertIsNotNone(match)
        factory = match.group()
        self.assertIn(f"obj = {NAME}(", source)
        self.assertIn("slot->object = obj;", source)
        renderer = (ROOT / "src/game/duel_field_card_render.c").read_text()
        start = renderer.index("    lm->t[0] =")
        translation = renderer[start:renderer.index("    a += 0xFF;", start)]
        start = renderer.index("    prim->u0 = prim->u2 =")
        uv = renderer[start:renderer.index("    if (NormalClip(", start)]
        clut, = [line for line in renderer.splitlines() if line.strip().startswith("prim->clut =")]
        constants = renderer[renderer.index("#ifndef DUEL_FIELD_CARD_CLUT_Y"):
                             renderer.index("#define CVECTOR_VIEW")]
        consumer = translation + uv + clut + "\n"
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "world-height"),
            ("-O2", "world-z"), ("-O2", "palette"), ("-O2", "unsigned-z"),
            ("-O2", "wrong-return"), ("-O2", "guard-store"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                candidate, stores = factory, consumer
                if mutation == "world-height":
                    candidate = candidate.replace("obj->out_y = -0x18;", "obj->out_y = 0x18;")
                elif mutation == "world-z":
                    candidate = candidate.replace("obj->field_34 = y;", "obj->field_34 = x;")
                elif mutation == "palette":
                    candidate = candidate.replace("obj->icon_variant = val;", "obj->icon_variant = val + 1;")
                elif mutation == "unsigned-z":
                    stores = stores.replace("d = obj->field_34.h.field_34;", "d = (u16)obj->field_34.h.field_34;")
                elif mutation == "wrong-return":
                    candidate = candidate.replace("return obj;", "return obj + 1;")
                elif mutation == "guard-store":
                    candidate = candidate.replace("return obj;", "((u8 *)obj)[sizeof(DisplayObject)] ^= 0x80;\n    return obj;")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual((candidate, stores), (factory, consumer))
                with tempfile.TemporaryDirectory(prefix="field-card-native-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "factory.c"
                    operations.write_text(
                        HEADERS + constants + candidate
                        + "\nvoid observe_front(DisplayObject *obj, MATRIX *lm, POLY_GT4 *prim) {\n"
                        "    s32 a, d;\n" + stores + "}\n"
                    )
                    witness = directory / "witness.c"
                    witness.write_text(HEADERS + f"\n#define FACTORY {NAME}\n" + WITNESS)
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
