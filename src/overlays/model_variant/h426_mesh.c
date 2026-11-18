#include "../../types.h"
#include "../model_variant/h426_entry_pal.h"
#include "../../game/gpu_packets.h"

void SpanishModelVariant443_DrawMesh(EntryState443 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    EntryState443 *state = context;
    POLY_G4 *quad = &state->packets.quad;
    ModelVariantTube *mesh = state->meshes;
    ModelVariantSheet *companion = state->sheets;
    GsOT *ot;
    s16 i, j, k;
    s16 row_angle, angle, radius;
    s32 tilt, row_index;
    SVECTOR *row;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    ratan2(state->view_delta.vz, state->view_delta.vx);
    ratan2(state->view_delta.vy, state->view_delta.vx);
    tilt = ratan2(state->direction.vz, state->direction.vy) + 3072;
    ratan2(state->projected.vy, state->projected.vx);
    radius = companion->size / 64;
    for (i = 0; i < 1; i++, mesh++) {
        for (j = 0, row_angle = state->mesh_angle; j < 9; j++, row_angle += 512) {
            for (k = 0, angle = row_angle, row_index = j,
                 row = (SVECTOR *)(row_index * sizeof(mesh->points[0]) + (u32)mesh->points);
                 k < 9; k++, angle = row_angle + k * 512) {
                row[k].vx = (state->direction.vx * state->progress / 1024) * row_index / 8
                    + (rcos(angle) * radius >> 12);
                row[k].vy = (state->direction.vy * state->progress / 1024) * row_index / 8
                    + (rsin(angle) * (rcos(tilt) * radius >> 12) >> 12);
                row[k].vz = (state->direction.vz * state->progress / 1024) * row_index / 8
                    + (rsin(angle) * (rsin(tilt) * radius >> 12) >> 12);
            }
        }
    }
    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = state->origin.vx;
    matrix.t[1] = state->origin.vy;
    matrix.t[2] = state->origin.vz;
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
    mesh = state->meshes;
    for (i = 0; i < 1; i++, mesh++) {
        for (j = 0; j < 8; j++) {
            for (k = 0; k < 8; k++) {
                depth = RotTransPers4(&mesh->points[j][k], &mesh->points[j][k + 1],
                                     &mesh->points[j + 1][k], &mesh->points[j + 1][k + 1],
                                     (long *)&quad->x0, (long *)&quad->x1,
                                     (long *)&quad->x2, (long *)&quad->x3,
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
                    Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                }
            }
        }
    }
    if (state->phase == 4 && state->progress < 1024) {
        state->progress = (state->clock.frame - state->config->mesh_start) * 1024
            / (state->config->mesh_end - state->config->mesh_start);
        if (state->progress >= 1024) {
            state->progress = 1024;
            state->phase = 5;
        }
    }
    state->mesh_angle -= state->clock.step * 128;
}
