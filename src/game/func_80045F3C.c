#include "../types.h"
#include "sound_voice_constants.h"

typedef struct {
    u8 pad00[0x40];
    u16 flags40;
    u8 pad42[0x49 - 0x42];
    u8 f49;
    u8 pad4A[0x4C - 0x4A];
    s16 count4C;
    u16 f4E;
    u32 f50;
    u32 f54;
    u32 f58;
    u32 f5C[8];
    u8 f7C;
    u8 f7D;
    u8 f7E;
    u8 pad7F;
    u8 entries[0x512 - 0x80];
    s16 f512;
    u8 pad514[0x157E - 0x514];
    s16 f157E;
    u8 pad1580[0x1584 - 0x1580];
    u8 f1584;
    u8 pad1585[0x1588 - 0x1585];
    s16 f1588;
    u8 f158A;
    u8 pad158B[0x15D8 - 0x158B];
    u32 f15D8;
    u8 pad15DC[0x15EC - 0x15DC];
    u8 f15EC;
    u8 f15ED;
    u8 f15EE;
    u8 f15EF;
    u8 pad15F0[0x15F4 - 0x15F0];
    s16 f15F4;
} SoundState;

typedef struct { u32 w[12]; } SoundEntry;
typedef struct { u32 w[8]; } Blk32;

extern SoundState *g_SDValue __attribute__((section(".data")));
#define SOUND (g_SDValue)

extern void SpuGetAllKeysStatus(void *);
extern void SpuSetKey(s32, s32);
extern s16 func_80049F50(void);
extern void func_80049C40(s16);
extern void func_80045C98(void);
extern void func_80045514(void);

void func_80045F3C(void)
{
    register SoundState *p asm("$3");
    u8 *e;
    s32 mask;
    s32 i;
    s32 off;
    register SoundState *q asm("$4");
    register SoundState *r asm("$2");

    SpuGetAllKeysStatus(&SOUND->f15D8);
    p = SOUND;
    mask = (p->f15EC == 3) << SD_VOICE_SLOT_KEY_SHIFT;
    if (p->f15ED == 3) {
        mask |= SD_VOICE_SLOT_MASK_BASE << 1;
    }
    if (p->f15EE == 3) {
        mask |= SD_VOICE_SLOT_MASK_BASE << 2;
    }
    if (p->f15EF == 3) {
        mask |= SD_VOICE_SLOT_MASK_BASE << 3;
    }
    if (mask != 0) {
        SpuSetKey(0, mask);
    }

    p = SOUND;
    if ((p->flags40 & 0x80) != 0 && p->f157E != -1 && func_80049F50() != 1) {
        func_80049C40(SOUND->f157E);
        p = SOUND;
        p->f1588 = -0x80;
        p->f158A = 0;
        p->flags40 = p->flags40 & 0xFF7F;
    }

    func_80045C98();
    p = SOUND;
    if (p->f7C != 0) {
        func_80045514();
        return;
    }
    if (p->count4C == 0) {
        return;
    }
    e = &p->entries[0];
    /* Four separate case bodies, not two shared ones: adjacent case values
       that share a body are merged into a case range, and the range tests GCC
       then emits are not the target's equality dispatch. Cross-jumping merges
       the duplicated bodies again, so nothing is spent on them. */
    switch (p->entries[0]) {
    case 0x44:
        if (p->f1588 == 0) {
            p->f1588 = *(u16 *)(e + 8);
            p->f1584 = e[2];
        }
        break;
    case 0x45:
        if (p->f1588 == 0) {
            p->f1588 = *(u16 *)(e + 8);
            p->f1584 = e[2];
        }
        break;
    case 0x29:
        if (p->f512 == 0) {
            p->f512 = *(u16 *)(e + 8);
            p->f49 = e[2];
        }
        break;
    case 0x2A:
        if (p->f512 == 0) {
            p->f512 = *(u16 *)(e + 8);
            p->f49 = e[2];
        }
        break;
    }

    p = SOUND;
    p->f7C = e[0];
    SOUND->f7D = 0;
    p = SOUND;
    p->f4E = *(u16 *)(e + 2);
    p->f50 = *(u32 *)(e + 4);
    p->f54 = *(u32 *)(e + 8);
    p->f58 = *(u32 *)(e + 0xC);
    p->f7E = e[1];
    /* Read the global twice rather than copying r into q. CSE turns the
       second read into the target's `move $a0,$v0`, and unlike an explicit
       copy it leaves the eight stores addressed off the first register. */
    r = SOUND;
    *(Blk32 *)&r->f5C[0] = *(Blk32 *)(e + 0x10);
    q = SOUND;
    q->f15F4 = 8;
    q->count4C = q->count4C - 1;
    func_80045514();

    p = SOUND;
    e += 0x30;
    i = 0;
    if (p->count4C <= 0) {
        return;
    }
    off = 0;
    do {
        *(u8 *)((u8 *)p + off + 0x80) = e[0];
        *(SoundEntry *)((u8 *)SOUND + off + 0x80) = *(SoundEntry *)e;
        off += 0x30;
        p = SOUND;
        i += 1;
        e += 0x30;
    } while (i < p->count4C);
}
