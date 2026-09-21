#include "../types.h"
#include "display_object_layout.h"
#include "display_object_lifecycle.h"

s32 DisplayObject_MarkInitialized(DisplayObjectLifecycle *object)
{
    if ((object->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
        object->flags |= DISPLAY_OBJECT_FLAG_ALLOCATED;
        return 0;
    }
    return 1;
}
