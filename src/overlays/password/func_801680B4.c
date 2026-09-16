#include "../../types.h"
#include "../../game/display_object.h"
#include "../../game/display_object_core.h"
#include "../../game/display_object_layout.h"

u8 *func_801680B4(s32 x, s32 y)
{
    DisplayObject *object;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, x, y, 0, 0, 0, 0x17, 0x101);
    object->flags = object->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    return (u8 *)object;
}
