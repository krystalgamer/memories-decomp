#define GINPUT_PAD1_HELD_IS_VOLATILE
#include "../types.h"
#include "input.h"
#include "func_80023D08.h"
#include "func_80023FBC.h"

/* Same 2-element history slot as reset_history_ring_and_flags.c's
   gInput_wPad1Held[2]; only index 0 is read here (declared scalar -- an array
   extern forces absolute lui/addiu addressing here instead of the target's
   gp-relative access). */

/* Priority-encodes gInput_wPad1Held's direction bits into a small index (-1 if
   none are set; otherwise whichever bit, checked in RIGHT/DOWN/LEFT/UP
   order, was seen last wins). Reloads the volatile global on every check
   rather than caching it in a register, matching the ROM's per-check reload. */
void func_80023FBC(GridCursor *a0) {
    s32 a1 = -1;

    if (gInput_wPad1Held & PAD_DIRECTION_MASK) {
        if (gInput_wPad1Held & PAD_DIRECTION_RIGHT) {
            a1 = 0;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_DOWN) {
            a1 = 1;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_LEFT) {
            a1 = 2;
        }
        if (gInput_wPad1Held & PAD_DIRECTION_UP) {
            a1 = 3;
        }
    }

    func_80023D08(a0, a1);
}
