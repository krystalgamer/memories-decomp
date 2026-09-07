#include "../types.h"
#include "file_transfer.h"

extern u32 D_8009B10C;
extern volatile u32 D_8009B0F4;
extern volatile u16 D_8009B112;
extern u32 D_8009B134;
extern u8 gFile_SecondaryTransferDescriptor[];
extern u8 gFile_PrimaryTransferDescriptor[];

extern volatile s32 D_8009B0F4_signed asm("D_8009B0F4");
extern void (*D_8009B10C_callback)(void) asm("D_8009B10C");
extern s32 D_8009B134_signed asm("D_8009B134");

extern u8 *File_InitTransferDescriptor(
    u8 *, s32, u8 *, s32, s32, void *, s32, s32
);
void func_80015010(void);

FileTransferDescriptor *File_RequestAsyncTransfer(
    s32 arg0,
    u8 *arg1,
    s32 arg2,
    s32 arg3,
    void *arg4,
    s32 arg5,
    s32 arg6
)
{
    FileTransferDescriptor *result;

    D_8009B0F4_signed |= 0x40;
    if (D_8009B10C_callback == 0) {
        if (((D_8009B0F4_signed & 0x2000030) | D_8009B134_signed) != 0) {
            result = (FileTransferDescriptor *)0;
            goto out;
        }
    } else {
        D_8009B10C_callback();
    }
    File_InitTransferDescriptor(
        gFile_PrimaryTransferDescriptor,
        arg0, arg1, arg2, arg3, arg4, arg5, arg6
    );
    result = (FileTransferDescriptor *)gFile_PrimaryTransferDescriptor;
out:
    D_8009B0F4_signed =
        result->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    return result;
}

FileTransferDescriptor *File_TryRequestAsyncTransfer(
    s32 arg0,
    u8 *arg1,
    s32 arg2,
    s32 arg3,
    void *arg4,
    s32 arg5,
    s32 arg6
)
{
    if (D_8009B10C == 0) {
        if (((D_8009B0F4 & 0x2000030) | D_8009B134) != 0) {
            return (FileTransferDescriptor *)0;
        }
    } else {
        ((void (*)(void))D_8009B10C)();
    }
    File_InitTransferDescriptor(
        gFile_PrimaryTransferDescriptor,
        arg0, arg1, arg2, arg3, arg4, arg5, arg6
    );
    return (FileTransferDescriptor *)gFile_PrimaryTransferDescriptor;
}

void func_80014FA4(void)
{
    s32 value;

    if ((D_8009B0F4 & 0x2000030) | D_8009B134) {
        value = 0x80;
        if ((D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
            (D_8009B0F4 & 0x80000)) {
            func_80015010();
        }
        D_8009B134 = value;
    }
}

void func_80015010(void)
{
    D_8009B112 &= 0x3FFC;
    D_8009B112 |= 2;
}

void func_80015038(void)
{
    if ((D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
        (D_8009B0F4 & 0x80000)) {
        func_80015010();
    }
}

FileTransferDescriptor *File_RequestSecondaryAsyncTransfer(
    s32 arg0,
    u8 *arg1,
    s32 arg2,
    s32 arg3,
    void *arg4,
    s32 arg5,
    s32 arg6
)
{
    FileTransferDescriptor *state;

    D_8009B0F4 &= ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
    if ((D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
        (D_8009B0F4 & 0x80000)) {
        func_80015010();
    }

    state = (FileTransferDescriptor *)gFile_SecondaryTransferDescriptor;
    File_InitTransferDescriptor(
        (u8 *)state, arg0, arg1, arg2, arg3, arg4, arg5, arg6
    );
    D_8009B0F4 |= FILE_TRANSFER_STATE_SECONDARY_PENDING;
    return state;
}
