#include "../types.h"
#include "display_object.h"
#include "sound.h"
#include "../unmatched.h"
#include "func_8002A2F4.h"
#include "func_8002A3CC.h"

s32 func_8002A3CC(void)
{
    LibraryMotionState *object = &D_800EA1E8;

    if (!object->active)
        return object->active;
    {
        s32 position;
        position = ((s32)object->x << 16) | object->x_fraction;
        position += object->velocity_x;
        object->x_fraction = position;
        object->x = position >> 16;
        position = ((s32)object->y << 16) | object->y_fraction;
        position += object->velocity_y;
        object->y = position >> 16;
        object->y_fraction = position;
    }
    if (--object->frames == 0) {
        object->active = 0;
        object->x = object->rest_x;
        object->y = object->rest_y;
        SD_SEPlayFull(53);
        func_8002A2F4((u8 *)object);
    }
    object->render->field_30.h.field_30 = object->x;
    object->render->field_30.h.field_32 = object->y;
    return object->active;
}
