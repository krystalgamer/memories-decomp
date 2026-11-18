#include "../../types.h"
#include "h424_entry_pal.h"

void SpanishModelVariant441_DrawFramebufferRings(Variant441EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant441EntryState *state = context;
    ModelVariantTripleCurtain *group = state->curtains;
    POLY_GT4 *quad = &state->plain;
    GsOT *ot;
    s32 i, j;
    s32 complete;
    s32 buffer;
    s32 angle_y, angle_x;
    s32 size;
    s32 depth;
    u32 tpage;
    u8 red, green, blue;

    ot = Graphics_GetDefaultOrderingTable();
    complete = 1;
    buffer = GsGetActiveBuff();
    angle_y = ratan2(state->aim.direction.vz, state->aim.direction.vx) + 2048;
    angle_x = ratan2(state->aim.direction.vy, state->aim.direction.vz) + 2048;
    if (angle_y < 2048) {
        angle_y = -angle_y + 1024;
    } else {
        angle_y += 1024;
    }
    for (i = 0; i < 5; i++, group++) {
        if (group->scale > 0) {
            size = rsin(group->scale) * 4096 >> 12;
            rotation.vx = -angle_x;
            rotation.vy = angle_y;
            rotation.vz = 0;
            matrix.t[0] = state->aim.target.vx +
                          state->aim.direction.vx * (group->scale-1024) / 2048;
            matrix.t[1] = state->aim.target.vy +
                          state->aim.direction.vy * (group->scale-1024) / 2048;
            matrix.t[2] = state->aim.target.vz +
                          state->aim.direction.vz * (group->scale-1024) / 2048;
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
                RotTransPers4(&group->a[j], &group->a[j+1],
                              &group->c[j], &group->c[j+1],
                              (long *)&quad->x0, (long *)&quad->x1,
                              (long *)&quad->x2, (long *)&quad->x3,
                              &interpolation, &flag);
                if (quad->x0 < 160) {
                    if (buffer == 0) {
                        tpage = GetTPage(2, 1, 320, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0, 0);
                    }
                    SetPolyGT4(quad);
                    quad->tpage = tpage;
                    setUV4(quad, quad->x0, quad->y0, quad->x1, quad->y1,
                           quad->x2, quad->y2, quad->x3, quad->y3);
                } else {
                    if (buffer == 0) {
                        tpage = GetTPage(2, 1, 448, 0);
                    } else {
                        tpage = GetTPage(2, 1, 128, 0);
                    }
                    SetPolyGT4(quad);
                    quad->tpage = tpage;
                    setUV4(quad, quad->x0-128, quad->y0, quad->x1-128, quad->y1,
                           quad->x2-128, quad->y2, quad->x3-128, quad->y3);
                }
                depth = RotTransPers4(&group->a[j], &group->a[j+1],
                                     &group->b[j], &group->b[j+1],
                                     (long *)&quad->x0, (long *)&quad->x1,
                                     (long *)&quad->x2, (long *)&quad->x3,
                                     &interpolation, &flag);
                SetSemiTrans(quad, 0);
                SetShadeTex(quad, 0);
                if (state->frame_count % 8 == 0) {
                    red = 255;
                    green = 0;
                    blue = 0;
                } else if (state->frame_count % 8 == 1) {
                    red = 255;
                    green = 128;
                    blue = 0;
                } else if (state->frame_count % 8 == 2) {
                    red = 255;
                    green = 255;
                    blue = 0;
                } else if (state->frame_count % 8 == 3) {
                    red = 0;
                    green = 255;
                    blue = 0;
                } else if (state->frame_count % 8 == 4) {
                    red = 0;
                    green = 255;
                    blue = 255;
                } else if (state->frame_count % 8 == 5) {
                    red = 0;
                    green = 0;
                    blue = 255;
                } else if (state->frame_count % 8 == 6) {
                    red = 255;
                    green = 0;
                    blue = 255;
                } else {
                    red = 255;
                    green = 0;
                    blue = 128;
                }
                setRGB0(quad, 255, 255, 255);
                setRGB1(quad, 255, 255, 255);
                setRGB2(quad, red, green, blue);
                setRGB3(quad, red, green, blue);
                if (depth > 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (group->scale < 2048) {
            complete = 0;
            group->scale += state->step * 32;
            if (group->scale >= 2048) {
                if (state->phase >= 5) {
                    group->scale = 2048;
                } else {
                    group->scale -= 2048;
                }
            }
        }
        if (i+1 == 5 && complete == 1 && state->phase == 5) {
            state->phase = 6;
        }
    }
}
