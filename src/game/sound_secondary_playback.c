#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_spatialization.h"
#include "sound_sequence_timing.h"
#include "../unmatched.h"

int SD_OpenSequence(u8 *input, short vab_id)
{
    unsigned int tag;
    SDSecondaryState *state;
    gSD_pSecondaryState->flag_0500 = 1;
    tag = *(unsigned int *)input;
    if (tag != SD_SEQUENCE_TAG_SEQ && tag != SD_SEQUENCE_TAG_MIDI &&
        tag != SD_SEQUENCE_TAG_KDT && tag != SD_SEQUENCE_TAG_KDT1)
        return -1;
    state = gSD_pSecondaryState;
    if (state->sequence_open_token == -1) {
        state->sequence_source = input;
        state->sequence_open_token = vab_id;
        state->sequence_state = 2;
        state->flag_0500 = 0;
        return 0;
    }
    state->flag_0500 = 0;
    return -1;
}

void SD_PlaySequence(s32 arg0)
{
    u8 one = 1;

    gSD_pSecondaryState->flag_0500 = one;
    if (gSD_pSecondaryState->sequence_open_token == -1) {
        gSD_pSecondaryState->flag_0500 = 0;
        return;
    }
    gSD_pSecondaryState->sequence_read_limit = 0x10000;
    gSD_pSecondaryState->sequence_read_base = gSD_pSecondaryState->sequence_source;
    SD_StartSequenceTracks();

    arg0 = (u16)arg0;
    if ((arg0 & 0xFF) == 0) {
        gSD_pSecondaryState->sequence_state = 4;
    } else {
        gSD_pSecondaryState->flag_0502 = one;
        gSD_pSecondaryState->sequence_state = 1;
    }
    gSD_pSecondaryState->flag_0500 = 0;
}

/* The play branch of SD_PlaySequence plus a fast-forward target in
   fast_forward_target_address. While that is nonzero, SD_ProcessSequenceTracks dispatches each
   track's next event group on every tick instead of waiting out its delta
   time, and clears it once a track's read address (sequence_read_base + pos) reaches
   target. */
void SD_PlaySequenceFastForward(s32 target)
{
    SDSecondaryState *state;

    gSD_pSecondaryState->flag_0500 = 1;
    state = gSD_pSecondaryState;
    if (state->sequence_open_token == -1) {
        state->flag_0500 = 0;
    } else {
        SDSecondaryState *first;
        SDSecondaryState *second;
        state->sequence_read_limit = 0x10000;
        state->sequence_read_base = state->sequence_source;
        SD_StartSequenceTracks_no_arg();
        first = gSD_pSecondaryState;
        first->flag_0502 = 1;
        second = gSD_pSecondaryState;
        first->fast_forward_target_address = target;
        second->sequence_state = 1;
        second->flag_0500 = 0;
    }
}

void SD_StopSequence(void)
{
    SDSecondaryState *initial = gSD_pSecondaryState;

    if (initial->sequence_open_token != -1) {
        SDSecondaryState *state;
        SDSecondaryState *other;
        initial->flag_0500 = 1;
        SD_ResetSequenceTracks();
        SD_ResetSecondaryPlayback();
        state = gSD_pSecondaryState;
        state->flag_0500 = 0;
        other = gSD_pSecondaryState;
        state->spatial_level_right = SD_SECONDARY_LEVEL_MAX;
        state->spatial_level_left = SD_SECONDARY_LEVEL_MAX;
        state->sequence_state = 2;
        other->flag_0502 = 0;
    }
}

void SD_CloseSequence(void)
{
    SDSecondaryState *state = gSD_pSecondaryState;

    if (state->sequence_open_token != -1) {
        state->flag_0500 = 1;
        gSD_pSecondaryState->sequence_open_token = -1;
        gSD_pSecondaryState->sequence_state = 0;
        gSD_pSecondaryState->spatial_level_right = SD_SECONDARY_LEVEL_MAX;
        gSD_pSecondaryState->spatial_level_left = SD_SECONDARY_LEVEL_MAX;
        gSD_pSecondaryState->flag_0500 = 0;
    }
}
