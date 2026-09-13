#include "../types.h"
#include "func_8004A6D8.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_event_runtime.h"
#include "sound_init.h"
#include "sound_transfer_lifecycle.h"

s32 func_8004975C(s32 value, s16 expected)
{
    register int saved;
    SDSecondaryState *state = D_8009B458;
    short current = state->transfer.field_0000;

    if (current != expected)
        return SD_TRANSFER_ERROR;
    saved = value;
    {
        SDSecondaryTransfer *entry = &state->transfer;
        SpuSetTransferStartAddr((u32)entry->field_0014);
        if (SpuWrite((u8 *)saved, (u32)entry->field_0010) !=
            entry->field_0010)
            return SD_TRANSFER_ERROR;
        entry->field_000C = saved;
    }
    return current;
}
