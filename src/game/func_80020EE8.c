#include "../types.h"
#include "display_object_lifecycle.h"
#include "display_object_layout.h"
#include "display_object_api.h"

typedef struct {
    u8 pad_00[8];
    u16 flags;
    u8 pad_0A[0x17];
    u8 field_21;
} Object;

void func_80020EE8(Object *object)
{
    if (func_80042B98((DisplayObjectLifecycle *)object) == 0) {
        object->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
    }
    object->field_21 -= 2;
    if (object->field_21 < 0xC0) {
        func_8004036C(object);
    }
}
