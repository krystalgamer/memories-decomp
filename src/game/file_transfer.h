#ifndef MEMORIES_DECOMP_FILE_TRANSFER_H
#define MEMORIES_DECOMP_FILE_TRANSFER_H

#include "../types.h"

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

FileTransferDescriptor *func_80014EEC(
    s32, u8 *, s32, s32, void *, s32, s32
);

#endif
