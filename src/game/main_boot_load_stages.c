#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "main_boot_load_stages.h"
#include "display_asset_banks.h"
#include "../unmatched.h"
#include "display_resource_banks.h"
#include "file_transfer_buffer.h"
#ifdef VERSION_EUROPE
#include "high_memory_addresses.h"
#include "european_text_bank_views.h"
#endif

/* Main_RunBootSequence queues the boot image with Main_LoadBootImageStage
   and the boot package with Main_LoadBootPackageStage. Both declarations
   belong to main_boot_load_stages.h. */

void Main_LoadBootPackageStage(FileTransferDescriptor *obj, s32 stage) {
#ifdef VERSION_JAPAN
    /* The Japanese boot package is transferred in three stages rather than
       four: a 32-sector first stage at 0x2C0, and a last stage that uploads
       the image and then points at gFile_pScreenBankBase, folding the US stages 2
       and 3 together. */
    switch (stage) {
    case 0: {
        u32 m;
        s32 w;

        m = 0xFFDDFFFF;
        obj->field_30.h.counter = 0x2C0;
        w = 0x40;
        obj->field_30.h.field_32 = 0;
        obj->w = w;
        gFile_dwTransferState &= m;
        m = 0x10000;
        obj->phase_size = m;
        gFile_dwTransferState |= m;
        obj->done = 2;
        obj->h = 0x10;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;
    }

    case 1:
        obj->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->done = 1;
        break;

    case 2:
        obj->x = 0x200;
        obj->y = 0xF8;
        obj->w = 0x100;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *)gFile_TransferBufferBase);
        obj->phase_size = 3 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = (s32)gFile_pScreenBankBase;
        obj->sector_destination = (s32)gFile_pScreenBankBase;
        obj->done = 1;
        break;
    }
#elif defined(VERSION_EUROPE)
    /* The European boot package takes eight stages: three header stages
       (0x280, 0x2C0 and 0x300 in the counter), the US image stages with the
       image at x 0x280, and three last stages that land in the European
       text banks. */
    switch (stage) {
    case 0: {
        u32 cleared;
        u32 loaded;
        u32 enabled;

        obj->field_30.h.counter = 0x280;
        obj->w = 0x40;
        obj->h = 0x10;
        cleared = gFile_dwTransferState & 0xFFDDFFFF;
        gFile_dwTransferState = cleared;
        do { loaded = gFile_dwTransferState; } while (0);
        do { enabled = 0x10000; } while (0);
        obj->field_30.h.field_32 = 0;
        gFile_dwTransferState = loaded | enabled;
        obj->done = 2;
        obj->phase_size = 12 * FILE_SECTOR_SIZE;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;
    }

    case 1: {
        u32 cleared;
        u32 loaded;
        u32 enabled;

        obj->field_30.h.counter = 0x2C0;
        obj->w = 0x40;
        obj->h = 0x10;
        cleared = gFile_dwTransferState & 0xFFDDFFFF;
        gFile_dwTransferState = cleared;
        do { loaded = gFile_dwTransferState; } while (0);
        do { enabled = 0x10000; } while (0);
        obj->field_30.h.field_32 = 0;
        gFile_dwTransferState = loaded | enabled;
        obj->done = 2;
        obj->phase_size = 13 * FILE_SECTOR_SIZE;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;
    }

    case 2: {
        u32 cleared;
        u32 loaded;
        u32 enabled;

        obj->field_30.h.counter = 0x300;
        obj->w = 0x40;
        obj->h = 0x10;
        cleared = gFile_dwTransferState & 0xFFDDFFFF;
        gFile_dwTransferState = cleared;
        do { loaded = gFile_dwTransferState; } while (0);
        do { enabled = 0x10000; } while (0);
        obj->field_30.h.field_32 = 0;
        gFile_dwTransferState = loaded | enabled;
        obj->done = 2;
        obj->phase_size = 13 * FILE_SECTOR_SIZE;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;
    }

    case 3:
        obj->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->done = 1;
        break;

    case 4:
        obj->x = 0x280;
        obj->y = 0xF8;
        obj->w = 0x100;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *)gFile_TransferBufferBase);
        obj->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE;
        obj->sector_destination = gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE;
        obj->done = 1;
        break;

    case 5:
        obj->x = 0x280;
        obj->y = 0xC0;
        obj->w = 0x10;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *)(gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE));
        obj->phase_size = 30 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = (s32)gFile_pBootTextStage5Destination;
        obj->sector_destination = (s32)gFile_pBootTextStage5Destination;
        obj->done = 1;
        break;

    case 6:
        obj->phase_size = 32 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = (s32)gFile_pBootTextStage6Destination;
        obj->sector_destination = (s32)gFile_pBootTextStage6Destination;
        obj->done = 1;
        break;

    case 7:
        obj->phase_size = 7 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = (s32)gFile_pBootTextStage7Destination;
        obj->sector_destination = (s32)gFile_pBootTextStage7Destination;
        obj->done = 1;
        break;
    }
