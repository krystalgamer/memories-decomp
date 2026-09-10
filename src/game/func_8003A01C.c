#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "../unmatched.h"
#include "func_8003A01C.h"

void func_8003A01C(FileTransferDescriptor *p, s32 mode)
{
    s32 v_0;
    s32 v_1;
    s32 v_2;
    s32 w;
    s32 n;
    s32 b;
    s32 t;
    s32 c;
    s32 k;
    s32 m;
    s32 m2;

    switch (mode) {
    case 0:
        m = 0xFFDDFFFF;
        v_0 = D_8009B0F4;
        n = p->position;
        p->field_30.h.field_32 = 0x100;
        p->w = 0x40;
        p->mode = 0x18000;
        p->h = 0x10;
        v_0 &= m;
        D_8009B0F4 = v_0;
        w = D_8009B0F4;
        w |= 0x10000;
        D_8009B0F4 = w;
        p->done = 2;
        b = D_8009B118;
        p->field_30.h.counter = 0x340 - n * 192;
        p->value_08 = b;
        p->value_0C = b + 0x800;
        break;

    case 1:
        m2 = 0xFFDCFFFF;
        *(s32 *)&p->mode = 0x800;
        v_1 = D_8009B0F4;
        t = D_8009B118;
        v_1 &= m2;
        D_8009B0F4 = v_1;
        p->value_0C = t;
        p->value_08 = t;
        p->done = 1;
        break;

    case 2:
        p->x = 0x200;
        k = p->position;
        p->h = 2;
        c = D_8009B118;
        p->w = 0x100;
        p->y = k * 2 + 0xF0;
        LoadImage2((RECT *)p, (u32 *)c);
        m2 = 0xFFDCFFFF;
        *(s32 *)&p->mode = 0x800;
        v_2 = D_8009B0F4;
        t = (s32)p->callback_data;
        v_2 &= m2;
        D_8009B0F4 = v_2;
        p->value_0C = t;
        p->value_08 = t;
        p->done = 1;
        break;
    }
}
