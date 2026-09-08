#include "../types.h"
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

s32 SD_ReadSequenceByte(void *reader)
{
    SDSecondaryState *state = D_8009B458;
    int offset = *(int *)reader;
    int value = state->field_07DC[offset];

    offset++;
    *(int *)reader = offset;
    if ((u32)state->field_07EC < (u32)offset) {
        ((u8 *)reader)[0x24] = 1;
        return -1;
    }
    return value;
}

s32 SD_ReadVariableLengthValue(void *input)
{
    int value = SD_ReadSequenceByte(input);
    int result;

    if (value == 0) {
        return 0;
    }
    if (value == SD_SEQUENCE_VLQ_INITIAL_STOP) {
        ((u8 *)input)[0x24] = 1;
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
