#include "../types.h"
#include "display_object_layout.h"

extern u8 D_800EFE48[];

void *func_80042B40(int value)
{
    u8 *object = D_800EFE48;
    int count = DISPLAY_OBJECT_POOL_CAPACITY;
    u8 *field = object + 108;
    do {
        if ((*(u16 *)(field - 100) & DISPLAY_OBJECT_FLAG_ALLOCATED) &&
            ((field[0] & 0xF) == value))
            return object;
        field += DISPLAY_OBJECT_RECORD_SIZE;
        count--;
        object += DISPLAY_OBJECT_RECORD_SIZE;
    } while (count != 0);
    return 0;
}
