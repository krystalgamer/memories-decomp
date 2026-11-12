#include "../types.h"
#include "../psyq/libspu.h"
#include "sd_key_off_secondary_object.h"
#include "sound_sequence_constants.h"
#include "sound.h"

void SD_KeyOffSecondaryNote(s32 channel, s32 note, s32 unused) {
    s32 mask;
    s32 i;
    s32 key_status;

    mask = 0;
    for (i = 0; i < gSD_pSecondaryState->object_count; i++) {
        if (gSD_pSecondaryState->objects[i].channel_index == (u8)channel &&
            gSD_pSecondaryState->objects[i].key_note == (u8)note) {
            SD_KeyOffSecondaryObject(i);
            mask |= gSD_adwVoiceMasks[i];
        }
    }

    if (mask != 0) {
        do {
            SpuSetKey(SPU_OFF, mask);
            key_status = SpuGetKeyStatus(mask);
        } while (key_status != SPU_OFF_ENV_ON && key_status != SPU_OFF);
    }

    {
        SDSecondaryRecord *q =
            (SDSecondaryRecord *)gSD_pSecondaryState + (u8)channel;
        q->velocity = 0;
    }
}
