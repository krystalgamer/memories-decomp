#include "../types.h"

typedef struct {
    s16 unk0;
    u8 unk2[4];
    u16 unk6;
    s8 unk8;
    u8 unk9[3];
} Rec12;

extern s32 gAiScript_aMemory[];
extern Rec12 gDuel_aActiveCards[];

s32 AiScript_ReadByte(void);

/* -fno-strength-reduce, fourth user. The record walk reads at +0, +2, +6
 * and +9 and gcc builds a SECOND giv biased at +2 because most of the reads
 * are there; that costs a callee-saved register and four frame instructions
 * (78 differences, +4 length). Retail has one cursor with plain
 * displacements. No source spelling removes the anchor -- an index form, a
 * struct cursor, dropping the named compare value and inlining the base
 * were all tried -- because the bias is the reducer's, not the source's. */

void AiScript_FindKiller(void) {
    u8 *base;
    u8 *c;
    u8 *ref;
    s32 a;
    s32 m;
    s32 w;
    s32 i;
    s32 k;
    s32 best;
    s32 d;
    s32 t;

    a = gAiScript_aMemory[AiScript_ReadByte()];
    best = 0x270F;
    m = gAiScript_aMemory[AiScript_ReadByte()];
    k = 0;
    w = AiScript_ReadByte();
    i = 1;
    base = gDuel_aActiveCards;
    c = base + 0xC;
    ref = base + a * 12;

    for (; i < 6; i++, c += 0xC) {
        if (*(s16 *)c == 0) {
            continue;
        }
        if ((*(u16 *)(c + 6) & 0x4000) != 0) {
            continue;
        }
        if (m == 0) {
            d = *(s16 *)(c + 2) - *(s16 *)(ref + 2);
        } else {
            d = *(s16 *)(c + 2) - *(s16 *)(ref + 4);
        }
        d = d + Duel_CalcGuardianStarMatchup(*(s8 *)(c + 9), *(s8 *)(ref + 9));
        if (d > 0) {
            t = *(s16 *)(c + 2);
            if (t < best) {
                best = t;
                k = i;
            }
        }
    }

    gAiScript_aMemory[w] = k;
}
