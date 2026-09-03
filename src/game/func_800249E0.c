#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"

extern u8 D_8015C424[];
extern u8 D_80177EA4[];
extern u8 D_8018C2D8[];
extern u8 D_8018C7D8[];
extern u8 gDuel_aPlayerHand[];
extern s32 gDuel_adwCardStats[];
extern int Duel_GetTerrainBoost();
extern int func_8007F978();

u8 *func_800249E0(s32 a, s32 b) {
    DuelCardRecord *p;
    u8 *q;
    u8 *r;
    u8 *tb;
    u8 *base;
    u8 *g;
    s32 idx;
    s32 m;
    s32 off;
    s32 n;
    s32 v;

    if ((a & 0x80) != 0) {
        idx = (a & 0x7F) + 0xF;
    } else {
        idx = a;
    }

    p = &D_801A7AD8[idx];
    p->flags = 0x8000;

    if (a >= 0xF && b < DECK_SIZE) {
        b += DECK_SIZE;
    }
    if ((b & 0x80) != 0) {
        b = (b & 0x7F) + DECK_SIZE;
    }

    n = b * 6;
    tb = D_8015C424;
    p->data = gDuel_aPlayerHand + n;
    p->table_index = idx;

    g = tb + n + 0x48000;
    v = *(u16 *)(g + 0x39FC);
    p->card_id = v;
    p->attack = (gDuel_adwCardStats[(s16)v - 1] & 0x1FF) * 0xA;
    m = idx * 2 + 1;
    p->defense =
        ((gDuel_adwCardStats[(s16)p->card_id - 1] >> 9) & 0x1FF) * 0xA;
    p->stat_modifier = 0;
    p->terrain_modifier =
        Duel_GetTerrainBoost((gDuel_adwCardStats[(s16)p->card_id - 1] >> 26) & 0x1F);

    base = D_80177EA4;
    q = base + idx * 0x10;
    off = *((u8 *)p->data + 3) * 0x580;
    *(s16 *)(q + 4) = 0x14;
    *(s16 *)(q + 6) = 0x20;
    *(s16 *)(q + 0) = (idx % 5) * 0x14 + 0x380;
    *(s16 *)(q + 2) = (idx / 5) * 0x20;
    func_8007F978(q, D_8018C2D8 + off);

    r = base + m * 8;
    *(s16 *)(r + 0) = 0x380;
    *(s16 *)(r + 2) = idx + 0xE0;
    *(s16 *)(r + 4) = 0x40;
    *(s16 *)(r + 6) = 1;
    func_8007F978(r, D_8018C7D8 + off);

    return (u8 *)p;
}
