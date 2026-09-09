#include "../types.h"
#include "duel_effect.h"
#include "mem_card.h"
#include "../unmatched.h"
extern void func_8003E490(void);

void func_8003E7D4(void)
{
    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        D_8009B3EB = 0;
    }
    func_8003E490();
}

void func_8003E80C(void)
{
    if (!(D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        D_8009B3EB = 1;
        gMemCard_wDialogFlags |= 0x200;
    }
    func_8003E490();
}
