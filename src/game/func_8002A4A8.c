/*
 * Keep one row snapshot: `func_8002A4A8`
 *
 * The 440-byte library-cursor motion setup matches under the existing uniform
 * `gcc_2_8_1_g8_split` profile with no register pins or inline assembly.
 * The shared `LibraryMotionState` already supplies the complete offsets and
 * signed integer/unsigned fractional halves missing from the terminal
 * incomplete-type attempts. The reference's two copies of its `col` argument leave the
 * unpinned entry move after the unchanged-cell guard, changing ten instruction
 * positions. Keeping one row snapshot through that guard and the Y-coordinate
 * calculation restores retail's initial move into `$t1` without pinning it.
 * The reference's argument names are reversed relative to the confirmed grid
 * symbols: the first argument drives X/`gCardGrid_bCursorColumn`, and the second
 * drives Y/`gCardGrid_bCursorRow`. The matching source uses those existing
 * `card_grid.h` declarations, without adding duplicate address-named globals.
 *
 * The same signed `delta` is assigned and shifted in place for both 16.16
 * velocity divisions. The unsigned fractional fields preserve the positive
 * `0x8000` constant and its `ori` instruction with the ordinary supported
 * profile; no additional assembler flags are needed. Same-cell requests return
 * without altering an active animation. Otherwise, the original frame count,
 * target coordinates, half-pixel initial fractions and active flag are retained.
 * The historical canonical rows and six-row terminal refinement history remain
 * intact, followed by one post-terminal resolution with the new source evidence.
 */
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
