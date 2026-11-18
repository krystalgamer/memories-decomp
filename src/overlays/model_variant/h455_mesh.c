#include "../../types.h"
#include "../model_variant/h455_entry.h"

void FrenchModelVariant472_DrawMesh(EntryState455 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    EntryState455 *state = context;
    EntryDisc455 *mesh = state->discs;
    POLY_GT4 *quad = &state->disc_packet;
    POLY_F4 *mask = &state->mask;
    GsOT *ot;
    s32 i, row, col;
    s32 active;
    s32 yaw, pitch;
    s32 depth;
    u32 page;
    u8 red, green;

    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(state->aim.direction.vz, state->aim.direction.vx) + 2048;
    pitch = ratan2(state->aim.direction.vy, state->aim.direction.vz) + 2048;
    active = GsGetActiveBuff();
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw += 1024;
    }
    for (i = 0; i < 1; i++, mesh++) {
        if (mesh->progress > 0) {
            rotation.vx = 0;
            rotation.vy = state->spin;
            rotation.vz = 0;
            matrix.t[0] = state->origin.vx;
            matrix.t[1] = state->origin.vy;
            matrix.t[2] = state->origin.vz;
            scale.vx = mesh->progress;
            scale.vy = mesh->progress;
            scale.vz = mesh->progress;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            for (row = 0, red = 128, green = 160; row < 6; row++) {
                for (col = 0; col < 16; col++) {
                    RotTransPers4(&mesh->lower[row][col], &mesh->lower[row][col + 1],
                                  &mesh->lower[row + 1][col], &mesh->lower[row + 1][col + 1],
                                  (long *)&quad->x1, (long *)&quad->x0,
                                  (long *)&quad->x3, (long *)&quad->x2,
                                  &interpolation, &flag);
                    if (quad->x0 < 160) {
                        if (active == 0) {
                            page = GetTPage(2, 1, 320, 0);
                        } else {
                            page = GetTPage(2, 1, 0, 0);
                        }
                        SetPolyGT4(quad);
                        quad->tpage = page;
                        setUV4(quad, quad->x0, quad->y0, quad->x1, quad->y1,
                               quad->x2, quad->y2, quad->x3, quad->y3);
                    } else {
                        if (active == 0) {
                            page = GetTPage(2, 1, 448, 0);
                        } else {
                            page = GetTPage(2, 1, 128, 0);
                        }
                        SetPolyGT4(quad);
                        quad->tpage = page;
                        setUV4(quad, quad->x0 - 128, quad->y0, quad->x1 - 128, quad->y1,
                               quad->x2 - 128, quad->y2, quad->x3 - 128, quad->y3);
                    }
                    depth = RotTransPers4(&mesh->upper[row][col], &mesh->upper[row][col + 1],
                                          &mesh->upper[row + 1][col], &mesh->upper[row + 1][col + 1],
                                          (long *)&quad->x0, (long *)&quad->x1,
                                          (long *)&quad->x2, (long *)&quad->x3,
                                          &interpolation, &flag);
                    SetSemiTrans(quad, 0);
                    SetShadeTex(quad, 0);
                    setRGB0(quad, red, green, green);
                    setRGB1(quad, red, green, green);
                    setRGB2(quad, red, green, green);
                    setRGB3(quad, red, green, green);
                    setRGB0(mask, red, red, red);
                    setXY4(mask, quad->x0, quad->y0, quad->x1, quad->y1,
                           quad->x2, quad->y2, quad->x3, quad->y3);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(quad, ot, (u16)depth);
                    }
                }
            }
        }
        if (mesh->progress < 4096 && state->phase == 1) {
            mesh->progress = ((state->clock.frame - state->config->mesh_start) << 12) /
                             (state->config->bloom_start - state->config->mesh_start);
            if (mesh->progress >= 4096) {
                mesh->progress = 4096;
                state->phase = 2;
            }
        } else {
            if (state->clock.frame >= state->config->fade_end) {
                mesh->progress = 4096 - ((state->clock.frame - state->config->fade_end) << 12) /
                                            (state->config->mesh_fade_end - state->config->fade_end);
            }
        }
    }
    state->spin += state->clock.step * 8;
}
