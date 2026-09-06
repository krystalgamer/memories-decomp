#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

s16 func_800498BC(s16 arg0)
{
    if (arg0 == 0)
        return SpuIsTransferCompleted(0);
    return SpuIsTransferCompleted(1);
}

void func_800498F8(void)
{
    s16 *value = &D_8009B458->transfer.field_0000;

    if (*value != -1)
        *value = -1;
}
