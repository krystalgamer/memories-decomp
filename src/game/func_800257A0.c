#include "../types.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

extern u8 D_800907D8[];
extern volatile u16 D_8009B112 __attribute__((section(".data")));
extern s16 D_8009B1D2;
extern u8 D_8009B1D5;
extern u16 D_8009B220;
extern u8 D_801A7AD8[];
extern u8 D_801A7B64[];

s32 func_80024E24(void);
u8 *func_8002C68C(s32 arg0);

void func_800257A0(void) {
    u8 *e;
    u8 *p;
    u8 *q;
    s32 i;
    s32 f;
    s32 g;

    if (func_80024E24() == 0) {
        func_8003FF88(0x8020);
        return;
    }

    f = D_8009B220;

    if ((f & 0x20) == 0) {
        if ((D_8009B112 & 0x4000) == 0) {
            return;
        }
        D_8009B220 = f | 0x20;
        e = func_8002C68C(0x11);
        if (D_8009B1D2 == 0x149) {
            g = D_8009B220;
            *(s16 *)(e + 0x1A) = 1;
            D_8009B220 = g | 0x40;
        }
        *(s16 *)(e + 0) = 0;
        *(s16 *)(e + 2) = 0;
        *(s16 *)(e + 4) = 0;
        return;
    }

    if ((D_8009B112 & 0x4000) != 0) {
        return;
    }

    if ((f & 0x40) != 0) {
        i = DUEL_FIELD_ROW_SIZE;
        while (1) {
            p = D_800907D8[
                i + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
            ] * DUEL_CARD_RECORD_SIZE + D_801A7AD8;
            if ((*(u16 *)(p + 0x16) & DUEL_CARD_FLAG_OCCUPIED) != 0 &&
                (*(u8 **)p)[0x68] == 0) {
                func_80024954(p);
            }
            i++;
            if (i >= DUEL_FIELD_SIDE_ZONE_COUNT) {
                break;
            }
        }
    } else {
        p = D_801A7B64;
        q = p + DUEL_CARD_SIDE_RECORD_COUNT * DUEL_CARD_RECORD_SIZE;
        i = 0;
        do {
            if ((*(u16 *)(p + 0x16) & DUEL_CARD_FLAG_OCCUPIED) != 0) {
                func_80024954(p);
            }
            if ((*(u16 *)(q + 0x16) & DUEL_CARD_FLAG_OCCUPIED) != 0) {
                func_80024954(q);
            }
            i++;
            p += DUEL_CARD_RECORD_SIZE;
            q += DUEL_CARD_RECORD_SIZE;
        } while (i < DUEL_FIELD_SIDE_ZONE_COUNT);
    }

    D_8009B220 = 0;
}
