/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 4 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/func_8001944C.c.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../game/color_constants.h"
#include "../game/display_object.h"
#include "../game/duel_display.h"
#include "../game/duel_card_staging.h"
#include "../game/func_8001944C.h"
#include "../game/graphics_frame.h"

extern s32 gGraphics_bActiveBuffer __attribute__((section(".data")));
#define gGraphics_bActiveBuffer (*(u8 *)&gGraphics_bActiveBuffer)

void func_8001944C(DisplayObject *o)
{
    register u8 *buf __asm__("$16");
    /* base reuses the loop pointer's register once the loop is dead. The pin
       is what orders the two %hi materialisations after the loop: without it
       the rectangle's high half is allocated first and lands in $3. */
    register u8 *base __asm__("$5");
    u8 *q;
    register u16 *p __asm__("$5");
    register s32 n __asm__("$6");
    s32 xoff;

    while (IsIdleGPU(10) != 0) {
        ;
    }

    xoff = 0;
    if (gGraphics_bActiveBuffer == 0) {
        xoff = 0x140;
    }

    buf = D_8015C424;
    D_800E9D70[0].x = *(s16 *)&o->field_30.h.field_30 + xoff;
    D_800E9D70[0].y = *(s16 *)&o->field_30.h.field_32;
    D_800E9D70[0].w = DUEL_CARD_READBACK_WIDTH_WORDS;
    D_800E9D70[0].h = DUEL_CARD_READBACK_HEIGHT;
    StoreImage2(&D_800E9D70[0], (u32 *)buf);

    p = (u16 *)buf;
    n = DUEL_CARD_READBACK_WORD_COUNT;
    do {
        *p = *p | COLOR_BGR555_STP_MASK;
        p++;
        n--;
    } while (n != 0);

    base = D_8015C424;
    q = base + 0x8000;
    *(u16 *)(q + 0x565C) = 0;
    *(u16 *)(q + 0x565E) = 0;
    *(u16 *)(q + 0x554A) = 0;
    *(u16 *)(q + 0x5548) = 0;
    *(u16 *)(q + 0x5546) = 0;
    *(u16 *)(q + 0x5430) = 0;
    *(u16 *)(base + 0x22E) = 0;
    *(u16 *)(base + 0x118) = 0;
    *(u16 *)(base + 0x114) = 0;
    *(u16 *)(base + 0x116) = 0;
    *(u16 *)(base + 0x2) = 0;

    /* Written here rather than between the width and height stores where it
       lands: the second scheduling pass sinks it, and having it early is what
       gives this symbol's high half the lower register. */
    *(u16 *)D_8015C424 = 0;
    D_800E9D70[0].x = 0x140;
    D_800E9D70[0].y = 0x100;
    D_800E9D70[0].w = DUEL_CARD_READBACK_WIDTH_WORDS;
    D_800E9D70[0].h = DUEL_CARD_READBACK_HEIGHT;
    LoadImage2(&D_800E9D70[0], (u32 *)D_8015C424);
}
