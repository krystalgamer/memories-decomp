#include "../types.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "display_object_config.h"
#include "display_object.h"

extern DisplayObject *D_8009B1F8;
extern void func_80015DB8(DisplayObject *);

DisplayObject *func_8001D518(DisplayObject *source)
{
    DisplayObject *object = D_8009B1F8;
    if (object == 0) {
        object = func_800400AC(func_8004002C(), 2);
        func_80040468(object, 4, 3, 8, 0xB, 0x1F0);
        object->field_6A = source->field_0A;
        func_800428EC(object, 1);
        object->update = (DisplayObjectCallback)func_80015DB8;
        object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    }
    return object;
}
