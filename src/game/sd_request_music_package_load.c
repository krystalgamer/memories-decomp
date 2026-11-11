#include "../types.h"
#include "sound_effect_request.h"
#include "../psyq/libspu.h"
#include "file_constants.h"

#include "sound.h"
#include "sound_driver.h"
#include "sd_request_music_package_load.h"

s32 SD_RequestMusicPackageLoad(s16 command_id, s32 unused) {
    SDValue *p = g_SDValue;

    if (*p->s_pwSD_MUSIC_TRACK != command_id >> 4) {
        SDValue *q;
        SDMusicTableEntry *entry;
        s32 off;

        SD_CancelMusicRequests();
        SD_ResetMusicState();
        SpuSetReverbModeType(SPU_REV_MODE_OFF);

        q = g_SDValue;
        q->s_wSD_STATUS |= 2;
        off = ((command_id & 0xFF0) >> 4) * sizeof(SDMusicTableEntry);
        entry = (SDMusicTableEntry *)(off + (s32)q->s_pstSD_MUSIC_TABLE);

        SD_EnqueueSpuRamTransfer(entry->spu_address, (s32)q->s_pwSD_MUSIC_TRACK,
                                 entry->sector_offset,
                                 entry->spu_sector_count << FILE_SECTOR_SHIFT,
                                 entry->ram_sector_count << FILE_SECTOR_SHIFT, 0x20);

        *g_SDValue->s_pwSD_MUSIC_TRACK = 0xFFFF;
    }

    return 0;
}
