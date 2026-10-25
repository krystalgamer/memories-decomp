#include "../types.h"
#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_result_load_package_stage.h"
#include "../unmatched.h"
#include "display_resource_banks.h"
#include "file_transfer_buffer.h"

void DuelResult_LoadPackageStage(FileTransferDescriptor *d, s32 mode)
{
    s32 one;
    s32 c;
    s32 m;
    s32 m2;
    s32 n;
    s32 t0;
    s32 u0;
    s32 t1;
    s32 t2;
    s32 v0;
    s32 v1;

    one = 1;
    switch (mode) {
    case 0:
        do {
            m = 0xFFDDFFFF;
        } while (0);
        do {
            n = 0x18000;
        } while (0);
        /* Stored through the members' addresses with the other signedness
           (and s32 for the u32 phase_size), so fold leaves them as non-struct
           stores. Spelling any of the four as a member store changes 8 to 22
           words of this case. */
        *(s16 *)&d->field_30.h.counter = 0x1C0;
        *(s16 *)&d->field_30.h.field_32 = 0x100;
        t0 = gFile_dwTransferState;
        *(u16 *)&d->w = 0x40;
        gFile_dwTransferState = t0 & m;
        u0 = gFile_dwTransferState;
        *(s32 *)&d->phase_size = n;
        gFile_dwTransferState = u0 | 0x10000;
        d->transfer_mode = 2;
        v0 = gFile_TransferBufferBase;
        do {
            d->h = 0x10;
        } while (0);
        d->sector_destination = v0;
        d->alternate_sector_destination = v0 + FILE_SECTOR_SIZE;
        break;

    case 1:
        m2 = 0xFFDCFFFF;
        d->phase_size = FILE_SECTOR_SIZE;
        t1 = gFile_dwTransferState;
        v1 = gFile_TransferBufferBase;
        gFile_dwTransferState = t1 & m2;
        d->alternate_sector_destination = v1;
        d->sector_destination = v1;
        goto join;

    case 2:
#ifdef VERSION_EUROPE
        /* The European image goes to (0x280, 0xD4). */
        d->x = 0x280;
        d->y = 0xD4;
        d->h = mode;
        c = gFile_TransferBufferBase;
        d->w = 0x100;
#else
        d->y = 0xF4;
        d->h = mode;
        c = gFile_TransferBufferBase;
        d->x = 0;
        d->w = 0x100;
#endif
        LoadImage2((RECT *)d, (u32 *)c);
        m2 = 0xFFDCFFFF;
        d->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        d->sector_destination = (s32)gDisplay_abResourceBank0;
        t2 = gFile_dwTransferState;
        d->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState = t2 & m2;
join:
        d->transfer_mode = one;
        break;
    }
}
