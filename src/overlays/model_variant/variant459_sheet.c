#include "../../types.h"

#include "model_variant.h"
#include "variant459_entry.h"

/* Draws a four-quad sheet at the variant origin. It grows to 0x1000 on
 * the first timing interval, expands to 0x2000, then shrinks to zero. */
void ModelVariant459_DrawSheet(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    Variant459EntryState *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    GsOT *ot;
    POLY_GT4 *poly;
    ModelVariantSheet *sheet;
    s32 i;
    s32 k;
    SVECTOR *v;
    s32 bias;
    s32 otz;

    work = (Variant459EntryState *)ctx;
    sheet = (ModelVariantSheet *)(ctx + 0xF60);
    ot = Graphics_GetDefaultOrderingTable();
    poly = (POLY_GT4 *)(ctx + 0x22E8);
    for (i = 0; i < 1; i++, sheet++) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->matrix.t[0];
        m.t[1] = work->matrix.t[1];
        m.t[2] = work->matrix.t[2];
        scale.vx = sheet->size + bias;
        scale.vy = sheet->size + bias;
        scale.vz = sheet->size + bias;
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
        for (k = 0, v = sheet->v0; k < 4; k++, v++) {
            otz = RotTransPers4(v, &sheet->v1[k], &sheet->v2[k], &sheet->v3[k],
                                (long *)&poly->x0, (long *)&poly->x1,
                                (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            poly->r0 = sheet->inner[0];
            poly->g0 = sheet->inner[1];
            poly->b0 = sheet->inner[2];
            poly->r1 = sheet->inner[0];
            poly->g1 = sheet->inner[1];
            poly->b1 = sheet->inner[2];
            poly->r2 = sheet->inner[0];
            poly->g2 = sheet->inner[1];
            poly->b2 = sheet->inner[2];
            poly->r3 = sheet->outer[0];
            poly->g3 = sheet->outer[1];
            poly->b3 = sheet->outer[2];
            otz = otz * 8 / 10;
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
        if (sheet->size < 0x1000 && work->phase == 0) {
            sheet->size = (u32)((work->clock.frame - work->config->start) << 12) /
                          (u32)(work->config->second - work->config->start);
            if (sheet->size >= 0x1000) {
                sheet->size = 0x1000;
                work->phase = 1;
            }
        } else if ((u32)work->clock.frame < (u32)work->config->field_24) {
            sheet->size += work->clock.step * 512;
            if (sheet->size >= 0x2000) {
                sheet->size = 0x2000;
            }
        } else {
            sheet->size = 0x2000 - (u32)((work->clock.frame - work->config->field_24) << 13) /
                          (u32)(work->config->field_28 - work->config->field_24);
            if (sheet->size <= 0) {
                sheet->size = 0;
            }
        }
    }
}
