#include "../../types.h"

#define DISPLAY_OBJECT_SPRITE_TEXTURE_U(texture) (((texture) & 0xF0) + 0x280)
#define DISPLAY_OBJECT_SPRITE_TEXTURE_V_BASE(texture) \
    ((((texture) & 0x300) >> 4) + 0xD0)

#include "../display_object_core.c"
