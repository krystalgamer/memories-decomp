#include "../../types.h"
#include "variant427_panels.h"

void func_8013D8A0(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Panel427State *state = (Panel427State *)context;
    u8 *primary = context + sizeof(state->unknown_0000);
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j;
    s32 extra;
    s32 x, y, z;
    s32 depth;

    ot = func_80058F10();
    polygon = &state->polygon;
    /* The byte cursor stays in the context; only the first three positions are primary records. */
    for (i = 0; i < 6; i++, sheet++, primary += sizeof(Panel427Primary)) {
        extra = 0;
        if (state->frame & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i < 3) {
            x = state->directions[i].vx * ((Panel427Primary *)primary)->progress / 1024;
            y = state->directions[i].vy * ((Panel427Primary *)primary)->progress / 1024;
            z = state->directions[i].vz * ((Panel427Primary *)primary)->progress / 1024;
            matrix.t[0] = state->matrices[i].t[0] + x;
            matrix.t[1] = state->matrices[i].t[1] + y;
            matrix.t[2] = state->matrices[i].t[2] + z;
        } else {
            matrix.t[0] = state->matrices[i - 3].t[0];
            matrix.t[1] = state->matrices[i - 3].t[1];
            matrix.t[2] = state->matrices[i - 3].t[2];
        }
        scale.vx = sheet->size + extra;
        scale.vy = sheet->size + extra;
        scale.vz = sheet->size + extra;
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
        for (j = 0; j < 4; j++) {
            depth = RotTransPers4(&sheet->v0[j], &sheet->v1[j], &sheet->v2[j], &sheet->v3[j],
                                 (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                 (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = sheet->inner[0];
            polygon->g0 = sheet->inner[1];
            polygon->b0 = sheet->inner[2];
            polygon->r1 = sheet->inner[0];
            polygon->g1 = sheet->inner[1];
            polygon->b1 = sheet->inner[2];
            polygon->r2 = sheet->inner[0];
            polygon->g2 = sheet->inner[1];
            polygon->b2 = sheet->inner[2];
            polygon->r3 = sheet->outer[0];
            polygon->g3 = sheet->outer[1];
            polygon->b3 = sheet->outer[2];
            depth = depth * 8 / 10;
            if (i >= 3 || ((Panel427Primary *)primary)->progress >= 0) {
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(polygon, ot, depth);
                }
            }
        }
        if (i < 3) {
            if (state->phase == 2) {
                sheet->size = 4096;
            } else if (state->phase == 3) {
                if (sheet->size < 8192) {
                    sheet->size += state->step * 512;
                    if (sheet->size >= 8192) {
                        sheet->size = 8192;
                        state->phase = 4;
                    }
                }
            }
            if (state->time >= state->descriptor->shrink_start && sheet->size > 0) {
                sheet->size = 8192 - (state->time - state->descriptor->shrink_start) * 8192
                    / (state->descriptor->shrink_end - state->descriptor->shrink_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else {
            if (state->phase == 0 && sheet->size < 4096) {
                sheet->size = (state->time - state->descriptor->grow_start) * 4096
                    / (state->descriptor->grow_end - state->descriptor->grow_start);
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->phase = 1;
                }
            }
            if (state->time >= state->descriptor->shrink_start && sheet->size > 0) {
                sheet->size = 4096 - (state->time - state->descriptor->shrink_start) * 4096
                    / (state->descriptor->shrink_end - state->descriptor->shrink_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        }
    }
}
