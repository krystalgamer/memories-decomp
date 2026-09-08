#include "../types.h"
#include "display_object_layout.h"
#include "display_object_api.h"

typedef struct {
    u8 pad_00[0x0C];
    u8 value_0C;
    u8 value_0D;
    u8 value_0E;
    u8 pad_0F[0x51];
    s16 step;
    u8 pad_62[0x0A];
    u8 flags;
} DisplayObjectLifecycle;

s32 func_80042B98(DisplayObjectLifecycle *object)
{
    if ((object->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
        object->flags |= DISPLAY_OBJECT_FLAG_ALLOCATED;
        return 0;
    }
    return 1;
}

void func_80042BC0(DisplayObjectLifecycle *object)
{
    s32 value = object->value_0C - object->step;

    if (value > 0) {
        object->value_0E = value;
        object->value_0D = value;
        object->value_0C = value;
    } else {
        func_8004036C(object);
    }
}
