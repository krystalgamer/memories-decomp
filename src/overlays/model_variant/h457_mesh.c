#include "../../types.h"
#include "h457_entry_pal.h"

void SpanishModelVariant474_DrawGouraudMesh(Variant474EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    u8 red[9], green[9], blue[9];
    long interpolation;
    long flag;
    Variant474EntryState *state = context;
    ModelVariantMesh *mesh = &state->mesh;
    POLY_G4 *polygon;
    GsOT *ot;
    s32 i, j;
    s32 size;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    ratan2(state->direction.vz, state->direction.vx);
    ratan2(state->direction.vy, state->direction.vz);
    polygon = &state->packets.quad;
    if (!(state->clock.frame_count & 1)) {
        size = state->mesh_size + state->mesh_size / 16;
    } else {
        size = state->mesh_size;
    }
    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = state->target.vx;
    matrix.t[1] = state->target.vy;
    matrix.t[2] = state->target.vz;
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
            red[i] = mesh->colors[i].r * state->mesh_intensity / 1024;
            green[i] = mesh->colors[i].g * state->mesh_intensity / 1024;
            blue[i] = mesh->colors[i].b * state->mesh_intensity / 1024;
        }
    } else {
        for (i = 0; i < 9; i++) {
            red[i] = mesh->colors[i].r;
            green[i] = mesh->colors[i].g;
            blue[i] = mesh->colors[i].b;
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
                                 (long *)&polygon->x0, (long *)&polygon->x1,
                                 (long *)&polygon->x2, (long *)&polygon->x3,
                                 &interpolation, &flag);
            if (depth > 0) {
                Graphics_SubmitDrawModePacket((u32 *)polygon, ot, (u16)depth, 1);
            }
        }
    }
    if (state->phase >= 3 && state->mesh_size < 4096) {
        state->mesh_size += state->clock.step * 256;
        if (state->mesh_size >= 4096) {
            state->mesh_size = 4096;
        }
    }
    if (state->phase == 5 && state->mesh_intensity > 0) {
        state->mesh_intensity -= state->clock.step * 32;
        if (state->mesh_intensity <= 0) {
            state->mesh_intensity = 0;
            state->phase = 6;
        }
    }
}
