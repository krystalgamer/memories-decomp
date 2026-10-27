#include "../../types.h"
#include "variant472_mesh.h"

void func_8013C734(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Mesh719State *state = (Mesh719State *)context;
    Mesh719 *mesh = state->meshes;
    POLY_GT4 *quad = &state->quad;
    POLY_F4 *mask = &state->mask;
    GsOT *ot;
    s32 i, row, col;
    s32 active;
    s32 yaw, pitch;
    s32 depth;
    u32 page;
    u8 red, green;

    ot = func_80058F10();
    yaw = ratan2(state->direction[2], state->direction[0]) + 2048;
    pitch = ratan2(state->direction[1], state->direction[2]) + 2048;
    active = GsGetActiveBuff();
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw += 1024;
    }
    for (i = 0; i < 1; i++, mesh++) {
        if (mesh->progress > 0) {
            rotation.vx = 0;
            rotation.vy = state->angle;
            rotation.vz = 0;
            matrix.t[0] = state->position[0];
            matrix.t[1] = state->position[1];
            matrix.t[2] = state->position[2];
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
                    RotTransPers4(&mesh->back[row][col], &mesh->back[row][col + 1],
                                  &mesh->back[row + 1][col], &mesh->back[row + 1][col + 1],
                                  (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x0,
                                  (PSXLONG *)&quad->x3, (PSXLONG *)&quad->x2,
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
                    depth = RotTransPers4(&mesh->front[row][col], &mesh->front[row][col + 1],
                                          &mesh->front[row + 1][col], &mesh->front[row + 1][col + 1],
                                          (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                          (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
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
            mesh->progress = ((state->time - state->timing->start) << 12) /
                             (state->timing->end - state->timing->start);
            if (mesh->progress >= 4096) {
                mesh->progress = 4096;
                state->phase = 2;
            }
        } else {
            if (state->time >= state->timing->fade_start) {
                mesh->progress = 4096 - ((state->time - state->timing->fade_start) << 12) /
                                            (state->timing->fade_end - state->timing->fade_start);
            }
        }
    }
    state->angle += state->step * 8;
}
