#include "../types.h"
#include "duel_card.h"

typedef struct {
    u8 pad_00[0xF];
    s8 col;
    s8 row;
} Cursor;

extern volatile u8 D_800907D8[];
extern u8 D_8009B1D5;
extern s32 Duel_CalcGuardianStarBonus(DuelCardRecord *, DuelCardRecord *);

s32 func_80023090(Cursor *cursor_a, Cursor *cursor_b)
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
