#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "func_80020BE4.h"
#include "../ygo_types.h"
#include "../unmatched.h"

void func_80020BE4(FileTransferDescriptor *p, s32 mode)
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
        p->mode = m;
        D_8009B0F4 = w | m;
        p->done = 2;
        d = D_8009B118;
        p->h = 0x10;
    } while (0);
    p->value_08 = d;
    p->value_0C = d + 0x800;
    return;

m1:
    p->mode = 0x800;
    D_8009B0F4 = D_8009B0F4 & 0xFFDCFFFF;
    p->value_0C = D_8009B118;
    p->value_08 = D_8009B118;
    goto tail;

m2:
    p->y = 0xF8;
    p->w = 0x100;
    e = D_8009B118;
    p->x = 0;
    p->h = 4;
    LoadImage2((RECT *)p, (u32 *)e);
    p->value_0C = (s32)D_801AF000;
    p->value_08 = (s32)D_801AF000;
    x = D_8009B0F4;
    p->mode = 0x800;
    D_8009B0F4 = x & 0xFFDCFFFF;

tail:
    p->done = one;
}
