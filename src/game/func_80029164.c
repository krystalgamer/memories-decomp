#include "../types.h"
#include "card_constants.h"
#include "file_transfer.h"
#include "duel_effect_resource_record.h"
#include "duel_effect_resource_setup.h"

FileTransferDescriptor *func_80029164(s32 slot, s32 value)
{
    FileTransferDescriptor *object;

    D_800EA0E8[slot].field_30 = value;
    object = File_TryRequestAsyncTransfer(
        0, 0, (value - 1) * 7 + CARD_COUNT, 7, func_800289BC, 0, 0);
    object->callback_data = (void *)slot;
    *(u32 *)0x8009B0F4 =
        object->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    return object;
}
