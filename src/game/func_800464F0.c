#include "../types.h"

typedef struct {
    u8 pad0[0x4C];
    s16 count;
    u8 pad1[0x7C - 0x4E];
    u8 f7C;
    u8 f7D;
    u8 pad2[0x80 - 0x7E];
    u8 entries[1];
} SoundState;

typedef struct {
    u32 words[12];
} SoundEntry;

extern SoundState *g_SDValue[];
#define SOUND_STATE (g_SDValue[0])

void func_800464F0(void)
{
    SoundState *p;
    register s32 i asm("a2");
    register s32 j asm("a1");
    register s32 k asm("a3");
    s32 tag;
    register SoundEntry *dst asm("v0");
    register u8 *src_base asm("v1");
    register SoundEntry *src asm("a0");
    register s32 c29 asm("t2");
    register s32 c24 asm("t1");
    register s32 c2b asm("t0");

    p = SOUND_STATE;
    i = 0;
    if (p->count <= 0) {
        goto tail_dispatch;
    }
    c29 = 0x29;
    c24 = 0x24;
    c2b = 0x2B;
    k = 0x30;
    j = i;

loop:
    if (p->count == 0) {
        goto tail_dispatch;
    }
    tag = p->entries[j];
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
    dst = (SoundEntry *)((u8 *)p + j);
    dst = (SoundEntry *)((u8 *)dst + 0x80);
    src_base = (u8 *)p + k;
    src = (SoundEntry *)(src_base + 0x80);
    *dst = *src;
    p = SOUND_STATE;
    p->count = (u16)p->count - 1;
    goto tail_test;

no_match:
    k += 0x30;

advance:
    j += 0x30;
    i += 1;

tail_test:
    p = SOUND_STATE;
    if (i >= p->count) {
        goto tail_dispatch;
    }
    if (i >= 0) {
        goto loop;
    }

tail_dispatch:
    {
        register SoundState *tail_p asm("v0") = SOUND_STATE;
        tag = tail_p->f7C;
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
    SOUND_STATE->f7C = 0;
    SOUND_STATE->f7D = 0;
}