#else
    switch (stage) {
    case 0:
        obj->field_30.h.counter = 0x280;
        obj->field_30.h.field_32 = 0;
        obj->w = 0x40;
        obj->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        obj->phase_size = 48 * FILE_SECTOR_SIZE;
        gFile_dwTransferState |= 0x10000;
        obj->done = 2;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        obj->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->done = 1;
        break;

    case 2:
        obj->x = 0x200;
        obj->y = 0xF8;
        obj->w = 0x100;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *)gFile_TransferBufferBase);
        obj->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE;
        obj->sector_destination = gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE;
        obj->done = 1;
        break;

    case 3:
        obj->x = 0x280;
        obj->y = 0xE8;
        obj->w = 0x10;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *)(gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE));
        obj->phase_size = 3 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = (s32)gFile_pScreenBankBase;
        obj->sector_destination = (s32)gFile_pScreenBankBase;
        obj->done = 1;
        break;
    }
#endif
}

#ifdef VERSION_EUROPE
/* The European boot image takes four stages: the header stage counts 0xC0,
   the image goes to (0x280, 0xE0), and a fourth stage reads three sectors
   to gFile_pScreenBankBase. */
void Main_LoadBootImageStage(FileTransferDescriptor *p, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    s32 x;
    s32 m;
    s32 d;
    s32 f;

    one = 1;

    if (mode == one) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        return;
    }
    if (mode == 2) {
        goto m2;
    }
    if (mode == 3) {
        goto m3;
    }
    return;

m0:
    m = 0xFFDDFFFF;
    p->field_30.h.counter = 0xC0;
    p->field_30.h.field_32 = 0x100;
    v = gFile_dwTransferState;
    f = 0x40;
    p->w = f;
    gFile_dwTransferState = v & m;
    w = gFile_dwTransferState;
    m = 0x10000;
    do {
        p->phase_size = m;
        gFile_dwTransferState = w | m;
        p->done = 2;
        d = gFile_TransferBufferBase;
        p->h = 0x10;
    } while (0);
    p->sector_destination = d;
    p->alternate_sector_destination = d + FILE_SECTOR_SIZE;
    return;

m1:
    p->phase_size = FILE_SECTOR_SIZE;
    gFile_dwTransferState = gFile_dwTransferState & 0xFFDCFFFF;
    p->alternate_sector_destination = gFile_TransferBufferBase;
    p->sector_destination = gFile_TransferBufferBase;
    goto tail;

m2:
    p->x = 0x280;
    p->y = 0xE0;
    p->w = 0x100;
    p->h = 4;
    LoadImage2((RECT *)p, (u32 *)gFile_TransferBufferBase);
    p->alternate_sector_destination = (u32)gDisplay_abResourceBank0;
    p->sector_destination = (u32)gDisplay_abResourceBank0;
    x = gFile_dwTransferState;
    p->phase_size = 0x800;
    gFile_dwTransferState = x & 0xFFDCFFFF;
    p->done = one;
    return;

m3:
    p->phase_size = 3 * FILE_SECTOR_SIZE;
    gFile_dwTransferState = gFile_dwTransferState & 0xFFDCFFFF;
    p->alternate_sector_destination = (u32)gFile_pScreenBankBase;
    p->sector_destination = (u32)gFile_pScreenBankBase;

tail:
    p->done = one;
}
#else
void Main_LoadBootImageStage(FileTransferDescriptor *p, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    s32 e;
    s32 x;
    s32 m;
    s32 d;
    s32 f;
    s32 hun;

    one = 1;

    if (mode == one) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        return;
    }
    if (mode == 2) {
        goto m2;
    }
    return;

m0:
    m = 0xFFDDFFFF;
    p->field_30.h.field_32 = 0x100;
    v = gFile_dwTransferState;
    f = 0x40;
    do {
        p->field_30.h.counter = 0;
    } while (0);
    p->w = f;
    gFile_dwTransferState = v & m;
    w = gFile_dwTransferState;
    m = 0x10000;
    do {
        p->phase_size = m;
        gFile_dwTransferState = w | m;
        p->done = 2;
        d = gFile_TransferBufferBase;
        p->h = 0x10;
    } while (0);
    p->sector_destination = d;
    p->alternate_sector_destination = d + FILE_SECTOR_SIZE;
    return;

m1:
    p->phase_size = FILE_SECTOR_SIZE;
    gFile_dwTransferState = gFile_dwTransferState & 0xFFDCFFFF;
    p->alternate_sector_destination = gFile_TransferBufferBase;
    p->sector_destination = gFile_TransferBufferBase;
    goto tail;

m2:
    do {
        hun = 0x100;
    } while (0);
    /* The RECT halves are stored through the members' addresses as u16.
     * A plain member store, or an s16 cast that fold turns back into one,
     * is an in-struct reference and is reordered against the gFile_TransferBufferBase
     * load; retail keeps these in source order around LoadImage2. */
    *(u16 *)&p->y = 0xF0;
    e = gFile_TransferBufferBase;
    *(u16 *)&p->x = hun;
    *(u16 *)&p->w = hun;
    *(u16 *)&p->h = 4;
    LoadImage2((RECT *)p, (u32 *)e);
    p->alternate_sector_destination = (u32)gDisplay_abResourceBank0;
    p->sector_destination = (u32)gDisplay_abResourceBank0;
    x = gFile_dwTransferState;
    p->phase_size = 0x800;
    gFile_dwTransferState = x & 0xFFDCFFFF;

tail:
    p->done = one;
}
#endif
