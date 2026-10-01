#include "../../types.h"
#include "variant448_orbit.h"

void func_8013D638(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    u8 *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *object;
    GsOT *ot;
    POLY_GT4 *poly;
    Variant448Orbit *sheet;
    s32 n, i, j, k;
    s32 jangle;
    s32 iangle;
    s32 bias;
    s32 angle;
    s32 count;
    s32 otz;

    work = ctx;
    object = ctx + 0xBD8;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x19BC);
    sheet = (Variant448Orbit *)(ctx + 0x11F0);
    count = 0;
    for (i = 0, iangle = 0; i < 3; i++, object += 0x208, iangle = (i << 10) / 3) {
        for (j = 0, jangle = 0; j < 2; j++, jangle += 2048) {
            if (MODEL_VARIANT_WORD(object, 0x200 + j * 4) > 0) {
                for (k = 0, angle = jangle + iangle; k < 3; k++, angle = jangle + iangle + (k << 12) / 3) {
                    bias = 0;
                    if (MODEL_VARIANT_WORD(work, 0x1AFC) & 1) {
                        bias = sheet->size[i][j][k] / 8;
                    }
                    rot.vx = 0;
                    rot.vy = 0;
                    rot.vz = 0;
                    m.t[0] = MODEL_VARIANT_WORD(work, 0x1AB8) + (((rcos(angle) * 3 << 6) * (j + 1)) >> 12);
                    m.t[1] = MODEL_VARIANT_WORD(work, 0x1ABC);
                    m.t[2] = MODEL_VARIANT_WORD(work, 0x1AC0) + (((rsin(angle) * 3 << 6) * (j + 1)) >> 12);
                    scale.vx = sheet->size[i][j][k] + bias;
                    scale.vy = sheet->size[i][j][k] + bias;
                    scale.vz = sheet->size[i][j][k] + bias;
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
                    for (n = 0; n < 4; n++) {
                        otz = RotTransPers4(&sheet->points[0][n], &sheet->points[1][n], &sheet->points[2][n], &sheet->points[3][n],
                            (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                        poly->r0 = sheet->inner.r;
                        poly->g0 = sheet->inner.g;
                        poly->b0 = sheet->inner.b;
                        poly->r1 = sheet->inner.r;
                        poly->g1 = sheet->inner.g;
                        poly->b1 = sheet->inner.b;
                        poly->r2 = sheet->inner.r;
                        poly->g2 = sheet->inner.g;
                        poly->b2 = sheet->inner.b;
                        poly->r3 = sheet->outer.r;
                        poly->g3 = sheet->outer.g;
                        poly->b3 = sheet->outer.b;
                        if (otz >= 0 && flag >= 0) {
                            GsSortPoly(poly, ot, otz);
                        }
                    }
                    if (sheet->size[i][j][k] < 8192 && sheet->done[i][j][k] == 0) {
                        sheet->size[i][j][k] += MODEL_VARIANT_WORD(work, 0x1B08) << 10;
                        if (sheet->size[i][j][k] >= 8192) {
                            sheet->size[i][j][k] = 8192;
                            sheet->done[i][j][k] = 1;
                        }
                    } else if (sheet->size[i][j][k] > 0 && sheet->done[i][j][k] == 1) {
                        sheet->size[i][j][k] -= MODEL_VARIANT_WORD(work, 0x1B08) << 8;
                        if (sheet->size[i][j][k] <= 0) {
                            sheet->size[i][j][k] = 0;
                            sheet->done[i][j][k] = 2;
                        }
                    }
                    count += sheet->done[i][j][k];
                    if (count >= 36 && MODEL_VARIANT_WORD(work, 0x1B3C) == 6) {
                        MODEL_VARIANT_WORD(work, 0x1B3C) = 7;
                    }
                }
            }
        }
    }
}
