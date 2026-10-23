#include "../types.h"
#include "sound.h"

/* Saturates the reuse-ranking counter at zero; out-of-range channels clear
   only that counter, not the object or its channel ownership. */
void SD_UpdateSecondaryObjectReuseCountdowns(void) {
    s32 i;

    for (i = 0; i < gSD_pSecondaryState->object_count; i++) {
        SDSecondaryObject *item = &gSD_pSecondaryState->objects[i];
        if (item->reuse_countdown != 0 && item->channel_index < SD_SEQUENCE_CHANNEL_COUNT) {
            item->reuse_countdown -= 1;
        } else {
            item->reuse_countdown = 0;
        }
    }
}
