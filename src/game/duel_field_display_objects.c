#define D_8009B34E_IN_DATA
#define D_8009B355_IN_DATA
#define D_8009B320_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#include "../types.h"
#include "card_constants.h"
#include "duel_side_state.h"
#include "duel_grid.h"
#include "ai.h"
#include "display_object_api.h"
#include "display_projection.h"

#include "duel_card.h"
#include "display_object.h"
#include "display_object_config.h"
#include "duel_field_display_objects.h"
#include "duel_effect.h"
#include "duel_selection_layout.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_staging.h"

extern u8 D_8009B344 __attribute__((section(".data")));

s32 func_80023090(DuelFieldCursor *cursor_a, DuelFieldCursor *cursor_b)
{
    register volatile u8 *grid asm("$9") = D_800907D8;
    register DuelCardRecord *records asm("$7") = D_801A7AD8;
    register s32 page asm("$6");
    register s32 offset_a asm("$4");
    s32 row_b;
    s32 index_a;
    s32 index_b;
    s32 slot_a;
    s32 slot_b;
    s32 side;
    s32 order;

    row_b = cursor_b->row;
    index_a = cursor_a->row * DUEL_FIELD_ROW_SIZE + cursor_a->col;
    side = D_8009B1D5;
    page = side * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    slot_a = grid[index_a + page];
    offset_a = slot_a * sizeof(DuelCardRecord);
    index_b = row_b * DUEL_FIELD_ROW_SIZE + cursor_b->col;
    slot_b = grid[index_b + page];

    order = Duel_CalcGuardianStarBonus(
        (DuelCardRecord *)((u8 *)records + offset_a),
        &records[slot_b]
    );

    if (order == 0) {
        return 4;
    }
    if (order < 0) {
        return 1;
    }
    return 6;
}

void func_80023144(DuelFieldDisplaySource *source, s32 index)
{
    DuelCardRecord *record = &D_801A7AD8[index];
    DuelEffectChannel *box;
    s32 *table;
    s32 style;
    s32 id;
    s32 value;
    s32 stats;

    D_8009B34E = 0;
    D_8009B355 = 0;
    style = 0x50;
    if (record->flags & 0x8000) {
        table = gDuel_adwCardStats;
        id = (s16)record->card_id;
        stats = *(s32 *)((s32)table + ((id - 1) << 2));
        D_8009B34E = 1;
        gDuel_wSelectedCardID = id;
        if (((stats >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) <
            CARD_TYPE_MAGIC) {
            if (record->flags & 0x200) {
                D_8009B344 =
                    (stats >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
            } else {
                D_8009B344 =
                    (stats >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
            }
            D_8009B344 = D_8009B344 + 0x17;
            value = Duel_CalcCardStats(record);
            D_801D5608[0].card_stats.attack = (s16)value;
            D_801D5608[0].card_stats.defense = value >> 16;
        } else {
            style = 0x51;
        }
        if (index % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE) {
            register s32 two asm("$4") = 2;

            D_8009B34E = two;
            if (record->flags & 0x1000) {
                D_8009B34E = 3;
            }
            value = index >= DUEL_CARD_SIDE_RECORD_COUNT;
            if (value == D_8009B1D5 && D_8009B1C8->field_1F == 0) {
                D_8009B34E = two;
            }
        } else {
            if (record->flags & 0x2000) {
                D_8009B34E = 0;
            }
        }
    }
    if (source->table_index == 3) {
        s32 rank;
        TextStagingValues *dst = D_801D5608;

        style += 4;
        rank = D_800E9FF0[D_8009B1D5 ^ 1].field_19;
        dst->card_stats.rank = rank;
        if (rank != 0) {
            if (rank < 0 || rank > 3) {
                dst->card_stats.rank = 3;
            }
            D_8009B355 = 1;
        }
        if (D_8009B34E != 0) {
            D_8009B355 = D_8009B355 | 2;
            D_8009B320 = func_80023090(
                (DuelFieldCursor *)source,
                (DuelFieldCursor *)
                    &D_800E9F48[D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE]
            );
        }
    }
    if (source->table_index == 2 && source->field_18 != 0) {
        s32 rank;
        register TextStagingValues *dst asm("$4");
        s32 side;

        side = D_8009B1D5;
        if (source->y < 2) {
            side = side ^ 1;
        }
        dst = D_801D5608;
        rank = D_800E9FF0[side].field_19;
        dst->card_stats.rank = rank;
        if (rank != 0) {
            if (rank < 0 || rank > 3) {
                dst->card_stats.rank = 3;
            }
            D_8009B355 = 1;
        }
        style += 2;
    }
    box = TextBox_Create(
        source->field_14, style,
        *(s16 *)&source->field_00->field_30.h.field_30 + 0x10,
        *(s16 *)&source->field_00->field_30.h.field_32 + source->field_16,
        0x120, 0x40
    );
    box->field_59 = *(u8 *)&source->field_00->field_16 + 1;
    func_80039A14((u8 *)box);
}

void func_8002348C(DuelFieldDisplaySource *source)
{
    u8 *table = D_800907D8;
    s32 index =
        source->y * DUEL_FIELD_ROW_SIZE + source->x +
        D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;

    func_80023144(source, table[index]);
}

void func_800234E4(DuelFieldDisplaySource *source)
{
    s32 index;
    DisplayObject *object;
    DuelFieldPosition *position;

    index = source->y * DUEL_FIELD_ROW_SIZE + source->x;
    object = func_800400AC(func_8004002C(), 2);
    func_80040468(
        (u8 *)object,
        4,
        3,
        source->table_index + D_8009B1D5 * 4,
        0x1F,
        0x100
    );

    {
        u8 *base = (u8 *)D_80090800;
        s32 offset = index * sizeof(DuelFieldPosition) +
            D_8009B1D5 * DUEL_FIELD_SIDE_POSITION_BYTES;

        position = (DuelFieldPosition *)(base + offset);
    }
    object->position.h.field_28 = position->x;
    object->position.h.field_2A = position->y;
    object->flags = object->flags | 0x28;
    object->update = (DisplayObjectCallback)func_80015D18;
    source->object = object;
}
