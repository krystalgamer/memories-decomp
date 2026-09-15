#include "../types.h"
#include "../game/graphics_frame.h"
#include "../game/library_runtime.h"
#include "../game/func_80029EB0.h"
#include "../game/card_constants.h"
#include "../psyq/libgs_abi_variants.h"

/* 0x800E9D9C is the second ordering-table pointer. ordering_tables.h declares
   it `GsOT *`; this unit reads it as a word, so the private spelling stays. */
extern s32 D_800E9D9C;

/*
 * Current best under gcc_2_8_1_g0_split: 268 instructions against 268, with
 * encoding distance 0. Compiled to an object and compared word for word with
 * the assembled target, 91 of 268 words differ, all placement and register
 * choices. Retail completes 0xF70130's ori only after reloading b,
 * materialises 0x8000000 before the first row store, and reloads b ahead of
 * the viewport read at the loop head. In the cursor tail it takes the
 * D_800EA1E8 address before the colour switch, loads 0xFF in the dispatch
 * branch's delay slot, and sets up the first GsSortGLine call's arguments
 * before the store that precedes it.
 */

/* Draws the scrolling card-list grid straight into the scratchpad primitive at
   0x1F800320. The first visible row comes from the viewport scroll divided by
   the 178-pixel row pitch; from there four rows of ten lines are walked, each
   line holding ten cells on the left at column 8 and, while the paired index
   is still below CARD_ID_END, ten more on the right at column 0xA8. A cell is drawn
   only when func_80029EB0 reports bit 0x80, tinted 0x808080 or 0x404040 on
   bit 0, and the run stops as soon as a line falls off the bottom of the
   screen. The tail then builds the cursor box at 0x1F800000, colouring it from
   the low seven bits of D_8009B09C through a four-way ramp, and draws its four
   edges with GsSortGLine. */
void func_80029EC4(void)
{
    u8 *p;
    u8 *q;
    u8 *pj;
    u8 *pk;
    s32 ot;
    s32 n;
    s32 idx;
    s32 i;
    s32 c;
    s32 j;
    s32 k;
    s32 r;
    s32 white;
    s32 grey;
    s32 a;
    s32 b;
    s32 y;
    s32 v;
    u8 *t;
    s32 m;
    u8 *tb;
    s32 e;
    s32 f;

    p = (u8 *)0x1F800320;
    n = (gGraphics_sViewportY - 8) / 178;
    ot = D_800E9D9C;
    if (n < 0) {
        return;
    }
    e = 0xE000C;
    f = 0xF70130;
    white = 0x808080;
    grey = 0x404040;
    a = n * 178 + 8;
    b = n * 25;
    *(u16 *)(p + 4) = 8;
    *(u32 *)(p + 8) = e;
    *(u32 *)(p + 0x10) = f;
    idx = b * 8;
    *(u16 *)(p + 6) = idx * 178 + 8;
    *(u16 *)(p + 0x12) = 0xF7;
    *(u16 *)(p + 0xE) = 0xF060;
    *(u32 *)p = 0x8000000;
    *(u16 *)(p + 0xC) = 0x1B;
    do {
        idx = b * 8;
        *(u16 *)(p + 6) = a;
        *(u16 *)(p + 6) = a - gGraphics_sViewportY;
        for (i = 0; i < 10; idx += 10, i++) {
            y = *(s16 *)(p + 6);
            j = idx + 1;
            tb = D_800EA1E8;
            if (y + *(u16 *)(p + 8) > 0) {
                if (y >= 0xF0) {
                    goto done;
                }
                k = idx + 0x65;
                c = 0;
                pk = &D_800EA1E8[k * 4];
                pj = &D_800EA1E8[j * 4];
                do {
                    r = func_80029EB0(tb, j);
                    if (r & 0x80) {
                        *(u32 *)(p + 0x14) = white;
                        if (r & 1) {
                            *(u32 *)(p + 0x14) = grey;
                        }
                        *(u16 *)(p + 4) = c + 8;
                        *(u16 *)(p + 0x10) = *(u16 *)(pj + 0x54);
                        GsSortFastSprite(p, ot, 2);
                    }
                    if (k < CARD_ID_END) {
                        r = func_80029EB0(tb, k);
                        if (r & 0x80) {
                            *(u32 *)(p + 0x14) = white;
                            if (r & 1) {
                                *(u32 *)(p + 0x14) = grey;
                            }
                            *(u16 *)(p + 4) = c + 0xA8;
                            *(u16 *)(p + 0x10) = *(u16 *)(pk + 0x54);
                            GsSortFastSprite(p, ot, 2);
                        }
                    }
                    pj += 4;
                    j++;
                    pk += 4;
                    c += 0xE;
                    k++;
                } while (c < 0x8A);
            }
            *(u16 *)(p + 6) = *(u16 *)(p + 6) + 0x10;
        }
        a += 178;
        n++;
        b += 25;
    } while (n < 4);

done:
    q = (u8 *)0x1F800000;
    v = D_8009B09C;
    *(u32 *)q = 0x50000000;
    q[0xE] = 0;
    q[0xD] = 0;
    q[0xC] = 0;
    q[0x11] = 0;
    q[0x10] = 0;
    q[0xF] = 0;
    m = v & 0x7F;
    switch (m / 32) {
    case 0:
        q[0xD] = m * 8;
        break;
    case 1:
        q[0xD] = 0xFF;
        v = m - 0x20;
        goto shared;
    case 2:
        q[0xD] = (0x5F - m) * 8;
        q[0x10] = 0xFF;
        break;
    case 3:
        v = 0x7F - m;
    shared:
        q[0x10] = v * 8;
        break;
    }
    *(u16 *)(q + 8) = 0;
    t = (u8 *)D_800EA1E8;
    *(u16 *)(q + 4) = *(u16 *)(t + 8) - gGraphics_sViewportX;
    *(u16 *)(q + 6) = *(u16 *)(t + 0xA) - gGraphics_sViewportY;
    *(u16 *)(q + 0xA) = *(u16 *)(t + 0xA) - gGraphics_sViewportY;
    GsSortGLine(q, ot, 1);
    *(u16 *)(q + 8) = 0x140;
    GsSortGLine(q, ot, 1);
    *(u16 *)(q + 0xA) = 0;
    *(u16 *)(q + 8) = *(u16 *)(q + 4);
    GsSortGLine(q, ot, 1);
    *(u16 *)(q + 0xA) = 0xF0;
    GsSortGLine(q, ot, 1);
}
