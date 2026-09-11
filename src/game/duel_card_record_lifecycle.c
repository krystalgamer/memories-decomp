#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "card_constants.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "duel_card.h"
#include "duel_card_data_transfer.h"
#include "duel_card_layout.h"
#include "duel_card_record_lifecycle.h"
#include "duel_card_staging.h"
#include "duel_deck_card.h"
#include "duel_deck_card_data.h"
#include "duel_grid.h"
#include "duel_terrain_boost.h"
#include "func_80016778.h"
#include "util_memory.h"
#define D_80177EA4_VISIBLE
#include "../unmatched.h"

/* The combined-deck producer and one duel card record's lifecycle, in address
   order. Duel_PopulateCombinedDeckData builds the deck records and copied card
   image blocks that Duel_SetupCardRecord consumes. The remaining functions
   release/reset a record, derive its terrain boost, fill it from that deck
   data, upload its art and name strip, and create/attach the card-type icon.

   The four former sources were recorded at gcc_2_8_1_g8, gcc_2_8_1_g8_split
   and gcc_2_8_1_g0_split. Every member compiles to an identical object at
   gcc_2_8_1_g8_split, which is the profile duel_terrain_boost.h's notes on
   gDuel_bTerrain assume for Duel_GetTerrainBoost, so the unit builds there. */

void Duel_PopulateCombinedDeckData(void)
{
    u8 *dst = D_8018C2D8;
    DuelDeckCardRecord *rec = gDuel_aDeckCardRecords;
    u8 *q;
    u8 *src;
    u8 *r;
    u16 *p;
    s32 i;
    s32 id;
    s32 w;
    u16 v;

    for (i = 0; i < COMBINED_DECK_SIZE; i++) {
        q = D_8015C424 + i * 2;
        v = *(u16 *)(q + DUEL_COMBINED_DECK_CARD_IDS_OFFSET);
        rec->id = v;
        id = (s16)v;
        r = D_8015C424 + i;
        rec->flags_04 = r[DUEL_COMBINED_DECK_CARD_FLAGS_OFFSET];
        rec->index_02 = i;
        rec->data_block_index = i;

        src = D_8015C424;
        p = gDuel_awUniqueDeckCardIds;
    search:
        w = *p;
        p++;
        if (w != id) {
            src += DUEL_CARD_DATA_BLOCK_SIZE;
            goto search;
        }
        Util_CopyWords(dst, src, DUEL_CARD_DATA_BLOCK_SIZE);
        dst += DUEL_CARD_DATA_BLOCK_SIZE;
        rec++;
    }
}

void func_80024914(DuelCardRecord *object)
{
    object->flags &= ~DUEL_CARD_FLAG_OCCUPIED;
    if (object->object != 0) {
        func_8004036C(object->object);
        object->object = 0;
    }
}

void func_80024954(DuelCardRecord *object)
{
    func_80024914(object);
    object->flags = 0;
}

extern u8 gDuel_bTerrain[];
/* Same byte, distinct compiler identity: keep both address materializations. */

s32 Duel_GetTerrainBoost(s32 cardType)
{
    u8 *terrain = gDuel_bTerrainCodegenAlias;

    if (gDuel_bTerrain[0] == 0 || cardType >= CARD_TYPE_MAGIC) {
        return 0;
    }

    return gDuel_aTerrainBoost[cardType][terrain[0] - 1] * CARD_STAT_SCALE;
}

/* Two RECTs per field slot: the card art at 2 * slot and the name strip at
   2 * slot + 1. LoadImage consumes both, which is what types the table. */

u8 *Duel_SetupCardRecord(s32 a, s32 b) {
    DuelCardRecord *p;
    RECT *q;
    RECT *r;
    u8 *tb;
    RECT *base;
    DuelStagedDeckRecordBlock *g;
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

    n = b * sizeof(DuelDeckCardRecord);
    tb = D_8015C424;
    p->data = (u8 *)gDuel_aDeckCardRecords + n;
    p->table_index = idx;

    g = (DuelStagedDeckRecordBlock *)(
        tb + n + DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
    v = *(u16 *)&g->record.id;
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

extern DuelFieldPosition D_800908A0[];

/* Allocates a display object, positions it, wires up its per-frame callback,
   and selects a small icon variant for non-monster card types. */
DuelCardDisplayObject *func_80024C1C(s32 cardId, s32 x, s32 y) {
    DuelCardDisplayObject *obj;
    u32 desc;
    s32 type;
    s16 val;

    obj = func_800400AC(func_8004002C(), 0);

    obj->out_y = -0x18;
    obj->out_x = x;
    obj->field_34 = y;
    obj->field_67 = 0;
    obj->field_69 = 0;
    obj->attribute = obj->attribute | DISPLAY_OBJECT_ATTRIBUTE_8BPP;

    desc = gDuel_adwCardStats[cardId - 1];
    obj->field_10 = (void *)func_80016778;
    obj->icon_state = 0;
    obj->field_5D = 0xC0;

    type = (s32)desc >> CARD_STAT_TYPE_SHIFT;
    type &= CARD_STAT_TYPE_MASK;
    obj->field_68 = (u8)type;
    obj->icon_variant = 0;

    if (type < CARD_TYPE_MAGIC) {
        goto end;
    }
    obj->icon_state = 0x38;

    switch (type) {
        case CARD_TYPE_EQUIP:
            obj->icon_variant = 1;
            goto end;
        case CARD_TYPE_MAGIC:
            val = 1;
            break;
        case CARD_TYPE_TRAP:
            val = 2;
            break;
        case CARD_TYPE_RITUAL:
            val = 3;
            break;
        default:
            goto end;
    }
    obj->icon_variant = val;

end:
    return obj;
}

void func_80024D34(s32 a, s32 b)
{
    u8 *slot;
    s32 idx;
    u8 *tb;
    DuelCardReplayRecordBlock *replay;
    DuelCardDisplayObject *obj;

    slot = Duel_SetupCardRecord(a, b);
    idx = a;
    if ((idx & 0x80) != 0) {
        idx = (idx & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    }
    tb = D_8015C424;
    replay = (DuelCardReplayRecordBlock *)(
        tb + idx * sizeof(DuelCardRecord) + DUEL_CARD_STAGING_REPLAY_BASE_OFFSET
    );
    obj = func_80024C1C(*(s16 *)replay->record.data, D_800908A0[idx].x,
                        D_800908A0[idx].y);
    *(DuelCardDisplayObject **)slot = obj;
    obj->card_index = idx;
}
