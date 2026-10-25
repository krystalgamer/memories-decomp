/* Shared card-list graphics setup and recent-acquisition maintenance.
   Trade and Build Deck use the blocking graphics loader; the editor also
   prunes recent IDs whose current chest quantity is zero. The transfer has
   four stages in NA/JP and five in Europe. */

#define D_8009B118_IN_DATA

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "card_constants.h"
#include "graphics_frame.h"
#include "image_transfer_rects.h"
#include "card_list_setup.h"
#ifdef VERSION_EUROPE
#include "duel_effect_resource_setup.h"
#endif
#include "../ygo_types.h"
#include "../unmatched.h"
#include "file_transfer_buffer.h"

#ifdef VERSION_EUROPE
void CardList_LoadSharedGraphicsStage(FileTransferDescriptor *p, s32 mode)
{
    s32 one;
    s32 w;
    s32 v;
    s32 t0;
    s32 t1;
    s32 c;
    s32 n;
    s32 u;
    s16 *g;
    s32 m;

    switch (mode) {
    case 0:
        m = 0xFFDDFFFF;
        *(s16 *)&p->field_30.h.counter = 0x300;
        *(s16 *)&p->field_30.h.field_32 = 0x100;
        *(u16 *)&p->w = 0x40;
        t0 = gFile_dwTransferState_abs;
        *(u16 *)&p->h = 0x10;
        gFile_dwTransferState_abs = t0 & m;
        gFile_dwTransferState_abs = gFile_dwTransferState_abs | 0x10000;
        p->transfer_mode = 2;
        v = gFile_TransferBufferBase;
        w = 64 * FILE_SECTOR_SIZE;
        *(s32 *)&p->phase_size = w;
        p->sector_destination = v;
        v += FILE_SECTOR_SIZE;
        p->alternate_sector_destination = v;
        break;

    case 1:
        *(s32 *)&p->phase_size = 5 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        p->alternate_sector_destination = gFile_TransferBufferBase;
        p->sector_destination = gFile_TransferBufferBase;
        p->transfer_mode = 1;
        break;

    case 2:
        g = (s16 *)gGraphics_aImageTransferRects;
        g[0] = 0x380;
        g[1] = 0x160;
        c = 0x40;
        g[2] = c;
        one = 0x10;
        g[3] = one;
        LoadImage2((RECT *)g,
                   (u32 *)(gFile_TransferBufferBase +
                           gText_bLanguageIndex * FILE_SECTOR_SIZE));

        m = 0xFFDDFFFF;
        t1 = gFile_dwTransferState_abs;
        *(s16 *)&p->field_30.h.counter = 0x340;
        *(s16 *)&p->field_30.h.field_32 = 0;
        *(u16 *)&p->w = c;
        *(u16 *)&p->h = one;
        gFile_dwTransferState_abs = t1 & m;
        u = gFile_dwTransferState_abs;
        n = 0x10000;
        gFile_dwTransferState_abs = u | n;
        p->transfer_mode = 2;
        v = gFile_TransferBufferBase;
        w = 8 * FILE_SECTOR_SIZE;
        *(s32 *)&p->phase_size = w;
        p->sector_destination = v;
        v += FILE_SECTOR_SIZE;
        p->alternate_sector_destination = v;
        break;

    case 3:
        *(s32 *)&p->phase_size = 4 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        p->alternate_sector_destination = gFile_TransferBufferBase;
        p->sector_destination = gFile_TransferBufferBase;
        p->transfer_mode = 1;
        break;

    case 4:
        g = (s16 *)gGraphics_aImageTransferRects;
        g[0] = 0x280;
        g[1] = 0xE0;
        g[2] = 0x100;
        g[3] = 0x10;
        LoadImage2((RECT *)g, (u32 *)gFile_TransferBufferBase);
        break;
    }
}
#else
void CardList_LoadSharedGraphicsStage(FileTransferDescriptor *p, s32 mode) {
    s32 one;
    s32 w;
    s32 v;
    s32 t0;
    s32 t1;
    s32 c;
    s32 n;
    s32 u;
    s16 *g;
    s32 m;
    s32 m2v;

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
    *(s16 *)&p->field_30.h.counter = 0x300;
    *(s16 *)&p->field_30.h.field_32 = 0x100;
    *(u16 *)&p->w = 0x40;
    t0 = gFile_dwTransferState_abs;
    *(u16 *)&p->h = 0x10;
    gFile_dwTransferState_abs = t0 & m;
    gFile_dwTransferState_abs = gFile_dwTransferState_abs | 0x10000;
    p->transfer_mode = 2;
    v = gFile_TransferBufferBase;
    w = 0x20000;
    *(s32 *)&p->phase_size = w;
    p->sector_destination = v;
    v += 0x800;
    p->alternate_sector_destination = v;
    return;

m1:
    m = 0xFFDDFFFF;
    *(s16 *)&p->field_30.h.counter = 0x340;
    *(u16 *)&p->w = 0x40;
    t1 = gFile_dwTransferState_abs;
    *(u16 *)&p->h = 0x10;
    gFile_dwTransferState_abs = t1 & m;
    u = gFile_dwTransferState_abs;
    n = 0x10000;
    p->field_30.h.field_32 = 0;
    gFile_dwTransferState_abs = u | n;
    p->transfer_mode = 2;
    v = gFile_TransferBufferBase;
    w = 0x4000;
    *(s32 *)&p->phase_size = w;
    p->sector_destination = v;
    v += 0x800;
    p->alternate_sector_destination = v;
    return;

m2:
    m2v = 0xFFDCFFFF;
    *(s32 *)&p->phase_size = 4 * FILE_SECTOR_SIZE;
    gFile_dwTransferState_abs = gFile_dwTransferState_abs & m2v;
    p->alternate_sector_destination = gFile_TransferBufferBase;
    p->sector_destination = gFile_TransferBufferBase;
    p->transfer_mode = 1;
    return;

m3:
    g = (s16 *)gGraphics_aImageTransferRects;
    c = 0x100;
    g[0] = c;
    g[1] = 0xF0;
    g[2] = c;
    g[3] = 0x10;
    LoadImage2((RECT *)g, (u32 *)gFile_TransferBufferBase);
}
#endif

