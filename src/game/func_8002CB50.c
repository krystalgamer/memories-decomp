#include "../types.h"
#include "display_object_work_slots.h"

void func_8002CB50(s32 *destination)
{
    s32 i;
    DisplayObject **source;

    i = 0;
    source = D_800E9EF0;
    while (i < DISPLAY_OBJECT_WORK_SLOT_COUNT) {
        *destination++ = (s32)*source++;
        i++;
    }
    *destination = 0;
}
