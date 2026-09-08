#include "../types.h"
#include "../psyq/libapi.h"

#include "sound.h"
#include "sound_event_runtime.h"

extern void SD_ProcessSequenceTracks(void);
extern void func_8004C84C(void);
extern void func_8004AAFC(void);

void func_8004B910(void)
{
    D_8009B458->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    DisableEvent(D_8009B458->event_handle);
    CloseEvent(D_8009B458->event_handle);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
}

void func_8004B990(void)
{
    D_8009B458->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
}

void func_8004B9E0(void)
{
    if (D_8009B458->flag_0501)
        return;
    D_8009B458->flag_0501 = 1;
    if (D_8009B458->flag_0502) {
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
    func_8004C84C();
    func_8004AAFC();
    D_8009B458->flag_0501 = 0;
}

#include "sound.h"
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
    SDSecondaryState *state = D_8009B458;
    int offset = reader->pos;
    int value = state->field_07DC[offset];

    offset++;
    reader->pos = offset;
    if ((u32)state->field_07EC < (u32)offset) {
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
