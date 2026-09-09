#include "../types.h"
#include "func_80032B38.h"

s32 func_80032B38(BuildDeckTransitionState *state)
{
    u16 flags = state->state;
    if ((flags & 0x8000) == 0) {
        state->state = flags | 0x8000;
        return 0;
    }
    return 1;
}
