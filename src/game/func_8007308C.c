#include "../types.h"

typedef struct {
    u8 unk0[4];
    u8 *base;   /* +4 */
    u8 *cursor; /* +8 */
    u8 unkC[8];
    /* Return-address stack for nested routines: func_80070DA8 pops
     * cursor = stack[--depth] and prints "ERROR:Can't Return From Routine"
     * when depth is already zero. */
    u8 depth;   /* +0x14 */
    u8 unk15[3];
    u8 *stack[8];  /* +0x18 */
} ByteReader;

typedef struct {
    s16 unk0;
    u8 unk2[4];
    u16 unk6;
    s8 unk8;
    u8 unk9[3];
} Rec12;

extern s32 gAiScript_aMemory[];
extern ByteReader gAiScript_State[];
extern u16 gAi_wBestDifference[];
extern Rec12 gDuel_aActiveCards[];

s32 AiScript_ReadByte(void);

void AiScript_FindBestAttack(void) {
    u8 *t;
    u8 *r;
    u8 *e;
    u8 *s;
    u8 *a;
    s32 want;
    s32 out;
    s32 i;
    s32 j;
    s32 v;
    u16 f;

    want = gAiScript_aMemory[AiScript_ReadByte()];
    out = AiScript_ReadByte();
    i = 1;
    t = (u8 *)gDuel_aActiveCards;
    r = t + 0xC;
    a = (u8 *)gAiScript_State;
    *(s16 *)(a + 0x98) = 0;
    s = a;

    while (i < 6) {
        if (*(s16 *)r == 0) {
            continue;
        }
        if ((*(u16 *)(r + 6) & 0x4000) != 0) {
            continue;
        }

        for (j = 0x38, e = t + 0x2A0; j < 0x3D; j++, e += 0xC) {
            if (*(s16 *)e == 0) {
                continue;
            }
            f = *(u16 *)(e + 6);
            if ((f & 0x4000) != 0) {
                continue;
            }
            if ((f & 0x800) != 0) {
                continue;
            }
            if (want != 0) {
                if ((f & 0x1000) != 0) {
                    continue;
                }
            }
            v = *(s16 *)(r + 2) - *(s16 *)(e + 2);
            v += Duel_CalcGuardianStarMatchup(*(s8 *)(r + 9), *(s8 *)(e + 9));
            if (*(u16 *)(s + 0x98) < v) {
                *(s16 *)(s + 0x98) = v;
                *(s8 *)(s + 0x9A) = i;
                *(s8 *)(s + 0x9B) = j;
            }
        }
    }

    if (gAi_wBestDifference[0] != 0) {
        gAiScript_aMemory[out] = 0;
    } else {
        gAiScript_aMemory[out] = 1;
    }
}
