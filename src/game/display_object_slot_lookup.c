#include "../types.h"

typedef struct {
    u8 pad_00[8];
    u16 flags;
    u8 pad_0A[0x66];
} DisplayObjectSlot;

extern DisplayObjectSlot D_800EFE48[];
extern DisplayObjectSlot D_800F0548[];

s32 func_8004002C(void)
{
    DisplayObjectSlot *entry = D_800F0548;
    s32 i;

    for (i = 16; i < 0x60; i++, entry++) {
        if ((entry->flags & 0x80) == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_8004006C(void)
{
    DisplayObjectSlot *entry = D_800EFE48;
    s32 i;

    for (i = 0; i < 0x60; i++, entry++) {
        if ((entry->flags & 0x80) == 0) {
            return i;
        }
    }
    return -1;
}
