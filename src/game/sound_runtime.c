#define SD_POLL_SEQUENCE_STATE_RETURNS_S16
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "func_80044DC0.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_voice_constants.h"
#include "../unmatched.h"

s32 SD_EnqueueCommand(SDCommand *src) {
    SDValue *b1;
    SDValue *b2;
    SDValue *b3;
    s32 n1;
    s32 n2;
    s32 one;

    b1 = g_SDValue;
    n1 = b1->command_count;
    if (n1 >= SD_COMMAND_QUEUE_COUNT) {
        goto full;
    }
    {
        b1->commands.c[n1].command = src->command;
    }

    b2 = g_SDValue;
    n2 = b2->command_count;
    b2->commands.c[n2] = *src;

    b3 = g_SDValue;
    one = 1;
    b3->command_count = b3->command_count + one;
    return one;
full:
    return 0;
}

void SD_UpdateFades(void) {
    u8 changed;
    s16 y;

    changed = 0;
    if (g_SDValue->field_1588 != 0) {
        g_SDValue->field_1586 += g_SDValue->field_1588;
        changed = 1;
        if (g_SDValue->field_1586 <= g_SDValue->field_158A &&
            g_SDValue->field_1588 < 0) {
            g_SDValue->field_1588 = 0;
            g_SDValue->field_1586 = g_SDValue->field_158A;
        }
        if (g_SDValue->field_1586 >= g_SDValue->field_158A &&
            g_SDValue->field_1588 > 0) {
            g_SDValue->field_1588 = 0;
            g_SDValue->field_1586 = g_SDValue->field_158A;
        }
    }
    if (g_SDValue->field_1582 != 0) {
        g_SDValue->field_1580 += g_SDValue->field_1582;
        changed = 1;
        if (g_SDValue->field_1580 <= g_SDValue->field_1584 &&
            g_SDValue->field_1582 < 0) {
            g_SDValue->field_1582 = 0;
            g_SDValue->field_1580 = g_SDValue->field_1584;
        }
        if (g_SDValue->field_1580 >= g_SDValue->field_1584 &&
            g_SDValue->field_1582 > 0) {
            g_SDValue->field_1582 = 0;
            g_SDValue->field_1580 = g_SDValue->field_1584;
        }
    }
    if (changed) {
        u16 v;

        v = g_SDValue->field_0044;
        if (v) {
            v *= g_SDValue->field_1580 + 1;
            v >>= 8;
        } else {
            v = 0;
        }
        if (v) {
            v *= g_SDValue->field_1586 + 1;
            v >>= 8;
        } else {
            v = 0;
        }
        y = v;
        func_80049F10(y, y);
    }
    if (g_SDValue->field_0512 != 0) {
        g_SDValue->cd_volume += g_SDValue->field_0512;
        if (g_SDValue->cd_volume <= g_SDValue->field_0049 &&
            g_SDValue->field_0512 < 0) {
            g_SDValue->field_0512 = 0;
            g_SDValue->cd_volume = g_SDValue->field_0049;
        }
        if (g_SDValue->cd_volume >= g_SDValue->field_0049 &&
            g_SDValue->field_0512 > 0) {
            g_SDValue->field_0512 = 0;
            g_SDValue->cd_volume = g_SDValue->field_0049;
        }
        y = g_SDValue->cd_volume;
        func_80044DC0(y);
    }
}

void SD_UpdateRuntime(void)
{
    SDValue *p;
    SDCommand *e;
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
        SD_PollSequenceState() != 1) {
        SD_StopSequence(g_SDValue->field_157E);
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
    e = &p->commands.c[0];
    /* Separate bodies preserve equality dispatch instead of case ranges;
       cross-jumping still shares the emitted bodies. */
    value = p->commands.c[0].command;
    switch (value) {
    case 0x44:
        if (p->field_1588 == 0) {
            p->field_1588 = (u16)e->field_0008;
            p->field_1584 = (u8)e->field_0002;
        }
        break;
    case 0x45:
        if (p->field_1588 == 0) {
            p->field_1588 = (u16)e->field_0008;
            p->field_1584 = (u8)e->field_0002;
        }
        break;
    case 0x29:
        if (p->field_0512 == 0) {
            p->field_0512 = (u16)e->field_0008;
            p->field_0049 = (u8)e->field_0002;
        }
        break;
    case 0x2A:
        if (p->field_0512 == 0) {
            p->field_0512 = (u16)e->field_0008;
            p->field_0049 = (u8)e->field_0002;
        }
        break;
    }

    p = g_SDValue;
    p->field_007C = e->command;
    g_SDValue->field_007D = 0;
    p = g_SDValue;
    p->field_004E = (u16)e->field_0002;
    p->field_0050 = e->field_0004;
    p->field_0054 = e->field_0008;
    p->field_0058 = e->field_000C;
    p->field_007E = e->field_0001;
    /* The second global read preserves separate pointers for the tail copy
       and queue bookkeeping after CSE. */
    r = g_SDValue;
    *(SDCommandTail *)&r->field_005C[0] = *(SDCommandTail *)&e->field_0010;
    q = g_SDValue;
    q->field_15F4 = 8;
    q->command_count = q->command_count - 1;
    func_80045514();

    loop_state = g_SDValue;
    e++;
    i = 0;
    if (loop_state->command_count <= 0) {
        return;
    }
    off = 0;
    do {
        /* Base-plus-offset order matters here. The byte store indexes the
           queue's byte view, whose ARRAY_REF keeps it; the block copy stays
           a byte-address sum, because &commands.b[off] puts the offset
           first. sound.h checks the queue offset. */
        loop_state->commands.b[off] = e->command;
        *(SDCommand *)((u8 *)g_SDValue + off + SD_COMMAND_QUEUE_BYTE_OFFSET) =
            *e;
        off += 0x30;
        loop_state = g_SDValue;
        i += 1;
        e++;
    } while (i < loop_state->command_count);
}
