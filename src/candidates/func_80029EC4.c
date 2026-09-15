#include "../types.h"
#include "../game/graphics_frame.h"
#include "../game/library_runtime.h"
#include "../game/func_80029EB0.h"
#include "../game/card_constants.h"
#include "../psyq/libgs.h"
#include "../game/ordering_tables.h"

/*
 * Current best under gcc_2_8_1_g0_split: 268 instructions against 268, with
 * encoding distance 0. Compiled to an object and compared word for word with
 * the assembled target, 49 of 268 words differ, all placement and register
 * choices. Reusing the initial row-index work, spelling the signed cursor
 * division explicitly, and holding the cursor attribute in the same work
 * variable close the latest differences. The canonical GsGLINE, GsOT and SDK
 * callback declarations and typed sprite fields emit the same bytes as
 * the earlier private views.
 * Remaining differences are the palette ori and row reload in the prologue,
 * the primitive attribute load, and cursor coordinate register allocation.
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
    GsSPRITE *p;
    GsGLINE *q;
    u8 *pj;
    u8 *pk;
    GsOT *ot;
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
    u8 *tb;
    s32 e;
    s32 f;
    s32 phase;

    p = (GsSPRITE *)0x1F800320;
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
    p->x = 8;
    *(u32 *)&p->w = e;
    *(u32 *)&p->cx = f;
    y = b * 8;
    p->y = y * 178 + 8;
    p->cy = 0xF7;
    *(u16 *)&p->u = 0xF060;
    p->attribute = 0x8000000;
    p->tpage = 0x1B;
    do {
        idx = b * 8;
        p->y = a;
        p->y = a - gGraphics_sViewportY;
        for (i = 0; i < 10; idx += 10, i++) {
            y = p->y;
            j = idx + 1;
            tb = D_800EA1E8;
            if (y + p->w > 0) {
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
                        *(u32 *)&p->r = white;
                        if (r & 1) {
                            *(u32 *)&p->r = grey;
                        }
                        p->x = c + 8;
                        p->cx = *(u16 *)(pj + 0x54);
                        GsSortFastSprite(p, ot, 2);
                    }
                    if (k < CARD_ID_END) {
                        r = func_80029EB0(tb, k);
                        if (r & 0x80) {
                            *(u32 *)&p->r = white;
                            if (r & 1) {
                                *(u32 *)&p->r = grey;
                            }
                            p->x = c + 0xA8;
                            p->cx = *(u16 *)(pk + 0x54);
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
            p->y = p->y + 0x10;
        }
        a += 178;
        n++;
        b += 25;
    } while (n < 4);

done:
    do {
        q = (GsGLINE *)0x1F800000;
    } while (0);
    v = D_8009B09C & 0x7F;
    y = 0x50000000;
    q->attribute = y;
    q->b0 = 0;
    q->g0 = 0;
    q->r0 = 0;
    q->b1 = 0;
    q->g1 = 0;
    q->r1 = 0;
    phase = v;
    do {
        t = D_800EA1E8;
    } while (0);
    if (v < 0) {
        phase = v + 31;
    }
    phase >>= 5;
    switch (phase) {
    case 0:
        q->g0 = v * 8;
        break;
    case 1:
        q->g0 = 0xFF;
        q->g1 = (v - 0x20) * 8;
        break;
    case 2:
        q->g0 = (0x5F - v) * 8;
        q->g1 = 0xFF;
        break;
    case 3:
        q->g1 = (0x7F - v) * 8;
        break;
    }
    q->x1 = 0;
    q->x0 = *(u16 *)(t + 8) - gGraphics_sViewportX;
    y = (u16)*(u16 *)(t + 0xA) - (u16)gGraphics_sViewportY;
    q->y1 = y;
    q->y0 = y;
    GsSortGLine(q, ot, 1);
    q->x1 = 0x140;
    GsSortGLine(q, ot, 1);
    q->y1 = 0;
    q->x1 = q->x0;
    GsSortGLine(q, ot, 1);
    q->y1 = 0xF0;
    GsSortGLine(q, ot, 1);
}
