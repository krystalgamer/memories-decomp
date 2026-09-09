#include "../types.h"
#include "file_transfer.h"

extern FileTransferDescriptor gFile_PrimaryTransferDescriptor;
extern FileTransferDescriptor gFile_SecondaryTransferDescriptor;

FileTransferDescriptor *func_80013B04(s32 index, s32 offset)
{
    if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
         D_8009B134) == 0) {
        gFile_PrimaryTransferDescriptor.absolute_lba =
            gFile_anLba[index] + offset;
        gFile_PrimaryTransferDescriptor.done = 0;
        D_8009B0F4 = 0x100010;
        return &gFile_PrimaryTransferDescriptor;
    }
    return 0;
}

FileTransferDescriptor *File_RequestSecondaryRangeTransfer(
    s32 a,
    s32 b,
    s32 c,
    s32 d
)
{
    FileTransferDescriptor *p;
    u8 *callback_data;

    D_8009B0F4 &= ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
    p = &gFile_SecondaryTransferDescriptor;
    if (D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) {
        if (D_8009B0F4 & FILE_TRANSFER_FLAG_SECTOR_RANGE) {
            func_80015010();
        }
    }
    p->absolute_lba = a;
    p->direct_destination = b;
    callback_data = (u8 *)&p->callback_data;
    callback_data[0] = (u8)c;
    callback_data[1] = (u8)d;
    p->done = 4;
    p->status_flags = FILE_TRANSFER_FLAG_SECTOR_RANGE;
    D_8009B0F4 |= FILE_TRANSFER_STATE_SECONDARY_PENDING;
    return p;
}
