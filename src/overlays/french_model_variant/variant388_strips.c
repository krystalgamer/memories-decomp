#include "../../types.h"
#include "variant388_strips.h"

#define W(o) MODEL_VARIANT_WORD(work, o)
#define H(o) MODEL_VARIANT_HALF(work, o)

/* Projects five cross sections along the path from the origin at +0x1818
 * towards +0x182C, scaled by the growth half at +0x1890, and draws two
 * POLY_GT4 strips between them, then advances the grow and shrink timers
 * against the timing record.
 *
 * The second strip reads its first columns through col/next, and both are
 * then overwritten before the flag store and kept live after the loop.
 * GCC 2.8.1 CSE then keeps col/next as copies of the first strip's column
 * pointers, while the flag and sort code goes back to the originals, as in
 * the target. */
void func_8013BCD4(u8 *ctx)
{
    u8 *work;
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    GsOT *ot;
    Strip388Unit *unit;
    POLY_GT4 *poly;
    Strip388Timing *rec;
    Strip388Unit *col;
    Strip388Unit *next;
    s16 i;
    s16 k;
    s16 width;
    s32 angle;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x16C0);
    angle = ratan2(H(0x183E), H(0x183C)) + 0x800;
    if (W(0x1898) > 0) {
        unit = (Strip388Unit *)work;
        width = H(0x1892) / 32;
        for (i = 0; i < 1; i++, unit++) {
            for (k = 0; k < 5; k++) {
                setVector(&unit->a[k], rcos(0x400) * width >> 12, rsin(0x400) * width >> 12, 0);
                setVector(&unit->b[k], 0, 0, 0);
                setVector(&unit->c[k], rcos(0xC00) * width >> 12, rsin(0xC00) * width >> 12, 0);
                rot.vx = 0;
                rot.vy = 0;
                rot.vz = angle;
                if (k == 0) {
                    m.t[0] = W(0x1818);
                    m.t[1] = W(0x181C);
                    m.t[2] = W(0x1820);
                } else {
                    m.t[0] = W(0x1818) + W(0x182C) * H(0x1890) / 1024 * k / 4;
                    m.t[1] = W(0x181C) + W(0x1830) * H(0x1890) / 1024 * k / 4;
                    m.t[2] = W(0x1820) + W(0x1834) * H(0x1890) / 1024 * k / 4;
                }
                scale.vx = 0x1000;
                scale.vy = 0x1000;
                scale.vz = 0x1000;
                RotMatrix(&rot, &m);
                coord.coord = m;
                coord.super = 0;
                coord.flg = 0;
                GsGetLs(&coord, &ls);
                GsSetLsMatrix(&ls);
                ReadRotMatrix(&ls);
                RotMatrix(&rot, &ls);
                ScaleMatrix(&ls, &scale);
                SetRotMatrix(&ls);
                unit->otz[k] = RotTransPers3(&unit->a[k], &unit->b[k], &unit->c[k], &unit->sa[k],
                                             &unit->sb[k], &unit->sc[k], &p, &unit->flag[k]);
            }
        }
        unit = (Strip388Unit *)work;
        for (i = 0; i < 1; i++, unit++) {
            for (k = 0; k < 4; k++) {
                poly->x0 = unit->sa[k];
                poly->y0 = (unit->sa[k] >> 16);
                poly->x1 = unit->sa[k + 1];
                poly->y1 = (unit->sa[k + 1] >> 16);
                poly->x2 = unit->sb[k];
                poly->y2 = (unit->sb[k] >> 16);
                poly->x3 = unit->sb[k + 1];
                poly->y3 = (unit->sb[k + 1] >> 16);
                poly->r0 = 0x80;
                poly->g0 = 0;
                poly->b0 = 0x80;
                poly->r1 = 0x80;
                poly->g1 = 0;
                poly->b1 = 0x80;
                poly->r2 = 0xC0;
                poly->g2 = 0xC0;
                poly->b2 = 0xC0;
                poly->r3 = 0xC0;
                poly->g3 = 0xC0;
                poly->b3 = 0xC0;
                if (unit->otz[k] < 0) {
                    unit->otz[k] = 0;
                }
                unit->flag[k] = 0;
                if (unit->otz[k] >= 0) {
                    GsSortPoly(poly, ot, unit->otz[k + 1]);
                }
                col = (Strip388Unit *)((u8 *)unit + k * 4);
                next = (Strip388Unit *)((u8 *)unit + (k + 1) * 4);
                poly->x0 = col->sc[0];
                poly->y0 = (col->sc[0] >> 16);
                poly->x1 = next->sc[0];
                poly->y1 = (next->sc[0] >> 16);
                poly->x2 = col->sb[0];
                poly->y2 = (col->sb[0] >> 16);
                poly->x3 = next->sb[0];
                poly->y3 = (next->sb[0] >> 16);
                poly->r0 = 0x80;
                poly->g0 = 0;
                poly->b0 = 0x80;
                poly->r1 = 0x80;
                poly->g1 = 0;
                poly->b1 = 0x80;
                poly->r2 = 0xC0;
                poly->g2 = 0xC0;
                poly->b2 = 0xC0;
                poly->r3 = 0xC0;
                poly->g3 = 0xC0;
                poly->b3 = 0xC0;
                if (col->otz[0] < 0) {
                    col->otz[0] = 0;
                }
                col = next = (Strip388Unit *)work;
                unit->flag[k] = 0;
                if (unit->otz[k] >= 0) {
                    GsSortPoly(poly, ot, unit->otz[k + 1]);
                }
            }
        }
        col = next = (Strip388Unit *)work;
        rec = (Strip388Timing *)W(0x186C);
        if (W(0x1880) + 1 == rec->frame) {
            if (W(0x1898) == 1 && H(0x1890) <= 0x400) {
                if (rec->grow_start <= (u32)W(0x185C)) {
                    H(0x1890) = ((u32)W(0x185C) - rec->grow_start << 10) / (rec->grow_end - rec->grow_start);
                }
                if (H(0x1890) >= 0x400) {
                    H(0x1890) = 0x400;
                    W(0x1898) = 2;
                }
            }
            if (H(0x1892) > 0) {
                rec = (Strip388Timing *)W(0x186C);
                if ((u32)W(0x185C) >= rec->shrink_start) {
                    H(0x1892) = 0x400 - ((u32)W(0x185C) - rec->shrink_start << 10) / (rec->shrink_end - rec->shrink_start);
                }
                if (H(0x1892) <= 0) {
                    H(0x1892) = 0;
                    if (W(0x1898) == 3) {
                        W(0x1898) = 4;
                    }
                }
            }
        }
    }
}
