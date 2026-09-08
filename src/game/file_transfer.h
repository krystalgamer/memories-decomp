#ifndef MEMORIES_DECOMP_FILE_TRANSFER_H
#define MEMORIES_DECOMP_FILE_TRANSFER_H

#include "../types.h"
#include "file_constants.h"

#define FILE_TRANSFER_STATE_PRIMARY_ACTIVE 0x10
#define FILE_TRANSFER_STATE_SECONDARY_PENDING 0x20
#define FILE_TRANSFER_REQUEST_BLOCKED_MASK 0x02000030
#define FILE_TRANSFER_DESCRIPTOR_WORD_COUNT 18

#define FILE_TRANSFER_DESCRIPTOR_OFFSET(type, member) ((u32)&(((type *)0)->member))

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    u32 value_08;
    u32 value_0C;
    u8 pad_10[0xC];
    u32 mode;
    u8 pad_20[0xC];
    u32 status_flags;
    u16 counter;
    u16 field_32;
    u8 pad_34[0x4];
    void *callback_data;
    u32 position;
    u32 result;
    u8 pad_44[0x2];
    u8 done;
    u8 substate;
} FileTransferDescriptor;

typedef char FileTransferDescriptor_size_must_be_0x48[
    sizeof(FileTransferDescriptor) == 0x48 ? 1 : -1
];
typedef char FileTransferDescriptor_w_offset_must_be_0x04[
    FILE_TRANSFER_DESCRIPTOR_OFFSET(FileTransferDescriptor, w) == 0x04
        ? 1 : -1
];
typedef char FileTransferDescriptor_h_offset_must_be_0x06[
    FILE_TRANSFER_DESCRIPTOR_OFFSET(FileTransferDescriptor, h) == 0x06
        ? 1 : -1
];
typedef char FileTransferDescriptor_value_08_offset_must_be_0x08[
    FILE_TRANSFER_DESCRIPTOR_OFFSET(FileTransferDescriptor, value_08) == 0x08
        ? 1 : -1
];
typedef char FileTransferDescriptor_status_flags_offset_must_be_0x2C[
    FILE_TRANSFER_DESCRIPTOR_OFFSET(FileTransferDescriptor, status_flags) ==
        FILE_TRANSFER_DESCRIPTOR_STATUS_FLAGS_BYTE_OFFSET ? 1 : -1
];
typedef char FileTransferDescriptor_counter_offset_must_be_0x30[
    FILE_TRANSFER_DESCRIPTOR_OFFSET(FileTransferDescriptor, counter) == 0x30
        ? 1 : -1
];
typedef char FileTransferDescriptor_field_32_offset_must_be_0x32[
    FILE_TRANSFER_DESCRIPTOR_OFFSET(FileTransferDescriptor, field_32) == 0x32
        ? 1 : -1
];
typedef char FileTransferDescriptor_callback_data_offset_must_be_0x38[
    FILE_TRANSFER_DESCRIPTOR_OFFSET(FileTransferDescriptor, callback_data) == 0x38
        ? 1 : -1
];
typedef char FileTransferDescriptor_done_offset_must_be_0x46[
    FILE_TRANSFER_DESCRIPTOR_OFFSET(FileTransferDescriptor, done) ==
        FILE_TRANSFER_DESCRIPTOR_STATE_BYTE_OFFSET ? 1 : -1
];
typedef char FileTransferDescriptor_substate_offset_must_be_0x47[
    FILE_TRANSFER_DESCRIPTOR_OFFSET(FileTransferDescriptor, substate) ==
        FILE_TRANSFER_DESCRIPTOR_SUBSTATE_BYTE_OFFSET ? 1 : -1
];

#undef FILE_TRANSFER_DESCRIPTOR_OFFSET

FileTransferDescriptor *File_RequestAsyncTransfer(
    s32, u8 *, s32, s32, void *, s32, s32
);
FileTransferDescriptor *File_TryRequestAsyncTransfer(
    s32, u8 *, s32, s32, void *, s32, s32
);
FileTransferDescriptor *File_RequestSecondaryAsyncTransfer(
    s32, u8 *, s32, s32, void *, s32, s32
);
FileTransferDescriptor *File_RequestSecondaryRangeTransfer(
    s32, s32, s32, s32
);
void File_WaitForTransfers(void);

#endif
