#include "../types.h"
#include "duel_card_layout.h"

typedef struct {
    u8 unk0[0x14];
    s16 unk14;
    u8 unk16[3];
    s8 unk19;
    u8 unk1A[6];
} Rec20;

typedef struct {
    u8 *unk0;
    u8 unk4[5];
    u8 unk9;
    u8 unkA[2];
} Rec0C;

extern u16 D_8009B23A;
extern u8 D_8009B1ED;
extern u8 D_8009B1EC;
extern u8 *D_8009B1C8;
extern u8 gDuel_bWinnerSide;
extern u8 D_8009B1D5;
extern u8 D_800907CC[];
extern Rec20 D_800E9FF0[];
extern Rec0C D_800EA030[];
extern u8 D_8015C424[];
extern u8 D_801A7AD8[];

void func_80018C34(u8 *arg0);
s32 Duel_HasAllExodiaPieces(void);
s32 func_80042B40(s32 arg0);
u8 *func_80018004();
void SD_SEPlayFull(s32 sound_id);

void func_80018DB4(void) {
    u8 *p;
    u8 *c;
    u8 *g;
    u8 *base;
    s32 i;
    s32 k;
    s32 v;
    s32 t;
    s32 b;
    s32 a;
    s32 n;
    s32 off;
    s32 y;

    v = D_8009B23A;
    if ((v & 0x8000) == 0) {
        D_8009B23A = v | 0x8000;
        D_8009B1ED = 1;
    }

    if ((D_8009B23A & 0x4000) == 0) {
        t = D_8009B1ED - 1;
        D_8009B1ED = t;
        if ((s8)t > 0) {
            return;
        }
        D_8009B1ED = 8;
        b = D_8009B1EC - 1;
        c = D_8009B1C8;
        D_8009B1EC = b;
        if (*(s8 *)(c + 0x18) >= 0x28) {
            gDuel_bWinnerSide = D_8009B1D5 ^ 1;
            *(s8 *)&D_800E9FF0[gDuel_bWinnerSide] = -0x28;
            D_8009B23A = 0xC;
            return;
        }
        i = 4 - (s8)b;
        a = D_800907CC[i + D_8009B1D5 * 5];
        if ((a & 0x80) != 0) {
            k = (a & 0x7F) + 0xF;
        } else {
            k = a;
        }
        func_800249E0(k, *(s8 *)(D_8009B1C8 + 0x18));
        p = func_80018004(
            &D_801A7AD8[k * DUEL_CARD_RECORD_SIZE],
            i * 60 + 0x14E,
            0x92
        );
        p[0x6C] = 1;
        *(s16 *)(p + 0x60) = 0xC;
        *(s32 *)(p + 0x24) = (s32)func_80018C34;
        D_800EA030[i].unk0 = p;
        base = D_8015C424;
        g = base + p[0x6A] * DUEL_CARD_RECORD_SIZE + 0x48000;
        y = *(s8 *)(*(s32 *)(g + 0x36B8) + 2);
        *(s8 *)(D_8009B1C8 + i + 0x1A) = y;
        n = *(u8 *)(D_8009B1C8 + 0x18);
        *(u8 *)(D_8009B1C8 + 0x18) = n + 1;
        if (*(s8 *)&D_8009B1EC == 0) {
            D_8009B23A = D_8009B23A | 0x4000;
        }
        SD_SEPlayFull(0xA);
    } else {
        if (func_80042B40(1) == 0) {
            D_8009B23A = 4;
            if (Duel_HasAllExodiaPieces() != 0) {
                D_8009B23A = 0xE;
            }
        }
    }
}
