#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_spatialization.h"
#define SD_START_SEQUENCE_TRACKS_PLAYBACK_CALLS
#include "sound_sequence_timing.h"

void func_80049BAC(s32 value)
{
    SDSecondaryState *state;

    D_8009B458->flag_0500 = 1;
    state = D_8009B458;
    if (state->field_07E0 == -1) {
        state->flag_0500 = 0;
    } else {
        SDSecondaryState *first;
        SDSecondaryState *second;
        state->field_07EC = 0x10000;
        state->field_07DC = state->field_07E8;
        SD_StartSequenceTracks_no_arg();
        first = D_8009B458;
        first->flag_0502 = 1;
        second = D_8009B458;
        first->field_080C = value;
        second->field_07E2 = 1;
        second->flag_0500 = 0;
    }
}

void func_80049C40(void)
{
    SDSecondaryState *initial = D_8009B458;

    if (initial->field_07E0 != -1) {
        SDSecondaryState *state;
        SDSecondaryState *other;
        initial->flag_0500 = 1;
        SD_ResetSequenceTracks();
        func_8004A518();
        state = D_8009B458;
        state->flag_0500 = 0;
        other = D_8009B458;
        state->field_07E6 = SD_SECONDARY_LEVEL_MAX;
        state->field_07E4 = SD_SECONDARY_LEVEL_MAX;
        state->field_07E2 = 2;
        other->flag_0502 = 0;
    }
}

void func_80049CB0(void)
{
    SDSecondaryState *state = D_8009B458;

    if (state->field_07E0 != -1) {
        state->flag_0500 = 1;
        D_8009B458->field_07E0 = -1;
        D_8009B458->field_07E2 = 0;
        D_8009B458->field_07E6 = SD_SECONDARY_LEVEL_MAX;
        D_8009B458->field_07E4 = SD_SECONDARY_LEVEL_MAX;
        D_8009B458->flag_0500 = 0;
    }
}
