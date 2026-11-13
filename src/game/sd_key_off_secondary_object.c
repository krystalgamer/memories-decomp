#include "../types.h"
#include "../psyq/libspu.h"
#include "sd_key_off_secondary_object.h"
#include "sound.h"

void SD_KeyOffSecondaryObject(s32 index)
{
    SDSecondaryObject *entry = &gSD_pSecondaryState->objects[index];
    int *item;
    int result;
    if (entry->channel_index >= SD_SEQUENCE_CHANNEL_COUNT)
        return;
    item = &gSD_adwVoiceMasks[index];
    do {
        SpuSetKey(SPU_OFF, *item);
        result = SpuGetKeyStatus(*item);
        if (result == SPU_OFF_ENV_ON)
            break;
    } while (result != SPU_OFF);
    entry->key_held = 0;
}
