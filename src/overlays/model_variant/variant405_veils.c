#include "../../types.h"

#include "variant405_veils.h"
#include "../french_model_variant/variant422_entry.h"

/* Draws five veils of sixteen textured POLY_GT4 strips that open and close
 * with their scale, white at the inner row and blue at the outer one; each
 * strip is textured from the screen half it lands on. Each scale grows to
 * 0x800 and wraps with a count until the timing record's deadline, and the
 * phase moves on once every veil has wrapped. */
void ModelVariant405_DrawVeils(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    /* Retail reserves 24 bytes before the two color locals. */
    u8 unknown_stack[24];
    CVECTOR dark;
    CVECTOR light;
    long p;
    long flag;
    Variant422EntryState *work;
    GsOT *ot;
    s32 i;
    s32 all;
    s32 buff;
    Variant405Veil *veil;
    POLY_GT4 *poly;
    s32 j;
    s32 angle;
    s32 otz;
    s32 tpage;

    veil = (Variant405Veil *)ctx;
    work = (Variant422EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    all = 1;
    buff = GsGetActiveBuff();
    poly = &work->extra;
    ratan2(work->direction.vz, work->direction.vx);
    ratan2(work->direction.vy, work->direction.vz);
    for (i = 0; i < 5; i++, veil++) {
        if (veil->scale > 0) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                setVector(&veil->a[j], rcos(angle) * (rsin(veil->scale) * 224 >> 12) >> 12, 0,
                          rsin(angle) * (rsin(veil->scale) * 224 >> 12) >> 12);
                setVector(&veil->b[j], rcos(angle) * (rsin(veil->scale) * 320 >> 12) >> 12,
                          (u32)(rsin(veil->scale) * 3) >> 8,
                          rsin(angle) * (rsin(veil->scale) * 320 >> 12) >> 12);
                setVector(&veil->c[j], rcos(angle) * (rsin(veil->scale) * 416 >> 12) >> 12,
                          (u32)(rsin(veil->scale) * 3) >> 8,
                          rsin(angle) * (rsin(veil->scale) * 416 >> 12) >> 12);
            }
            light.r = 0xFF;
            light.g = 0xFF;
            light.b = 0xFF;
            dark.r = 0;
            dark.g = 0x80;
            dark.b = 0xFF;
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            m.t[0] = work->matrix.t[0];
            m.t[1] = work->matrix.t[1] - (rcos(veil->scale) * 224 >> 12);
            m.t[2] = work->matrix.t[2];
            RotMatrix(&rot, &m);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            for (j = 0; j < 16; j++) {
                RotTransPers4(&veil->a[j], &veil->a[j + 1], &veil->c[j], &veil->c[j + 1],
                              (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
                              (long *)&poly->x3, &p, &flag);
                if (poly->x0 < 0xA0) {
                    if (buff == 0) {
                        tpage = GetTPage(2, 1, 0x140, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0, 0);
                    }
                    SetPolyGT4(poly);
                    poly->u0 = poly->x0;
                    poly->v0 = poly->y0;
                    poly->u1 = poly->x1;
                    poly->v1 = poly->y1;
                    poly->u2 = poly->x2;
                    poly->v2 = poly->y2;
                    poly->u3 = poly->x3;
                    poly->v3 = poly->y3;
                    poly->tpage = tpage;
                } else {
                    if (buff == 0) {
                        tpage = GetTPage(2, 1, 0x1C0, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0x80, 0);
                    }
                    SetPolyGT4(poly);
                    poly->u0 = poly->x0 - 0x80;
                    poly->v0 = poly->y0;
                    poly->u1 = poly->x1 - 0x80;
                    poly->v1 = poly->y1;
                    poly->u2 = poly->x2 - 0x80;
                    poly->v2 = poly->y2;
                    poly->u3 = poly->x3 - 0x80;
                    poly->v3 = poly->y3;
                    poly->tpage = tpage;
                }
                otz = RotTransPers4(&veil->a[j], &veil->a[j + 1], &veil->b[j], &veil->b[j + 1],
                                    (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
                                    (long *)&poly->x3, &p, &flag);
                SetSemiTrans(poly, 1);
                SetShadeTex(poly, 0);
                poly->r0 = light.r;
                poly->g0 = light.g;
                poly->b0 = light.b;
                poly->r1 = light.r;
                poly->g1 = light.g;
                poly->b1 = light.b;
                poly->r2 = dark.r;
                poly->g2 = dark.g;
                poly->b2 = dark.b;
                poly->r3 = dark.r;
                poly->g3 = dark.g;
                poly->b3 = dark.b;
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, otz);
                }
            }
        }
        if (veil->scale < 0x800) {
            veil->scale += work->clock.step * 40;
            if (veil->scale >= 0x800) {
                if (*(u32 *)(*(u8 **)((u8 *)work + 0x1DC8) + 0x24) <= (u32)work->clock.frame) {
                    veil->scale = 0x800;
                    veil->count = 1;
                } else {
                    veil->scale -= 0x800;
                }
                if (work->phase == 0 &&
                    *(u32 *)(*(u8 **)((u8 *)work + 0x1DC8) + 0x20) <= (u32)work->clock.frame) {
                    work->phase = 2;
                }
            }
        }
        all *= veil->count;
        if (i + 1 == 5 && all == 1 && work->phase == 2) {
            work->phase = i + 1;
        }
    }
}
