#include "../../types.h"

#include "model_variant.h"
#include "variant459_entry.h"

/* Header 459's form of the header-418 webs: faded past scale 0x800, grown by
 * step * 128 to 0x1000, and sorted whenever the depth is positive. */
void ModelVariant459_DrawWebs(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant459EntryState *work;
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

    work = (Variant459EntryState *)ctx;
    web = (ModelVariantWeb *)work;
    ot = Graphics_GetDefaultOrderingTable();
    done = 1;
    i = 0;
    line = (GsGLINE *)((u8 *)work + 0x237C);
    ratan2(work->view_delta.vz, work->view_delta.vx);
    ratan2(work->view_delta.vy, work->view_delta.vx);
    ratan2(work->direction.vz, work->direction.vy);
    ratan2(work->screen_delta.vy, work->screen_delta.vx);
    do {
        if (web->scale > 0) {
            level = web->scale;
            size = level;
        } else {
            size = 0;
            level = size;
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
                if (otz > 0) {
                    GsSortGLine(line, ot, otz);
                }
            }
        }
        if (web->scale < 0x1000) {
            web->scale += work->clock.step * 128;
            if (web->scale >= 0x1000) {
                if (work->phase < 4) {
                    web->scale -= 0x1000;
                } else {
                    web->scale = 0x1000;
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
