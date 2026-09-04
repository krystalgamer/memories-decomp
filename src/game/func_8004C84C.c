#include "../types.h"
#include "sound.h"

/* Walks the secondary-object array up to object_count and decrements each
   active object's field_001E; invalid or inactive entries are cleared. */
void func_8004C84C(void) {
    s32 i;

    for (i = 0; i < D_8009B458->object_count; i++) {
        SDSecondaryObject *item = &D_8009B458->objects[i];
        if (item->field_001E != 0 && item->field_0003 < 0x10) {
            item->field_001E -= 1;
        } else {
            item->field_001E = 0;
        }
    }
}
