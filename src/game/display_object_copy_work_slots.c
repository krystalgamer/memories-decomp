#include "../types.h"
#include "display_object_work_slots.h"

void DisplayObject_CopyWorkSlots(s32 *destination)
{
    s32 i;
    DisplayObject *G32 *source;

    i = 0;
    source = gDisplayObject_apWorkSlots;
    while (i < DISPLAY_OBJECT_WORK_SLOT_COUNT) {
        *destination++ = (s32)*source++;
        i++;
    }
    *destination = 0;
}
