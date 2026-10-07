#include "../../types.h"

#include "variant456_sheets.h"

#define SET_SCALE(v, s) \
    do { \
        (v).vx = (s); \
        (v).vy = (s); \
        (v).vz = (s); \
    } while (0)

/* Eight sheets of four POLY_GT4 quads. The first sheet sits at the origin and
 * flickers by an eighth of its size on odd frames; the others sit at their own
 * positions, never shrink below zero and widen by an eighth on even frames.
 * The first sheet grows to 0x800 over the timing record's growth window and
 * shrinks over its fade window. The others spread to 0x2000 from the spread
 * start and then fade their colours out, advancing the phase to 3 and 5. */
void func_8013DACC(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheets456State *state = (Sheets456State *)context;
    Sheet456 *sheet = state->sheets;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, j;
    s32 bias;
    s32 size;
    s32 depth;
    s32 r, g, b, outer_r, outer_g, outer_b;

    ot = func_80058F10();
    quad = &state->quad;
    for (i = 0; i < 8; i++, sheet++) {
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = state->origin[0];
            matrix.t[1] = state->origin[1];
            matrix.t[2] = state->origin[2];
            bias = 0;
            if (state->frame & 1) {
                bias = sheet->size / 8;
            }
            SET_SCALE(scale, sheet->size + bias);
        } else {
            if (sheet->size < 0) {
                size = 0;
            } else {
                size = sheet->size;
            }
            if (!(state->frame & 1)) {
                size += size / 8;
            }
            matrix.t[0] = sheet->position[0];
            matrix.t[1] = sheet->position[1];
            matrix.t[2] = sheet->position[2];
            SET_SCALE(scale, size);
        }
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
        if (i == 0) {
            setUV4(quad, 64, 0, 127, 0, 64, 63, 127, 63);
        } else {
            setUV4(quad, 64, 64, 127, 64, 64, 127, 127, 127);
        }
        if (sheet->faded == 0) {
            quad->r0 = sheet->inner[0];
            quad->g0 = sheet->inner[1];
            quad->b0 = sheet->inner[2];
            quad->r1 = sheet->inner[0];
            quad->g1 = sheet->inner[1];
            quad->b1 = sheet->inner[2];
            quad->r2 = sheet->inner[0];
            quad->g2 = sheet->inner[1];
            quad->b2 = sheet->inner[2];
            quad->r3 = sheet->outer[0];
            quad->g3 = sheet->outer[1];
            quad->b3 = sheet->outer[2];
        } else {
            r = sheet->inner[0] * sheet->fade / 1024;
            g = sheet->inner[1] * sheet->fade / 1024;
            b = sheet->inner[2] * sheet->fade / 1024;
            outer_r = sheet->outer[0] * sheet->fade / 1024;
            outer_g = sheet->outer[1] * sheet->fade / 1024;
            outer_b = sheet->outer[2] * sheet->fade / 1024;
            quad->r0 = r;
            quad->g0 = g;
            quad->b0 = b;
            quad->r1 = r;
            quad->g1 = g;
            quad->b1 = b;
            quad->r2 = r;
            quad->g2 = g;
            quad->b2 = b;
            quad->r3 = outer_r;
            quad->g3 = outer_g;
            quad->b3 = outer_b;
        }
        for (j = 0; j < 4; j++) {
            depth = RotTransPers4(&sheet->v0[j], &sheet->v1[j],
                                 &sheet->v2[j], &sheet->v3[j],
                                 (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                 (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                 &interpolation, &flag);
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, (u16)depth);
            }
        }
        if (i == 0) {
            if (state->phase == 0 && sheet->size < 2048) {
                sheet->size = (state->time - state->timing->grow_start) * 2048
                    / (state->timing->grow_end - state->timing->grow_start);
                if (sheet->size >= 2048) {
                    sheet->size = 2048;
                    state->phase = 1;
                }
            }
            if (state->time >= state->timing->fade_start) {
                sheet->size = 2048 - (state->time - state->timing->fade_start) * 2048
                    / (state->timing->fade_end - state->timing->fade_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else if (sheet->size < 8192 && state->timing->spread_start <= state->time) {
            sheet->size += state->step * 2048;
            if (sheet->size >= 8192) {
                sheet->size = 8192;
                sheet->faded = 1;
                if (i + 1 == 8) {
                    state->phase = 3;
                }
            }
        } else if (sheet->fade > 0 && sheet->faded == 1) {
            sheet->fade -= state->step * 32;
            if (sheet->fade <= 0) {
                sheet->fade = 0;
                if (i + 1 == 8 && state->phase == 3) {
                    state->phase = 5;
                }
            }
        }
    }
}
