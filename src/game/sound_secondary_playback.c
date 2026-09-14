#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_spatialization.h"
#include "sound_sequence_timing.h"
#include "../unmatched.h"

int func_80049A64(void *input, short value)
{
    unsigned int tag;
    SDSecondaryState *state;
    D_8009B458->flag_0500 = 1;
    tag = *(unsigned int *)input;
    if (tag != SD_SEQUENCE_TAG_SEQ && tag != SD_SEQUENCE_TAG_MIDI &&
        tag != SD_SEQUENCE_TAG_KDT && tag != SD_SEQUENCE_TAG_KDT1)
        return -1;
    state = D_8009B458;
    if (state->field_07E0 == -1) {
        state->field_07E8 = input;
        state->field_07E0 = value;
        state->field_07E2 = 2;
        state->flag_0500 = 0;
        return 0;
    }
    state->flag_0500 = 0;
    return -1;
}

void SD_PlaySequence(s32 arg0)
{
    u8 one = 1;

    D_8009B458->flag_0500 = one;
    if (D_8009B458->field_07E0 == -1) {
        D_8009B458->flag_0500 = 0;
        return;
    }
    D_8009B458->field_07EC = 0x10000;
    D_8009B458->field_07DC = D_8009B458->field_07E8;
    SD_StartSequenceTracks();

    arg0 = (u16)arg0;
    if ((arg0 & 0xFF) == 0) {
        D_8009B458->field_07E2 = 4;
    } else {
        D_8009B458->flag_0502 = one;
        D_8009B458->field_07E2 = 1;
    }
    D_8009B458->flag_0500 = 0;
}

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
