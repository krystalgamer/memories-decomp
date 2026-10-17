#include "../types.h"
#include "../psyq/libspu.h"
#include "sd_key_off_secondary_object.h"
#include "sound.h"

#define SD_SECONDARY_OBJECT_OFFSET_VIEW(state, offset) \
    ((SDSecondaryObject *)((u8 *)(state) + (offset)))

void SD_KeyOffSecondaryObject(int index)
{
    int table_offset = index << 2;
    int object_offset = (table_offset + index) << 3;
    /* Direct objects[index] syntax changes GCC's index calculation. */
    SDSecondaryObject *entry =
        SD_SECONDARY_OBJECT_OFFSET_VIEW(
            gSD_pSecondaryState, SD_SECONDARY_OBJECT_ARRAY_OFFSET + object_offset);
    int *item;
    int result;
    if (entry->channel_index >= SD_SEQUENCE_CHANNEL_COUNT)
        return;
    item = &D_80011434[index];
    do {
        SpuSetKey(SPU_OFF, *item);
        result = SpuGetKeyStatus(*item);
        if (result == SPU_OFF_ENV_ON)
            break;
    } while (result != SPU_OFF);
    entry->key_held = 0;
}
