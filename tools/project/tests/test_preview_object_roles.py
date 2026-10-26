"""Exercise actual pointer lifecycle helpers and constructor link tail, not pool code."""

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
HEADERS = r"""
#include "src/types.h"
#include "src/game/display_object.h"
#include "src/game/display_object_core.h"
#include "src/game/duel_effect_resource_record.h"
#include "src/game/card_preview_objects.h"
#include "src/game/card_preview_callbacks.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct {
    u8 before[16];
    DuelEffectResourceRecord records[4];
    u8 gap[16];
    DisplayObject objects[2];
    u8 after[16];
} GuardedState;
extern GuardedState state;
#define gCardPreview_Resources state.records
"""
WITNESS = r"""
GuardedState state, expected;
static u32 slot, calls, error, cases;
static DisplayObject *primary, *secondary;
DisplayObject *link_tail(s32 index, DisplayObject *object);
void CardPreview_UpdateVariant(DisplayObject *source, s32 arg)
{
    error = 1;
}
void DisplayObject_ReleaseIfPresent(void *object)
{
    if (calls > 1 || object != (calls == 0 ? primary : secondary) ||
        state.records[slot].primary_object != primary ||
        state.records[slot].secondary_object != secondary) error = 2;
    calls++;
}
static void fill(u32 seed)
{
    u32 i;
    for (i = 0; i < sizeof(state); i++) ((u8 *)&state)[i] = i * 37 + seed;
}
static void snapshot(void)
{
    u32 i;
    for (i = 0; i < sizeof(state); i++) ((u8 *)&expected)[i] = ((u8 *)&state)[i];
}
static void put32(void *p, u32 v)
{
    ((u8 *)p)[0] = v; ((u8 *)p)[1] = v >> 8;
    ((u8 *)p)[2] = v >> 16; ((u8 *)p)[3] = v >> 24;
}
static int same(void)
{
    u32 i;
    for (i = 0; i < sizeof(state); i++)
        if (((u8 *)&state)[i] != ((u8 *)&expected)[i]) return 0;
    return 1;
}
int main(void)
{
    u32 operation, a, b, which;
    DisplayObject *values[3];
    values[0] = 0; values[1] = &state.objects[0]; values[2] = &state.objects[1];
    for (operation = 0; operation < 2; operation++)
        for (slot = 0; slot < 4; slot++)
            for (a = 0; a < 3; a++)
                for (b = 0; b < 3; b++) {
                    fill(a * 13 + b);
                    state.records[slot].primary_object = primary = values[a];
                    state.records[slot].secondary_object = secondary = values[b];
                    snapshot();
                    put32((u8 *)&expected.records[slot], 0);
                    put32((u8 *)&expected.records[slot] + 4, 0);
                    calls = error = 0;
                    if (operation == 0) DuelEffect_ReleaseResourceObjects(slot);
                    else DuelEffect_ClearResourceObjectPointers(slot);
                    if (error || calls != (operation == 0 ? 2 : 0) || !same()) return 1;
                    cases++;
                }
    for (slot = 0; slot < 4; slot++)
        for (which = 0; which < 2; which++) {
            fill(slot + which);
            primary = &state.objects[which];
            secondary = &state.objects[which ^ 1];
            state.records[slot].secondary_object = secondary;
            snapshot();
            put32((u8 *)&expected.records[slot], (u32)primary);
            expected.objects[which].field_65 = 3;
            expected.objects[which ^ 1].field_65 = 3;
            put32((u8 *)&expected.objects[which] + 0x10, (u32)CardPreview_UpdateVariant);
            put32((u8 *)&expected.objects[which ^ 1] + 0x54, (u32)primary);
            calls = error = 0;
            if (link_tail(slot, primary) != primary || calls || error || !same()) return 1;
            cases++;
        }
    return cases == 80 ? 0 : 2;
}
"""


class PreviewObjectRoleTests(unittest.TestCase):

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_helpers_and_link_tail_with_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        unit = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        release = unit[
            unit.index("void DuelEffect_ReleaseResourceObjects("):
            unit.index("void DuelEffect_ClearResourceObjectPointers(")
        ]
        clear = unit[unit.index("void DuelEffect_ClearResourceObjectPointers("):]
        source = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        match = re.search(
            r"(?s)\(\(DisplayObject \*\)entry->secondary_object\)->field_65 = 3;.*?return object;",
            source,
        )
        self.assertIsNotNone(match)
        original_tail = match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "reverse-release"),
            ("-O2", "clear-before-release"), ("-O2", "wrong-link"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                body, tail = release, original_tail
                if mutation == "reverse-release":
                    body = body.replace("entry->primary_object", "entry->temporary_member", 1)
                    body = body.replace("entry->secondary_object", "entry->primary_object", 1)
                    body = body.replace("entry->temporary_member", "entry->secondary_object", 1)
                elif mutation == "clear-before-release":
                    body = body.replace(
                        "DisplayObject_ReleaseIfPresent(entry->primary_object);",
                        "entry->primary_object = 0;\nDisplayObject_ReleaseIfPresent(entry->primary_object);",
                    )
                elif mutation == "wrong-link":
                    tail = tail.replace("secondary->field_54 = primary;", "secondary->field_54 = secondary;")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual((body, tail), (release, original_tail))
                with tempfile.TemporaryDirectory(prefix="preview-object-roles-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        HEADERS + body + "\n" + clear + "\n"
                        "DisplayObject *link_tail(s32 index, DisplayObject *object) {\n"
                        "DuelEffectResourceRecord *entry = &state.records[index];\n"
                        + tail + "\n}\n"
                    )
                    witness = directory / "witness.c"
                    witness.write_text(HEADERS + WITNESS)
                    start = directory / "start.S"
                    start.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT),
                         str(operations), str(witness), str(start), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