#ifndef CARD_LIST_GRAPHICS_REQUEST_FILE_ID
#define CARD_LIST_GRAPHICS_REQUEST_FILE_ID 0x2189
#endif
#ifndef CARD_LIST_GRAPHICS_REQUEST_SECTOR_COUNT
#define CARD_LIST_GRAPHICS_REQUEST_SECTOR_COUNT 0x4C
#endif

void CardList_LoadSharedGraphics(void)
{
    File_RequestAsyncTransfer(
        0, 0, CARD_LIST_GRAPHICS_REQUEST_FILE_ID, CARD_LIST_GRAPHICS_REQUEST_SECTOR_COUNT,
        CardList_LoadSharedGraphicsStage, 0, 0);
    File_WaitForTransfers();
}

/*
 * Reward-drop compaction. It matches all 136 bytes under split-address
 * lowering, which keeps the table's high half in a temporary before
 * publishing its address; without splitting, the first two words change.
 */
void Library_PruneRecentCardDrops(void)
{
    s16 *source;
    u8 *base;
    s32 i;
    u32 current;
    s16 *destination;

    source = gDuel_awRecentCardDrops;
    base = (u8 *)((u32)source - DUEL_RECENT_DROPS_CHEST_DISTANCE);
    i = DUEL_RECENT_CARD_DROP_COUNT - 1;
    current = (u32)(source + DUEL_RECENT_CARD_DROP_COUNT - 1);
    for (; i >= 0; i--, current -= sizeof(*source)) {
        if (*(s16 *)current != 0 && base[*(s16 *)current - 1] == 0)
            *(s16 *)current = 0;
    }
    destination = source;
    for (i = 0; i < DUEL_RECENT_CARD_DROP_COUNT; i++, source++) {
        if (*source) {
            if (source != destination) {
                *destination = *source;
                *source = 0;
            }
            destination++;
        }
    }
}
