#include "../types.h"
#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_result_load_outro_package_stage.h"
#include "../ygo_types.h"
#include "../unmatched.h"
#include "display_resource_banks.h"
#include "file_transfer_buffer.h"

void DuelResult_LoadOutroPackageStage(FileTransferDescriptor *p, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    s32 e;
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
        p->transfer_mode = 2;
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
#ifdef VERSION_EUROPE
    /* The European build uploads the row at (640, 216). */
    p->x = 0x280;
    p->y = 0xD8;
    p->w = 0x100;
    e = gFile_TransferBufferBase;
#else
    p->y = 0xF8;
    p->w = 0x100;
    e = gFile_TransferBufferBase;
    p->x = 0;
#endif
    p->h = 4;
    LoadImage2((RECT *)p, (u32 *)e);
    p->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
    p->sector_destination = (s32)gDisplay_abResourceBank0;
    x = gFile_dwTransferState;
    p->phase_size = FILE_SECTOR_SIZE;
    gFile_dwTransferState = x & 0xFFDCFFFF;

tail:
    p->transfer_mode = one;
}
