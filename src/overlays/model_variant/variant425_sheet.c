#include "../../types.h"

#include "model_variant.h"
#include "variant425_entry.h"

/* Draws the four-quad sheet and updates its timed scale. After growth,
 * phase 1 also advances the value at +0x2734 to 0x600 before phase 2. */
void ModelVariant425_DrawSheet(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    Variant425EntryState *work;
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

    work = (Variant425EntryState *)ctx;
    sheet = (ModelVariantSheet *)(ctx + 0x1E68);
    ot = Graphics_GetDefaultOrderingTable();
    poly = (POLY_GT4 *)(ctx + 0x25A4);
    for (i = 0; i < 1; i++, sheet++) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->orbit.vx;
        m.t[1] = work->orbit.vy;
        m.t[2] = work->orbit.vz;
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
                          (u32)(MODEL_VARIANT_WORD(work->config, 0x20) - work->config->start);
            if (sheet->size >= 0x1000) {
                sheet->size = 0x1000;
                work->phase = 1;
            }
        } else if ((u32)work->clock.frame >= (u32)MODEL_VARIANT_WORD(work->config, 0x2C)) {
            sheet->size = 0x1000 - (u32)((work->clock.frame - MODEL_VARIANT_WORD(work->config, 0x2C)) << 12) /
                          (u32)(MODEL_VARIANT_WORD(work->config, 0x30) - MODEL_VARIANT_WORD(work->config, 0x2C));
            if (sheet->size <= 0) {
                sheet->size = 0;
                if (work->phase == 3) {
                    work->phase = 4;
                }
            }
        }
        if (work->phase == 1) {
            if (work->sweep < 0x600) {
                work->sweep += work->clock.step * 16;
                if (work->sweep >= 0x600) {
                    work->sweep = 0x600;
                    work->phase = 2;
                }
            }
        }
    }
}
