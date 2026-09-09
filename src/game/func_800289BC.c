#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "duel_effect_resource_record.h"

extern s32 D_8009B118;
extern u8 D_800EA0E8[];

void func_800289BC(u8 *p, s32 mode)
{
    DuelEffectResourceRecord *e;
    RECT *rect;
    s32 b;
    s32 x;

    if (mode == 0) {
        *(s32 *)(p + 0x1C) = 0x3800;
        D_8009B0F4 &= 0xFFDCFFFF;
        *(s32 *)(p + 0xC) = D_8009B118;
        *(s32 *)(p + 8) = D_8009B118;
        p[0x46] = 1;
        return;
    }

    e = (DuelEffectResourceRecord *)(D_800EA0E8 + (*(s32 *)(p + 0x38) << 6));

    rect = &e->rects[0];
    b = D_8009B118;
    e->rects[0].w = 0x33;
    e->rects[0].h = 0x60;
    e->rects[0].x = e->src_x;
    e->rects[0].y = e->src_y;
    LoadImage(rect, (u32 *)b);

    rect = &e->rects[1];
    x = e->field_2C;
    b = D_8009B118 + 0x2640;
    e->rects[1].w = 0x100;
    e->rects[1].h = 1;
    e->rects[1].x = x;
    e->rects[1].y = e->field_2E;
    LoadImage(rect, (u32 *)b);

    rect = &e->rects[2];
    x = e->src_x;
    b = D_8009B118 + 0x2840;
    e->rects[2].w = 0x18;
    e->rects[2].h = 0xE;
    e->rects[2].x = x;
    e->rects[2].y = e->src_y + 0x60;
    LoadImage(rect, (u32 *)b);

    rect = &e->rects[3];
    b = D_8009B118 + 0x2AE0;
    e->rects[3].w = 8;
    e->rects[3].h = 0x58;
    e->rects[3].x = e->src_x + 0x38;
    e->rects[3].y = e->src_y;
    LoadImage(rect, (u32 *)b);
}
