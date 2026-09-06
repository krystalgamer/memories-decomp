#include "../types.h"
#include "sound.h"

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

int SD_ReadSequenceByte(u8 *reader)
{
    SDSecondaryState *state = D_8009B458;
    int offset = *(int *)reader;
    int value = state->field_07DC[offset];

    offset++;
    *(int *)reader = offset;
    if ((u32)state->field_07EC < (u32)offset) {
        reader[0x24] = 1;
        return -1;
    }
    return value;
}

int SD_ReadVariableLengthValue(u8 *input)
{
    int value = SD_ReadSequenceByte(input);
    int result;

    if (value == 0) {
        return 0;
    }
    if (value == 255) {
        input[0x24] = 1;
        return 0;
    }
    result = value;
    if (result & 0x80) {
        result &= 0x7F;
        do {
            value = SD_ReadSequenceByte(input);
            result = (result << 7) + (value & 0x7F);
        } while (value & 0x80);
    }
    return result;
}
