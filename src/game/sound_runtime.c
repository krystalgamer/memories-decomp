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
            v = (u16)(v * (g_SDValue->field_1580 + 1));
            v >>= 8;
        } else {
            v = 0;
        }
        if (v) {
            v = (u16)(v * (g_SDValue->field_1586 + 1));
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
