#include "../types.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_layout.h"

#define DISPLAY_OBJECT_FROM_FIELD_6C(field) \
    ((DisplayObject *)((field) - 0x6C))

void *func_80042B40(s32 value)
{
    u8 *object = (u8 *)D_800EFE48;
    s32 count = DISPLAY_OBJECT_POOL_CAPACITY;
    u8 *field = (u8 *)&((DisplayObject *)object)->field_6C;

    do {
        if ((DISPLAY_OBJECT_FROM_FIELD_6C(field)->flags &
             DISPLAY_OBJECT_FLAG_ALLOCATED) &&
            ((DISPLAY_OBJECT_FROM_FIELD_6C(field)->field_6C & 0xF) == value))
            return object;
        field += DISPLAY_OBJECT_RECORD_SIZE;
        count--;
        object += DISPLAY_OBJECT_RECORD_SIZE;
    } while (count != 0);
    return 0;
}
