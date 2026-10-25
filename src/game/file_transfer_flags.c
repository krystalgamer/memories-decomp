#include "../types.h"
#include "file_transfer.h"
#include "file_transfer_state.h"

FileTransferDescriptor *File_RequestAsyncTransfer(
    s32 arg0,
    u8 *arg1,
    s32 arg2,
    s32 arg3,
    FileTransferCallback callback,
    s32 initial_phase_index,
    s32 arg6
)
{
    FileTransferDescriptor *result;

    gFile_dwTransferState |= FILE_TRANSFER_STATE_PRIMARY_REQUEST_LOCKED;
    if (gFile_BeforePrimaryRequestCallback == 0) {
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) != 0) {
            result = (FileTransferDescriptor *)0;
            goto out;
        }
    } else {
        CALL32(void (*)(void), gFile_BeforePrimaryRequestCallback)();
    }
    File_InitTransferDescriptor(
        &gFile_PrimaryTransferDescriptor,
        arg0, arg1, arg2, arg3, callback, initial_phase_index, arg6
    );
    result = &gFile_PrimaryTransferDescriptor;
out:
    gFile_dwTransferState =
        result->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    return result;
}

FileTransferDescriptor *File_TryRequestAsyncTransfer(
    s32 arg0,
    u8 *arg1,
    s32 arg2,
    s32 arg3,
    FileTransferCallback callback,
    s32 initial_phase_index,
    s32 arg6
)
{
    if (gFile_BeforePrimaryRequestCallback == 0) {
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) != 0) {
            return (FileTransferDescriptor *)0;
        }
    } else {
        CALL32(void (*)(void), gFile_BeforePrimaryRequestCallback)();
    }
    File_InitTransferDescriptor(
        &gFile_PrimaryTransferDescriptor,
        arg0, arg1, arg2, arg3, callback, initial_phase_index, arg6
    );
    return &gFile_PrimaryTransferDescriptor;
}

void File_CancelTransfers(void)
{
    s32 value;

    if ((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | gFile_dwCancelRequestState) {
        value = 0x80;
        if ((gFile_dwTransferState & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
            (gFile_dwTransferState & FILE_TRANSFER_FLAG_SECTOR_RANGE)) {
            File_StopRangeTransfer();
        }
        gFile_dwCancelRequestState = value;
    }
}

void File_StopRangeTransfer(void)
{
    gFile_wTransferStepFlags &= 0x3FFC;
    gFile_wTransferStepFlags |= 2;
}

void File_StopActiveRangeTransfer(void)
{
    if ((gFile_dwTransferState & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
        (gFile_dwTransferState & FILE_TRANSFER_FLAG_SECTOR_RANGE)) {
        File_StopRangeTransfer();
    }
}

FileTransferDescriptor *File_RequestSecondaryAsyncTransfer(
    s32 arg0,
    u8 *arg1,
    s32 arg2,
    s32 arg3,
    FileTransferCallback callback,
    s32 initial_phase_index,
    s32 arg6
)
{
    FileTransferDescriptor *state;

    gFile_dwTransferState &= ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
    if ((gFile_dwTransferState & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
        (gFile_dwTransferState & FILE_TRANSFER_FLAG_SECTOR_RANGE)) {
        File_StopRangeTransfer();
    }

    state = &gFile_SecondaryTransferDescriptor;
    File_InitTransferDescriptor(
        state, arg0, arg1, arg2, arg3, callback, initial_phase_index, arg6
    );
    gFile_dwTransferState |= FILE_TRANSFER_STATE_SECONDARY_PENDING;
    return state;
}

/* Counts the current phase down one sector and fires its callback at zero. */
void File_StepTransferPhase(FileTransferDescriptor *object)
{
    object->phase_remaining -= FILE_SECTOR_SIZE;
    if (object->phase_remaining <= 0) {
        object->phase_size = 0;
        if (object->phase_callback != 0) {
            s32 count = object->phase_index++;

            CALL32(FileTransferCallback, object->phase_callback)(object, count);
        }
        object->phase_remaining = object->phase_size;
    }
}
