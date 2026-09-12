#include "../types.h"
#include "../game/graphics_frame.h"
#include "../game/library_runtime.h"
#include "../game/func_80029EB0.h"

/* 0x800E9D9C is the second ordering-table pointer. ordering_tables.h declares
   it `GsOT *`; this unit reads it as a word, so the private spelling stays. */
extern s32 D_800E9D9C;

/* src/psyq/libgs.h:791 and :802 declare these as
   GsSortFastSprite(GsSPRITE *, GsOT *, unsigned short) and
   GsSortGLine(GsGLINE *, GsOT *, unsigned short). Including it builds this
   object byte for byte the same, and costs ten new warnings: the two
   primitives here are scratchpad addresses held as u8 *, and the ordering
   table is carried as a word, so every call reports an incompatible pointer
   and arg 2 "makes pointer from integer without a cast". Making that coherent
   means retyping the primitives and the handle, which is a change to what this
   unit says about the objects, not to where its declarations live. */
extern void GsSortFastSprite(u8 *, s32, s32);
extern void GsSortGLine(u8 *, s32, s32);

/*
 * Current best under gcc_2_8_1_g0: 269 instructions against 268, with
 * encoding distance 5 and 97 differing positions. The target is split-address
 * code; retuning this source under gcc_2_8_1_g0_split remains the next step.
 * The residual non-split build shares one D_800EA1E8 address, emits one extra
 * shift, and leaves two delay slots empty.
 */

/* Draws the scrolling card-list grid straight into the scratchpad primitive at
   0x1F800320. The first visible row comes from the viewport scroll divided by
   the 178-pixel row pitch; from there four rows of ten lines are walked, each
   line holding ten cells on the left at column 8 and, while the paired index
   is still below 0x2D3, ten more on the right at column 0xA8. A cell is drawn
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
    s32 a;
    s32 b;
    s32 idx;
    s32 i;
    s32 c;
    s32 j;
    s32 k;
    s32 r;
    s32 white;
    s32 grey;
    s32 y;
    s32 v;

    n = (gGraphics_sViewportY - 8) / 178;
    ot = D_800E9D9C;
    if (n < 0) {
        return;
    }
    white = 0x808080;
    grey = 0x404040;
    a = n * 178 + 8;
    b = n * 25;
    p = (u8 *)0x1F800320;
    *(u16 *)(p + 4) = 8;
    *(u32 *)(p + 8) = 0xE000C;
    *(u32 *)(p + 0x10) = 0xF70130;
    *(u16 *)(p + 6) = b * 8 * 178 + 8;
    *(u16 *)(p + 0x12) = 0xF7;
    *(u16 *)(p + 0xE) = 0xF060;
    *(u32 *)p = 0x8000000;
    *(u16 *)(p + 0xC) = 0x1B;
    do {
        idx = b * 8;
        *(u16 *)(p + 6) = a;
        *(u16 *)(p + 6) = a - gGraphics_sViewportY;
        for (i = 0; i < 10; i++, idx += 10) {
            y = *(s16 *)(p + 6);
            if (y + *(u16 *)(p + 8) > 0) {
                j = idx + 1;
                if (y >= 0xF0) {
                    goto done;
                }
                k = idx + 0x65;
                c = 0;
                pk = &D_800EA1E8[k * 4];
                pj = &D_800EA1E8[j * 4];
                do {
                    r = func_80029EB0(D_800EA1E8, j);
                    if (r & 0x80) {
                        *(u32 *)(p + 0x14) = white;
                        if (r & 1) {
                            *(u32 *)(p + 0x14) = grey;
                        }
                        *(u16 *)(p + 4) = c + 8;
                        *(u16 *)(p + 0x10) = *(u16 *)(pj + 0x54);
                        GsSortFastSprite(p, ot, 2);
                    }
                    if (k < 0x2D3) {
                        r = func_80029EB0(D_800EA1E8, k);
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
    i = v & 0x7F;
    switch (i / 32) {
    case 0:
        q[0xD] = i * 8;
        break;
    case 1:
        q[0xD] = 0xFF;
        q[0x10] = (i - 0x20) * 8;
        break;
    case 2:
        q[0xD] = (0x5F - i) * 8;
        q[0x10] = 0xFF;
        break;
    case 3:
        q[0x10] = (0x7F - i) * 8;
        break;
    }
    *(u16 *)(q + 8) = 0;
    *(u16 *)(q + 4) = *(u16 *)(D_800EA1E8 + 8) - gGraphics_sViewportX;
    *(u16 *)(q + 6) = *(u16 *)(D_800EA1E8 + 0xA) - gGraphics_sViewportY;
    *(u16 *)(q + 0xA) = *(u16 *)(D_800EA1E8 + 0xA) - gGraphics_sViewportY;
    GsSortGLine(q, ot, 1);
    *(u16 *)(q + 8) = 0x140;
    GsSortGLine(q, ot, 1);
    *(u16 *)(q + 0xA) = 0;
    *(u16 *)(q + 8) = *(u16 *)(q + 4);
    GsSortGLine(q, ot, 1);
    *(u16 *)(q + 0xA) = 0xF0;
    GsSortGLine(q, ot, 1);
}
