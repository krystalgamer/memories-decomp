#define G_SDVALUE_AGGREGATE
#include "../types.h"
#include "sound.h"

#define SOUND_STATE ((SDValue *)g_SDValue[0])

void func_800464F0(void)
{
    SDValue *p;
    register s32 i asm("a2");
    register s32 j asm("a1");
    register s32 k asm("a3");
    s32 tag;
    register SDCommand *dst asm("v0");
    register u8 *src_base asm("v1");
    register SDCommand *src asm("a0");
    register s32 c29 asm("t2");
    register s32 c24 asm("t1");
    register s32 c2b asm("t0");

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
    /* The byte form, and the grouping, are both load-bearing: retail
           adds the record base to the running byte offset in that order.
           sound_runtime.c keeps the same shape over the same queue. */
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
    k += 0x30;
    goto advance;

high_range:
    if (tag != c2b) {
        goto no_match;
    }

match:
    p = SOUND_STATE;
    dst = (SDCommand *)((u8 *)p + j);
    dst = (SDCommand *)((u8 *)dst + SD_COMMAND_QUEUE_BYTE_OFFSET);
    src_base = (u8 *)p + k;
    src = (SDCommand *)(src_base + SD_COMMAND_QUEUE_BYTE_OFFSET);
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
        register SDValue *tail_p asm("v0") = SOUND_STATE;
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
