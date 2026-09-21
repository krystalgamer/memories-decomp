#define G_SDVALUE_VOLATILE
#include "../types.h"
#include "func_80049010.h"
#include "../psyq/libspu.h"
#include "file_constants.h"

#include "sound.h"
#include "sound_output.h"
#include "func_80049138.h"
#include "../unmatched.h"

s32 SD_RequestMusicPackageLoad(s16 arg0, s32 arg1) {
    SDValue *p = g_SDValue;

    if (*p->music_track != arg0 >> 4) {
        SDValue *q;
        SDMusicTableEntry *entry;
        s32 off;

        func_80046294();
        SD_ResetMusicState();
        SpuSetReverbModeType(SPU_REV_MODE_OFF);

        q = g_SDValue;
        q->flags_0040 |= 2;
        off = ((arg0 & 0xFF0) >> 4) * sizeof(SDMusicTableEntry);
        entry = (SDMusicTableEntry *)(off + (s32)q->field_1560);

        func_800471D0(entry->field_04, (s32)q->music_track, entry->field_02,
                      entry->field_00 << FILE_SECTOR_SHIFT,
                      entry->field_01 << FILE_SECTOR_SHIFT, 0x20);

        *g_SDValue->music_track = 0xFFFF;
    }

    return 0;
}
