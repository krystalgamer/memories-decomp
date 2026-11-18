#include "../../types.h"
#include "../model_variant/h462_entry_pal.h"

void FrenchModelVariant379_DrawGrid(Variant479EntryState *context)
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
    Variant479EntryState *state = context;
    ModelVariantGridSheet *sheet = state->grids;
    Variant479Rib *rib = state->ribs;
    POLY_GT4 *quad = &state->frame_poly;
    GsOT *ot;
    s32 i, j, k;
    s32 size, count;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    count = 0;
    for (i = 0; i < 5; i++, sheet++, rib++) {
        if (!(state->clock.frame_count & 1)) {
            size = sheet->size + state->size / 32;
        } else {
            size = sheet->size;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        matrix.t[0] = state->points[i].vx;
        matrix.t[1] = state->points[i].vy;
        matrix.t[2] = state->points[i].vz;
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
                red[k] = sheet->colors[k].r;
                green[k] = sheet->colors[k].g;
                blue[k] = sheet->colors[k].b;
            } else {
                red[k] = sheet->colors[k].r * (8192 - sheet->size) / 4096;
                green[k] = sheet->colors[k].g * (8192 - sheet->size) / 4096;
                blue[k] = sheet->colors[k].b * (8192 - sheet->size) / 4096;
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
                if (rib->size >= 1024 && depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                }
            }
        }
        if (rib->size >= 1024) {
            if (sheet->size <= 8192) {
                sheet->size += state->clock.step * 192;
                if (sheet->size >= 8192) {
                    if (state->phase < 5) {
                        sheet->size = 0;
                        rib->size = 0;
                        rib->fade = 0;
                        rib->state = 1;
                    } else {
                        sheet->size = 0;
                        rib->size = 0;
                        rib->fade = 0;
                        rib->state = 3;
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
