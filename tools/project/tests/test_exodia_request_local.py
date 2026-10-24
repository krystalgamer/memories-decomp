"""Exercise the three actual request fragments, not the Exodia state machine."""

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
FIXTURE = r"""
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef char counter_byte[sizeof(gDuel_bPresentationCursor) == 1 ? 1 : -1];
typedef struct {
    u8 before[16];
    DuelEffectRequest request;
    u8 gap[16];
    DuelEffectRequest *G32 scene;
    u8 counter;
    u8 after[16];
} GuardedRequest;
typedef struct {
    u8 before[16];
    DisplayObject objects[DISPLAY_OBJECT_WORK_SLOT_COUNT];
    u8 gap[16];
    DisplayObject *G32 slots[DISPLAY_OBJECT_WORK_SLOT_COUNT];
    u8 after[16];
} GuardedDisplays;
extern GuardedRequest state;
extern GuardedDisplays displays;
#define gDuelEffect_pTrackedRequest state.scene
#define gDuel_bPresentationCursor state.counter
#define gDisplayObject_apWorkSlots displays.slots
"""
WITNESS = r"""
GuardedRequest state, expected;
GuardedDisplays displays, expected_displays;
static DuelEffectRequest previous_scene;
static const u32 addresses[8] = {
    0x80013800, 0x80100000, 0x00103800, 0x10003800,
    0xFFFFF000, 0xFFFFFFFF, 0x7FFFFFFF, 0x7FFF8000
};
static u32 phase, value, initial_counter, word, random_value, interval, events, error;
static u32 cases;
void sparkle(void);
void centre(void);
void card_spark(void);
static void put16(void *p, u32 v) { ((u8 *)p)[0] = v; ((u8 *)p)[1] = v >> 8; }
static void put32(void *p, u32 v)
{
    ((u8 *)p)[0] = v; ((u8 *)p)[1] = v >> 8;
    ((u8 *)p)[2] = v >> 16; ((u8 *)p)[3] = v >> 24;
}
DuelEffectRequest *DuelEffect_AllocateRequest(s32 id)
{
    if (events != 0 || id != (phase == 2 ? 0 : 0x13)) error = 1;
    events = events * 10 + 1;
    return &state.request;
}
int rand(void)
{
    if (phase != 0 || events != 1 || state.scene != &state.request ||
        state.counter != initial_counter) error = 2;
    events = events * 10 + 2;
    return random_value;
}
s32 Rand_GetInterval(s32 divisor)
{
    if (divisor != 0xB0 || events != 12 || state.scene != &state.request ||
        (u16)state.request.field_00 != (random_value & 0xFF) + 0x20 ||
        state.counter != initial_counter) error = 3;
    events = events * 10 + 3;
    return interval;
}
void SD_SEPlayFull(u32 id)
{
    if (phase != 2 || events != 1 || id != 0x17 ||
        state.scene != &previous_scene || state.counter != initial_counter ||
        (u16)state.request.field_00 != ((value + 0x1A) & 0xFFFF) ||
        (u16)state.request.field_02 != (((value ^ 0x8000) + 0x1E) & 0xFFFF) ||
        (u32)state.request.buffer != word + 0xA000 + initial_counter * 0x1000 ||
        state.request.field_1A != 9) error = 4;
    events = events * 10 + 4;
}
static int run_case(u32 which, u32 bits, u32 counter, u32 address)
{
    u32 i;
    phase = which; value = bits; initial_counter = counter; word = address;
    random_value = bits & 0x7FFF;
    interval = bits % 0xB0;
    for (i = 0; i < sizeof(state); i++) ((u8 *)&state)[i] = i * 37 + bits;
    for (i = 0; i < sizeof(displays); i++) ((u8 *)&displays)[i] = i * 19 + bits;
    for (i = 0; i < DISPLAY_OBJECT_WORK_SLOT_COUNT; i++) {
        displays.slots[i] = &displays.objects[DISPLAY_OBJECT_WORK_SLOT_COUNT - 1 - i];
        displays.objects[i].field_30.h.field_30 = bits + i * 117;
        displays.objects[i].field_30.h.field_32 = bits + i * 313;
    }
    if (which == 2) {
        displays.slots[counter]->field_30.h.field_30 = bits;
        displays.slots[counter]->field_30.h.field_32 = bits ^ 0x8000;
    }
    state.scene = &previous_scene;
    state.counter = counter;
    state.request.buffer = (void *)address;
    for (i = 0; i < sizeof(state); i++) ((u8 *)&expected)[i] = ((u8 *)&state)[i];
    for (i = 0; i < sizeof(displays); i++)
        ((u8 *)&expected_displays)[i] = ((u8 *)&displays)[i];
    if (which == 0) {
        expected.scene = &state.request;
        put16(&expected.request.field_00, (random_value & 0xFF) + 0x20);
        put16(&expected.request.field_02, interval + 0x20);
        put32(&expected.request.buffer, address + (counter & 3) * 0x2000);
        expected.counter = counter + 1;
    } else if (which == 1) {
        expected.scene = &state.request;
        put16(&expected.request.field_00, 0xA0);
        put16(&expected.request.field_02, 0x78);
        put32(&expected.request.buffer, address + 0x8000);
    } else {
        put16(&expected.request.field_00, bits + 0x1A);
        put16(&expected.request.field_02, (bits ^ 0x8000) + 0x1E);
        put32(&expected.request.buffer, address + 0xA000 + counter * 0x1000);
        expected.request.field_1A = 9;
        expected.counter = counter + 1;
    }
    events = error = 0;
    if (which == 0) sparkle();
    else if (which == 1) centre();
    else card_spark();
    if (error || events != (which == 0 ? 123 : which == 1 ? 1 : 14)) return 1;
    for (i = 0; i < sizeof(state); i++)
        if (((u8 *)&state)[i] != ((u8 *)&expected)[i]) return 1;
    for (i = 0; i < sizeof(displays); i++)
        if (((u8 *)&displays)[i] != ((u8 *)&expected_displays)[i]) return 1;
    cases++;
    return 0;
}
int main(void)
{
    u32 i, value, slot;
    for (i = 0; i < 4; i++)
        for (value = 0; value < 65536; value++)
            if (run_case(0, value, value >> 8, addresses[i])) return 1;
    for (i = 0; i < 4; i++)
        for (value = 0; value < 256; value++)
            if (run_case(1, value, value, addresses[i])) return 1;
    for (slot = 0; slot < DISPLAY_OBJECT_WORK_SLOT_COUNT; slot++)
        for (value = 0; value < 65536; value++)
            if (run_case(2, value, slot, addresses[value & 7])) return 1;
    return cases == 590848 ? 0 : 2;
}
"""


