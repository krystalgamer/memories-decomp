#include "../../../../src/types.h"

typedef struct {
    u8 pad00[0x40];
    u16 flags;
    u8 pad42[0x4C - 0x42];
    s16 count;
    u8 pad4E[0x7C - 0x4E];
    u8 f7C;
    u8 f7D;
    u8 pad7E[0x80 - 0x7E];
    u8 entries[0x157E - 0x80];
    s16 f157E;
} SoundState;

typedef struct { u32 words[12]; } SoundEntry;

extern SoundState *g_SDValue __attribute__((section(".data")));
#define SOUND_STATE (g_SDValue)

extern s16 func_80049F50(void);
extern void func_80049C40(s16 arg0);

void func_80046294(void)
{
    SoundState *p;
    register s32 i asm("a3");
    register s32 j asm("a2");
    register s32 k asm("t0");
    SoundState *q;
    s32 tag;

    p = SOUND_STATE;
    i = 0;
    if (p->count <= 0) {
        goto tail;
    }
    k = 0x30;
    j = i;

    do {
    if (p->count == 0) {
        goto tail;
    }
    tag = p->entries[j];
    switch (tag) {
    case 0x42:
    case 0x43:
    case 0x45:
    case 0x46:
    case 0x48:
        p = SOUND_STATE;
    {
        register SoundEntry *dst1 asm("v0");
        register u8 *src_base1 asm("v1");
        SoundEntry *src1;

        dst1 = (SoundEntry *)((u8 *)p + j);
        dst1 = (SoundEntry *)((u8 *)dst1 + 0x80);
        src_base1 = (u8 *)p + k;
        src1 = (SoundEntry *)(src_base1 + 0x80);
        *dst1 = *src1;
    }
        p = SOUND_STATE;
        p->count = (u16)p->count - 1;
        goto test;
    case 0x20:
        q = SOUND_STATE;
        if (*(s32 *)&q->entries[j + 0x10] != 0x20) {
            goto test;
        }
        p = q;
        p = SOUND_STATE;
    {
        register SoundEntry *dst2 asm("a0");
        u8 *src_base2;
        SoundEntry *src2;

        dst2 = (SoundEntry *)((u8 *)p + j);
        dst2 = (SoundEntry *)((u8 *)dst2 + 0x80);
        src_base2 = (u8 *)p + k;
        src2 = (SoundEntry *)(src_base2 + 0x80);
        *dst2 = *src2;
    }
        p = SOUND_STATE;
        p->count = (u16)p->count - 1;
        goto test;
    default:
        k += 0x30;
        j += 0x30;
        i += 1;
        goto test;
    }

test:
    p = SOUND_STATE;
    } while (i < p->count && i >= 0);
    goto tail;

tail:
    switch (SOUND_STATE->f7C) {
    case 0x42:
    case 0x43:
    case 0x45:
    case 0x46:
    case 0x48:
        SOUND_STATE->f7C = 0;
        SOUND_STATE->f7D = 0;
        break;
    }

    p = SOUND_STATE;
    if ((p->flags & 0x80) == 0) {
        return;
    }
    if (p->f157E == -1) {
        return;
    }
    if (func_80049F50() == 1) {
        func_80049C40(SOUND_STATE->f157E);
    }
    SOUND_STATE->flags &= 0xFF7F;
}
