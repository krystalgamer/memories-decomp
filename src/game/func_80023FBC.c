#define GINPUT_PAD1_HELD_IS_VOLATILE
#include "../types.h"
#include "input.h"
#include "func_80023D08.h"
#include "func_80023FBC.h"

/* gInput_wPad1Held is the first entry of the two-element pad-1/pad-2 run
   that Input_ResetPads walks down from &gInput_wPad2Held in input_pads.c;
   only the pad-1 entry is read here, so this unit takes the volatile scalar
   arm -- an array extern forces absolute lui/addiu addressing here instead
   of the target's gp-relative access. */

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
