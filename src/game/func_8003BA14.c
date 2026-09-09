#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

extern s32 D_8009B118;

void func_8003BA14(FileTransferDescriptor *object, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    s32 d;
    s32 m;
    s32 v2;
    s32 w2;
    s32 d2;
    s32 k;
    s32 c;
    s32 hun;
    s32 n;
    s32 x;
    u8 *y;

    /* Direct mode and y members change old-GCC commoning/register allocation. */
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
    do {
        k = 0x18000;
    } while (0);
    c = 0x100;
    object->field_30.h.counter = c;
    object->field_30.h.field_32 = c;
    v = D_8009B0F4;
    object->w = 0x40;
    D_8009B0F4 = v & m;
    w = D_8009B0F4;
    n = 0x10000;
    do {
        *(s32 *)&object->mode = k;
        D_8009B0F4 = w | n;
        object->done = 2;
        d = D_8009B118;
        object->h = 0x10;
    } while (0);
    object->value_08 = d;
    object->value_0C = d + 0x800;
    return;

m1:
    object->field_30.h.counter = 0x1C0;
    object->field_30.h.field_32 = 0x100;
    object->w = 0x40;
    v2 = D_8009B0F4;
    object->h = 0x10;
    D_8009B0F4 = v2 & 0xFFDDFFFF;
    w2 = D_8009B0F4;
    D_8009B0F4 = w2 | 0x10000;
    object->done = 2;
    d2 = D_8009B118;
    *(s32 *)&object->mode = 0x8000;
    object->value_08 = d2;
    object->value_0C = d2 + 0x800;
    return;

m2:
    m = 0xFFDCFFFF;
    *(s32 *)&object->mode = 0x800;
    x = D_8009B0F4 & m;
    y = (u8 *)D_8009B118;
    goto tail;

m3:
    do {
        hun = 0x100;
    } while (0);
    *(s16 *)((u8 *)object + 2) = 0xF0;
    k = D_8009B118;
    object->x = hun;
    object->w = hun;
    object->h = 4;
    LoadImage2((RECT *)object, (u32 *)k);
    m = 0xFFDCFFFF;
    *(s32 *)&object->mode = 0x7800;
    x = D_8009B0F4 & m;
    y = D_800101D8;

tail:
    D_8009B0F4 = x;
    object->value_0C = (s32)y;
    object->value_08 = (s32)y;
    object->done = one;
}
