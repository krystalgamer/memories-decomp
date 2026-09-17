#include "../types.h"
#include "func_8002E3FC.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"

void *func_8002E3FC(void) {
    DisplayObject *object =
        DisplayObject_AcquireSlot((s32)DisplayObject_FindFreeSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object,0x10,0xB0,0,0,0,0xD,0x100);
    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_SelectOrderingTable1(object);
    return object;
}
