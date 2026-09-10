#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "../unmatched.h"
#include "../ygo_types.h"
#include "func_8002FB78.h"

extern s32 D_80010000;
extern u8 D_801A8000[];

void func_8002FB78(FileTransferDescriptor *p, s32 mode)
{
    s32 v_0;
    s32 v_1;
    s32 v_2;
    s32 v_3;
    s32 w;
    s32 t;
    s32 m;
    s32 c;

    switch (mode) {
    case 0:
        *(s16 *)&p->field_30.h.counter = 0x340;
        *(s16 *)((u8 *)p + 4) = 0x40;
        m = 0xFFDDFFFF;
        v_0 = D_8009B0F4;
        *(s16 *)((u8 *)p + 6) = 0x10;
        v_0 &= m;
        D_8009B0F4 = v_0;
        w = D_8009B0F4;
        *(s16 *)((u8 *)p + 0x32) = 0;
        w |= 0x10000;
        D_8009B0F4 = w;
        p->done = 2;
        t = (p->value_08 = D_8009B118);
        *(s32 *)&p->mode = 0x8000;
        p->value_0C = t + 0x800;
        break;

    case 1:
        *(s32 *)&p->mode = 0x800;
        m = 0xFFDCFFFF;
        v_1 = D_8009B0F4;
        t = D_8009B118;
        v_1 &= m;
        D_8009B0F4 = v_1;
        p->value_0C = t;
        p->value_08 = t;
        p->done = 1;
        break;

    case 2:
        c = D_8009B118;
        p->x = 0x100;
        p->y = 0xF0;
        p->w = 0x100;
        p->h = 1;
        LoadImage2((RECT *)p, (u32 *)c);
        *(s32 *)((u8 *)p + 0xC) = (s32)D_801A8000;
        *(s32 *)((u8 *)p + 8) = (s32)D_801A8000;
        m = 0xFFDCFFFF;
        v_2 = D_8009B0F4;
        *(s32 *)((u8 *)p + 0x1C) = 0x1000;
        v_2 &= m;
        D_8009B0F4 = v_2;
        p->done = 1;
        break;

    case 3:
        *(s32 *)&p->mode = 0xF000;
        m = 0xFFDCFFFF;
        v_3 = D_8009B0F4;
        t = D_80010000;
        v_3 &= m;
        D_8009B0F4 = v_3;
        p->value_0C = t;
        p->value_08 = t;
        p->done = 1;
        break;
    }
}
