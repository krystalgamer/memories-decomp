#define GRAPHICS_ACTIVE_BUFFER_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "color_constants.h"
#include "display_object.h"
#include "duel_display.h"
#include "duel_card_staging.h"
#include "func_8001944C.h"
#include "graphics_frame.h"

void func_8001944C(DisplayObject *o)
{
    u8 *buf;
    u8 *q;
    u16 *p;
    s32 xoff;

    while (IsIdleGPU(10) != 0) {
        ;
    }

    xoff = 0;
    if (gGraphics_bActiveBuffer == 0) {
        xoff = 0x140;
    }

    buf = D_8015C424;
    D_800E9D70[0].x = o->field_30.h.field_30 + xoff;
    D_800E9D70[0].y = o->field_30.h.field_32;
    D_800E9D70[0].w = DUEL_CARD_READBACK_WIDTH_WORDS;
    D_800E9D70[0].h = DUEL_CARD_READBACK_HEIGHT;
    StoreImage2(&D_800E9D70[0], (u32 *)buf);

    p = (u16 *)buf;
    xoff = DUEL_CARD_READBACK_WORD_COUNT;
    do {
        *p = *p | COLOR_BGR555_STP_MASK;
        p++;
        xoff--;
    } while (xoff != 0);

    p = (u16 *)D_8015C424;
    q = (u8 *)p + 0x8000;
    *(u16 *)(q + 0x565C) = 0;
    *(u16 *)(q + 0x565E) = 0;
    *(u16 *)(q + 0x554A) = 0;
    *(u16 *)(q + 0x5548) = 0;
    *(u16 *)(q + 0x5546) = 0;
    *(u16 *)(q + 0x5430) = 0;
    *(u16 *)((u8 *)p + 0x22E) = 0;
    *(u16 *)((u8 *)p + 0x118) = 0;
    *(u16 *)((u8 *)p + 0x114) = 0;
    *(u16 *)((u8 *)p + 0x116) = 0;
    *(u16 *)((u8 *)p + 0x2) = 0;

    *(u16 *)D_8015C424 = 0;
    D_800E9D70[0].x = 0x140;
    D_800E9D70[0].y = 0x100;
    D_800E9D70[0].w = DUEL_CARD_READBACK_WIDTH_WORDS;
    D_800E9D70[0].h = DUEL_CARD_READBACK_HEIGHT;
    LoadImage2(&D_800E9D70[0], (u32 *)D_8015C424);
}
