#include "../types.h"
#include "func_80019B2C.h"
#include "func_80019BA0.h"

void func_80019BA0(DisplayObject *arg0, u8 arg1, s16 arg2, s16 arg3)
{
    arg0->field_6C = 1;
    arg0->field_20.b.field_21 = arg1;
    *(s16 *)&arg0->position.h.field_28 = arg2;
    *(s16 *)&arg0->position.h.field_2A = arg3;
    arg0->update = (DisplayObjectCallback)func_80019B2C;
    arg0->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
}
