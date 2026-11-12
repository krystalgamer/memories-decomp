#include "../../types.h"

#include "model_variant.h"
#include "variant418_entry.h"

/* Draws three webs of 6x6 GsGLINE segments between their near and far grids,
 * fading each web's colour once its scale passes 0x1000, and grows each scale
 * to 0x2000, wrapping it until the fourth phase. */
void ModelVariant418_DrawWebs(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant418EntryState *work;
    GsOT *ot;
    GsGLINE *line;
    ModelVariantWeb *web;
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

    work = (Variant418EntryState *)ctx;
    web = (ModelVariantWeb *)work;
    ot = Graphics_GetDefaultOrderingTable();
    done = 1;
    i = 0;
    line = (GsGLINE *)((u8 *)work + 0x1AE0);
    ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx);
    ratan2(work->aim.view_delta.vy, work->aim.view_delta.vx);
    ratan2(work->aim.direction.vz, work->aim.direction.vy);
    ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx);
    do {
        if (web->scale > 0) {
            level = web->scale;
            size = level;
        } else {
            size = 0;
            level = size;
        }
        if (size > 0x1000) {
            fade = 0x2000 - size;
            r = web->color.r * fade / 4096;
            g = web->color.g * fade / 4096;
            b = web->color.b * fade / 4096;
        } else {
            r = web->color.r;
            g = web->color.g;
            b = web->color.b;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->matrix.t[0];
        m.t[1] = work->matrix.t[1];
        m.t[2] = work->matrix.t[2];
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
        for (j = 0; j < 6; j++) {
            for (k = 0; k < 6; k++) {
                line->attribute = 0x50000000;
                otz = RotTransPers4(&web->near[j][k], &web->far[j][k], &web->near[j][k], &web->far[j][k],
                                    (long *)&line->x0, (long *)&line->x1, (long *)&line->x0,
                                    (long *)&line->x1, &p, &flag);
                line->r0 = r;
                line->g0 = g;
                line->b0 = b;
                line->r1 = 0;
                line->g1 = 0;
                line->b1 = 0;
                if (otz >= 0 && flag >= 0) {
                    GsSortGLine(line, ot, otz);
                }
            }
        }
        if (web->scale < 0x2000) {
            web->scale += work->clock.step * 0xC0;
            if (web->scale >= 0x2000) {
                if (work->phase < 4) {
                    web->scale -= 0x2000;
                } else {
                    web->scale = 0x2000;
                }
            } else {
                done = 0;
            }
        }
        if (i + 1 == 3 && done == 1 && work->phase == 4) {
            work->phase = 5;
        }
        i++;
        web++;
    } while (i < 3);
}
