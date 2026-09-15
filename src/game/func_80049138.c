#define G_SDVALUE_VOLATILE
#include "../types.h"
#include "func_80049010.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output.h"
#include "../unmatched.h"

typedef struct {
    u8 field_00;
    u8 field_01;
    u16 field_02;
    s32 field_04;
} SDMusicTableEntry;

typedef char SDMusicTableEntry_size_must_be_8[
    sizeof(SDMusicTableEntry) == 8 ? 1 : -1
];

s32 func_80049138(s16 arg0, s32 arg1) {
    SDValue *p = g_SDValue;

    if (*p->music_track != arg0 >> 4) {
        SDValue *q;
        SDMusicTableEntry *entry;
        s32 off;

        func_80046294();
        func_80049010();
        SpuSetReverbModeType(SPU_REV_MODE_OFF);

        q = g_SDValue;
        q->flags_0040 |= 2;
        off = (arg0 & 0xFF0) >> 1;
        entry = (SDMusicTableEntry *)(off + (s32)q->field_1560);

        func_800471D0(entry->field_04, (s32)q->music_track, entry->field_02,
                      entry->field_00 << 11, entry->field_01 << 11, 0x20);

        *g_SDValue->music_track = 0xFFFF;
    }

    return 0;
}
