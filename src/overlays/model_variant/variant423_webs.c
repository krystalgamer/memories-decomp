#include "../../types.h"

#include "model_variant.h"
#include "variant423_entry.h"

/* Header 423's form of the header-416 webs: the records start at work, and
 * the lines are projected with RotTransPers3. */
void ModelVariant423_DrawWebs(u8 *ctx)
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
    Variant423EntryState *work;
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

    work = (Variant423EntryState *)ctx;
    web = (ModelVariantWebNarrow *)work;
    ot = Graphics_GetDefaultOrderingTable();
    done = 1;
    i = 0;
    line = (GsGLINE *)((u8 *)work + 0xE84);
    ratan2(work->view_delta.vz, work->view_delta.vx);
    ratan2(work->view_delta.vy, work->view_delta.vx);
    ratan2(work->direction.vz, work->direction.vy);
    ratan2(work->screen_delta.vy, work->screen_delta.vx);
    do {
        if (work->tail.phase == 0) {
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
        } else if (work->tail.phase == 1) {
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
        if (work->tail.phase < 2) {
            m.t[0] = work->matrix.t[0];
            m.t[1] = work->matrix.t[1];
            m.t[2] = work->matrix.t[2];
        } else {
            m.t[0] = work->target.vx;
            m.t[1] = work->target.vy;
            m.t[2] = work->target.vz;
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
                otz = RotTransPers3(&web->near[j][k], &web->far[j][k], &web->near[j][k], (long *)&line->x0,
                                    (long *)&line->x1, (long *)&line->x0, &p, &flag);
                if (work->tail.phase < 2) {
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
                if (otz >= 0 && flag >= 0) {
                    GsSortGLine(line, ot, otz);
                }
            }
        }
        if (work->tail.phase == 0) {
            if (web->scale > 0) {
                timing = (u8 *)work->config;
                web->scale = (u32)((work->clock.frame - MODEL_VARIANT_WORD(timing, 0x1C)) * 3 << 12) /
                                 (MODEL_VARIANT_WORD(timing, 0x20) - MODEL_VARIANT_WORD(timing, 0x1C)) -
                             0x1000;
                web->scale = (i << 12) / 3 - web->scale;
                if (web->scale <= 0) {
                    web->scale += 0x1000;
                }
            }
        } else if (work->tail.phase == 1) {
            web->scale = -((i << 13) / 3);
        } else if (web->scale < 0x2000) {
            web->scale += work->clock.step * 0x180;
            if (web->scale >= 0x2000) {
                if (work->tail.phase >= 4) {
                    web->scale = 0x2000;
                    if (i + 1 == 3 && done == 1 && work->tail.phase == 4) {
                        work->tail.phase = 5;
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
