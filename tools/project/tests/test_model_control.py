"""Full controller witnesses with real ILP32 layouts and opaque helper calls.

The freestanding startup avoids a host multilib-libc dependency. Only the
bootstrap is host assembly; the tested game implementation remains ordinary C.
"""

from __future__ import annotations

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/model_slot_control.c"
START = """
.text
.globl _start
_start:
    call main
    mov %eax, %ebx
    mov $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""

WITNESS = r"""
ModelSlot gModel_aSlots[MODEL_SLOT_COUNT];
u8 gModel_bActiveSlotIndex;
u8 gModel_bSceneMode;
u8 *gModel_pSlot0PrimaryModule;
u8 *gModel_pSlot1PrimaryModule;
s32 gModel_Slot0VariantModuleAddress;
s32 gModel_Slot1VariantModuleAddress;
s32 gDuel_adwCardStats[2] = {1 << 26, 5 << 26};
char gModel_szContinueTrace[] = "CONTINUE\n";
char gModel_szWhyTrace[8] = "WHY?\n";
char gModel_szBackTrace[8] = "BACK\n";
char gModel_szStoneTrace[8] = "STONE\n";
char gModel_szHitTrace[8] = "HIT\n";
char gModel_szOuchTrace[8] = "OUCH\n";
char gModel_szTermTrace[8] = "TERM\n";

typedef struct { s32 kind, a, b, c; } Event;
static Event events[64], saved_events[64];
static ModelSlot saved_slots[MODEL_SLOT_COUNT];
static u8 contexts[4];
static u32 event_count, saved_count, random_count;
static s32 fixture_error, executed, seeks;
static s32 chosen_index, initial_state, result_state, relation, defense, command_mode, mutate;
static u32 serial;

static void clear_bytes(void *p, u32 count)
{
    volatile u8 *bytes = p;
    while (count--) *bytes++ = 0;
}

static void copy_bytes(void *out, const void *in, u32 count)
{
    volatile u8 *dst = out;
    const volatile u8 *src = in;
    while (count--) *dst++ = *src++;
}

static s32 equal_bytes(const void *left, const void *right, u32 count)
{
    const volatile u8 *a = left;
    const volatile u8 *b = right;
    while (count--) if (*a++ != *b++) return 0;
    return 1;
}

static void event(s32 kind, s32 a, s32 b, s32 c)
{
    if (event_count >= 64) {
        fixture_error = 1;
        return;
    }
    events[event_count].kind = kind;
    events[event_count].a = a;
    events[event_count].b = b;
    events[event_count].c = c;
    event_count++;
}

static s32 slot_index(ModelSlot *slot)
{
    s32 i;
    for (i = 0; i < MODEL_SLOT_COUNT; i++) if (slot == &gModel_aSlots[i]) return i;
    fixture_error = 2;
    return 0;
}

static s32 module(s32 side, s32 secondary, u8 *context, s32 command)
{
    ModelSlot *slot = &gModel_aSlots[chosen_index];
    ModelSlot *other = &gModel_aSlots[chosen_index ^ 1];
    s32 context_id = -1;
    s32 i;
    for (i = 0; i < 4; i++) if (context == &contexts[i]) context_id = i;
    if (side != chosen_index || context != &contexts[chosen_index * 2 + secondary] ||
        gModel_bActiveSlotIndex != chosen_index) fixture_error = 3;
    event(10 + secondary, side, command, context_id);
    if (!secondary) {
        if (mutate && (serial & 1)) slot->animation_control_state = 6;
        return 0;
    }
    if (command != -1) return 0;
    executed = 1;
    if (mutate) {
        u16 *stats = (u16 *)&other->field_CF8;
        stats[0] ^= 0x20;
        other->field_DFF ^= 1;
    }
    return result_state;
}

static s32 primary0(u8 *p, s32 c) { return module(0, 0, p, c); }
static s32 primary1(u8 *p, s32 c) { return module(1, 0, p, c); }
static s32 secondary0(u8 *p, s32 c) { return module(0, 1, p, c); }
static s32 secondary1(u8 *p, s32 c) { return module(1, 1, p, c); }

void Model_SetHandlerCallbackState(s32 value)
{
    event(1, value, 0, 0);
    if (!value && executed) {
        if (mutate && serial % 5 == 0) gModel_aSlots[chosen_index].animation_control_state = 4;
        executed = 0;
    }
}

