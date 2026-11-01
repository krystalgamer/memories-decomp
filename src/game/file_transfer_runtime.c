/*
 * The CD file-transfer runtime: file-state initialization, the position query
 * and the transfer setup helpers (0x80013898-0x80013B04); the primary and
 * secondary descriptor setup, the ready callback and the command-completion
 * callbacks (0x80013B04-0x80014294); transfer activation, stepping and the
 * per-frame service through SD_ConfigureTransferPhase and
 * File_RequestSecondarySlot; and last the descriptor request, cancellation and
 * phase-stepping helpers (0x80014E1C-0x800151B0).
 *
 * FILE_TRANSFER_BYTE_VIEWS only adds the byte views D_801D4200_raw and
 * gFile_FilterParamsByte from file_transfer.h, so it is selected for the whole unit.
 */
#define FILE_TRANSFER_BYTE_VIEWS

#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libspu.h"

/* The loader's zero-initialized state, defined here in address order before
 * file_transfer.h declares it, so the compiler emits it in this order and the
 * unit's small zero-initialized data reproduces the retail layout (#2602). */
volatile s32 gFile_ServiceReentryLatch;
u16 gFile_RangeWaitCountdown;
void (*G32 gFile_RangeReadCallback)(void);
volatile u32 D_8009B0F4;
u32 *G32 gFile_ReadySourceCursor;
s32 D_8009B0FC;
volatile u16 gFile_wStepState;
char gFile_TargetLoc[4];
u8 D_8009B108;
void (*G32 gFile_BeforePrimaryRequestCallback)(void);
u8 D_8009B110;
volatile u16 gFile_wTransferStepFlags;
u8 gFile_ReadyEventCount;
s32 D_8009B118;
char gFile_FilterParams[1];
void (*G32 gFile_RangePauseCallback)(void);
volatile u16 gFile_ServiceCallGate;
void (*G32 gFile_SecondaryPayloadCompleteCallback)(void);
s32 D_8009B12C;
s32 gFile_CommandRetryCount;
u32 D_8009B134;
s32 gFile_ReadyDataEventCount;
s32 D_8009B13C;

