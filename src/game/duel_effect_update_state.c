#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_tables.h"

extern u8 D_8009B254, D_8009B24A, D_8009B248;
s32 DuelEffect_UpdateState(void) {
    u8 v = D_8009B254;
    if (v == 0) return 0;
    if ((v & 0x80) == 0) {
        D_8009B24A = v;
        D_8009B254 = v | 0x80;
        D_8009B248 = 0;
        return 1;
    }
    if (v & 0x40) {
        D_8009B254 = 0;
        return 0;
    }
    gDuelEffect_apfnStateHandler[D_8009B24A]();
    return 1;
}