int FntPrint(char *text)
{
    s32 label = -1;
    if (text == gModel_szContinueTrace) label = 0;
    if (text == gModel_szBackTrace) label = 5;
    if (text == gModel_szStoneTrace) label = 3;
    if (text == gModel_szHitTrace) label = 1;
    if (text == gModel_szOuchTrace) label = 4;
    if (text == gModel_szTermTrace) label = 2;
    if (text == gModel_szWhyTrace) label = 6;
    if (label < 0) fixture_error = 4;
    event(20, label, 0, 0);
    if (mutate && label == 4) gModel_aSlots[chosen_index].animation_control_state = 1;
    return 0;
}

s32 Model_GetFrameStep(void)
{
    event(30, serial % 6, 0, 0);
    return serial % 6;
}

void Model_ControlSlotAnimation(s32 index, s32 animation, s32 flag)
{
    if (index != (chosen_index ^ 1) || flag != 1) fixture_error = 5;
    event(31, index, animation, flag);
    gModel_aSlots[index].active_animation_command_row = animation;
    gModel_aSlots[index].animation_position = 0;
    if (mutate) gModel_aSlots[index].field_DF8 ^= 1;
}

void Model_SeekSlotPartAnimation(ModelSlot *slot, s32 part, s32 animation, u32 frame)
{
    s32 index = slot_index(slot);
    if (index != (chosen_index ^ 1)) fixture_error = 6;
    event(32, part, animation, frame);
    if (mutate && seeks++ == 0) {
        slot->current_animation_row ^= 1;
        slot->animation_position++;
        if (serial & 1) slot->animation_sequence_count = 1;
    }
}

int rand(void)
{
    s32 value = ((serial + random_count++) & 3) << 8;
    event(33, value, 0, 0);
    return value;
}

void SD_SEPlay(s32 id, s32 volume, s32 pan)
{
    event(34, id, volume, pan);
    if ((id != 3 && id != 4) || volume != 255 || pan != 0) fixture_error = 7;
}

static s32 selection(ModelSlot *self, ModelSlot *other, s32 index)
{
    u16 *a = (u16 *)&self->field_CF8;
    u16 *b = (u16 *)&other->field_CF8;
    if (other->field_DFF) return a[0] > b[1] ? 6 : 8;
    if (a[0] > b[0]) return 6;
    if (a[0] < b[0]) return 5;
    return index ? 6 : 0x85;
}

static void reference(s32 index)
{
    ModelSlot *self = &gModel_aSlots[index];
    ModelSlot *other = &gModel_aSlots[index ^ 1];
    ModelControlHandler handler = index ? secondary1 : secondary0;
    s32 *commands = (s32 *)((u8 *)self + 0xD08);
    s32 decision, state, step, changed, frame, part;
    if (!self->is_active) return;
    gModel_bActiveSlotIndex = index;
    if (commands[2] >= 0) {
        Model_SetHandlerCallbackState(1);
        (index ? primary1 : primary0)(*(u8 **)((u8 *)self + 0xDE8), -1);
        Model_SetHandlerCallbackState(0);
    }
    if (self->animation_control_state == 6)
        self->animation_control_state = self->current_animation_row == self->field_DFE + 3 || self->field_DF8 == 0x309 ? 7 : 2;
    if (self->animation_control_state == 2) return;
    if (commands[self->field_DFE] < 0) {
        if (self->current_animation_row == self->field_DFE + 3) {
            if ((s32)(self->animation_position >> 4) >= (s32)(self->animation_duration_rows[self->current_animation_row].max >> 1)) {
                Model_ControlSlotAnimation(index ^ 1, selection(self, other, index) & 0x7F, 1);
                self->animation_control_state = 2;
            }
        } else if (self->animation_control_state == 8) self->animation_control_state = 2;
        if (self->animation_control_state == 7) self->animation_control_state = 8;
        return;
    }
    if (self->animation_control_state == 7) {
        Model_SetHandlerCallbackState(1);
        handler(*(u8 **)((u8 *)self + 0xDEC), commands[self->field_DFE] % 1000);
        Model_SetHandlerCallbackState(0);
        self->animation_control_state = 8;
        self->animation_sound_position = 0;
    }
    state = self->animation_control_state;
    if (self->current_animation_row != self->field_DFE + 3 && self->field_DF8 != 0x309 &&
        state != 0 && state != 1 && state != 3 && state != 4 && state != 5) return;
    decision = selection(self, other, index);
    Model_SetHandlerCallbackState(1);
    self->animation_control_state = handler(*(u8 **)((u8 *)self + 0xDEC), -1);
    Model_SetHandlerCallbackState(0);
    switch (self->animation_control_state) {
    case 0: FntPrint(gModel_szContinueTrace); return;
    case 2: FntPrint(gModel_szTermTrace); return;
    case 5:
        FntPrint(gModel_szBackTrace);
        if (other->active_animation_command_row == 5 && other->animation_position >= 0x60) Model_ControlSlotAnimation(index ^ 1, 1, 1);
        return;
    case 3:
        FntPrint(gModel_szStoneTrace);
        if (decision == 6) {
            other->stone_texture_state = 0x80;
            Model_ControlSlotAnimation(index ^ 1, 6, 1);
            return;
        }
        if (decision & 0x80) {
            other->stone_texture_state = 0xFF;
            Model_ControlSlotAnimation(index ^ 1, 5, 1);
            return;
        }
        break;
    case 1:
    case 4: break;
    default: FntPrint(gModel_szWhyTrace); self->animation_control_state = 2; return;
    }
    step = Model_GetFrameStep();
    step = self->playback_speed * (step + 1);
    decision = selection(self, other, index) & 0x7F;
    changed = 0;
    if (self->animation_control_state == 1) FntPrint(gModel_szHitTrace);
    if (self->animation_control_state == 4) {
        FntPrint(gModel_szOuchTrace);
        if (self->animation_control_state == 4 && decision == 6 && other->active_animation_command_row != 6) decision = 5;
    }
    if (other->active_animation_command_row != decision) {
        Model_ControlSlotAnimation(index ^ 1, decision, 1);
        changed = 1;
    } else if (other->playback_state != 0x23 && other->animation_position >= 0xA0 && ((rand() >> 8) & 3)) {
        other->animation_position -= step;
        frame = other->animation_position;
        for (part = 0; part < other->animation_sequence_count; part++)
            Model_SeekSlotPartAnimation(other, part, other->current_animation_row, frame);
        if ((rand() >> 8) & 1) changed = 1;
    }
    if (gModel_bSceneMode == 15 && changed && decision == 8)
        SD_SEPlay(((gDuel_adwCardStats[other->field_DF8] >> 26) & 31) == 1 ? 3 : 4, 255, 0);
}

