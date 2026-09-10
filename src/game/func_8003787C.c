#include "../types.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"
#include "func_8003787C.h"

/* Same state_51/bit80 gating as duel_effect_state_callbacks.c's func_800378D8,
   but additionally calls func_80039FD4(D_8009B328) before clearing state_51
   when D_8009B328->display_effect_step is zero. */
void func_8003787C(DuelEffectChannel *a0) {
    u8 v1;
    MenuRecord *a0b;

    v1 = a0->state_51;
    if (!(v1 & 0x80)) {
        a0->state_51 = v1 | 0x80;
    }
    a0b = D_8009B328;
    if (a0b->display_effect_step == 0) {
        func_80039FD4((u8 *)a0b);
        a0->state_51 = 0;
    }
}
