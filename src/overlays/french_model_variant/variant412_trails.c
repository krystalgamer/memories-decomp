#include "../../types.h"
#include "variant412_trails.h"

void func_8013BB98(u8 *ctx)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX part;
    CVECTOR inner[6][31];
    CVECTOR outer[6][31];
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    u8 *work;
    GsOT *ot;
    s16 i;
    s16 j;
    s32 dx;
    s32 dz;
    s32 last;
    s32 spread;
    s32 fade;
    s32 depth;
    s32 x_offset;
    Variant412Trail *trail;
    POLY_G4 *quad;
    SVECTOR *row;
    SVECTOR *row_points;
    SVECTOR *point;

    work = ctx;
    trail = (Variant412Trail *)work;
    ot = func_80058F10();
    ratan2(MODEL_VARIANT_HALF(work, 0x1AEA), MODEL_VARIANT_HALF(work, 0x1AE8));
    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = MODEL_VARIANT_WORD(work, 0x1AC4);
    matrix.t[1] = MODEL_VARIANT_WORD(work, 0x1AC8);
    matrix.t[2] = MODEL_VARIANT_WORD(work, 0x1ACC);
    scale.vx = 4096;
    scale.vy = 4096;
    scale.vz = 4096;
    RotMatrix(&rotation, &matrix);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    quad = (POLY_G4 *)(work + 0x1920);
    if ((u32)MODEL_VARIANT_WORD(work, 0x1B0C)
        < ((Variant412TimingLink *)(work + 0x1B24))->timing->capture_start) {
        last = -1;
    } else if ((u32)MODEL_VARIANT_WORD(work, 0x1B0C)
               < ((Variant412TimingLink *)(work + 0x1B24))->timing->capture_stop) {
        last = MODEL_VARIANT_WORD(work, 0x1B18);
        if (MODEL_VARIANT_WORD(work, 0x1B08) != MODEL_VARIANT_WORD(work, 0x1B10)) {
            MODEL_VARIANT_WORD(work, 0x1B18)++;
        }
        for (i = 0; i < 6; i++) {
            GsGetLw(((Variant412Links *)(work + 0x1B30))->part[i], &part);
            if (i == 0) {
                ((SVECTOR *)work)[last].vx = part.t[0];
                ((SVECTOR *)work)[last].vy = part.t[1];
                ((SVECTOR *)work)[last].vz = part.t[2];
            } else {
                /* These are guest work addresses, not native-stack addresses. */
                row = (SVECTOR *)(i * sizeof(trail->original[0]) + (u32)trail);
                row[last].vx = part.t[0];
                row[last].vy = part.t[1];
                row[last].vz = part.t[2];
                if (i == 5) {
                    dx = trail->original[5][last].vx - trail->original[0][last].vx;
                    dz = trail->original[5][last].vz - trail->original[0][last].vz;
                }
                /* Retail consumes unwritten differences on rows 1 through 4. */
                trail->angle[last] = ratan2(dz, dx) + 2048;
            }
        }
    } else {
        last = MODEL_VARIANT_WORD(work, 0x1B18) - 1;
    }
    for (i = 0; i < 6; i++) {
        for (j = 0; j <= last; j++) {
            spread = trail->progress[j] * 2;
            fade = 1024 - trail->progress[j];
            x_offset = rcos(trail->angle[j]) * spread >> 12;
            row_points = (SVECTOR *)(i * sizeof(trail->original[0]) + (u32)trail);
            point = (SVECTOR *)(j * sizeof(SVECTOR) + (u32)row_points);
            point[6 * 31].vx = trail->original[i][j].vx
                + x_offset;
            point += 6 * 31;
            point->vy = trail->original[i][j].vy;
            point->vz = trail->original[i][j].vz
                + (rsin(trail->angle[j]) * spread >> 12);
            if (trail->progress[j] > 512) {
                inner[i][j].r = trail->inner[i][j].r * fade / 512;
                inner[i][j].g = trail->inner[i][j].g * fade / 512;
                inner[i][j].b = trail->inner[i][j].b * fade / 512;
                outer[i][j].r = trail->outer[i][j].r * fade / 512;
                outer[i][j].g = trail->outer[i][j].g * fade / 512;
                outer[i][j].b = trail->outer[i][j].b * fade / 512;
            } else {
                inner[i][j].r = trail->inner[i][j].r;
                inner[i][j].g = trail->inner[i][j].g;
                inner[i][j].b = trail->inner[i][j].b;
                outer[i][j].r = trail->outer[i][j].r;
                outer[i][j].g = trail->outer[i][j].g;
                outer[i][j].b = trail->outer[i][j].b;
            }
            if (i == 0) {
                if (trail->progress[j] < 1024) {
                    trail->progress[j] += 64;
                    if (trail->progress[j] >= 1024) {
                        trail->progress[j] = 1024;
                        if (j == MODEL_VARIANT_WORD(work, 0x1B18) - 1
                            && MODEL_VARIANT_WORD(work, 0x1B64) == 0) {
                            MODEL_VARIANT_WORD(work, 0x1B64) = 1;
                        }
                    }
                }
            }
        }
    }
    for (i = 0; i < 5; i++) {
        for (j = 0; j < last; j++) {
            depth = RotTransPers4(&trail->moved[i][j], &trail->moved[i][j + 1],
                                 &trail->moved[i + 1][j], &trail->moved[i + 1][j + 1],
                                 (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                 (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                 &interpolation, &flag);
            setRGB0(quad, inner[i][j].r, inner[i][j].g, inner[i][j].b);
            setRGB1(quad, inner[i][j + 1].r, inner[i][j + 1].g, inner[i][j + 1].b);
            setRGB2(quad, outer[i + 1][j].r, outer[i + 1][j].g, outer[i + 1][j].b);
            setRGB3(quad, outer[i + 1][j + 1].r, outer[i + 1][j + 1].g, outer[i + 1][j + 1].b);
            if (depth >= 0 && flag >= 0) {
                func_8005B260((u32 *)quad, ot, (u16)depth, 1);
            }
        }
    }
}
