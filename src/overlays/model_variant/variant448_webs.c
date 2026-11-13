#include "../../types.h"

#include "model_variant.h"
#include "variant448_entry.h"

/* Header 448's form of the header-397 phased webs: the records start at
 * (u8 *)work + 0x6F8, phase 0 shrinks each scale by step * 0xE0, and a line is
 * sorted whenever its depth is positive. */
void ModelVariant448_DrawWebs(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    u8 *timing;
    ModelVariantWebNarrow *web;
    GsOT *ot;
    GsGLINE *line;
    Variant448EntryState *work;
    s16 i;
    s16 j;
    s16 k;
    s32 size;
    s32 level;
    s32 fade;
    s32 done;
    u8 r;
    u8 g;
    u8 b;
    s32 otz;

    work = (Variant448EntryState *)ctx;
    timing = (u8 *)work + 0x11F0;
    web = work->webs;
    ot = Graphics_GetDefaultOrderingTable();
    done = 1;
    i = 0;
    line = (GsGLINE *)((u8 *)work + 0x1A84);
    ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx);
    ratan2(work->aim.view_delta.vy, work->aim.view_delta.vx);
    ratan2(work->aim.direction.vz, work->aim.direction.vy);
    ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx);
    do {
        if (work->phase == 0) {
            if (web->scale < 0x1000) {
                level = web->scale;
                size = level;
            } else {
                size = 0x1000;
                level = 0;
            }
            if (size > 0x800) {
                fade = 0x1000 - size;
                r = web->color.r * fade / 2048;
                g = web->color.g * fade / 2048;
                b = web->color.b * fade / 2048;
            } else {
                r = web->color.r;
                g = web->color.g;
                b = web->color.b;
            }
        } else if (work->phase == 1) {
            level = 0x1000;
            b = g = r = 0;
        } else {
            if (web->scale > 0) {
                level = web->scale;
                size = level;
            } else {
                size = 0;
                level = size;
            }
            if (size > 0x1800) {
                fade = 0x2000 - size;
                r = web->color.r * fade / 2048;
                g = web->color.g * fade / 2048;
                b = web->color.b * fade / 2048;
            } else {
                r = web->color.r;
                g = web->color.g;
                b = web->color.b;
            }
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (work->phase < 2) {
            m.t[0] = work->matrix.t[0];
            m.t[1] = work->matrix.t[1];
            m.t[2] = work->matrix.t[2];
        } else {
            m.t[0] = work->aim.target.vx;
            m.t[1] = work->aim.target.vy;
            m.t[2] = work->aim.target.vz;
        }
        scale.vx = level;
        scale.vy = level;
        scale.vz = level;
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 6; k++) {
                line->attribute = 0x50000000;
                otz = RotTransPers4(&web->near[j][k], &web->far[j][k], &web->near[j][k], &web->far[j][k],
                                    (long *)&line->x0, (long *)&line->x1, (long *)&line->x0,
                                    (long *)&line->x1, &p, &flag);
                if (work->phase < 2) {
                    line->r0 = 0;
                    line->g0 = 0;
                    line->b0 = 0;
                    line->r1 = r;
                    line->g1 = g;
                    line->b1 = b;
                } else {
                    line->r0 = r;
                    line->g0 = g;
                    line->b0 = b;
                    line->r1 = 0;
                    line->g1 = 0;
                    line->b1 = 0;
                }
                if (otz > 0) {
                    GsSortGLine(line, ot, otz);
                }
            }
        }
        if (work->phase == 0) {
            if (web->scale > 0) {
                web->scale -= work->clock.step * 0xE0;
                if (web->scale <= 0) {
                    if (MODEL_VARIANT_WORD(timing, 0x88) > 0x800) {
                        web->scale = 0;
                    } else {
                        web->scale += 0x1000;
                    }
                }
            }
            if (MODEL_VARIANT_WORD(timing, 0x88) > 0x800 && web->scale > 0x1000) {
                web->scale = 0;
            }
        } else if (work->phase == 1) {
            web->scale = -((i << 13) / 3);
        } else if (web->scale < 0x2000) {
            web->scale += work->clock.step << 8;
            if (web->scale >= 0x2000) {
                if (work->phase >= 4) {
                    web->scale = 0x2000;
                    if (i + 1 == 3 && done == 1 && work->phase == 4) {
                        work->phase = 5;
                    }
                } else {
                    web->scale -= 0x2000;
                    done = 0;
                }
            }
        }
        i++;
        web++;
    } while (i < 3);
}
