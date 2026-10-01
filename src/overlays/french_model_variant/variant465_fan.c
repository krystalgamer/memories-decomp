#include "../../types.h"
#include "variant465_fan.h"

void func_8013C24C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    GsOT *ot;
    s32 i;
    s32 j;
    u8 *work;
    Variant465Fan *fan;
    POLY_G3 *triangle;
    POLY_G4 *quad;
    s32 otz;

    work = ctx;
    fan = (Variant465Fan *)work;
    triangle = (POLY_G3 *)(work + 0x1948);
    ot = func_80058F10();
    quad = (POLY_G4 *)(work + 0x1964);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x1AB8);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x1ABC);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x1AC0);
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
                               (PSXLONG *)&triangle->x0, (PSXLONG *)&triangle->x1,
                               (PSXLONG *)&triangle->x2, &p, &flag);
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
                func_8005B260((u32 *)triangle, ot, (u16)otz, 1);
            }
            otz = RotTransPers4(&fan->inner[j + 1], &fan->inner[j + 2],
                               &fan->outer[j], &fan->outer[j + 1],
                               (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                               (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3, &p, &flag);
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
                func_8005B260((u32 *)quad, ot, (u16)otz, 1);
            }
        }
        if (MODEL_VARIANT_WORD(work, 0x1B3C) == 0) {
            if (fan->size < 0x1000) {
                fan->size = (u32)((MODEL_VARIANT_WORD(work, 0x1B00) -
                    MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B10), 0x1C)) << 12) /
                    (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B10), 0x20) -
                    MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B10), 0x1C));
                if (fan->size >= 0x1000) {
                    fan->size = 0x1000;
                    if (i + 1 == 1) {
                        MODEL_VARIANT_WORD(work, 0x1B3C) = i + 1;
                    }
                }
            }
        } else if ((u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B10), 0x24) <=
                   (u32)MODEL_VARIANT_WORD(work, 0x1B00)) {
            if (fan->size > 0) {
                fan->size -= MODEL_VARIANT_WORD(work, 0x1B08) << 7;
                if (fan->size <= 0) {
                    fan->size = 0;
                }
            } else if (MODEL_VARIANT_WORD(work, 0x1B3C) == 2) {
                MODEL_VARIANT_WORD(work, 0x1B3C) = 3;
            }
        }
    }
}
