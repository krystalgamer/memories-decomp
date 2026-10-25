#include "../types.h"
#include "../psyq/libapi.h"

#include "sound.h"
#include "sound_event_runtime.h"

void SD_CloseSequenceTimerEvent(void)
{
    gSD_pSecondaryState->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    DisableEvent(gSD_pSecondaryState->event_handle);
    CloseEvent(gSD_pSecondaryState->event_handle);
    ExitCriticalSection();
    gSD_pSecondaryState->flag_0500 = 0;
}

void SD_StopSequenceTimer(void)
{
    gSD_pSecondaryState->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    ExitCriticalSection();
    gSD_pSecondaryState->flag_0500 = 0;
}

void SD_UpdateSecondarySequence(void)
{
    if (gSD_pSecondaryState->flag_0501)
        return;
    gSD_pSecondaryState->flag_0501 = 1;
    if (gSD_pSecondaryState->flag_0502) {
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
    }
    SD_UpdateSecondaryObjectReuseCountdowns();
    SD_UpdateSecondaryObjectVoices();
    gSD_pSecondaryState->flag_0501 = 0;
}

#include "sound_sequence_reader.h"

/* Compares arg2 bytes; returns the difference at the first mismatch. */
s32 SD_CompareBytes(u8 *arg0, u8 *arg1, s32 arg2) {
    if (arg2 == 0) {
        return -1;
    }

    while (--arg2 != 0 && *arg0 == *arg1) {
        arg0++;
        arg1++;
    }

    return *arg0 - *arg1;
}

s32 SD_ReadSequenceByte(SDSequenceTrack *reader)
{
    SDSecondaryState *state = gSD_pSecondaryState;
    int offset = reader->pos;
    int value = state->sequence_read_base[offset];

    offset++;
    reader->pos = offset;
    if ((u32)state->sequence_read_limit < (u32)offset) {
        reader->ended = 1;
        return -1;
    }
    return value;
}

s32 SD_ReadVariableLengthValue(SDSequenceTrack *input)
{
    int value = SD_ReadSequenceByte(input);
    int result;

    if (value == 0) {
        return 0;
    }
    if (value == SD_SEQUENCE_VLQ_INITIAL_STOP) {
        input->ended = 1;
        return 0;
    }
    result = value;
    if (result & SD_SEQUENCE_VLQ_CONTINUATION_BIT) {
        result &= SD_SEQUENCE_VLQ_PAYLOAD_MASK;
        do {
            value = SD_ReadSequenceByte(input);
            result = (result << SD_SEQUENCE_VLQ_PAYLOAD_BITS) +
                     (value & SD_SEQUENCE_VLQ_PAYLOAD_MASK);
        } while (value & SD_SEQUENCE_VLQ_CONTINUATION_BIT);
    }
    return result;
}
