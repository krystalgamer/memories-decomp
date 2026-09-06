#include "../types.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

typedef struct {
    u8 *unk0;
    u8 unk4[5];
    u8 unk9;
    u8 unkA[2];
} Rec0C;

extern u8 D_800907D8[];
extern u16 D_8009B162;
extern u8 *D_8009B1B4;
extern u8 D_8009B1D5;
extern u8 D_800E9F48[];
extern Rec0C D_800EA030[];
extern u8 D_8015C424[];
extern u8 D_801A7AD8[];
extern s32 gDuel_adwCardStats[];

void func_8001B938(u8 *p) {
    u8 *r;
    u8 *e;
    u8 *b;
    u8 *g;
    s32 k;
    s32 c;
    s32 i;

    D_8009B1B4 = D_800E9F48 + D_8009B1D5 * 112;
    D_8009B1B4[0x19] = 0;
    D_8009B1B4[0x13] = 1;
    *(s16 *)(D_8009B1B4 + 0xC) = 0x74;
    D_8009B1B4[0x18] = 0;
    D_8009B1B4[0x11] = 2;
    D_8009B1B4[0x12] = 3;

    if (p[0x15] == 0) {
        b = D_8015C424;
        r = D_800EA030[*(s8 *)(p + 0xE)].unk0;
        g = b + r[0x6A] * DUEL_CARD_RECORD_SIZE + 0x48000;
        i = (gDuel_adwCardStats[*(s16 *)*(s32 *)(g + 0x36B8) - 1] >>
             CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    k = i;
        if (k >= CARD_TYPE_MAGIC) {
            if (r[0x21] != 0 || k == CARD_TYPE_TRAP) {
                D_8009B1B4[0x11] = 3;
                D_8009B1B4[0x12] = 4;
            }
        }
    }

    D_8009B1B4[0x10] = D_8009B1B4[0x11];
    c = D_800907D8[
        *(s8 *)(D_8009B1B4 + 0x10) * DUEL_FIELD_ROW_SIZE +
        D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
    ];
    e = D_801A7AD8 + c * DUEL_CARD_RECORD_SIZE;

    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++, e += DUEL_CARD_RECORD_SIZE) {
        if ((*(u16 *)(e + 0x16) & DUEL_CARD_FLAG_OCCUPIED) == 0) {
            D_8009B1B4[0xF] = i;
            break;
        }
    }

    D_8009B162 = 3;
}
