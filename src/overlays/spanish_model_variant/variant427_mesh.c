#include "../../types.h"
#include "variant427_mesh.h"

void func_8013BFB4(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    u8 red[9], green[9], blue[9];
    PSXLONG interpolation;
    PSXLONG flag;
    Mesh427State *state = (Mesh427State *)context;
    Mesh427Companion *companion = state->companions;
    Mesh427 *mesh = state->meshes;
    POLY_GT4 *polygon = &state->polygon;
    GsOT *ot;
    s32 i, row, column;
    s32 size;
    s32 depth;

    ot = func_80058F10();
    for (i = 0; i < 3; i++, mesh++, companion++) {
        if (!(state->frame & 1)) {
            size = mesh->size + state->size / 32;
        } else {
            size = mesh->size;
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
        if (state->phase >= 5) {
            for (row = 0; row < 9; row++) {
                red[row] = mesh->colors[row][0] * state->intensity / 1024;
                green[row] = mesh->colors[row][1] * state->intensity / 1024;
                blue[row] = mesh->colors[row][2] * state->intensity / 1024;
            }
        } else {
            for (row = 0; row < 9; row++) {
                red[row] = mesh->colors[row][0];
                green[row] = mesh->colors[row][1];
                blue[row] = mesh->colors[row][2];
            }
        }
        for (row = 0; row < 8; row++) {
            polygon->r0 = red[row];
            polygon->g0 = green[row];
            polygon->b0 = blue[row];
            polygon->r1 = red[row];
            polygon->g1 = green[row];
            polygon->b1 = blue[row];
            polygon->r2 = red[row + 1];
            polygon->g2 = green[row + 1];
            polygon->b2 = blue[row + 1];
            polygon->r3 = red[row + 1];
            polygon->g3 = green[row + 1];
            polygon->b3 = blue[row + 1];
            for (column = 0; column < 16; column++) {
                depth = RotTransPers4(&mesh->rows[row][column], &mesh->rows[row][column + 1],
                                     &mesh->rows[row + 1][column], &mesh->rows[row + 1][column + 1],
                                     (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                     (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
                                     &interpolation, &flag);
                if (companion->progress >= 1024 && depth >= 0 && flag >= 0) {
                    func_8005B260((u32 *)polygon, ot, (u16)depth, 1);
                }
            }
        }
        if (companion->progress >= 1024) {
            if (state->phase < 5) {
                if (mesh->size < 4096) {
                    mesh->size += state->step * 256;
                    if (mesh->size >= 4096) {
                        mesh->size = 4096;
                    }
                }
            } else if (mesh->size < 16384) {
                mesh->size += state->step * 64;
                if (mesh->size >= 16384) {
                    mesh->size = 16384;
                }
            }
        }
    }
    if (state->phase >= 3 && state->size < 4096) {
        state->size += state->step * 256;
        if (state->size >= 4096) {
            state->size = 4096;
        }
    }
    if (state->phase == 5 && state->intensity > 0) {
        state->intensity = 1024 - (state->meshes[0].size - 4096) / 12;
        if (state->intensity <= 0) {
            state->intensity = 0;
            state->phase = 6;
        }
    }
}