class ExodiaRequestLocalTests(unittest.TestCase):

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_three_fragments_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_draw_resolution.c").read_text()
        # The unit also holds the drawn-card slide, the draw-resolution
        # functions and the card readback; keep the shared preamble and the
        # Exodia result's own declarations. graphics_frame.h serves only the
        # readback and does not compile on the host.
        prefix = (
            source[:source.index("void Duel_UpdateDrawCardSlide(DisplayObject *o)")]
            .replace('#include "graphics_frame.h"\n', "")
            + source[source.index("extern s32 D_800E9F04[];"):
                     source.index("void DuelScene_UpdateExodiaResult(void)")]
        )
        matches = list(re.finditer(
            r"(?s)fx = DuelEffect_AllocateRequest\((?:0x13|0)\);"
            r".*?return;", source,
        ))
        self.assertEqual(len(matches), 3)
        originals = [match.group() for match in matches]
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "scaled-burst-word"),
            ("-O2", "missing-position-offset"), ("-O2", "missing-publication"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                fragments = originals.copy()
                if mutation == "scaled-burst-word":
                    fragments[2] = fragments[2].replace("(k << 12)", "(k << 14)")
                elif mutation == "missing-position-offset":
                    fragments[2] = fragments[2].replace(
                        "d->field_30.h.field_30 + 0x1A", "d->field_30.h.field_30"
                    )
                elif mutation == "missing-publication":
                    fragments[0] = fragments[0].replace("gDuelEffect_pTrackedRequest = fx;", "(void)fx;")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(fragments, originals)
                declarations = (
                    "DuelEffectRequest *fx; s32 r, n, t14;\n",
                    "DuelEffectRequest *fx;\n",
                    "DuelEffectRequest *fx; s32 k, anim; u32 sum;\n"
                    "DisplayObject *G32 *slot; DisplayObject *d;\n",
                )
                with tempfile.TemporaryDirectory(prefix="exodia-request-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        prefix + FIXTURE + "\n" + "\n".join(
                            "void " + function + "(void) {\n" + declarations[index]
                            + fragments[index] + "\n}\n"
                            for index, function in enumerate(("sparkle", "centre", "card_spark"))
                        )
                    )
                    witness = directory / "witness.c"
                    witness.write_text(prefix + FIXTURE + WITNESS)
                    start = directory / "start.S"
                    start.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT / "src/game"),
                         str(operations), str(witness), str(start), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
