#include "../types.h"
#include "sound.h"

/* Walks the secondary-object array up to object_count and decrements each
   active object's tent_age_countdown; invalid or inactive entries are cleared. */
void tent_SD_AgeSecondaryObjects(void) {
    s32 i;

    for (i = 0; i < D_8009B458->object_count; i++) {
        SDSecondaryObject *item = &D_8009B458->objects[i];
        if (item->tent_age_countdown != 0 && item->channel_index < SD_SEQUENCE_CHANNEL_COUNT) {
            item->tent_age_countdown -= 1;
        } else {
            item->tent_age_countdown = 0;
        }
    }
}
