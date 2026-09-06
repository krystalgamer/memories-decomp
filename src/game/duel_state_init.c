#include "../types.h"
#include "card_constants.h"

typedef struct {
    u8 unk0[0x14];
    s16 unk14;
    u8 unk16[3];
    s8 unk19;
    u8 unk1A[6];
} Rec20;

typedef struct {
    int field_00;
    int field_04;
    u8 pad_08;
    u8 field_09;
    u8 pad_0A[2];
} HandEntry;

extern u8 *D_8009B1C8;
extern u8 D_8009B1D5;
extern u16 D_8009B230;
extern u16 D_8009B234;
extern u16 D_8009B236;
extern s8 D_8009B360[];
extern s8 gDuel_bOpponentID __attribute__((section(".data")));
extern Rec20 D_800E9FF0[];
extern s8 D_800EA02F[];
extern HandEntry D_800EA030[HAND_SIZE];

void func_800175A0(void) {
    u16 sp[2];
    Rec20 *e;
    s32 k;
    s32 j;
    u32 m;
    u16 t;
    u8 *q;
    s8 *r;

    e = D_800E9FF0;
    if (gDuel_bOpponentID < 0) {
        sp[0] = D_8009B234;
        sp[1] = D_8009B236;
    } else {
        sp[1] = DUEL_STARTING_LIFE_POINTS;
        sp[0] = DUEL_STARTING_LIFE_POINTS;
    }

    for (k = 0; k < 2; k++, e++) {
        j = 4;
        r = (s8 *)e + j;
        for (; j >= 0; j--, r--) {
            r[0x1A] = -1;
        }
        q = (u8 *)e;
        e->unk16[2] = 0;
        t = sp[k];
        *(s16 *)(e->unk0 + 0x12) = 0;
        e->unk19 = 0;
        e->unk14 = t;
        *(u16 *)e->unk16 = t;
        e->unk1A[5] = 0;
        for (m = 0; m < 13; m++) {
            *q = 0;
            q++;
        }
    }

    if (D_8009B360[0] < 0) {
        if (gDuel_bOpponentID < 0) {
            D_800E9FF0[1].unk1A[5] = *(u8 *)&D_8009B230;
            D_800E9FF0[0].unk1A[5] = *(u8 *)&D_8009B230;
        } else {
            D_800EA02F[0] = -1;
        }
    } else {
        if (gDuel_bOpponentID >= 0) {
            D_800EA02F[0] = -1;
        }
    }
    D_8009B1C8 = (u8 *)&D_800E9FF0[D_8009B1D5];
}

void Duel_ClearHandSlots(void)
{
    u8 *entry = (u8 *)D_800EA030;
    int i = 0;
    u8 *field_09 = entry + 9;

    do {
        *(int *)(field_09 - 5) = 0;
        *(int *)entry = 0;
        *field_09 = 0;
        field_09 += sizeof(HandEntry);
        i++;
        entry += sizeof(HandEntry);
    } while (i < HAND_SIZE);
}
