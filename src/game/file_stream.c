#include "../types.h"
#include "file_constants.h"
#include "file_cd_helpers.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../unmatched.h"
#include "file_transfer_buffer.h"

void File_InitTransferState(s32 value)
{
    gFile_TransferBufferBase = value;
    D_8009B110 = 0;
    D_8009B108 = 0;
    gFile_dwTransferState = 0;
    gFile_RangePauseCallback = 0;
    gFile_RangeReadCallback = 0;
    gFile_dwCancelRequestState = 0;
    gFile_wTransferStepFlags = 0;
    gFile_BeforePrimaryRequestCallback = 0;
    D_8009B12C = 0;
    gFile_ServiceCallGate = 0;
    gFile_ServiceReentryLatch = 0;
    gFile_CommandRetryCount = 0;
    while (DsInit() == 0) {
    }
    D_8009B0FC = 1;
}

void File_GetPosition(s32 *output, const char *path)
{
    DslFILE file;

    while (DsSearchFile(&file, (char *)path) == 0) {
    }
    *output = CdPosToInt_8007E710((const CdlLOC *)&file.pos);
}

void File_SetTransferLocation(
    FileTransferDescriptor *transfer,
    s32 file_index,
    s32 sector_offset,
    s32 vertical
)
{
    transfer->total_bytes = vertical;
    file_index &= FILE_TRANSFER_FILE_INDEX_MASK;
    if (vertical < 0)
        transfer->total_bytes = -(vertical << FILE_SECTOR_SHIFT);

    if (sector_offset < 0) {
        transfer->file_bytes = 0;
        transfer->absolute_lba = -sector_offset;
    } else {
        register volatile s32 *lbas = gFile_anLba;

        transfer->file_bytes = sector_offset << FILE_SECTOR_SHIFT;
        transfer->absolute_lba = lbas[file_index] + sector_offset;
    }
}

FileTransferDescriptor *File_InitTransferDescriptor(
    FileTransferDescriptor *transfer,
    s32 flags,
    u8 *source,
    s32 sector,
    s32 vertical,
    FileTransferCallback callback,
    s32 initial_phase_index,
    s32 length
)
{
    transfer->loader_argument = source;
    File_SetTransferLocation(transfer, flags, sector, -vertical);
    transfer->transfer_mode = 1;
    transfer->substate = 0;
    transfer->buffer_index = 0;
    transfer->phase_size = 0;
    transfer->phase_callback = callback;
    transfer->phase_index = initial_phase_index;
    if (length) {
        if (flags & 0x1000000) {
            transfer->direct_destination = length;
        } else {
            transfer->phase_size = transfer->total_bytes;
            if (length < 0) {
                transfer->transfer_mode = 1;
                transfer->alternate_sector_destination = length;
                transfer->sector_destination = length;
            } else {
                flags |= 0x10000;
                transfer->transfer_mode = 2;
                transfer->field_30.h.field_32 = ((u32)length) >> 16;
                transfer->field_30.h.counter = length;
                transfer->w = FILE_TRANSFER_DEFAULT_IMAGE_WORD_WIDTH;
                transfer->h = FILE_TRANSFER_DEFAULT_IMAGE_HEIGHT;
                transfer->sector_destination = gFile_TransferBufferBase;
                transfer->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
            }
        }
    }
    transfer->status_flags = flags;
    return transfer;
}

FileTransferDescriptor *File_RequestSecondarySeek(s32 file_index, s32 sector_offset)
{
    FileTransferDescriptor *transfer;

    if (gFile_dwTransferState & FILE_TRANSFER_STATE_SECONDARY_PENDING)
        return 0;

    transfer = &gFile_SecondaryTransferDescriptor;
    File_SetTransferLocation(
        transfer, file_index & FILE_TRANSFER_FILE_INDEX_MASK, sector_offset, 0
    );
    transfer->transfer_mode = 0;
    transfer->status_flags = 0x00100000;
    gFile_dwTransferState |= FILE_TRANSFER_STATE_SECONDARY_PENDING;
    return transfer;
}
