#include "../types.h"
#include "duel_terrain_boost.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_deck_card.h"

extern u8 D_8015C424[];
/* Two RECTs per field slot: the card art at 2 * slot and the name strip at
   2 * slot + 1. LoadImage consumes both, which is what types the table. */
extern RECT D_80177EA4[];
extern u8 D_8018C2D8[];
extern u8 D_8018C7D8[];

u8 *Duel_SetupCardRecord(s32 a, s32 b) {
    DuelCardRecord *p;
    RECT *q;
    RECT *r;
    u8 *tb;
    RECT *base;
    u8 *g;
    s32 idx;
    s32 m;
    s32 off;
    s32 n;
    s32 v;

    if ((a & 0x80) != 0) {
        idx = (a & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    } else {
        idx = a;
    }

    p = &D_801A7AD8[idx];
    p->flags = DUEL_CARD_FLAG_OCCUPIED;

    if (a >= DUEL_CARD_SIDE_RECORD_COUNT && b < DECK_SIZE) {
        b += DECK_SIZE;
    }
    if ((b & 0x80) != 0) {
        b = (b & 0x7F) + DECK_SIZE;
    }

    n = b * 6;
    tb = D_8015C424;
    p->data = (u8 *)gDuel_aDeckCardRecords + n;
    p->table_index = idx;

    g = tb + n + 0x48000;
    v = *(u16 *)(g + 0x39FC);
    p->card_id = v;
    p->attack =
        (gDuel_adwCardStats[(s16)v - 1] & CARD_STAT_VALUE_MASK) *
        CARD_STAT_SCALE;
    m = idx * 2 + 1;
    p->defense =
        ((gDuel_adwCardStats[(s16)p->card_id - 1] >>
          CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
        CARD_STAT_SCALE;
    p->stat_modifier = 0;
    p->terrain_modifier =
        Duel_GetTerrainBoost(
            (gDuel_adwCardStats[(s16)p->card_id - 1] >>
             CARD_STAT_TYPE_SHIFT) &
            CARD_STAT_TYPE_MASK);

    base = D_80177EA4;
    q = &base[idx * 2];
    off = ((DuelDeckCardRecord *)p->data)->data_block_index *
          DUEL_CARD_DATA_BLOCK_SIZE;
    q->w = 0x14;
    q->h = 0x20;
    q->x = (idx % DUEL_FIELD_ROW_SIZE) * 0x14 + 0x380;
    q->y = (idx / DUEL_FIELD_ROW_SIZE) * 0x20;
    LoadImage(q, (u32 *)(D_8018C2D8 + off));

    r = &base[m];
    r->x = 0x380;
    r->y = idx + 0xE0;
    r->w = 0x40;
    r->h = 1;
    LoadImage(r, (u32 *)(D_8018C7D8 + off));

    return (u8 *)p;
}
