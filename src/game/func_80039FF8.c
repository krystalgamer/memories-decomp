#include "../types.h"
#include "display_effect_lifecycle.h"

void func_80039FF8(DisplayEffectState *object)
{
    u8 flags = object->field_32;

    if ((flags & 3) == 0) {
        object->field_32 = flags | 0x10;
        object->state = 0;
    }
}
