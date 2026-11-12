#include "../../types.h"
#include "variant476_globe.h"
#include "../model_variant/variant459_entry.h"

void ModelVariant459_DrawGlobe(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    ModelVariantMesh *globe;
    GsOT *ot;
    POLY_GT4 *poly;
    Variant459EntryState *work;
    u8 *timing;
    s32 i;
    s32 j;
    s32 otz;

    work = (Variant459EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    globe = (ModelVariantMesh *)((u8 *)work + 0xFF8);
    ratan2(MODEL_VARIANT_WORD(work, 0x2778), MODEL_VARIANT_WORD(work, 0x2770));
    poly = &work->veil;
    ratan2(MODEL_VARIANT_WORD(work, 0x2774), MODEL_VARIANT_WORD(work, 0x2778));
    if (work->slot == 0) {
        rot.vx = MODEL_VARIANT_HALF(work, 0x2804);
        rot.vy = 0;
        rot.vz = 0;
    } else {
        rot.vx = -MODEL_VARIANT_HALF(work, 0x2804);
        rot.vy = 0;
        rot.vz = 0;
    }
    m.t[0] = work->target.vx;
    m.t[1] = work->target.vy;
    m.t[2] = work->target.vz;
    scale.vx = MODEL_VARIANT_WORD(work, 0x27FC);
    scale.vy = MODEL_VARIANT_WORD(work, 0x27FC);
    scale.vz = MODEL_VARIANT_WORD(work, 0x27FC);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    if (MODEL_VARIANT_WORD(work, 0x284C) < 5) {
        MODEL_VARIANT_WORD(work, 0x280C) = 1024 - (MODEL_VARIANT_WORD(work, 0x27FC) - 8192) / 8;
        for (i = 0; i < 9; i++) {
            globe->colors[i][0] = (i * 255 / 8) * MODEL_VARIANT_WORD(work, 0x280C) / 1024;
            globe->colors[i][1] = MODEL_VARIANT_WORD(work, 0x280C) / 16;
            globe->colors[i][2] = (255 - i * 255 / 8) * MODEL_VARIANT_WORD(work, 0x280C) / 1024;
        }
    } else {
        for (i = 0; i < 9; i++) {
            globe->colors[i][0] = MODEL_VARIANT_WORD(work, 0x280C) / 8;
            globe->colors[i][1] = MODEL_VARIANT_WORD(work, 0x280C) / 8;
            globe->colors[i][2] = MODEL_VARIANT_WORD(work, 0x280C) / 8;
        }
    }
    for (i = 0; i < 8; i++) {
        setRGB0(poly, globe->colors[i][0], globe->colors[i][1], globe->colors[i][2]);
        setRGB1(poly, globe->colors[i][0], globe->colors[i][1], globe->colors[i][2]);
        setRGB2(poly, globe->colors[i + 1][0], globe->colors[i + 1][1], globe->colors[i + 1][2]);
        setRGB3(poly, globe->colors[i + 1][0], globe->colors[i + 1][1], globe->colors[i + 1][2]);
        for (j = 0; j < 16; j++) {
            otz = RotTransPers4(&globe->rows[i][j], &globe->rows[i][j + 1],
                               &globe->rows[i + 1][j], &globe->rows[i + 1][j + 1],
                               (long *)&poly->x0, (long *)&poly->x1,
                               (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x2814) <= 128) {
        MODEL_VARIANT_WORD(work, 0x2814) += 8;
        if (MODEL_VARIANT_WORD(work, 0x2814) >= 128) {
            MODEL_VARIANT_WORD(work, 0x2814) = 0;
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x284C) < 3) {
        if (MODEL_VARIANT_WORD(work, 0x27FC) < 8192) {
            MODEL_VARIANT_WORD(work, 0x27FC) += work->clock.step * 512;
            if (MODEL_VARIANT_WORD(work, 0x27FC) >= 8192) {
                MODEL_VARIANT_WORD(work, 0x27FC) = 8192;
                MODEL_VARIANT_WORD(work, 0x284C) = 3;
            }
        }
    } else if (MODEL_VARIANT_WORD(work, 0x284C) == 3) {
        MODEL_VARIANT_WORD(work, 0x27FC) = (rcos(MODEL_VARIANT_WORD(work, 0x2808)) * 2048 >> 12) + 6144;
        MODEL_VARIANT_WORD(work, 0x2808) += work->clock.step * 64;
        timing = (u8 *)work->config;
        if ((u32)MODEL_VARIANT_WORD(timing, 0x34) < (u32)MODEL_VARIANT_WORD(work, 0x27B0)) {
            MODEL_VARIANT_WORD(work, 0x284C) = 4;
        }
    } else if (MODEL_VARIANT_WORD(work, 0x284C) == 4) {
        MODEL_VARIANT_WORD(work, 0x27FC) += work->clock.step * 256;
        if (MODEL_VARIANT_WORD(work, 0x27FC) >= 16384) {
            MODEL_VARIANT_WORD(work, 0x284C) = 5;
            MODEL_VARIANT_WORD(work, 0x280C) = 1024;
        }
    } else if (MODEL_VARIANT_WORD(work, 0x284C) == 5) {
        MODEL_VARIANT_WORD(work, 0x27FC) += work->clock.step * 256;
        if (MODEL_VARIANT_WORD(work, 0x280C) > 0) {
            MODEL_VARIANT_WORD(work, 0x280C) -= work->clock.step * 32;
            if (MODEL_VARIANT_WORD(work, 0x280C) <= 0) {
                MODEL_VARIANT_WORD(work, 0x280C) = 0;
                MODEL_VARIANT_WORD(work, 0x284C) = 6;
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0x2804) += work->clock.step * 64;
}
