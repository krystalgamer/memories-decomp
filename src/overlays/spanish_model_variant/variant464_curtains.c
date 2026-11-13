#include "../../types.h"

#include "../model_variant/model_variant.h"
#include "../french_model_variant/variant464_entry.h"

/* Each frame rebuilds the two rows of both shown curtains around the
 * rotating angle at (u8 *)work + 0x1C5C, lifting the outer row by the first
 * sheet's size when flag bit 0 is set, and draws each strip as a POLY_GT4
 * when its depth is positive. A curtain that reaches full size wraps
 * and counts a cycle before phase 4, and stops there from phase 4 on. */
void SpanishModelVariant464_DrawCurtains(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant464EntryState *work;
    ModelVariantCurtain *curtain;
    GsOT *ot;
    s32 i;
    s32 lift;
    s32 yaw;
    s32 pitch;
    s32 radius;
    POLY_GT4 *poly;
    s32 j;
    s32 otz;
    s32 size;
    s32 done;
    s32 angle;
    ModelVariantSheet *sheet;
    s16 fade;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 r1;
    u8 g1;
    u8 b1;

    work = (Variant464EntryState *)ctx;
    curtain = work->curtains;
    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(work->aim.direction.vz, work->aim.direction.vx) + 0x800;
    sheet = (ModelVariantSheet *)work->sheets;
    poly = &work->curtain_poly;
    pitch = ratan2(work->aim.direction.vy, work->aim.direction.vz) + 0x800;
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw += 0x400;
    }
    lift = (((s32)work->clock.frame_count & 1) << 5) * sheet->size / 4096;
    done = 1;
    i = 0;
    radius = lift + 0x180;
    for (; i < 2; i++, curtain++) {
        if (curtain->scale > 0) {
            size = curtain->scale;
            for (j = 0, angle = work->field_1C5C; j < 17;
                 j++, angle = work->field_1C5C + j * 256) {
                setVector(&curtain->a[j], (u32)rcos(angle) >> 4, (u32)rsin(angle) >> 4, 0);
                setVector(&curtain->b[j], rcos(angle) * radius >> 12, rsin(angle) * radius >> 12, lift + 0x80);
            }
            rot.vx = -pitch;
            rot.vy = yaw;
            rot.vz = 0;
            m.t[0] = work->aim.target.vx;
            m.t[1] = work->aim.target.vy;
            m.t[2] = work->aim.target.vz;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            if (curtain->scale < 0x800) {
                r0 = ((u8 *)work)[0x1C54];
                g0 = ((u8 *)work)[0x1C55];
                b0 = ((u8 *)work)[0x1C56];
                r1 = ((u8 *)work)[0x1C58];
                g1 = ((u8 *)work)[0x1C59];
                b1 = ((u8 *)work)[0x1C5A];
            } else {
                fade = 0x1000 - curtain->scale;
                r0 = ((u8 *)work)[0x1C54] * fade / 2048;
                g0 = ((u8 *)work)[0x1C55] * fade / 2048;
                b0 = ((u8 *)work)[0x1C56] * fade / 2048;
                r1 = ((u8 *)work)[0x1C58] * fade / 2048;
                g1 = ((u8 *)work)[0x1C59] * fade / 2048;
                b1 = ((u8 *)work)[0x1C5A] * fade / 2048;
            }
            for (j = 0; j < 16; j++) {
                otz = RotTransPers4(&curtain->a[j], &curtain->a[j + 1], &curtain->b[j], &curtain->b[j + 1],
                                    (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
                                    (long *)&poly->x3, &p, &flag);
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = r0;
                poly->g1 = g0;
                poly->b1 = b0;
                poly->r2 = r1;
                poly->g2 = g1;
                poly->b2 = b1;
                poly->r3 = r1;
                poly->g3 = g1;
                poly->b3 = b1;
                if (otz > 0) {
                    GsSortPoly(poly, ot, (u16)otz);
                }
            }
        }
        if (curtain->scale < 0x1000) {
            curtain->scale += work->clock.step << 7;
            if (curtain->scale >= 0x1000) {
                if (work->tail.phase >= 4) {
                    curtain->scale = 0x1000;
                } else {
                    curtain->scale -= 0x1000;
                    curtain->count++;
                }
                if (i + 1 == 2 && done == 1 && work->tail.phase == 4) {
                    work->tail.phase = 5;
                }
            }
        }
    }
    work->field_1C5C += work->clock.step * 80;
}
