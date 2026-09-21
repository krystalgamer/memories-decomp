#include "../types.h"
#include "display_effect_lifecycle.h"

s32 func_80039F1C(DisplayEffectState *object)
{
    u8 state = object->state;

    if ((state & 0x80) == 0) {
        object->state = state | 0x80;
        return 0;
    }
    return 1;
}
