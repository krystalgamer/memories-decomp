#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

extern s32 func_80077150(void *a0, s32 a1);

/* Validates the caller's state token against the pending transfer's
   discriminant, clamps the requested byte count to what remains in the
   window, forwards it to SpuRead, and advances the consumed-byte
   counter. Returns the state token once the window fills, -2 while more
   remains, or -1 on a state mismatch or short transfer. */
s32 func_800497E0(void *rec, s32 count, s32 state) {
    SDSecondaryState *v1 = D_8009B458;
    SDSecondaryTransfer *s1;
    s32 result;

    if (v1->transfer.field_0000 != (s16)state) {
        return -1;
    }

    s1 = &v1->transfer;
    SpuSetTransferStartAddr(
        (u32)(s1->field_0014 + v1->bytes_consumed)
    );

    {
        s32 remaining = s1->field_0010 - D_8009B458->bytes_consumed;
        if ((u32)remaining < (u32)count) {
            count = remaining;
        }
    }

    result = func_80077150(rec, count);
    if (result != count) {
        return -1;
    }

    {
        u32 total = D_8009B458->bytes_consumed + count;
        D_8009B458->bytes_consumed = total;
        if (total >= (u32)s1->field_0010) {
            return (s16)state;
        }
    }

    return -2;
}
