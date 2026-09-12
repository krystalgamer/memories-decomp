#include "../types.h"
#include "sound.h"
#include "sound_transfer_lifecycle.h"

s32 func_800496C4(u8 *input, s16 expected, s32 value)
{
    /* Retail reserves eight stack bytes without accessing them. */
    volatile s32 pad[2];
    s32 zero;
    s32 requested;
    SDSecondaryState *initial;
    SDSecondaryState *state;
    SDSecondaryTransfer *entry;

    do {
        zero = 0;
    } while (0);
    requested = expected;
    initial = D_8009B458;
    initial->bytes_consumed = 0;
    if (requested == SD_TRANSFER_STATE_INACTIVE &&
        initial->transfer.field_0000 != requested) {
        return SD_TRANSFER_ERROR;
    }
    state = D_8009B458;
    state->transfer.field_0000 = zero;
    entry = &state->transfer;
    entry->field_0004 = input;
    entry->field_0008 = (*(u16 *)(input + 0x12) << 9) + 0xA20;
    entry->field_0010 = *(s32 *)(input + 0x0C) - entry->field_0008;
    entry->field_0018 = input[0x18];
    entry->field_001B = input[0x19];
    entry->field_0014 = (u8 *)value;
    return 0;
}
