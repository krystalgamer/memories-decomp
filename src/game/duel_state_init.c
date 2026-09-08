#include "../types.h"
#include "duel_side_state.h"
#include "card_constants.h"
#include "duel_hand.h"
#include "duel_grid.h"

extern u8 D_8009B1D5;
extern u16 D_8009B230;
extern u16 D_8009B234;
extern u16 D_8009B236;
extern s8 D_8009B360[];
extern s8 gDuel_bOpponentID __attribute__((section(".data")));
extern s8 D_800EA02F[];

void func_800175A0(void) {
    u16 sp[DUEL_SIDE_COUNT];
    DuelSideState *e;
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

    for (k = 0; k < DUEL_SIDE_COUNT; k++, e++) {
        j = 4;
        r = (s8 *)e + j;
        for (; j >= 0; j--, r--) {
            r[0x1A] = -1;
        }
        q = (u8 *)e;
        e->field_18 = 0;
        t = sp[k];
        e->displayed_life_points = 0;
        e->field_19 = 0;
        e->life_points.signed_value = t;
        e->max_life_points = t;
        e->field_1F = 0;
        for (m = 0; m < 13; m++) {
            *q = 0;
            q++;
        }
    }

    if (D_8009B360[0] < 0) {
        if (gDuel_bOpponentID < 0) {
            D_800E9FF0[1].field_1F = *(u8 *)&D_8009B230;
            D_800E9FF0[0].field_1F = *(u8 *)&D_8009B230;
        } else {
            D_800EA02F[0] = -1;
        }
    } else {
        if (gDuel_bOpponentID >= 0) {
            D_800EA02F[0] = -1;
        }
    }
    D_8009B1C8 = &D_800E9FF0[D_8009B1D5];
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
        field_09 += sizeof(DuelHandSlot);
        i++;
        entry += sizeof(DuelHandSlot);
    } while (i < HAND_SIZE);
}
