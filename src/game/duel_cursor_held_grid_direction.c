#include "../types.h"
#include "input.h"
#include "duel_cursor_grid_direction.h"
#include "duel_cursor_held_grid_direction.h"

/* Priority-encodes gInput_awPadHeld[0]'s direction bits into a small index (-1 if
   none are set; otherwise whichever bit, checked in RIGHT/DOWN/LEFT/UP
   order, was seen last wins). Reloads the volatile global on every check
   rather than caching it in a register, matching the ROM's per-check reload. */
void DuelCursor_ProcessHeldGridDirection(DuelSelectionRecord *a0) {
    s32 a1 = -1;

    if (gInput_awPadHeld[0] & PAD_DIRECTION_MASK) {
        if (gInput_awPadHeld[0] & PAD_DIRECTION_RIGHT) {
            a1 = 0;
        }
        if (gInput_awPadHeld[0] & PAD_DIRECTION_DOWN) {
            a1 = 1;
        }
        if (gInput_awPadHeld[0] & PAD_DIRECTION_LEFT) {
            a1 = 2;
        }
        if (gInput_awPadHeld[0] & PAD_DIRECTION_UP) {
            a1 = 3;
        }
    }

    DuelCursor_ProcessGridDirection(a0, a1);
}
