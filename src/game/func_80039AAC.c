#include "../types.h"
#include "display_object_fade.h"

int func_80039AAC(u8 *object)
{
    u8 flags = object[0x13];
    if ((flags & DISPLAY_OBJECT_FADE_FLAG_INITIALIZED) == 0) {
        object[0x13] = flags | DISPLAY_OBJECT_FADE_FLAG_INITIALIZED;
        return 0;
    }
    return 1;
}
