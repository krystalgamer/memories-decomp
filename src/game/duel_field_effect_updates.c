#include "../types.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

extern volatile u16 D_8009B112 __attribute__((section(".data")));
extern u8 *D_8009B17C;
extern s16 D_8009B1D2;
extern u8 D_8009B1D5;
extern s16 D_8009B20C[4];
extern u16 D_8009B220;
extern u8 D_800907D8[];
extern DuelFieldPosition D_80090800[];
extern u8 D_801A7AD8_bytes[] asm("D_801A7AD8");
extern u8 D_801A7B64[];

extern void func_80019BA0(u8 *arg0, u8 arg1, s16 arg2, s16 arg3);
extern s32 func_80024E24(void);
extern u8 *func_8002C604(s32 arg0);
extern u8 *func_8002C68C(s32 arg0);
extern void SD_SEPlayFull(s32 arg0);

void DuelEffect_UpdateFieldMarker(void) {
    DuelCardRecord *r;
    u8 *p;
    u8 *e;
    u8 *t;
    s32 f;
    s32 c;
    s32 v;
    s32 n;
    s32 w;

    if (func_80024E24() == 0) {
        D_8009B20C[1] = -1;
    }

    f = D_8009B220;

    if ((f & 0x40) != 0) {
        if ((f & 0x20) == 0) {
            if (D_8009B17C[0x1D] != 0) {
                n = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                    DUEL_FIELD_ROW_SIZE;
                c = D_800907D8[D_8009B20C[1] + n];
                r = &D_801A7AD8[c];
                if ((*(s32 *)&r->terrain_modifier & 0x88000000) ==
                    0x88000000) {
                    func_80019BA0((u8 *)r->object, 0xC0, 0, 6);
                    r->flags &= ~DUEL_CARD_FLAG_DEFENSE_POSITION;
                }
                D_8009B220 = D_8009B220 | 0x20;
            }
        }

        if ((D_8009B17C[0x1C] & 0x80) != 0) {
            return;
        }
        D_8009B220 = D_8009B220 & 0xFF9F;
        return;
    }

    v = *(u16 *)&D_8009B20C[1] + 1;
    D_8009B20C[1] = v;
    if ((s16)v >= DUEL_FIELD_ROW_SIZE) {
        D_8009B220 = 0;
        return;
    }

    p = func_8002C604(0xC);
    t = (u8 *)D_80090800;
    e = (
        (D_8009B20C[1] + DUEL_FIELD_ROW_SIZE) *
            sizeof(DuelFieldPosition) +
        D_8009B1D5 * DUEL_FIELD_SIDE_POSITION_BYTES
    ) + t;
    w = *(u16 *)(e + 0);
    D_8009B17C = p;
    *(s16 *)(p + 2) = 0;
    *(s16 *)(p + 0) = w;
    *(s16 *)(p + 4) = *(u16 *)(e + 2);
    SD_SEPlayFull(0x20);

    D_8009B220 = D_8009B220 | 0x40;
}

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
            ] * DUEL_CARD_RECORD_SIZE + D_801A7AD8_bytes;
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
