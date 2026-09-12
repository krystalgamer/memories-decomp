#include "../types.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "func_8002A3CC.h"
#include "func_80029590.h"

void func_80029590(void)
{
    s32 i = 0;
    LibraryMotionState *state = &D_800EA1E8;
    for (; i < 8; i++) {
        DisplayObject *object = func_800400AC(func_8004002C(), 2);
        func_800404CC(object, 8, (i >> 1) * 178 + 8,
                      0, 3, i, 25, 262);
        if (i & 1)
            *(s16 *)&object->field_30.h.field_30 = 168;
        object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        object->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        func_8004293C(object);
        func_800428EC((u8 *)object, 0);
        state->slots[i] = object;
    }
}