static void setup(void)
{
    static const u16 frames[] = {0, 0x5F, 0x60, 0x9F, 0xA0, 0xB0, 0x200};
    static const u8 animations[] = {1, 5, 6, 8};
    ModelSlot *self = &gModel_aSlots[chosen_index];
    ModelSlot *other = &gModel_aSlots[chosen_index ^ 1];
    s32 *commands;
    u16 *a, *b;
    s32 i;
    clear_bytes(gModel_aSlots, sizeof(gModel_aSlots));
    clear_bytes(events, sizeof(events));
    event_count = random_count = 0;
    fixture_error = executed = seeks = 0;
    gModel_bActiveSlotIndex = 0xFF;
    gModel_bSceneMode = serial % 3 ? 15 : 0;
    gModel_pSlot0PrimaryModule = (u8 *)((u32)primary0 - 4);
    gModel_pSlot1PrimaryModule = (u8 *)((u32)primary1 - 4);
    gModel_Slot0VariantModuleAddress = (s32)secondary0 - 4;
    gModel_Slot1VariantModuleAddress = (s32)secondary1 - 4;
    *(u8 **)((u8 *)self + 0xDE8) = &contexts[chosen_index * 2];
    *(u8 **)((u8 *)self + 0xDEC) = &contexts[chosen_index * 2 + 1];
    self->is_active = serial % 11 != 0;
    self->animation_control_state = initial_state;
    self->field_DFE = serial & 1;
    self->current_animation_row = serial & 2 ? self->field_DFE + 3 : 0;
    self->field_DF8 = serial % 5 ? 1 : 0x309;
    self->animation_position = frames[(serial + 1) % 7];
    self->animation_sound_position = 0x1234;
    self->playback_speed = serial % 7 ? 1 + serial % 4 : 255;
    commands = (s32 *)((u8 *)self + 0xD08);
    commands[0] = command_mode == 0 ? -1 : command_mode == 1 ? 0 : 1234567;
    commands[1] = command_mode == 0 ? -1 : command_mode == 1 ? 999 : 2001000;
    commands[2] = serial & 4 ? -1 : 7;
    for (i = 0; i < MODEL_SLOT_ROW_COUNT; i++)
        self->animation_duration_rows[i].max = (self->animation_position >> 4) * 2 + (serial % 3) * 2;
    other->active_animation_command_row = animations[serial % 4];
    other->animation_position = frames[serial % 7];
    other->playback_state = serial % 7 ? 0 : 0x23;
    other->animation_sequence_count = serial % 3;
    other->current_animation_row = 4;
    other->field_DFF = defense;
    other->field_DF8 = serial & 1;
    a = (u16 *)&self->field_CF8;
    b = (u16 *)&other->field_CF8;
    a[0] = serial & 8 ? 0xFFFF : 100;
    b[0] = relation == 0 ? a[0] - 1 : relation == 1 ? a[0] : (u16)(a[0] + 1);
    b[1] = relation == 0 ? a[0] - 1 : a[0];
}

