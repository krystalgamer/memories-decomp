#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libspu.h"
#include "file_transfer.h"
#include "file_ready_sector.h"

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FILE_PRIMARY_TRANSFER)
FileTransferDescriptor *File_TryStartPrimaryTransfer(s32 index, s32 offset)
{
    if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
         D_8009B134) == 0) {
        gFile_PrimaryTransferDescriptor.absolute_lba =
            gFile_anLba[index] + offset;
        gFile_PrimaryTransferDescriptor.done = 0;
        D_8009B0F4 = 0x100010;
        return &gFile_PrimaryTransferDescriptor;
    }
    return 0;
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FILE_SECONDARY_RANGE_TRANSFER)
FileTransferDescriptor *File_RequestSecondaryRangeTransfer(
    s32 a,
    s32 b,
    s32 c,
    s32 d
)
{
    FileTransferDescriptor *p;
    u8 *callback_data;

    D_8009B0F4 &= ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
    p = &gFile_SecondaryTransferDescriptor;
    if (D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) {
        if (D_8009B0F4 & FILE_TRANSFER_FLAG_SECTOR_RANGE) {
            func_80015010();
        }
    }
    p->absolute_lba = a;
    p->direct_destination = b;
    callback_data = (u8 *)&p->callback_data;
    callback_data[0] = (u8)c;
    callback_data[1] = (u8)d;
    p->done = 4;
    p->status_flags = FILE_TRANSFER_FLAG_SECTOR_RANGE;
    D_8009B0F4 |= FILE_TRANSFER_STATE_SECONDARY_PENDING;
    return p;
}
#endif

#ifndef VERSION_JAPAN
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
    D_8009B114++;
    if (event != 1) {
        return;
    }
    D_8009B138 = (s32)((u32)D_8009B138 + 1);
    p = D_8009AF18;
    switch (p->done) {
    case 1:
        if ((D_8009B0F4 & 0x200000) == 0) {
            if ((D_8009B0F4 & 0x40000000) == 0) {
                CdGetSector(
                    (void *)p->value_08,
                    FILE_SECTOR_SIZE / sizeof(u32)
                );
            } else {
                i = 0;
                t = p;
                src = D_8009B0F8;
                do {
                    word = src[i];
                    ((u32 *)t->value_08)[i] = word;
                    i++;
                } while (i < (s32)(FILE_SECTOR_SIZE / sizeof(u32)));
            }
            D_8009AF18->value_08 += FILE_SECTOR_SIZE;
        }
        transfer = D_8009AF18;
        remaining = transfer->total_bytes;
        D_8009B0F8 += FILE_SECTOR_SIZE / sizeof(u32);
        remaining -= FILE_SECTOR_SIZE;
        transfer->total_bytes = remaining;
        if (remaining <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        q = D_8009AF18;
        q->phase_remaining -= FILE_SECTOR_SIZE;
        if (q->phase_remaining <= 0) {
            q->phase_size = 0;
            if (q->phase_callback != 0) {
                q->phase_callback(q, q->result++);
            }
            q->phase_remaining = q->phase_size;
        }
        if (D_8009AF18->total_bytes <= 0) {
            goto clear;
        }
        return;
    case 2:
        /* The image phase alternates between the value_08 and value_0C
           buffers. Retail adds the descriptor base first (`addu v0,a0,v0`);
           `(&p->value_08)[i]` puts the scaled index first instead, so the
           pair is still selected by an integer sum with the member-derived
           base offset. */
        dst = (u8 *)*(u32 *)(
            (u32)p + ((p->buffer_index & 1) << 2) +
            (u32)&((FileTransferDescriptor *)0)->value_08
        );
        if ((D_8009B0F4 & 0x40000000) == 0) {
            CdGetSector(dst, FILE_SECTOR_SIZE / sizeof(u32));
        } else {
            src = D_8009B0F8;
            for (i = 0; i < (s32)(FILE_SECTOR_SIZE / sizeof(u32)); ++i) {
                ((u32 *)dst)[i] = src[i];
            }
            D_8009B0F8 += FILE_SECTOR_SIZE / sizeof(u32);
        }
        D_8009AF18->total_bytes -= FILE_SECTOR_SIZE;
        if (D_8009AF18->total_bytes <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        image = D_8009AF18;
        image->x = image->field_30.h.counter;
        image->y = image->field_30.h.field_32;
        while (LoadImage2((RECT *)D_8009AF18, (u32 *)dst)) {
        }
        if ((D_8009B0F4 & 0x20000) != 0) {
            D_8009AF18->field_30.h.counter += 0x40;
        } else {
            r = D_8009AF18;
            image_y = r->field_30.h.field_32 + 0x10;
            r->field_30.h.field_32 = image_y;
            if ((image_y & 0xFF) == 0) {
                r->field_30.h.field_32 = (image_y ^ 0x100) & 0x100;
                r->field_30.h.counter += 0x40;
            }
        }
        q = D_8009AF18;
        q->phase_remaining -= FILE_SECTOR_SIZE;
        if (q->phase_remaining > 0) {
            goto counter;
        }
        goto step;
    case 3:
        n = FILE_SECTOR_SIZE;
        dst = (u8 *)p->value_08;
        if (p->phase_remaining < n) {
            n = p->phase_remaining;
        }
        if ((D_8009B0F4 & 0x40000000) == 0) {
            CdGetSector(dst, n / 4);
        } else {
            src = D_8009B0F8;
            for (i = 0; i < n / 4; ++i) {
                ((u32 *)dst)[i] = src[i];
            }
            D_8009B0F8 = (u32 *)((u8 *)D_8009B0F8 + n);
        }
        D_8009AF18->total_bytes -= FILE_SECTOR_SIZE;
        if (D_8009AF18->total_bytes <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        SpuSetTransferStartAddr(D_8009AF18->field_30.word);
        SpuWrite(dst, (u32)n);
        q = D_8009AF18;
        q->field_30.word += n;
        q->phase_remaining -= FILE_SECTOR_SIZE;
        if (q->phase_remaining > 0) {
            goto counter;
        }
    step:
        q->phase_size = 0;
        if (q->phase_callback != 0) {
            q->phase_callback(q, q->result++);
        }
        q->phase_remaining = q->phase_size;
    counter:
        counter = D_8009AF18;
        counter->buffer_index++;
        if (counter->total_bytes > 0) {
            return;
        }
    clear:
        D_8009B0F4 &= ~0x100;
        return;
    }
}

/* Command-completion callbacks for the same asynchronous disc-transfer
   runtime. Each re-issues its command on event 5 and clears the busy bit on
   event 2. */
void func_800140A0(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        DsPacket(0xA0, (DslLOC *)D_8009B104, 6, (DslCB)func_800140A0, -1);
    } else if (event == 2) {
        DsReadySystemMode(1);
        DsStartReadySystem((DslRCB)File_TransferReadyCallback, -1);
        D_8009B114 = 0;
        D_8009B138 = 0;
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void func_80014134(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        DsPacket(0xA0, (DslLOC *)D_8009B104, 0x15, (DslCB)func_80014134, -1);
    } else if (event == 2) {
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void func_800141A8(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        DsCommand(9, 0, (DslCB)func_800141A8, -1);
    } else if (event == 2) {
        gFile_PrimaryTransferDescriptor.substate = 1;
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FILE_COMMAND_CALLBACK)
void func_80014220(s32 event)
{
    event &= 0xFF;
    if (event == 5) {
        D_8009B130++;
        DsCommand(9, 0, (DslCB)func_80014220, -1);
    } else if (event == 2) {
        D_8009B100 = event;
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}
#endif
