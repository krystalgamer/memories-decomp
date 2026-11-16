#include "../../types.h"
#include "variant473_entry.h"

void FrenchModelVariant385_DrawGrid(EntryState473 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    u8 red[9];
    u8 green[9];
    u8 blue[9];
    long interpolation;
    long flag;
    EntryState473 *state = context;
    ModelVariantGridSheet *sheet = state->meshes;
    EntryRibbon473 *ribbon = state->ribbons;
    POLY_GT4 *quad = &state->extra;
    GsOT *ot;
    s32 i, j, k;
    s32 size, count;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    ratan2(state->direction.vz, state->direction.vx);
    ratan2(state->direction.vy, state->direction.vz);
    count = 0;
    for (i = 0; i < 5; i++, sheet++, ribbon++) {
        if (!(state->clock.frame_count & 1)) {
            size = sheet->size + state->size / 32;
        } else {
            size = sheet->size;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        matrix.t[0] = state->origins[i].vx;
        matrix.t[1] = state->origins[i].vy;
        matrix.t[2] = state->origins[i].vz;
        scale.vx = size;
        scale.vy = size;
        scale.vz = size;
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        for (k = 0; k < 9; k++) {
            if (sheet->size < 4096) {
                red[k] = sheet->colors[k][0];
                green[k] = sheet->colors[k][1];
                blue[k] = sheet->colors[k][2];
            } else {
                red[k] = sheet->colors[k][0] * (8192 - sheet->size) / 4096;
                green[k] = sheet->colors[k][1] * (8192 - sheet->size) / 4096;
                blue[k] = sheet->colors[k][2] * (8192 - sheet->size) / 4096;
            }
        }
        for (j = 0; j < 8; j++) {
            setRGB0(quad, red[j], green[j], blue[j]);
            setRGB1(quad, red[j], green[j], blue[j]);
            setRGB2(quad, red[j + 1], green[j + 1], blue[j + 1]);
            setRGB3(quad, red[j + 1], green[j + 1], blue[j + 1]);
            for (k = 0; k < 16; k++) {
                depth = RotTransPers4(&sheet->points[j][k], &sheet->points[j][k + 1],
                                      &sheet->points[j + 1][k], &sheet->points[j + 1][k + 1],
                                      (long *)&quad->x0, (long *)&quad->x1,
                                      (long *)&quad->x2, (long *)&quad->x3,
                                      &interpolation, &flag);
                if (ribbon->progress >= 1024 && depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (ribbon->progress >= 1024) {
            if (sheet->size <= 8192) {
                sheet->size += state->clock.step * 192;
                if (sheet->size >= 8192) {
                    if (state->phase < 5) {
                        sheet->size = 0;
                        ribbon->progress = 0;
                        ribbon->fade = 0;
                        ribbon->state = 1;
                    } else {
                        sheet->size = 0;
                        ribbon->progress = 0;
                        ribbon->fade = 0;
                        ribbon->state = 3;
                        sheet->done = 1;
                    }
                }
            }
        } else if (state->phase == 5) {
            sheet->done = 1;
        }
        count += sheet->done;
    }
    if (count == 5 && state->phase == count) {
        state->phase = 6;
    }
}
