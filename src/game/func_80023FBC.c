#include "../types.h"
#include "input.h"

/* Same 2-element history slot as reset_history_ring_and_flags.c's
   D_8009B3A4[2]; only index 0 is read here (declared scalar -- an array
   extern forces absolute lui/addiu addressing here instead of the target's
   gp-relative access). */
extern volatile u16 D_8009B3A4;

struct Obj {
    u8 pad[25];
    u8 field25;
};

extern void func_80023D08(struct Obj *a0, s32 a1);

/* Priority-encodes D_8009B3A4's direction bits into a small index (-1 if
   none are set; otherwise whichever bit, checked in RIGHT/DOWN/LEFT/UP
   order, was seen last wins). Reloads the volatile global on every check
   rather than caching it in a register, matching the ROM's per-check reload. */
void func_80023FBC(struct Obj *a0) {
    s32 a1 = -1;

    if (D_8009B3A4 & PAD_DIRECTION_MASK) {
        if (D_8009B3A4 & PAD_DIRECTION_RIGHT) {
            a1 = 0;
        }
        if (D_8009B3A4 & PAD_DIRECTION_DOWN) {
            a1 = 1;
        }
        if (D_8009B3A4 & PAD_DIRECTION_LEFT) {
            a1 = 2;
        }
        if (D_8009B3A4 & PAD_DIRECTION_UP) {
            a1 = 3;
        }
    }

    func_80023D08(a0, a1);
}