s32 main(void)
{
    static const s32 results[] = {0, 1, 2, 3, 4, 5, 7, 255};
    s32 result_index;
    u8 saved_active;
    u32 cases = 0;
    /* Observe the linker alias at runtime instead of folding distinct names. */
    volatile u32 linked_alias = (u32)&gModel_Slot0MetadataView;
    volatile u32 parent_field = (u32)&gModel_aSlots[0].field_CF8;
    if (sizeof(void *) != 4 || linked_alias != parent_field) return 90;
    for (chosen_index = 0; chosen_index < 2; chosen_index++)
    for (initial_state = 0; initial_state < 9; initial_state++)
    for (result_index = 0; result_index < 8; result_index++)
    for (relation = 0; relation < 3; relation++)
    for (defense = 0; defense < 2; defense++)
    for (command_mode = 0; command_mode < 3; command_mode++)
    for (mutate = 0; mutate < 2; mutate++) {
        serial = cases;
        result_state = results[result_index];
        setup();
        Model_UpdateSlotControl(chosen_index);
        if (fixture_error) return fixture_error;
        copy_bytes(saved_slots, gModel_aSlots, sizeof(saved_slots));
        copy_bytes(saved_events, events, sizeof(saved_events));
        saved_count = event_count;
        saved_active = gModel_bActiveSlotIndex;
        setup();
        reference(chosen_index);
        if (fixture_error) return fixture_error + 10;
        if (!equal_bytes(saved_slots, gModel_aSlots, sizeof(saved_slots))) return 20;
        if (saved_count != event_count || !equal_bytes(saved_events, events, sizeof(saved_events))) return 21;
        if (saved_active != gModel_bActiveSlotIndex) return 22;
        cases++;
    }
    return cases == 5184 ? 0 : 91;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host C compiler and ILP32 execution",
)
def controller_section() -> str:
    """The unit's preamble plus the control step's own section, which is
    what this witness links; the unit's other functions call out of it."""
    text = SOURCE.read_text()
    head, rest = text.split("void Model_UpdateSlotAnimation(", 1)
    after = rest.split("\n}\n", 1)[1]
    return head + after.split("void Model_ResetSlot(", 1)[0]


class ModelControlTests(unittest.TestCase):
    def run_witness(self, optimization: str, mutation: bool = False):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="model-control-", dir=REPOSITORY / "tmp") as temporary:
            directory = Path(temporary)
            text = controller_section()
            old = "commands[slot->field_DFE]"
            self.assertIn(old, text)
            if mutation:
                text = text.replace(old, "commands[slot->field_DFE + 1]")
            text = re.sub(
                r'^#include "([^"]+)"',
                lambda m: '#include "' + str((SOURCE.parent / m[1]).resolve()) + '"',
                text, flags=re.MULTILINE,
            )
            source = directory / ("wrong-command.c" if mutation else "controller.c")
            source.write_text(text)
            witness = directory / "witness.c"
            prefix = controller_section().split("void Model_UpdateSlotControl(", 1)[0]
            prefix = re.sub(
                r'^#include "([^"]+)"',
                lambda m: '#include "' + str((SOURCE.parent / m[1]).resolve()) + '"',
                prefix, flags=re.MULTILINE,
            )
            witness.write_text(prefix + WITNESS)
            startup = directory / "start.S"
            startup.write_text(START)
            binary = directory / "witness"
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            compiled = subprocess.run([
                "cc", "-m32", optimization, "-std=gnu99", "-ffreestanding", "-fno-builtin",
                "-fno-strict-aliasing", "-fno-pie", "-no-pie", "-fno-stack-protector",
                "-nostdlib", str(source), str(witness), str(startup),
                "-Wl,--defsym,gModel_Slot0MetadataView=gModel_aSlots+3320", "-o", str(binary),
            ], capture_output=True, text=True, timeout=60, env=environment)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            return subprocess.run([str(binary)], capture_output=True, text=True, timeout=60)

    def test_actual_controller_matches_reference_at_o0_and_o2(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization)
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_wrong_command_selector_is_rejected(self):
        result = self.run_witness("-O2", mutation=True)
        self.assertIn(result.returncode, (20, 21, 22), result.stderr)
