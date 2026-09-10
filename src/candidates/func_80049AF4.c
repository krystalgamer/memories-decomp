/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_secondary_playback.c.
 */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"
#include "../game/sound_spatialization.h"
#define SD_START_SEQUENCE_TRACKS_PLAYBACK_CALLS
#include "../game/sound_sequence_timing.h"

void func_80049AF4(s32 arg0)
{
    register s32 one asm("$17") = 1;

    D_8009B458->flag_0500 = one;

    if (D_8009B458->field_07E0 == -1) {
        D_8009B458->flag_0500 = 0;
        return;
    }

    D_8009B458->field_07EC = 0x10000;
    D_8009B458->field_07DC = D_8009B458->field_07E8;
    SD_StartSequenceTracks(D_8009B458);

    if ((arg0 & 0xFF) == 0) {
        D_8009B458->field_07E2 = 4;
    } else {
        D_8009B458->flag_0502 = one;
        D_8009B458->field_07E2 = 1;
    }

    D_8009B458->flag_0500 = 0;
}

