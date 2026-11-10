#include "../../types.h"

#include "model_variant.h"
#include "../french_model_variant/variant439_entry.h"

/* Draws three curtains of sixteen POLY_GT4 strips between their two rows of
 * points, turned to face the variant's direction and faded once their scale
 * passes 0x800, and grows each scale to 0x1000, wrapping it and counting the
 * wraps until the fourth phase. */
void ModelVariant422_DrawCurtains(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant439EntryState *work;
    GsOT *ot;
    s32 i;
    s32 yaw;
    s32 pitch;
    ModelVariantCurtain *curtain;
    POLY_GT4 *poly;
    s32 j;
    s32 otz;
    s32 size;
    s32 done;
    s16 fade;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 r1;
    u8 g1;
    u8 b1;

    work = (Variant439EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    curtain = work->curtains;
    yaw = ratan2(work->direction.vz, work->direction.vx) + 0x800;
    poly = &work->curtain_poly;
    pitch = ratan2(work->direction.vy, work->direction.vz) + 0x800;
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw += 0x400;
    }
    done = 1;
    for (i = 0; i < 3; i++, curtain++) {
        size = curtain->scale;
        if (size > 0) {
            rot.vx = -pitch;
            rot.vy = yaw;
            rot.vz = 0;
            m.t[0] = work->target.vx;
            m.t[1] = work->target.vy;
            m.t[2] = work->target.vz;
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
                r0 = ((u8 *)work)[0x1970];
                g0 = ((u8 *)work)[0x1971];
                b0 = ((u8 *)work)[0x1972];
                r1 = ((u8 *)work)[0x1974];
                g1 = ((u8 *)work)[0x1975];
                b1 = ((u8 *)work)[0x1976];
            } else {
                fade = 0x1000 - curtain->scale;
                r0 = ((u8 *)work)[0x1970] * fade / 2048;
                g0 = ((u8 *)work)[0x1971] * fade / 2048;
                b0 = ((u8 *)work)[0x1972] * fade / 2048;
                r1 = ((u8 *)work)[0x1974] * fade / 2048;
                g1 = ((u8 *)work)[0x1975] * fade / 2048;
                b1 = ((u8 *)work)[0x1976] * fade / 2048;
            }
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
            for (j = 0; j < 16; j++) {
                otz = RotTransPers4(&curtain->a[j], &curtain->a[j + 1], &curtain->b[j], &curtain->b[j + 1],
                                    (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
                                    (long *)&poly->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, otz);
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
                if (i + 1 == 3 && done == 1 && work->tail.phase == 4) {
                    work->tail.phase = 5;
                }
            }
        }
    }
    work->field_1978 += work->clock.step * 80;
}
