#include "../types.h"
#include "../psyq/libspu.h"
#include "func_8004A7C0.h"
#include "sound.h"

void func_8004A7C0(int index)
{
    int table_offset = index << 2;
    int object_offset = (table_offset + index) << 3;
    /* Direct objects[index] syntax changes GCC's index calculation. */
    SDSecondaryObject *entry =
        (SDSecondaryObject *)((u8 *)D_8009B458 + 0x180 + object_offset);
    int *item;
    int result;
    if (entry->channel_index >= SD_SEQUENCE_CHANNEL_COUNT)
        return;
    item = (int *)((u8 *)D_80011434 + table_offset);
    do {
        SpuSetKey(SPU_OFF, *item);
        result = SpuGetKeyStatus(*item);
        if (result == SPU_OFF_ENV_ON)
            break;
    } while (result != SPU_OFF);
    entry->field_000F = 0;
}
