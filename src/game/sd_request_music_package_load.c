#define G_SDVALUE_VOLATILE
#include "../types.h"
#include "sd_reset_music_state.h"
#include "../psyq/libspu.h"
#include "file_constants.h"

#include "sound.h"
#include "sound_output.h"
#include "sd_request_music_package_load.h"
#include "../unmatched.h"

s32 SD_RequestMusicPackageLoad(s16 arg0, s32 arg1) {
    SDValue *p = g_SDValue;

    if (*p->music_track != arg0 >> 4) {
        SDValue *q;
        SDMusicTableEntry *entry;
        s32 off;

        SD_CancelMusicRequests();
        SD_ResetMusicState();
        SpuSetReverbModeType(SPU_REV_MODE_OFF);

        q = g_SDValue;
        q->status_flags |= 2;
        off = ((arg0 & 0xFF0) >> 4) * sizeof(SDMusicTableEntry);
        entry = (SDMusicTableEntry *)(off + (s32)q->music_table);

        SD_EnqueueSpuRamTransfer(entry->spu_address, (s32)q->music_track,
                                 entry->sector_offset,
                                 entry->spu_sector_count << FILE_SECTOR_SHIFT,
                                 entry->ram_sector_count << FILE_SECTOR_SHIFT, 0x20);

        *g_SDValue->music_track = 0xFFFF;
    }

    return 0;
}
