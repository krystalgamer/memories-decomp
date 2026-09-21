#include "../types.h"
#include "display_object.h"
#include "display_object_layout.h"

void *DisplayObject_ConfigureSpriteResource(
    DisplayObject *configured,
    int field_67,
    int field_68,
    int field_69,
    int color,
    int texture
)
{
    u16 flags;

    configured->field_67 = field_67;
    configured->field_68 = field_68;
    configured->field_69 = field_69;
    configured->field_66 = color;
    *(u8 *)&configured->field_5E = color >> 16;
    *((u8 *)&configured->field_5E + 1) = color >> 8;
    configured->field_40.h.field_40 = texture & 0x3F0;
    configured->field_40.h.field_42 = (texture & 0xF) + 0xF0;
    flags = configured->flags & ~DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET;
    configured->flags = flags;
    if (texture & 0x8000) {
        configured->flags = flags | DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET;
    }
    return configured;
}
