#include "../types.h"
#include "display_object.h"
#include "display_object_helpers.h"

int DisplayObject_RunUpdateAndCheckRenderable(DisplayObject *object)
{
    /* Called as void (*)(void), with no argument, on purpose: the slot's
       declared type takes a u8 *, but the ambient argument register is what
       retail passes. See #2887. */
    void (*callback)(void) = (void (*)(void))object->update;

    if (callback != 0)
        callback();
    return ((object->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ==
            DISPLAY_OBJECT_RENDERABLE_MASK);
}
