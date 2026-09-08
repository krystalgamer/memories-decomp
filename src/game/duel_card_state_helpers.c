#include "../types.h"
#include "ai_constants.h"
#include "duel_card_layout.h"

extern unsigned char D_801AB00C[];
extern unsigned char D_8009B1D5;
extern void func_80027DF8(void *, int);

void func_80028220(void)
{
    func_80027DF8(D_801AB00C, D_8009B1D5);
    func_80027DF8(D_801AB00C + AI_ACTIVE_CARD_SIDE_BYTE_STRIDE,
                 D_8009B1D5 ^ 1);
}

int func_80028260(int value)
{
    if (value & 0x80) {
        return (value & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    }
    return value;
}
