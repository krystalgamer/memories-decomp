/* The combined-deck request and producer and one duel card record's
   lifecycle, in address order. Duel_RequestCombinedDeckData sorts and
   de-duplicates the combined deck's card ids and requests their card data;
   Duel_PopulateCombinedDeckData then builds the deck records and copied
   card image blocks that Duel_SetupCardRecord consumes. The remaining
   functions release/reset a record, derive its terrain boost, fill it from
   that deck data, upload its thumbnail and CLUT, and create/attach the
   world-space field-card object.

   The record-lifecycle sources were recorded at gcc_2_8_1_g8,
   gcc_2_8_1_g8_split and gcc_2_8_1_g0_split, and the request was its own
   gcc_2_8_1_g8_split unit. Every member compiles to an identical object at
   gcc_2_8_1_g8_split, which is the profile duel_terrain_boost.h's notes on
   gDuel_bTerrain assume for Duel_GetTerrainBoost, so the unit builds there. */

#define DUEL_SCREEN_TABLES_TYPED_POSITIONS
#define DUEL_TERRAIN_AS_ARRAY

#include "../types.h"
#include "duel_deck_card_data.h"
#include "../psyq/qsort.h"
#include "card_constants.h"
#include "duel_card_data_transfer.h"
#include "duel_deck_card.h"
#include "duel_card_staging.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "util_memory.h"
#include "util_compare_s16.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "duel_card_record_lifecycle.h"
#include "duel_screen_tables.h"
#include "duel_terrain_boost.h"
#include "display_object_set_resource_variant_from_sign.h"
#include "../unmatched.h"

void Duel_RequestCombinedDeckData(void)
{
    u16 *source = gDuel_awCombinedDeckCardIds;
    u16 *output;
    FileTransferDescriptor *result;
    s32 i;
    s32 previous;
    s32 first_id;
    u8 *table;
    s32 value;

    Util_CopyWords(
        (u8 *)source,
        (u8 *)source - DUEL_DECK_ID_BUFFER_STRIDE,
        COMBINED_DECK_SIZE * sizeof(u16)
    );
    qsort(source, COMBINED_DECK_SIZE, sizeof(u16), (int (*)())Util_CompareS16);

    output = source + DUEL_DECK_ID_BUFFER_STRIDE / sizeof(u16);
    previous = 0;
    for (i = 0; i < COMBINED_DECK_SIZE; i++) {
        value = *source;
        if (value != previous) {
            *output = value;
            previous = value;
            output++;
        }
        source++;
    }

    *output = DUEL_CARD_ID_LIST_END;
    table = gDuel_abCardStagingBuffer;
    first_id =
        ((u16 *)table)[DUEL_UNIQUE_DECK_CARD_IDS_OFFSET / sizeof(u16)];
    result = File_TryRequestAsyncTransfer(
        0, (u8 *)0, first_id - 1, previous - first_id + 1,
        Duel_StepCardDataTransfer, 0, 0
    );
    result->callback_data = gDuel_awUniqueDeckCardIds;
    result->position = (u32)table;
    gFile_dwTransferState_abs =
        result->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}

void Duel_PopulateCombinedDeckData(void)
{
    DuelCardThumbnailBlock *dst = gDuel_aCardThumbnailBlocks;
    DuelDeckCardRecord *rec = gDuel_aDeckCardRecords;
    DuelCombinedDeckInput *input;
    u8 *src;
    u16 *p;
    s32 i;
    s32 id;
    s32 w;
    u16 v;

    for (i = 0; i < COMBINED_DECK_SIZE; i++) {
        /* Keep this binding in the loop so GCC reuses the staging base below. */
        input = (DuelCombinedDeckInput *)gDuel_abCardStagingBuffer;
        v = input->card_ids[i];
        rec->id = v;
        id = (s16)v;
        rec->pre_shuffle_index = input->pre_shuffle_indices[i];
        rec->deck_index = i;
        rec->data_block_index = i;

        src = gDuel_abCardStagingBuffer;
        p = gDuel_awUniqueDeckCardIds;
    search:
        w = *p;
        p++;
        if (w != id) {
            src += DUEL_CARD_DATA_BLOCK_SIZE;
            goto search;
        }
        Util_CopyWords((u8 *)dst, src, sizeof(*dst));
        dst++;
        rec++;
    }
}

void DuelCard_DeactivateRecord(DuelCardRecord *object)
{
    object->flags &= ~DUEL_CARD_FLAG_OCCUPIED;
    if (object->object != 0) {
        DisplayObject_ReleaseIfPresent(object->object);
        object->object = 0;
    }
}

