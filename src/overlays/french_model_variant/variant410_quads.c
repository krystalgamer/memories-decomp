#include "../../types.h"
#include "variant410_quads.h"

void func_8013BAAC(u8 *context)
{
    Family410QuadView *work = (Family410QuadView *)context;
    SVECTOR rotation;
    /* Retail reserves 16 unaccessed bytes between rotation and scale. */
    u8 unknown_stack[16];
    s32 turn;
    s32 tilt;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Family410QuadRecord *record = &work->record;
    GsOT *ot;
    POLY_FT4 *quad;
    s16 i, j;
    s32 depth;
    s16 angle, angle2;
    s32 done;
    u8 r, g, b;

    ot = func_80058F10();
    done = 0;
    quad = &work->quad;
    turn = ratan2(work->direction[1], work->direction[2]);
    tilt = ratan2(work->screen_delta[1], work->screen_delta[0]);
    for (i = 0; i < 1; i++, record++) {
        for (j = 0, angle = 0, angle2 = 0; j < 10; j++, angle += 1300, angle2 += 1700) {
            if (record->scale[j] >= 0) {
                if (record->scale[j] > 3072) {
                    r = record->color.r * (4096 - record->scale[j]) / 1024;
                    g = record->color.g * (4096 - record->scale[j]) / 1024;
                    b = record->color.b * (4096 - record->scale[j]) / 1024;
                } else {
                    r = record->color.r;
                    g = record->color.g;
                    b = record->color.b;
                }
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = 0;
                matrix.t[0] = work->target.vx + ((rcos(angle + angle2 + record->angle[j]) * 128) >> 12);
                matrix.t[1] = work->target.vy + ((rsin(angle + record->angle[j]) * 128) >> 12);
                matrix.t[2] = work->target.vz + ((rsin(angle2 + record->angle[j]) * 128) >> 12);
                scale.vx = record->scale[j];
                scale.vy = record->scale[j];
                scale.vz = record->scale[j];
                RotMatrix(&rotation, &matrix);
                coordinate.coord = matrix;
                coordinate.super = 0;
                coordinate.flg = 0;
                GsGetLs(&coordinate, &light);
                GsSetLsMatrix(&light);
                ReadRotMatrix(&light);
                RotMatrix(&rotation, &light);
                ScaleMatrix(&light, &scale);
                SetRotMatrix(&light);
                depth = RotTransPers4(&record->points[0][j], &record->points[1][j],
                                     &record->points[2][j], &record->points[3][j],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag);
                setRGB0(quad, r, g, b);
                if (depth >= 0 && flag >= 0 && record->scale[j] < 4096) {
                    GsSortPoly(quad, ot, depth);
                }
            }
            if (work->phase < 3) {
                if (record->scale[j] < 4096) {
                    record->scale[j] += work->step << 8;
                    if (record->scale[j] >= 4096) {
                        record->scale[j] = 0;
                        if (work->phase == 1) {
                            work->phase = 2;
                        }
                    }
                }
            } else {
                if (record->scale[j] < 4096) {
                    record->scale[j] += work->step << 8;
                    if (record->scale[j] >= 4096) {
                        record->scale[j] = 4096;
                        record->done[j] = 1;
                    }
                }
            }
            done += record->done[j];
            if (j + 1 == 10 && done >= 10 && work->phase == 3) {
                work->phase = 4;
            }
        }
    }
}
