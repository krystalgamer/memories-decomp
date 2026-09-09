#include "../types.h"
#include "duel_side_state.h"
#include "duel_card_layout.h"
#include "func_80027DF8.h"

extern AiActiveCard D_801AB00C[];

void func_80028220(void)
{
    func_80027DF8(D_801AB00C, D_8009B1D5);
    func_80027DF8(D_801AB00C + AI_ACTIVE_CARD_SIDE_SLOT_STRIDE,
                 D_8009B1D5 ^ 1);
}

int func_80028260(int value)
{
    if (value & 0x80) {
        return (value & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    }
    return value;
}
