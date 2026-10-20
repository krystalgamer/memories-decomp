#include "../types.h"
#include "file_transfer.h"

FileTransferDescriptor *File_RequestAsyncTransfer(
    s32 arg0,
    u8 *arg1,
    s32 arg2,
    s32 arg3,
    FileTransferCallback arg4,
    s32 arg5,
    s32 arg6
)
{
    FileTransferDescriptor *result;

    D_8009B0F4 |= FILE_TRANSFER_STATE_PRIMARY_REQUEST_LOCKED;
    if (gFile_BeforePrimaryRequestCallback == 0) {
        if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134) != 0) {
            result = (FileTransferDescriptor *)0;
            goto out;
        }
    } else {
        CALL32(void (*)(void), gFile_BeforePrimaryRequestCallback)();
    }
    File_InitTransferDescriptor(
        &gFile_PrimaryTransferDescriptor,
        arg0, arg1, arg2, arg3, arg4, arg5, arg6
    );
    result = &gFile_PrimaryTransferDescriptor;
out:
    D_8009B0F4 =
        result->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    return result;
}

FileTransferDescriptor *File_TryRequestAsyncTransfer(
    s32 arg0,
    u8 *arg1,
    s32 arg2,
    s32 arg3,
    FileTransferCallback arg4,
    s32 arg5,
    s32 arg6
)
{
    if (gFile_BeforePrimaryRequestCallback == 0) {
        if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134) != 0) {
            return (FileTransferDescriptor *)0;
        }
    } else {
        CALL32(void (*)(void), gFile_BeforePrimaryRequestCallback)();
    }
    File_InitTransferDescriptor(
        &gFile_PrimaryTransferDescriptor,
        arg0, arg1, arg2, arg3, arg4, arg5, arg6
    );
    return &gFile_PrimaryTransferDescriptor;
}

void File_CancelTransfers(void)
{
    s32 value;

    if ((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134) {
        value = 0x80;
        if ((D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
            (D_8009B0F4 & FILE_TRANSFER_FLAG_SECTOR_RANGE)) {
            File_StopRangeTransfer();
        }
        D_8009B134 = value;
    }
}

void File_StopRangeTransfer(void)
{
    D_8009B112 &= 0x3FFC;
    D_8009B112 |= 2;
}

void File_StopActiveRangeTransfer(void)
{
    if ((D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
        (D_8009B0F4 & FILE_TRANSFER_FLAG_SECTOR_RANGE)) {
        File_StopRangeTransfer();
    }
}

FileTransferDescriptor *File_RequestSecondaryAsyncTransfer(
    s32 arg0,
    u8 *arg1,
    s32 arg2,
    s32 arg3,
    FileTransferCallback arg4,
    s32 arg5,
    s32 arg6
)
{
    FileTransferDescriptor *state;

    D_8009B0F4 &= ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
    if ((D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
        (D_8009B0F4 & FILE_TRANSFER_FLAG_SECTOR_RANGE)) {
        File_StopRangeTransfer();
    }

    state = &gFile_SecondaryTransferDescriptor;
    File_InitTransferDescriptor(
        state, arg0, arg1, arg2, arg3, arg4, arg5, arg6
    );
    D_8009B0F4 |= FILE_TRANSFER_STATE_SECONDARY_PENDING;
    return state;
}

/* Counts the current phase down one sector and fires its callback at zero. */
void File_StepTransferPhase(FileTransferDescriptor *object)
{
    object->phase_remaining -= FILE_SECTOR_SIZE;
    if (object->phase_remaining <= 0) {
        object->phase_size = 0;
        if (object->phase_callback != 0) {
            s32 count = object->result++;

            CALL32(FileTransferCallback, object->phase_callback)(object, count);
        }
        object->phase_remaining = object->phase_size;
    }
}
