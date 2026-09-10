/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 4 variables to hard registers, so it is kept here as a candidate
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