void DuelCard_RemoveFromField(DuelCardRecord *object)
{
    DuelCard_DeactivateRecord(object);
    object->flags = 0;
}

/* Same byte, distinct compiler identity: keep both address materializations. */

s32 Duel_GetTerrainBoost(s32 cardType)
{
    u8 *terrain = gDuel_bTerrainCodegenAlias;

    if (gDuel_bTerrain[0] == 0 || cardType >= CARD_TYPE_MAGIC) {
        return 0;
    }

    return gDuel_aTerrainBoost[cardType][terrain[0] - 1] * CARD_STAT_SCALE;
}

/* Two RECTs per live slot: the thumbnail at 2 * slot and the CLUT at
   2 * slot + 1. LoadImage consumes both, which is what types the table. */

DuelCardRecord *Duel_SetupCardRecord(s32 a, s32 b) {
    DuelCardRecord *p;
    RECT *q;
    RECT *r;
    u8 *tb;
    RECT *base;
    DuelStagedDeckRecordBlock *g;
    s32 idx;
    s32 m;
    s32 block_index;
    s32 n;
    s32 v;

    if ((a & 0x80) != 0) {
        idx = (a & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    } else {
        idx = a;
    }

    p = &gDuel_aCardRecords[idx];
    p->flags = DUEL_CARD_FLAG_OCCUPIED;

    if (a >= DUEL_CARD_SIDE_RECORD_COUNT && b < DECK_SIZE) {
        b += DECK_SIZE;
    }
    if ((b & 0x80) != 0) {
        b = (b & 0x7F) + DECK_SIZE;
    }

    n = b * sizeof(DuelDeckCardRecord);
    tb = gDuel_abCardStagingBuffer;
    p->data = &gDuel_aDeckCardRecords[b];
    p->table_index = idx;

    g = (DuelStagedDeckRecordBlock *)(
        tb + n + DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
    v = (u16)g->record.id;
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

    base = gDuel_aCardUploadRects;
    q = &base[idx * 2];
    block_index = ((DuelDeckCardRecord *)p->data)->data_block_index;
    q->w = 0x14;
    q->h = 0x20;
    q->x = (idx % DUEL_FIELD_ROW_SIZE) * 0x14 + 0x380;
    q->y = (idx / DUEL_FIELD_ROW_SIZE) * 0x20;
    LoadImage(q, (u32 *)gDuel_aCardThumbnailBlocks[block_index].pixels);

    r = &base[m];
    r->x = 0x380;
    r->y = idx + 0xE0;
    r->w = 0x40;
    r->h = 1;
    LoadImage(r, (u32 *)(D_8018C7D8 +
                        block_index * DUEL_CARD_DATA_BLOCK_SIZE));

    return p;
}

/* Creates the world-space field-card object: x/-24/z translation,
   facing callback and type-selected front UV/CLUT for the 3D path. */
DuelCardDisplayObject *Duel_CreateFieldCardObject(s32 cardId, s32 x, s32 y) {
    DuelCardDisplayObject *obj;
    u32 desc;
    s32 type;
    s16 val;

    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 0);

    obj->out_y = -0x18;
    obj->out_x = x;
    obj->field_34 = y;
    obj->deck_number_marker = 0;
    obj->field_69 = 0;
    obj->attribute = obj->attribute | DISPLAY_OBJECT_ATTRIBUTE_8BPP;

    desc = gDuel_adwCardStats[cardId - 1];
    obj->field_10.callback = DisplayObject_SetResourceVariantFromSign;
    obj->icon_state = 0;
    obj->field_5D = 0xC0;

    type = (s32)desc >> CARD_STAT_TYPE_SHIFT;
    type &= CARD_STAT_TYPE_MASK;
    obj->card_type = (u8)type;
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

void Duel_SetupFieldCardFromDeck(s32 a, s32 b)
{
    DuelCardRecord *slot;
    s32 idx;
    u8 *tb;
    DuelCardReplayRecordBlock *replay;
    DuelCardDisplayObject *obj;

    slot = Duel_SetupCardRecord(a, b);
    idx = a;
    if ((idx & 0x80) != 0) {
        idx = (idx & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    }
    tb = gDuel_abCardStagingBuffer;
    replay = (DuelCardReplayRecordBlock *)(
        (u8 *)&((DuelCardRecord *)tb)[idx] +
        DUEL_CARD_STAGING_REPLAY_BASE_OFFSET
    );
    obj = Duel_CreateFieldCardObject(
        ((DuelDeckCardRecord *)replay->record.data)->id,
        gDuel_aCardSlotGroundPositions[idx].x,
        gDuel_aCardSlotGroundPositions[idx].y);
    slot->object = obj;
    obj->card_index = idx;
}
