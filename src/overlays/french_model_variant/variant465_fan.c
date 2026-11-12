#include "../../types.h"
#include "variant465_fan.h"
#include "../model_variant/variant448_entry.h"

void ModelVariant448_DrawRadialDisc(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    GsOT *ot;
    s32 i;
    s32 j;
    Variant448EntryState *work;
    Variant465Fan *fan;
    POLY_G3 *triangle;
    POLY_G4 *quad;
    s32 otz;

    work = (Variant448EntryState *)ctx;
    fan = (Variant465Fan *)work;
    triangle = &work->packets.triangle;
    ot = Graphics_GetDefaultOrderingTable();
    quad = &work->packets.quad;
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = work->position.vx;
    m.t[1] = work->position.vy;
    m.t[2] = work->position.vz;
    scale.vx = fan->size;
    scale.vy = fan->size;
    scale.vz = fan->size;
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    for (i = 0; i < 1; i++, fan++) {
        for (j = 0; j < 16; j++) {
            otz = RotTransPers3(&fan->inner[0], &fan->inner[j + 2], &fan->inner[j + 1],
                               (long *)&triangle->x0, (long *)&triangle->x1,
                               (long *)&triangle->x2, &p, &flag);
            triangle->r0 = fan->center_color[0];
            triangle->g0 = fan->center_color[1];
            triangle->b0 = fan->center_color[2];
            triangle->r1 = fan->middle_color[0];
            triangle->g1 = fan->middle_color[1];
            triangle->b1 = fan->middle_color[2];
            triangle->r2 = fan->middle_color[0];
            triangle->g2 = fan->middle_color[1];
            triangle->b2 = fan->middle_color[2];
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)triangle, ot, (u16)otz, 1);
            }
            otz = RotTransPers4(&fan->inner[j + 1], &fan->inner[j + 2],
                               &fan->outer[j], &fan->outer[j + 1],
                               (long *)&quad->x0, (long *)&quad->x1,
                               (long *)&quad->x2, (long *)&quad->x3, &p, &flag);
            quad->r0 = fan->middle_color[0];
            quad->g0 = fan->middle_color[1];
            quad->b0 = fan->middle_color[2];
            quad->r1 = fan->middle_color[0];
            quad->g1 = fan->middle_color[1];
            quad->b1 = fan->middle_color[2];
            quad->r2 = fan->outer_color[0];
            quad->g2 = fan->outer_color[1];
            quad->b2 = fan->outer_color[2];
            quad->r3 = fan->outer_color[0];
            quad->g3 = fan->outer_color[1];
            quad->b3 = fan->outer_color[2];
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)otz, 1);
            }
        }
        if (work->phase == 0) {
            if (fan->size < 0x1000) {
                fan->size = (u32)((work->clock.frame -
                    work->config->start) << 12) /
                    (u32)(MODEL_VARIANT_WORD(work->config, 0x20) -
                    work->config->start);
                if (fan->size >= 0x1000) {
                    fan->size = 0x1000;
                    if (i + 1 == 1) {
                        work->phase = i + 1;
                    }
                }
            }
        } else if ((u32)MODEL_VARIANT_WORD(work->config, 0x24) <=
                   (u32)work->clock.frame) {
            if (fan->size > 0) {
                fan->size -= work->clock.step << 7;
                if (fan->size <= 0) {
                    fan->size = 0;
                }
            } else if (work->phase == 2) {
                work->phase = 3;
            }
        }
    }
}
