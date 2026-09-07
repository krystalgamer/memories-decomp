#include "../types.h"
#include "card_constants.h"
#include "file_transfer.h"

extern u8 D_800EA0E8[];
extern volatile u32 D_8009B0F4;
extern void func_800289BC(void);
FileTransferDescriptor *func_80029164(s32 slot, s32 value)
{
    u8 *base = D_800EA0E8;
    u8 *record = &base[slot * 64];
    FileTransferDescriptor *object;

    *(u16 *)(record + 0x30) = value;
    object = File_TryRequestAsyncTransfer(
        0, 0, (value - 1) * 7 + CARD_COUNT, 7, func_800289BC, 0, 0);
    object->callback_data = (void *)slot;
    D_8009B0F4 = object->status_flags | 0x10;
    return object;
}
