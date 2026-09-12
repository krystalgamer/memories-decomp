/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_field_display_objects.c.
 */
#define D_8009B34E_IN_DATA
#define D_8009B355_IN_DATA
#define D_8009B320_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#include "../types.h"
#include "../game/card_constants.h"
#include "../game/duel_side_state.h"
#include "../game/duel_grid.h"
#include "../game/ai.h"
#include "../game/display_object_api.h"
#include "../game/display_projection.h"

#include "../game/duel_card.h"
#include "../game/display_object.h"
#include "../game/display_object_config.h"
#include "../game/duel_field_display_objects.h"
#include "../game/duel_effect.h"
#include "../game/duel_selection_layout.h"
#include "../game/text_box_lifecycle.h"
#include "../game/text_box_runtime.h"
#include "../game/text_staging.h"

extern u8 D_8009B344 __attribute__((section(".data")));

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
            if (value == D_8009B1D5 &&
                D_8009B1C8->card_view_mode == 0) {
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
    func_80039A14((struct DuelEffectChannel *)box);
}
