#define G_SDVALUE_AGGREGATE
#include "../types.h"
#include "sound.h"

#define SOUND_STATE ((SDValue *)g_SDValue[0])

void func_800464F0(void)
{
    SDValue *p;
    s32 i;
    s32 j;
    s32 k;
    s32 tag;
    SDCommand *dst;
    SDCommand *src;
    s32 c29;
    s32 c24;
    s32 c2b;

    p = SOUND_STATE;
    i = 0;
    if (p->command_count <= 0) {
        goto tail_dispatch;
    }
    c29 = 0x29;
    c24 = 0x24;
    c2b = 0x2B;
    k = 0x30;
    j = i;

loop:
    if (p->command_count == 0) {
        goto tail_dispatch;
    }
    tag = *(u8 *)((u8 *)p + j + SD_COMMAND_QUEUE_BYTE_OFFSET);
    if (tag == c29) {
        goto match;
    }
    if (tag >= 0x2A) {
        goto high_range;
    }
    if (tag == c24) {
        goto match;
    }
    goto no_match;

high_range:
    if (tag != c2b) {
        goto no_match;
    }

match:
    p = SOUND_STATE;
    src = (SDCommand *)((u8 *)p + j);
    dst = (SDCommand *)((u8 *)src + SD_COMMAND_QUEUE_BYTE_OFFSET);
    src = (SDCommand *)((u8 *)p + k + SD_COMMAND_QUEUE_BYTE_OFFSET);
    *dst = *src;
    p = SOUND_STATE;
    p->command_count = (u16)p->command_count - 1;
    goto tail_test;

no_match:
    k += 0x30;

advance:
    j += 0x30;
    i += 1;

tail_test:
    p = SOUND_STATE;
    if (i >= p->command_count) {
        goto tail_dispatch;
    }
    if (i >= 0) {
        goto loop;
    }

tail_dispatch:
    {
        SDValue *tail_p = SOUND_STATE;
        tag = tail_p->field_007C;
    }
    if (tag == 0x29) {
        goto clear;
    }
    if (tag >= 0x2A) {
        goto check_2B;
    }
    if (tag == 0x24) {
        goto clear;
    }
    return;

check_2B:
    if (tag != 0x2B) {
        return;
    }

clear:
    SOUND_STATE->field_007C = 0;
    SOUND_STATE->field_007D = 0;
}
