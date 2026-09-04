#include "../types.h"
#include "sound.h"

int func_8004BAE4(u8 *reader)
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

int func_8004BB34(u8 *input)
{
    int value = func_8004BAE4(input);
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
            value = func_8004BAE4(input);
            result = (result << 7) + (value & 0x7F);
        } while (value & 0x80);
    }
    return result;
}
