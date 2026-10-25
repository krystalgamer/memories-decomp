/*
 * The CD file-transfer runtime: the primary and secondary descriptor setup,
 * the ready callback and the command-completion callbacks
 * (0x80013B04-0x80014294), then transfer activation, stepping and the
 * per-frame service through SD_ConfigureTransferPhase and
 * File_RequestSecondarySlot.
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
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "file_constants.h"
#include "file_cd_helpers.h"
#include "../external_funcs.h"
#include "../unmatched.h"
#include "file_transfer_buffer.h"

FileTransferDescriptor *File_TryStartPrimaryTransfer(s32 index, s32 offset)
{
    if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
         gFile_dwCancelRequestState) == 0) {
        gFile_PrimaryTransferDescriptor.absolute_lba =
            gFile_anLba[index] + offset;
        gFile_PrimaryTransferDescriptor.transfer_mode = 0;
        gFile_dwTransferState = 0x100010;
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

    gFile_dwTransferState &= ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
    p = &gFile_SecondaryTransferDescriptor;
    if (gFile_dwTransferState & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) {
        if (gFile_dwTransferState & FILE_TRANSFER_FLAG_SECTOR_RANGE) {
            File_StopRangeTransfer();
        }
    }
    p->absolute_lba = a;
    p->direct_destination = b;
    callback_data = (u8 *)&p->callback_data;
    callback_data[0] = (u8)c;
    callback_data[1] = (u8)d;
    p->transfer_mode = 4;
    p->status_flags = FILE_TRANSFER_FLAG_SECTOR_RANGE;
    gFile_dwTransferState |= FILE_TRANSFER_STATE_SECONDARY_PENDING;
    return p;
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
        if ((gFile_dwTransferState & 0x200000) == 0) {
            if ((gFile_dwTransferState & 0x40000000) == 0) {
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
        if ((gFile_dwTransferState & 0x40000000) == 0) {
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
        if ((gFile_dwTransferState & 0x20000) != 0) {
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
        if ((gFile_dwTransferState & 0x40000000) == 0) {
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
    FileTransferDescriptor *p;
    void (*cb)(void);
    void (*cb2)(void);
    u8 *q;
    s32 n;
    s32 m;

    p = &gFile_PrimaryTransferDescriptor;
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
                p->field_30.word = 0;
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
            cb = gFile_RangePauseCallback;
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
            q = &gFile_FilterParamsByte + 1;
            /* In sector-range mode the word at 0x38 carries the
               CdlSetfilter channel (low byte) and file (next byte). */
            *q = ((u8 *)&p->callback_data)[0];
            q[-1] = ((u8 *)&p->callback_data)[1];
            if (DsCommand(DslSetfilter, (u8 *)(q - 1),
                          (DslCB)File_HandleSetFilterEvent, -1) <= 0) {
                return;
            }
            gFile_dwTransferState = gFile_dwTransferState | FILE_TRANSFER_STATE_COMMAND_BUSY;
            return;
        case 4:
            CdIntToPos_8007E600(
                p->absolute_lba, CDL_LOC_VIEW(gFile_TargetLoc)
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
            cb2 = gFile_RangeReadCallback;
            if (cb2 != 0) {
                cb2();
            }
        case 6:
            gFile_RangeWaitCountdown = gFile_RangeWaitCountdown - 1;
            if ((s16)gFile_RangeWaitCountdown > 0) {
                if ((s32)p->field_30.word < p->direct_destination) {
                    return;
                }
            }
            gFile_wTransferStepFlags = gFile_wTransferStepFlags & 0x3FFC;
            cb = gFile_RangePauseCallback;
            gFile_wTransferStepFlags = gFile_wTransferStepFlags | 2;
call_back:
            if (cb != 0) {
                cb();
            }
        }
        return;
    }
    if (p->transfer_mode == 5) {
        switch (p->substate) {
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
    CdIntToPos_8007E600(p->absolute_lba, CDL_LOC_VIEW(gFile_TargetLoc));
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
        if (p->phase_callback != 0) {
            CALL32(FileTransferCallback, p->phase_callback)(p, p->phase_index++);
        }
        p->phase_remaining = p->phase_size;
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
void SD_ConfigureTransferPhase(FileTransferDescriptor *object, s32 mode)
{
    FileRequestSlot *shared;
    s32 value;
    s32 base;
    s32 position;
    void (*callback)(void);

    shared = &gFile_ActiveRequestSlot;
    if (mode == 1)
        goto reduced;
    if (mode >= 2)
        goto high;
    if (mode == 0)
        goto full;
    return;
high:
    if (mode == 2)
        goto tail;
    return;
full:
    if (shared->field_14 == 0) {
        object->phase_index = 2;
        goto reduced;
    }
    object->transfer_mode = 3;
    base = gFile_TransferBufferBase;
    object->sector_destination = base;
    object->alternate_sector_destination = base + FILE_SECTOR_SIZE;
    object->field_30.word = shared->field_0C;
    value = shared->field_14;
    object->phase_size = value;
    goto fix;
reduced:
    if (shared->field_18 == 0)
        goto tail;
    position = shared->field_10;
    gFile_dwTransferState &= 0xFFDCFFFF;
    object->alternate_sector_destination = position;
    object->sector_destination = position;
    object->transfer_mode = 1;
    value = shared->field_18;
    object->phase_size = value;
fix:
    if (value < 0)
        object->phase_size = -(value << FILE_SECTOR_SHIFT);
    return;
tail:
    callback = gFile_SecondaryPayloadCompleteCallback;
    if (callback != 0)
        callback();
}

s32 File_RequestSecondarySlot(FileRequestSlot *p, u8 *q) {
    u8 *e;
    u8 *r;
    FileTransferCallback f;
    s32 a;
    s32 b;
    s32 c;
    s32 n;
    s32 m;
    s32 v;
    s32 w;
    s32 t;

    if (p == (FileRequestSlot *)0) {
        return gFile_dwTransferState & FILE_TRANSFER_STATE_SECONDARY_PENDING;
    }

    a = p->field_14;
    b = p->field_18;
    c = p->field_1C;

    if ((a | b | c) == 0) {
        return (s32)File_RequestSecondarySeek(p->field_00, p->field_04);
    }

    if (c != 0) {
        if (c < 0) {
            gFile_wTransferStepFlags = gFile_wTransferStepFlags & 0x3FFC;
            gFile_wTransferStepFlags = gFile_wTransferStepFlags | 2;
            return 1;
        }
        n = *(s32 *)gFile_nMasterXaLba + p->field_04;
        return (s32)File_RequestSecondaryRangeTransfer(
            n, n + c, p->field_1F, p->field_1E
        );
    }

    a = a + b;
    if (a == 0) {
        return 0;
    }

    {
        w = p->field_00;
        /* Borrowed local: `m`'s real assignment is three lines down and this
         * one is dead, but it ties the negation's pseudo to m's allocation
         * and rotates $s2/$s3/$s4 into retail's order. A fresh name, six
         * declaration orders and four statement orders are all 6. */
        m = a;
        t = -m;
        r = D_801D4200_raw;
        *(FileRequestSlot *)(r + 0x20) = *p;
        /* The post-copy reload goes through the member's address as u32:
           a plain p->field_04 (or an s32 cast, which fold turns back into
           one) exchanges the request pointer's and sector-offset local's
           allocation and changes 24 words. */
        m = *(u32 *)&p->field_04;
        gFile_dwTransferState =
            gFile_dwTransferState & ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
        v = w | 0x1400000;
        f = SD_ConfigureTransferPhase_callback;

        if ((gFile_dwTransferState & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) != 0) {
            if ((gFile_dwTransferState & FILE_TRANSFER_FLAG_SECTOR_RANGE) != 0) {
                File_StopRangeTransfer();
            }
        }

        File_InitTransferDescriptor(
            &gFile_SecondaryTransferDescriptor, v, q, m, t, f, 0, (s32)p
        );
        e = (u8 *)&gFile_SecondaryTransferDescriptor;
        gFile_dwTransferState =
            gFile_dwTransferState | FILE_TRANSFER_STATE_SECONDARY_PENDING;
    }

    return (s32)e;
}
