#include "../types.h"
#include "sound.h"

/* Validates a byte value against the secondary-object capacity and stores it
   into the s16 field 0x510 of *D_8009B458. Returns the stored value, or 0xFF
   if out of range or zero. */
s32 func_80049600(u32 a0) {
    u8 x = a0 & 0xFF;

    if (x >= SD_SECONDARY_OBJECT_COUNT + 1) {
        return 0xFF;
    }
    if (x == 0) {
        return 0xFF;
    }
    D_8009B458->object_count = x;
    return x;
}
