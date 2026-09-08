#include "../types.h"
#include "file_transfer.h"

struct FilePositionState {
    char pad24[0x24];
    s32 f24;
    char pad46[FILE_TRANSFER_DESCRIPTOR_STATE_BYTE_OFFSET - 0x28];
    u8 f46;
};

struct FileTransferState {
    char pad00[0x24];
    s32 f24;
    char pad28[FILE_TRANSFER_DESCRIPTOR_STATUS_FLAGS_BYTE_OFFSET - 0x28];
    s32 f2C;
    char pad30[0x34 - 0x30];
    s32 f34;
    u8 f38;
    u8 f39;
    char pad3A[FILE_TRANSFER_DESCRIPTOR_STATE_BYTE_OFFSET - 0x3A];
    u8 f46;
};

extern volatile u32 D_8009B0F4;
extern volatile u32 D_8009B134;
extern struct FilePositionState gFile_PrimaryTransferDescriptor;
extern struct FileTransferState gFile_SecondaryTransferDescriptor;
extern s32 gFile_anLba[];
extern void func_80015010(void);

struct FilePositionState *func_80013B04(s32 index, s32 offset)
{
    if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
         D_8009B134) == 0) {
        gFile_PrimaryTransferDescriptor.f24 = gFile_anLba[index] + offset;
        gFile_PrimaryTransferDescriptor.f46 = 0;
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
    struct FileTransferState *p;

    D_8009B0F4 &= ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
    p = &gFile_SecondaryTransferDescriptor;
    if (D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) {
        if (D_8009B0F4 & FILE_TRANSFER_FLAG_SECTOR_RANGE) {
            func_80015010();
        }
    }
    p->f24 = a;
    p->f34 = b;
    p->f38 = (u8)c;
    p->f39 = (u8)d;
    p->f46 = 4;
    p->f2C = FILE_TRANSFER_FLAG_SECTOR_RANGE;
    D_8009B0F4 |= FILE_TRANSFER_STATE_SECONDARY_PENDING;
    return (FileTransferDescriptor *)p;
}
