#include "../types.h"
#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "../unmatched.h"

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
    v = D_8009B0F4;
    f = 0x40;
    do {
        p->field_30.h.counter = 0;
    } while (0);
    p->w = f;
    D_8009B0F4 = v & m;
    w = D_8009B0F4;
    m = 0x10000;
    do {
        p->phase_size = m;
        D_8009B0F4 = w | m;
        p->done = 2;
        d = D_8009B118;
        p->h = 0x10;
    } while (0);
    p->value_08 = d;
    p->value_0C = d + FILE_SECTOR_SIZE;
    return;

m1:
    p->phase_size = FILE_SECTOR_SIZE;
    D_8009B0F4 = D_8009B0F4 & 0xFFDCFFFF;
    p->value_0C = D_8009B118;
    p->value_08 = D_8009B118;
    goto tail;

m2:
    do {
        hun = 0x100;
    } while (0);
    /* The RECT halves are stored through the members' addresses as u16.
     * A plain member store, or an s16 cast that fold turns back into one,
     * is an in-struct reference and is reordered against the D_8009B118
     * load; retail keeps these in source order around LoadImage2. */
    *(u16 *)&p->y = 0xF0;
    e = D_8009B118;
    *(u16 *)&p->x = hun;
    *(u16 *)&p->w = hun;
    *(u16 *)&p->h = 4;
    LoadImage2((RECT *)p, (u32 *)e);
    p->value_0C = (u32)D_801AF000;
    p->value_08 = (u32)D_801AF000;
    x = D_8009B0F4;
    p->phase_size = 0x800;
    D_8009B0F4 = x & 0xFFDCFFFF;

tail:
    p->done = one;
}
