#include "../../types.h"
#include "../model_variant/variant417_entry.h"

void FrenchModelVariant417_DrawFramebufferFeedback(Variant417EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    ModelVariantSheet *sheets = context->sheets;
    Variant417EntryState *state = context;
    ModelVariantTripleCurtain *group = state->curtains;
    POLY_GT4 *quad = &state->plain;
    GsOT *ot;
    s32 i, j;
    s32 width, radius;
    s32 size, depth;
    s32 active;
    s32 yaw, pitch;
    s32 angle;
    s32 z;
    u32 page;

    ot = Graphics_GetDefaultOrderingTable();
    active = GsGetActiveBuff();
    yaw = ratan2(state->aim.direction.vz, state->aim.direction.vx) + 2048;
    pitch = ratan2(state->aim.direction.vy, state->aim.direction.vz) + 2048;
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw += 1024;
    }
    width = sheets[1].size / 128;
    radius = sheets[1].size * 100 / 8192;
    for (i = 0; i < 5; i++, group++) {
        if (group->scale > 0) {
            size = rsin(group->scale) * 4096 >> 12;
            if (state->phase >= 5) {
                z = -width;
                for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                    setVector(&group->b[j], rcos(angle) * (256 + radius) >> 12,
                              rsin(angle) * (256 + radius) >> 12, z);
                    setVector(&group->c[j], rcos(angle) * (284 + radius) >> 12,
                              rsin(angle) * (284 + radius) >> 12, z);
                }
            }
            rotation.vx = -pitch;
            rotation.vy = yaw;
            rotation.vz = 0;
            matrix.t[0] = state->aim.target.vx +
                          state->aim.direction.vx * (group->scale - 1024) / 2048;
            matrix.t[1] = state->aim.target.vy +
                          state->aim.direction.vy * (group->scale - 1024) / 2048;
            matrix.t[2] = state->aim.target.vz +
                          state->aim.direction.vz * (group->scale - 1024) / 2048;
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
            for (j = 0; j < 16; j++) {
                RotTransPers4(&group->a[j], &group->a[j + 1],
                              &group->c[j], &group->c[j + 1],
                              (long *)&quad->x0, (long *)&quad->x1,
                              (long *)&quad->x2, (long *)&quad->x3,
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
                depth = RotTransPers4(&group->a[j], &group->a[j + 1],
                                     &group->b[j], &group->b[j + 1],
                                     (long *)&quad->x0, (long *)&quad->x1,
                                     (long *)&quad->x2, (long *)&quad->x3,
                                     &interpolation, &flag);
                SetSemiTrans(quad, 1);
                SetShadeTex(quad, 0);
                setRGB0(quad, group->color_a.r, group->color_a.g, group->color_a.b);
                setRGB1(quad, group->color_a.r, group->color_a.g, group->color_a.b);
                setRGB2(quad, group->color_b.r, group->color_b.g, group->color_b.b);
                setRGB3(quad, group->color_b.r, group->color_b.g, group->color_b.b);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (group->scale < 2048) {
            group->scale += state->step * 48;
            if (group->scale >= 2048) {
                group->scale -= 2048;
                if (state->phase == 4) {
                    group->growth++;
                    if (group->growth >= 2) {
                        state->phase = 5;
                    }
                }
            }
        }
    }
}
