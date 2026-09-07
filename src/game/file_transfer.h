#ifndef MEMORIES_DECOMP_FILE_TRANSFER_H
#define MEMORIES_DECOMP_FILE_TRANSFER_H

#include "../types.h"

#define FILE_TRANSFER_STATE_PRIMARY_ACTIVE 0x10
#define FILE_TRANSFER_STATE_SECONDARY_PENDING 0x20
#define FILE_TRANSFER_REQUEST_BLOCKED_MASK 0x02000030

typedef struct {
    u8 pad_00[0x8];
    u32 value_08;
    u32 value_0C;
    u8 pad_10[0xC];
    u32 mode;
    u8 pad_20[0xC];
    u32 status_flags;
    u16 counter;
    u8 pad_32[0x6];
    void *callback_data;
    u32 position;
    u32 result;
    u8 pad_44[0x2];
    u8 done;
} FileTransferDescriptor;

FileTransferDescriptor *File_RequestAsyncTransfer(
    s32, u8 *, s32, s32, void *, s32, s32
);
FileTransferDescriptor *File_TryRequestAsyncTransfer(
    s32, u8 *, s32, s32, void *, s32, s32
);
FileTransferDescriptor *File_RequestSecondaryAsyncTransfer(
    s32, u8 *, s32, s32, void *, s32, s32
);

#endif
