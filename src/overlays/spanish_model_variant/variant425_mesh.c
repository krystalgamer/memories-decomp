#include "../../types.h"
#include "variant425_mesh.h"

void func_8013BEE4(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    u8 red[9], green[9], blue[9];
    PSXLONG interpolation;
    PSXLONG flag;
    Mesh425State *state = (Mesh425State *)context;
    Mesh425Rows *mesh = &state->mesh;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j;
    s32 size;
    s32 depth;

    ot = func_80058F10();
    ratan2(state->direction[2], state->direction[0]);
    ratan2(state->direction[1], state->direction[2]);
    polygon = &state->polygon;
    if (!(state->frame & 1)) {
        size = state->size + state->size / 16;
    } else {
        size = state->size;
    }
    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = state->origin.vx;
    matrix.t[1] = state->origin.vy;
    matrix.t[2] = state->origin.vz;
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
    if (state->phase >= 5) {
        for (i = 0; i < 9; i++) {
            red[i] = mesh->colors[i][0] * state->intensity / 1024;
            green[i] = mesh->colors[i][1] * state->intensity / 1024;
            blue[i] = mesh->colors[i][2] * state->intensity / 1024;
        }
    } else {
        for (i = 0; i < 9; i++) {
            red[i] = mesh->colors[i][0];
            green[i] = mesh->colors[i][1];
            blue[i] = mesh->colors[i][2];
        }
    }
    for (i = 0; i < 8; i++) {
        polygon->r0 = red[i];
        polygon->g0 = green[i];
        polygon->b0 = blue[i];
        polygon->r1 = red[i];
        polygon->g1 = green[i];
        polygon->b1 = blue[i];
        polygon->r2 = red[i + 1];
        polygon->g2 = green[i + 1];
        polygon->b2 = blue[i + 1];
        polygon->r3 = red[i + 1];
        polygon->g3 = green[i + 1];
        polygon->b3 = blue[i + 1];
        for (j = 0; j < 16; j++) {
            depth = RotTransPers4(&mesh->rows[i][j], &mesh->rows[i][j + 1],
                                 &mesh->rows[i + 1][j], &mesh->rows[i + 1][j + 1],
                                 (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                 (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
                                 &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
    }
    if (state->phase >= 3 && state->size < 4096) {
        state->size += state->step * 32;
        if (state->size >= 4096) {
            state->size = 4096;
        }
    }
    if (state->phase == 5 && state->intensity > 0) {
        state->intensity -= state->step * 32;
        if (state->intensity <= 0) {
            state->intensity = 0;
            state->phase = 6;
        }
    }
}
