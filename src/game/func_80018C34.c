#include "../types.h"
#include "display_object_interpolation.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_lifecycle.h"
#include "func_80018C34.h"

void func_80018C34(DisplayObject *o)
{
    if (!func_80042B98((DisplayObjectLifecycle *)o)) {
        o->field_2C.h.field_2C = 0x400 / o->field_60;
        o->position.h.field_28 = o->field_30.h.field_30 - 0x140;
        DisplayObject_SavePosition((DisplayObjectSnapshot *)o);
        o->field_60 = -0x400;
    }
    Widget_SlideSine((DisplayObjectPosition *)o, (s16)o->position.h.field_28,
                  (s16)o->field_30.h.field_32, o->field_60);
    o->field_60 += o->field_2C.h.field_2C;
    if (o->field_60 >= 0) {
        o->update = 0;
        o->field_6C = 0;
        o->field_30.h.field_30 = (s16)o->position.h.field_28;
    }
}
