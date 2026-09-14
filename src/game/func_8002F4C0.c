#include "../types.h"
#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "func_8002F4C0.h"
#include "../unmatched.h"

void func_8002F4C0(FileTransferDescriptor *d, s32 mode)
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
        t0 = D_8009B0F4;
        *(u16 *)&d->w = 0x40;
        D_8009B0F4 = t0 & m;
        u0 = D_8009B0F4;
        *(s32 *)&d->phase_size = n;
        D_8009B0F4 = u0 | 0x10000;
        d->done = 2;
        v0 = D_8009B118;
        do {
            d->h = 0x10;
        } while (0);
        d->value_08 = v0;
        d->value_0C = v0 + FILE_SECTOR_SIZE;
        break;

    case 1:
        m2 = 0xFFDCFFFF;
        d->phase_size = FILE_SECTOR_SIZE;
        t1 = D_8009B0F4;
        v1 = D_8009B118;
        D_8009B0F4 = t1 & m2;
        d->value_0C = v1;
        d->value_08 = v1;
        goto join;

    case 2:
        d->y = 0xF4;
        d->h = mode;
        c = D_8009B118;
        d->x = 0;
        d->w = 0x100;
        LoadImage2((RECT *)d, (u32 *)c);
        m2 = 0xFFDCFFFF;
        d->value_0C = (s32)D_801AF000;
        d->value_08 = (s32)D_801AF000;
        t2 = D_8009B0F4;
        d->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 = t2 & m2;
join:
        d->done = one;
        break;
    }
}
