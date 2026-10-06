#include "../../types.h"
#include "variant443_mesh.h"
#include "../../game/gpu_packets.h"

void func_8013D98C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Mesh443State *state = (Mesh443State *)context;
    POLY_G4 *quad = &state->quad;
    Mesh443 *mesh = state->mesh;
    Companion443 *companion = &state->companion;
    GsOT *ot;
    s16 i, j, k;
    s16 row_angle, angle, radius;
    s32 tilt, row_index;
    SVECTOR *row;
    s32 depth;

    ot = func_80058F10();
    ratan2(state->direction[2], state->direction[0]);
    ratan2(state->direction[1], state->direction[0]);
    tilt = ratan2(state->path[2], state->path[1]) + 3072;
    ratan2(state->projected.vy, state->projected.vx);
    radius = companion->size / 64;
    for (i = 0; i < 1; i++, mesh++) {
        for (j = 0, row_angle = state->angle; j < 9; j++, row_angle += 512) {
            for (k = 0, angle = row_angle, row_index = j,
                 row = (SVECTOR *)(row_index * sizeof(mesh->points[0]) + (u32)mesh->points);
                 k < 9; k++, angle = row_angle + k * 512) {
                row[k].vx = (state->path[0] * state->progress / 1024) * row_index / 8
                    + (rcos(angle) * radius >> 12);
                row[k].vy = (state->path[1] * state->progress / 1024) * row_index / 8
                    + (rsin(angle) * (rcos(tilt) * radius >> 12) >> 12);
                row[k].vz = (state->path[2] * state->progress / 1024) * row_index / 8
                    + (rsin(angle) * (rsin(tilt) * radius >> 12) >> 12);
            }
        }
    }
    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = state->origin[0];
    matrix.t[1] = state->origin[1];
    matrix.t[2] = state->origin[2];
    /* Retail initializes this vector but does not apply it. */
    scale.vx = 4096;
    scale.vy = 4096;
    scale.vz = 4096;
    RotMatrix(&rotation, &matrix);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    mesh = state->mesh;
    for (i = 0; i < 1; i++, mesh++) {
        for (j = 0; j < 8; j++) {
            for (k = 0; k < 8; k++) {
                depth = RotTransPers4(&mesh->points[j][k], &mesh->points[j][k + 1],
                                     &mesh->points[j + 1][k], &mesh->points[j + 1][k + 1],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag);
                quad->r0 = mesh->colors[k].r;
                quad->g0 = mesh->colors[k].g;
                quad->b0 = mesh->colors[k].b;
                quad->r1 = mesh->colors[k + 1].r;
                quad->g1 = mesh->colors[k + 1].g;
                quad->b1 = mesh->colors[k + 1].b;
                quad->r2 = mesh->colors[k].r;
                quad->g2 = mesh->colors[k].g;
                quad->b2 = mesh->colors[k].b;
                quad->r3 = mesh->colors[k + 1].r;
                quad->g3 = mesh->colors[k + 1].g;
                quad->b3 = mesh->colors[k + 1].b;
                if (depth >= 0 && flag >= 0) {
                    func_8005B260((u32 *)quad, ot, (u16)depth, 1);
                }
            }
        }
    }
    if (state->phase == 4 && state->progress < 1024) {
        state->progress = (state->time - state->timing->grow_start) * 1024
            / (state->timing->grow_end - state->timing->grow_start);
        if (state->progress >= 1024) {
            state->progress = 1024;
            state->phase = 5;
        }
    }
    state->angle -= state->step * 128;
}
