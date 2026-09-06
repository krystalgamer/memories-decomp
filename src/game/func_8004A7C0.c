#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

extern int D_80011434[];
extern int func_80077090(int);

void func_8004A7C0(int index)
{
    int table_offset = index << 2;
    int object_offset = (table_offset + index) << 3;
    /* Direct objects[index] syntax changes GCC's index calculation. */
    SDSecondaryObject *entry =
        (SDSecondaryObject *)((u8 *)D_8009B458 + 0x180 + object_offset);
    int *item;
    int result;
    if (entry->field_0003 >= 16)
        return;
    item = (int *)((u8 *)D_80011434 + table_offset);
    do {
        SpuSetKey(0, *item);
        result = func_80077090(*item);
        if (result == 2)
            break;
    } while (result != 0);
    entry->field_000F = 0;
}
