#include "../../types.h"
#include "variant379_grid.h"

void func_8013BFD0(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    u8 red[9];
    u8 green[9];
    u8 blue[9];
    PSXLONG interpolation;
    PSXLONG flag;
    Grid379State *state = (Grid379State *)context;
    Grid379Sheet *sheet = state->sheets;
    Grid379Pulse *pulse = state->pulses;
    POLY_GT4 *quad = &state->quad;
    GsOT *ot;
    s32 i, j, k;
    s32 size, count;
    s32 depth;

    ot = func_80058F10();
    count = 0;
    for (i = 0; i < 5; i++, sheet++, pulse++) {
        if (!(state->flags & 1)) {
            size = sheet->size + state->pulse / 32;
        } else {
            size = sheet->size;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        matrix.t[0] = state->positions[i].vx;
        matrix.t[1] = state->positions[i].vy;
        matrix.t[2] = state->positions[i].vz;
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
                                      (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                      (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                      &interpolation, &flag);
                if (pulse->progress >= 1024 && depth >= 0 && flag >= 0) {
                    func_8005B260((u32 *)quad, ot, (u16)depth, 1);
                }
            }
        }
        if (pulse->progress >= 1024) {
            if (sheet->size <= 8192) {
                sheet->size += state->step * 192;
                if (sheet->size >= 8192) {
                    if (state->phase < 5) {
                        sheet->size = 0;
                        pulse->progress = 0;
                        pulse->offset = 0;
                        pulse->state = 1;
                    } else {
                        sheet->size = 0;
                        pulse->progress = 0;
                        pulse->offset = 0;
                        pulse->state = 3;
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