#include "file_transfer.h"
#include "file_transfer_state.h"
#include "file_constants.h"
#include "file_cd_helpers.h"
#include "../external_funcs.h"
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
    s32 size
)
{
    transfer->total_bytes = size;
    file_index &= FILE_TRANSFER_FILE_INDEX_MASK;
    if (size < 0)
        transfer->total_bytes = -(size << FILE_SECTOR_SHIFT);

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
    u8 *loader_argument,
    s32 sector_offset,
    s32 size,
    FileTransferCallback callback,
    s32 initial_phase_index,
    s32 target
)
{
    transfer->loader_argument = loader_argument;
    File_SetTransferLocation(transfer, flags, sector_offset, -size);
    transfer->transfer_mode = 1;
    transfer->substate = 0;
    transfer->buffer_index = 0;
    transfer->phase_size = 0;
    transfer->phase_callback = callback;
    transfer->phase_index = initial_phase_index;
    if (target) {
        if (flags & 0x1000000) {
            transfer->direct_destination = target;
        } else {
            transfer->phase_size = transfer->total_bytes;
            if (target < 0) {
                transfer->transfer_mode = 1;
                transfer->alternate_sector_destination = target;
                transfer->sector_destination = target;
            } else {
                flags |= 0x10000;
                transfer->transfer_mode = 2;
                transfer->field_30.h.field_32 = ((u32)target) >> 16;
                transfer->field_30.h.counter = target;
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

FileTransferDescriptor *File_TryStartPrimaryTransfer(s32 file_index, s32 sector_offset)
{
    if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
         gFile_dwCancelRequestState) == 0) {
        gFile_PrimaryTransferDescriptor.absolute_lba =
            gFile_anLba[file_index] + sector_offset;
        gFile_PrimaryTransferDescriptor.transfer_mode = 0;
        gFile_dwTransferState = 0x100010;
        return &gFile_PrimaryTransferDescriptor;
    }
    return 0;
}

FileTransferDescriptor *File_RequestSecondaryRangeTransfer(
    s32 start_lba,
    s32 end_lba,
    s32 filter_channel,
    s32 filter_file
)
{
    FileTransferDescriptor *transfer;
    u8 *callback_data;

    gFile_dwTransferState &= ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
    transfer = &gFile_SecondaryTransferDescriptor;
    if (gFile_dwTransferState & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) {
        if (gFile_dwTransferState & FILE_TRANSFER_FLAG_SECTOR_RANGE) {
            File_StopRangeTransfer();
        }
    }
    transfer->absolute_lba = start_lba;
    transfer->direct_destination = end_lba;
    callback_data = (u8 *)&transfer->callback_data;
    callback_data[0] = (u8)filter_channel;
    callback_data[1] = (u8)filter_file;
    transfer->transfer_mode = 4;
    transfer->status_flags = FILE_TRANSFER_FLAG_SECTOR_RANGE;
    gFile_dwTransferState |= FILE_TRANSFER_STATE_SECONDARY_PENDING;
    return transfer;
}

void File_TransferReadyCallback(s32 arg)
{
    s32 event;
    s32 remaining;
    FileTransferDescriptor *transfer;
    FileTransferDescriptor *p;
    FileTransferDescriptor *q;
    FileTransferDescriptor *r;
    FileTransferDescriptor *image;
    FileTransferDescriptor *counter;
    FileTransferDescriptor *t;
    u8 *dst;
    u32 *src;
    u32 word;
    s32 i;
    s32 n;
    s32 image_y;

    event = arg & 0xFF;
    gFile_ReadyEventCount++;
    if (event != 1) {
        return;
    }
    gFile_ReadyDataEventCount = (s32)((u32)gFile_ReadyDataEventCount + 1);
    p = gFile_ReadyTransferDescriptor;
    switch (p->transfer_mode) {
    case 1:
        if ((gFile_dwTransferState & FILE_TRANSFER_FLAG_SKIP_RAM_WRITE) == 0) {
            if ((gFile_dwTransferState & FILE_TRANSFER_FLAG_MEMORY_SOURCE) == 0) {
                CdGetSector(
                    (void *)p->sector_destination,
                    FILE_SECTOR_SIZE / sizeof(u32)
                );
            } else {
                i = 0;
                t = p;
                src = gFile_ReadySourceCursor;
                do {
                    word = src[i];
                    ((u32 *)t->sector_destination)[i] = word;
                    i++;
                } while (i < (s32)(FILE_SECTOR_SIZE / sizeof(u32)));
            }
            gFile_ReadyTransferDescriptor->sector_destination += FILE_SECTOR_SIZE;
        }
        transfer = gFile_ReadyTransferDescriptor;
        remaining = transfer->total_bytes;
        gFile_ReadySourceCursor += FILE_SECTOR_SIZE / sizeof(u32);
        remaining -= FILE_SECTOR_SIZE;
        transfer->total_bytes = remaining;
        if (remaining <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        q = gFile_ReadyTransferDescriptor;
        q->phase_remaining -= FILE_SECTOR_SIZE;
        if (q->phase_remaining <= 0) {
            q->phase_size = 0;
            if (q->phase_callback != 0) {
                CALL32(FileTransferCallback, q->phase_callback)(q, q->phase_index++);
            }
            q->phase_remaining = q->phase_size;
        }
        if (gFile_ReadyTransferDescriptor->total_bytes <= 0) {
            goto clear;
        }
        return;
    case 2:
        /* The image phase alternates between the sector_destination and alternate_sector_destination
           buffers. Retail adds the descriptor base first (`addu v0,a0,v0`);
           `(&p->sector_destination)[i]` puts the scaled index first instead, so the
           pair is still selected by an integer sum with the member-derived
           base offset. */
        dst = (u8 *)*(u32 *)(
            (u32)p + ((p->buffer_index & 1) << 2) +
            (u32)&((FileTransferDescriptor *)0)->sector_destination
        );
        if ((gFile_dwTransferState & FILE_TRANSFER_FLAG_MEMORY_SOURCE) == 0) {
            CdGetSector(dst, FILE_SECTOR_SIZE / sizeof(u32));
        } else {
            src = gFile_ReadySourceCursor;
            for (i = 0; i < (s32)(FILE_SECTOR_SIZE / sizeof(u32)); ++i) {
                ((u32 *)dst)[i] = src[i];
            }
            gFile_ReadySourceCursor += FILE_SECTOR_SIZE / sizeof(u32);
        }
        gFile_ReadyTransferDescriptor->total_bytes -= FILE_SECTOR_SIZE;
        if (gFile_ReadyTransferDescriptor->total_bytes <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        image = gFile_ReadyTransferDescriptor;
        image->x = image->field_30.h.counter;
        image->y = image->field_30.h.field_32;
        while (LoadImage2((RECT *)gFile_ReadyTransferDescriptor, (u32 *)dst)) {
        }
        if ((gFile_dwTransferState & FILE_TRANSFER_FLAG_IMAGE_ADVANCE_X) != 0) {
            gFile_ReadyTransferDescriptor->field_30.h.counter += 0x40;
        } else {
            r = gFile_ReadyTransferDescriptor;
            image_y = r->field_30.h.field_32 + 0x10;
            r->field_30.h.field_32 = image_y;
            if ((image_y & 0xFF) == 0) {
                r->field_30.h.field_32 = (image_y ^ 0x100) & 0x100;
                r->field_30.h.counter += 0x40;
            }
        }
        q = gFile_ReadyTransferDescriptor;
        q->phase_remaining -= FILE_SECTOR_SIZE;
        if (q->phase_remaining > 0) {
            goto counter;
        }
        goto step;
    case 3:
        n = FILE_SECTOR_SIZE;
        dst = (u8 *)p->sector_destination;
        if (p->phase_remaining < n) {
            n = p->phase_remaining;
        }
        if ((gFile_dwTransferState & FILE_TRANSFER_FLAG_MEMORY_SOURCE) == 0) {
            CdGetSector(dst, n / 4);
        } else {
            src = gFile_ReadySourceCursor;
            for (i = 0; i < n / 4; ++i) {
                ((u32 *)dst)[i] = src[i];
            }
            gFile_ReadySourceCursor = (u32 *)((u8 *)gFile_ReadySourceCursor + n);
        }
        gFile_ReadyTransferDescriptor->total_bytes -= FILE_SECTOR_SIZE;
        if (gFile_ReadyTransferDescriptor->total_bytes <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        SpuSetTransferStartAddr(gFile_ReadyTransferDescriptor->field_30.word);
        SpuWrite(dst, (u32)n);
        q = gFile_ReadyTransferDescriptor;
        q->field_30.word += n;
        q->phase_remaining -= FILE_SECTOR_SIZE;
        if (q->phase_remaining > 0) {
            goto counter;
        }
    step:
        q->phase_size = 0;
        if (q->phase_callback != 0) {
            CALL32(FileTransferCallback, q->phase_callback)(q, q->phase_index++);
        }
        q->phase_remaining = q->phase_size;
    counter:
        counter = gFile_ReadyTransferDescriptor;
        counter->buffer_index++;
        if (counter->total_bytes > 0) {
            return;
        }
    clear:
        gFile_dwTransferState &= ~0x100;
        return;
    }
}

/* Command-completion callbacks for the same asynchronous disc-transfer
   runtime. Each re-issues its command on event 5 and clears the busy bit on
   event 2. */
void File_HandleReadEvent(u8 event)
{
    if (event == 5) {
        gFile_CommandRetryCount++;
        DsPacket(0xA0, (DslLOC *)gFile_TargetLoc, 6, (DslCB)File_HandleReadEvent, -1);
    } else if (event == 2) {
        DsReadySystemMode(1);
        DsStartReadySystem((DslRCB)File_TransferReadyCallback, -1);
        gFile_ReadyEventCount = 0;
        gFile_ReadyDataEventCount = 0;
        gFile_dwTransferState &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void File_HandleSeekEvent(u8 event)
{
    if (event == 5) {
        gFile_CommandRetryCount++;
        DsPacket(0xA0, (DslLOC *)gFile_TargetLoc, 0x15, (DslCB)File_HandleSeekEvent, -1);
    } else if (event == 2) {
        gFile_dwTransferState &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void File_HandleCancelPauseEvent(u8 event)
{
    if (event == 5) {
        gFile_CommandRetryCount++;
        DsCommand(9, 0, (DslCB)File_HandleCancelPauseEvent, -1);
    } else if (event == 2) {
        gFile_PrimaryTransferDescriptor.substate = 1;
        gFile_dwTransferState &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void File_HandleRangePauseEvent(s32 event)
{
    event &= 0xFF;
    if (event == 5) {
        gFile_CommandRetryCount++;
        DsCommand(9, 0, (DslCB)File_HandleRangePauseEvent, -1);
    } else if (event == 2) {
        gFile_wStepState = event;
        gFile_dwTransferState &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

#define CDL_LOC_VIEW(location) ((CdlLOC *)(location))
#define DSL_LOC_VIEW(location) ((DslLOC *)(location))

/* The rest of the asynchronous disc-transfer runtime: the later
   command-completion callbacks, secondary-to-primary activation, transfer
   advancement and request dispatch. The nine functions are contiguous and
   communicate through the shared descriptors, request slots and gFile_dwTransferState
   state word. The ready-sector callback and first four command callbacks are
   earlier definitions in this same grouped unit. */

void File_HandleSetFilterEvent(u8 event)
{
    if (event == DslDiskError) {
        gFile_CommandRetryCount++;
        DsCommand(DslSetfilter, (u8 *)gFile_FilterParams,
                  (DslCB)File_HandleSetFilterEvent, -1);
    } else if (event == DslComplete) {
        gFile_wStepState = 4;
        gFile_dwTransferState &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void File_HandleRangeReadEvent(u8 event)
{
    if (event == 5) {
        gFile_CommandRetryCount++;
        DsPacket(
            0x4A, DSL_LOC_VIEW(gFile_TargetLoc), 0x1B,
            (DslCB)File_HandleRangeReadEvent, -1);
    } else if (event == 2) {
        gFile_wStepState = 5;
        gFile_dwTransferState |= FILE_TRANSFER_STATE_POSITION_QUERY_PENDING;
        gFile_dwTransferState &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void File_HandleLogicalPositionEvent(u8 event, u8 *result)
{
    s32 value;
    s32 *destination;

    if (event == DslComplete) {
        destination = (s32 *)&gFile_PrimaryTransferDescriptor.field_30;
        value = CdPosToInt_8007E710((const CdlLOC *)result);
        if (value > 0)
            *destination = value;
        gFile_dwTransferState &= ~FILE_TRANSFER_STATE_POSITION_QUERY_BUSY;
    }
}

void File_ActivateTransfer(void)
{
    *(FileTransferDescriptorWords *)&gFile_PrimaryTransferDescriptor =
        *(FileTransferDescriptorWords *)&gFile_SecondaryTransferDescriptor;
    *(FileRequestSlot *)D_801D4200_raw =
        *(FileRequestSlot *)(D_801D4200_raw + 32);
    if (gFile_PrimaryTransferDescriptor.transfer_mode == 4)
        gFile_wTransferStepFlags |= 1;
    gFile_dwTransferState =
        gFile_PrimaryTransferDescriptor.status_flags |
        FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}

void File_FinishActiveTransfer(void){gFile_dwTransferState&=FILE_TRANSFER_STATE_SECONDARY_PENDING|FILE_TRANSFER_STATE_PRIMARY_REQUEST_LOCKED;if((gFile_dwTransferState&FILE_TRANSFER_STATE_SECONDARY_PENDING)&&!(gFile_dwTransferState&FILE_TRANSFER_STATE_PRIMARY_REQUEST_LOCKED)){File_ActivateTransfer();if(gFile_dwCancelRequestState){int v=0x80;if((gFile_dwTransferState&FILE_TRANSFER_STATE_PRIMARY_ACTIVE)&&(gFile_dwTransferState&FILE_TRANSFER_FLAG_SECTOR_RANGE))File_StopRangeTransfer();gFile_dwCancelRequestState=v;}}else gFile_dwCancelRequestState=0;}

void File_StepActiveTransfer(void)
{
    FileTransferDescriptor *transfer;
    void (*pause_callback)(void);
    void (*read_callback)(void);
    u8 *filter_channel;
    s32 n;
    s32 m;

    transfer = &gFile_PrimaryTransferDescriptor;
    if (gFile_dwTransferState & FILE_TRANSFER_STATE_POSITION_QUERY_PENDING) {
        if (!(gFile_dwTransferState & FILE_TRANSFER_STATE_POSITION_QUERY_BUSY)) {
            if (DsCommand(DslGetlocL, 0,
                          (DslCB)File_HandleLogicalPositionEvent, 0) > 0) {
                gFile_dwTransferState =
                    gFile_dwTransferState | FILE_TRANSFER_STATE_POSITION_QUERY_BUSY;
            }
        }
    }
    if (gFile_dwTransferState & FILE_TRANSFER_STATE_COMMAND_BUSY) {
        return;
    }
    if (gFile_dwTransferState & FILE_TRANSFER_FLAG_SECTOR_RANGE) {
        if (!(gFile_wTransferStepFlags & 0x8000)) {
            gFile_wTransferStepFlags = gFile_wTransferStepFlags | 0x8000;
            if ((gFile_wTransferStepFlags & 3) == 0) {
                gFile_wTransferStepFlags = 0;
                goto call_144B8;
            }
            if (!(gFile_wTransferStepFlags & 0x2000)) {
                gFile_wStepState = 0;
            }
            if (gFile_wTransferStepFlags & 2) {
                gFile_wTransferStepFlags = gFile_wTransferStepFlags & 0xFFFE;
            } else {
                transfer->field_30.word = 0;
            }
        }
        switch (gFile_wStepState) {
        case 0:
            gFile_wTransferStepFlags = gFile_wTransferStepFlags | 0x2000;
            gFile_wStepState = 1;
        case 1:
            if (DsCommand(9, 0, (DslCB)File_HandleRangePauseEvent, -1) <= 0) {
                return;
            }
            gFile_dwTransferState = gFile_dwTransferState | FILE_TRANSFER_STATE_COMMAND_BUSY;
            pause_callback = gFile_RangePauseCallback;
            goto call_back;
        case 2:
            gFile_wTransferStepFlags = gFile_wTransferStepFlags & 0xDFFF;
            if (gFile_wTransferStepFlags & 1) {
                goto set_state3;
            }
            gFile_wTransferStepFlags = 0;
call_144B8:
            File_FinishActiveTransfer();
            return;
set_state3:
            gFile_wStepState = 3;
        case 3:
            gFile_wTransferStepFlags = gFile_wTransferStepFlags | 0x1000;
            filter_channel = &gFile_FilterParamsByte + 1;
            /* In sector-range mode the word at 0x38 carries the
               CdlSetfilter channel (low byte) and file (next byte). */
            *filter_channel = ((u8 *)&transfer->callback_data)[0];
            filter_channel[-1] = ((u8 *)&transfer->callback_data)[1];
            if (DsCommand(DslSetfilter, (u8 *)(filter_channel - 1),
                          (DslCB)File_HandleSetFilterEvent, -1) <= 0) {
                return;
            }
            gFile_dwTransferState = gFile_dwTransferState | FILE_TRANSFER_STATE_COMMAND_BUSY;
            return;
        case 4:
            CdIntToPos_8007E600(
                transfer->absolute_lba, CDL_LOC_VIEW(gFile_TargetLoc)
            );
            if (DsPacket(
                    0x4A, DSL_LOC_VIEW(gFile_TargetLoc), 0x1B,
                    (DslCB)File_HandleRangeReadEvent, -1) <= 0) {
                return;
            }
            gFile_dwTransferState = gFile_dwTransferState | FILE_TRANSFER_STATE_COMMAND_BUSY;
            return;
        case 5:
            gFile_wStepState = 6;
            gFile_RangeWaitCountdown = 0x258;
            gFile_wTransferStepFlags = gFile_wTransferStepFlags & 0xEFFF;
            gFile_wTransferStepFlags = gFile_wTransferStepFlags | 0x4000;
            read_callback = gFile_RangeReadCallback;
            if (read_callback != 0) {
                read_callback();
            }
        case 6:
            gFile_RangeWaitCountdown = gFile_RangeWaitCountdown - 1;
            if ((s16)gFile_RangeWaitCountdown > 0) {
                if ((s32)transfer->field_30.word < transfer->direct_destination) {
                    return;
                }
            }
            gFile_wTransferStepFlags = gFile_wTransferStepFlags & 0x3FFC;
            pause_callback = gFile_RangePauseCallback;
            gFile_wTransferStepFlags = gFile_wTransferStepFlags | 2;
call_back:
            if (pause_callback != 0) {
                pause_callback();
            }
        }
        return;
    }
    if (transfer->transfer_mode == 5) {
        switch (transfer->substate) {
        case 0:
            DsEndReadySystem();
            CdReadyCallback(0);
            if (DsCommand(9, 0, (DslCB)File_HandleCancelPauseEvent, -1) <= 0) {
                return;
            }
            gFile_dwTransferState = gFile_dwTransferState | FILE_TRANSFER_STATE_COMMAND_BUSY;
            return;
        case 1:
            DsEndReadySystem();
            CdReadyCallback(0);
            goto call_144B8;
        }
        return;
    }
    if (gFile_dwTransferState & 0x80) {
        if (gFile_dwTransferState & 0x100) {
            return;
        }
        goto call_144B8;
    }
    CdIntToPos_8007E600(transfer->absolute_lba, CDL_LOC_VIEW(gFile_TargetLoc));
    if (gFile_dwTransferState & 0x100000) {
        if ((s32)gFile_dwTransferState < 0) {
            goto call_144B8;
        }
        if (DsPacket(
                0xA0, DSL_LOC_VIEW(gFile_TargetLoc), 0x15,
                (DslCB)File_HandleSeekEvent, -1) <= 0) {
            return;
        }
        gFile_dwTransferState = gFile_dwTransferState | 0x480;
        return;
    }
    if (!(gFile_dwTransferState & 0x800000)) {
        gFile_dwTransferState = gFile_dwTransferState | 0x800000;
        if (transfer->phase_callback != 0) {
            CALL32(FileTransferCallback, transfer->phase_callback)(transfer, transfer->phase_index++);
        }
        transfer->phase_remaining = transfer->phase_size;
        return;
    }
    if (gFile_dwTransferState & 0x400000) {
        if (SpuIsTransferCompleted(0) == 0) {
            return;
        }
        gFile_dwTransferState = gFile_dwTransferState & 0xFFBFFFFF;
    }
    if ((s32)gFile_dwTransferState >= 0) {
        if (DsPacket(
                0xA0, DSL_LOC_VIEW(gFile_TargetLoc), 6,
                (DslCB)File_HandleReadEvent, -1) == 0) {
            return;
        }
        gFile_dwTransferState = gFile_dwTransferState | FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
    gFile_dwTransferState = gFile_dwTransferState | 0x180;
}

void File_ServiceTransfers(s32 arg0)
{
    if (gFile_ServiceCallGate != 0) {
        gFile_ServiceCallGate = 0;
        return;
    }
    gFile_ServiceCallGate = 1;
    if (gFile_ServiceReentryLatch != 0) {
        return;
    }
    gFile_ServiceReentryLatch = 1;
    if (!(gFile_dwTransferState & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
        (gFile_dwTransferState & FILE_TRANSFER_STATE_SECONDARY_PENDING)) {
        File_ActivateTransfer();
    }
    if (gFile_dwTransferState & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) {
        if (gFile_dwCancelRequestState != 0 && !(gFile_dwCancelRequestState & 0x40)) {
            gFile_dwCancelRequestState |= 0x40;
            gFile_PrimaryTransferDescriptor.transfer_mode = 5;
            gFile_PrimaryTransferDescriptor.substate = 0;
        }
        File_StepActiveTransfer();
    } else {
        gFile_dwCancelRequestState = 0;
    }
    gFile_ServiceReentryLatch = 0;
}

/* SD_ConfigureTransferPhase is a transfer-phase callback of the same family as
   Model_LoadAuxiliaryPackageStage and func_80057728: File_RequestSecondarySlot below installs it through
   File_InitTransferDescriptor's FileTransferCallback parameter, so its first
   argument is the descriptor that entry point fills in. It programs the same
   fields the other two do -- the sector_destination/alternate_sector_destination source window,
   phase_size, the word at field_30 and transfer_mode -- which is what its old private
   record named
   value_8, value_c, value_1c, value_30 and mode_46. */
void SD_ConfigureTransferPhase(FileTransferDescriptor *descriptor, s32 phase_index)
{
    FileRequestSlot *request;
    s32 phase_size;
    s32 sector_buffer;
    s32 ram_destination;
    void (*complete_callback)(void);

    request = &gFile_ActiveRequestSlot;
    if (phase_index == 1)
        goto reduced;
    if (phase_index >= 2)
        goto high;
    if (phase_index == 0)
        goto full;
    return;
high:
    if (phase_index == 2)
        goto tail;
    return;
full:
    if (request->spu_transfer_size == 0) {
        descriptor->phase_index = 2;
        goto reduced;
    }
    descriptor->transfer_mode = 3;
    sector_buffer = gFile_TransferBufferBase;
    descriptor->sector_destination = sector_buffer;
    descriptor->alternate_sector_destination = sector_buffer + FILE_SECTOR_SIZE;
    descriptor->field_30.word = request->spu_destination;
    phase_size = request->spu_transfer_size;
    descriptor->phase_size = phase_size;
    goto fix;
reduced:
    if (request->ram_transfer_size == 0)
        goto tail;
    ram_destination = request->ram_destination;
    gFile_dwTransferState &= 0xFFDCFFFF;
    descriptor->alternate_sector_destination = ram_destination;
    descriptor->sector_destination = ram_destination;
    descriptor->transfer_mode = 1;
    phase_size = request->ram_transfer_size;
    descriptor->phase_size = phase_size;
fix:
    if (phase_size < 0)
        descriptor->phase_size = -(phase_size << FILE_SECTOR_SHIFT);
    return;
tail:
    complete_callback = gFile_SecondaryPayloadCompleteCallback;
    if (complete_callback != 0)
        complete_callback();
}

s32 File_RequestSecondarySlot(FileRequestSlot *request, u8 *loader_argument) {
    u8 *descriptor_bytes;
    u8 *request_slots;
    FileTransferCallback phase_callback;
    s32 size;
    s32 ram_size;
    s32 range_sector_count;
    s32 start_lba;
    s32 sector_offset;
    s32 flags;
    s32 file_selector;
    s32 request_size;

    if (request == (FileRequestSlot *)0) {
        return gFile_dwTransferState & FILE_TRANSFER_STATE_SECONDARY_PENDING;
    }

    size = request->spu_transfer_size;
    ram_size = request->ram_transfer_size;
    range_sector_count = request->range_sector_count;

    if ((size | ram_size | range_sector_count) == 0) {
        return (s32)File_RequestSecondarySeek(request->file_selector, request->sector_offset);
    }

    if (range_sector_count != 0) {
        if (range_sector_count < 0) {
            gFile_wTransferStepFlags = gFile_wTransferStepFlags & 0x3FFC;
            gFile_wTransferStepFlags = gFile_wTransferStepFlags | 2;
            return 1;
        }
        start_lba = *(s32 *)gFile_nMasterXaLba + request->sector_offset;
        return (s32)File_RequestSecondaryRangeTransfer(
            start_lba, start_lba + range_sector_count, request->cd_filter_channel, request->cd_filter_file
        );
    }

    size = size + ram_size;
    if (size == 0) {
        return 0;
    }

    {
        file_selector = request->file_selector;
        /* Borrowed local: `sector_offset`'s real assignment is three lines down and this
         * one is dead, but it ties the negation's pseudo to sector_offset's allocation
         * and rotates $s2/$s3/$s4 into retail's order. A fresh name, six
         * declaration orders and four statement orders are all 6. */
        sector_offset = size;
        request_size = -sector_offset;
        request_slots = D_801D4200_raw;
        *(FileRequestSlot *)(request_slots + 0x20) = *request;
        /* The post-copy reload goes through the member's address as u32:
           a plain request->sector_offset (or an s32 cast, which fold turns back into
           one) exchanges the request pointer's and sector-offset local's
           allocation and changes 24 words. */
        sector_offset = *(u32 *)&request->sector_offset;
        gFile_dwTransferState =
            gFile_dwTransferState & ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
        flags = file_selector | 0x1400000;
        phase_callback = SD_ConfigureTransferPhase_callback;

        if ((gFile_dwTransferState & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) != 0) {
            if ((gFile_dwTransferState & FILE_TRANSFER_FLAG_SECTOR_RANGE) != 0) {
                File_StopRangeTransfer();
            }
        }

        File_InitTransferDescriptor(
            &gFile_SecondaryTransferDescriptor, flags, loader_argument, sector_offset, request_size, phase_callback, 0, (s32)request
        );
        descriptor_bytes = (u8 *)&gFile_SecondaryTransferDescriptor;
        gFile_dwTransferState =
            gFile_dwTransferState | FILE_TRANSFER_STATE_SECONDARY_PENDING;
    }

    return (s32)descriptor_bytes;
}

FileTransferDescriptor *File_RequestAsyncTransfer(
    s32 flags,
    u8 *loader_argument,
    s32 sector_offset,
    s32 size,
    FileTransferCallback callback,
    s32 initial_phase_index,
    s32 target
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
        flags, loader_argument, sector_offset, size, callback, initial_phase_index, target
    );
    result = &gFile_PrimaryTransferDescriptor;
out:
    gFile_dwTransferState =
        result->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    return result;
}

FileTransferDescriptor *File_TryRequestAsyncTransfer(
    s32 flags,
    u8 *loader_argument,
    s32 sector_offset,
    s32 size,
    FileTransferCallback callback,
    s32 initial_phase_index,
    s32 target
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
        flags, loader_argument, sector_offset, size, callback, initial_phase_index, target
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
    s32 flags,
    u8 *loader_argument,
    s32 sector_offset,
    s32 size,
    FileTransferCallback callback,
    s32 initial_phase_index,
    s32 target
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
        state, flags, loader_argument, sector_offset, size, callback, initial_phase_index, target
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
