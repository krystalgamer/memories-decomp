#define FUNC_80049F50_RETURNS_S16
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "func_80044DC0.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_voice_constants.h"
#include "../unmatched.h"

void SD_UpdateRuntime(void)
{
    SDValue *p;
    u8 *e;
    s32 mask;
    s32 i;
    s32 off;
    s32 value;
    SDValue *loop_state;
    SDValue *q;
    SDValue *r;

    SpuGetAllKeysStatus((char *)g_SDValue->field_15D8);
    p = g_SDValue;
    mask = (p->field_15EC == 3) << SD_VOICE_SLOT_KEY_SHIFT;
    if (p->field_15ED == 3) {
        mask |= SD_VOICE_SLOT_MASK_BASE << 1;
    }
    if (p->field_15EE == 3) {
        mask |= SD_VOICE_SLOT_MASK_BASE << 2;
    }
    if (p->field_15EF == 3) {
        mask |= SD_VOICE_SLOT_MASK_BASE << 3;
    }
    if (mask != 0) {
        SpuSetKey(0, mask);
    }

    p = g_SDValue;
    if ((p->flags_0040 & 0x80) != 0 &&
        p->field_157E != -1 &&
        func_80049F50() != 1) {
        func_80049C40(g_SDValue->field_157E);
        p = g_SDValue;
        value = p->flags_0040;
        p->field_1588 = -0x80;
        p->field_158A = 0;
        value &= 0xFF7F;
        p->flags_0040 = value;
    }

    SD_UpdateFades();
    p = g_SDValue;
    if (p->field_007C != 0) {
        func_80045514();
        return;
    }
    if (p->command_count == 0) {
        return;
    }
    e = (u8 *)&p->commands.c[0];
    /* Separate bodies preserve equality dispatch instead of case ranges;
       cross-jumping still shares the emitted bodies. */
    value = p->commands.c[0].command;
    switch (value) {
    case 0x44:
        if (p->field_1588 == 0) {
            p->field_1588 = *(u16 *)(e + 8);
            p->field_1584 = e[2];
        }
        break;
    case 0x45:
        if (p->field_1588 == 0) {
            p->field_1588 = *(u16 *)(e + 8);
            p->field_1584 = e[2];
        }
        break;
    case 0x29:
        if (p->field_0512 == 0) {
            p->field_0512 = *(u16 *)(e + 8);
            p->field_0049 = e[2];
        }
        break;
    case 0x2A:
        if (p->field_0512 == 0) {
            p->field_0512 = *(u16 *)(e + 8);
            p->field_0049 = e[2];
        }
        break;
    }

    p = g_SDValue;
    p->field_007C = e[0];
    g_SDValue->field_007D = 0;
    p = g_SDValue;
    p->field_004E = *(u16 *)(e + 2);
    p->field_0050 = *(u32 *)(e + 4);
    p->field_0054 = *(u32 *)(e + 8);
    p->field_0058 = *(u32 *)(e + 0xC);
    p->field_007E = e[1];
    /* The second global read preserves separate pointers for the tail copy
       and queue bookkeeping after CSE. */
    r = g_SDValue;
    *(SDCommandTail *)&r->field_005C[0] = *(SDCommandTail *)(e + 0x10);
    q = g_SDValue;
    q->field_15F4 = 8;
    q->command_count = q->command_count - 1;
    func_80045514();

    loop_state = g_SDValue;
    e += 0x30;
    i = 0;
    if (loop_state->command_count <= 0) {
        return;
    }
    off = 0;
    do {
        /* Preserve base-plus-offset ordering; sound.h checks the queue offset. */
        *(u8 *)((u8 *)loop_state + off + SD_COMMAND_QUEUE_BYTE_OFFSET) = e[0];
        *(SDCommand *)((u8 *)g_SDValue + off + SD_COMMAND_QUEUE_BYTE_OFFSET) =
            *(SDCommand *)e;
        off += 0x30;
        loop_state = g_SDValue;
        i += 1;
        e += 0x30;
    } while (i < loop_state->command_count);
}
