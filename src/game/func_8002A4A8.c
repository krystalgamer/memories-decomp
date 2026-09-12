#include "../types.h"
#include "card_grid.h"
#include "func_8002A3CC.h"

void func_8002A4A8(s32 column, s32 row, s32 frames)
{
    s32 target_row = row;
    LibraryMotionState *state = &D_800EA1E8;
    s32 x;
    s32 delta;

    if (column == gCardGrid_bCursorColumn && target_row == gCardGrid_bCursorRow) {
        return;
    }
    gCardGrid_bCursorColumn = column;
    gCardGrid_bCursorRow = target_row;
    state->frames = frames;
    if (column >= 10) {
        x = (column % 10) * 14 + 0xAE;
    } else {
        x = (column % 10) * 14 + 0xE;
    }
    {
        s32 velocity_x;
        s32 velocity_y;
        s32 bank = target_row / 10;
        s32 y;

        delta = (s16)x - state->x;
        delta <<= 16;
        velocity_x = delta / frames;
        y = bank * 178 + (target_row % 10) * 16 + 0xE;
        delta = (s16)y - state->y;
        delta <<= 16;
        velocity_y = delta / frames;
        state->rest_x = (s16)x;
        state->rest_y = (s16)y;
        state->y_fraction = 0x8000;
        state->x_fraction = 0x8000;
        delta = 1;
        state->active = delta;
        state->velocity_x = velocity_x;
        state->velocity_y = velocity_y;
    }
}
