#include "../../types.h"
#include "variant417_entry.h"

void SpanishModelVariant417_DrawPhasedLines(Variant417EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant417EntryState *state = context;
    ModelVariantSheet *companion = state->sheets;
    ModelVariantWebNarrow *group = state->webs;
    GsGLINE *line = &state->line;
    GsOT *ot;
    s16 i, j, k;
    s32 complete;
    s32 size, opacity, fade;
    u8 red, green, blue;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    complete = 1;
    ratan2(state->aim.view_delta.vz, state->aim.view_delta.vx);
    ratan2(state->aim.view_delta.vy, state->aim.view_delta.vx);
    ratan2(state->aim.direction.vz, state->aim.direction.vy);
    ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx);
    for (i = 0; i < 3; i++, group++) {
        if (state->phase == 0) {
            if (group->scale < 4096) {
                size = group->scale;
                opacity = size;
            } else {
                opacity = 4096;
                size = 0;
            }
            if (opacity > 2048) {
                fade = 4096 - opacity;
                red = group->color.r * fade / 2048;
                green = group->color.g * fade / 2048;
                blue = group->color.b * fade / 2048;
            } else {
                red = group->color.r;
                green = group->color.g;
                blue = group->color.b;
            }
        } else if (state->phase == 1) {
            size = 4096;
            red = 0;
            green = 0;
            blue = 0;
        } else {
            if (group->scale > 0) {
                size = group->scale;
                opacity = size;
            } else {
                opacity = 0;
                size = 0;
            }
            if (opacity > 6144) {
                fade = 8192 - opacity;
                red = group->color.r * fade / 2048;
                green = group->color.g * fade / 2048;
                blue = group->color.b * fade / 2048;
            } else {
                red = group->color.r;
                green = group->color.g;
                blue = group->color.b;
            }
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (state->phase < 2) {
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
        } else {
            matrix.t[0] = state->aim.target.vx;
            matrix.t[1] = state->aim.target.vy;
            matrix.t[2] = state->aim.target.vz;
        }
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
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 6; k++) {
                line->attribute = 0x50000000;
                depth = RotTransPers4(&group->near[j][k], &group->far[j][k],
                                     &group->near[j][k], &group->far[j][k],
                                     (long *)&line->x0, (long *)&line->x1,
                                     (long *)&line->x0, (long *)&line->x1,
                                     &interpolation, &flag);
                if (state->phase < 2) {
                    line->r0 = 0;
                    line->g0 = 0;
                    line->b0 = 0;
                    line->r1 = red;
                    line->g1 = green;
                    line->b1 = blue;
                } else {
                    line->r0 = red;
                    line->g0 = green;
                    line->b0 = blue;
                    line->r1 = 0;
                    line->g1 = 0;
                    line->b1 = 0;
                }
                if (depth >= 0 && flag >= 0) {
                    GsSortGLine(line, ot, (u16)depth);
                }
            }
        }
        if (state->phase == 0) {
            if (group->scale > 0) {
                group->scale -= state->step * 192;
                if (group->scale <= 0) {
                    if (companion->size > 2048) {
                        group->scale = 0;
                    } else {
                        group->scale += 4096;
                    }
                }
            }
            if (companion->size > 2048 && group->scale > 4096) {
                group->scale = 0;
            }
        } else if (state->phase == 1) {
            group->scale = -(i * 8192 / 3);
        } else {
            if (group->scale < 8192) {
                group->scale += state->step * 256;
                if (group->scale >= 8192) {
                    if (state->phase >= 5) {
                        group->scale = 8192;
                        if (i + 1 == 3 && complete == 1 && state->phase == 5) {
                            state->phase = 6;
                        }
                    } else {
                        group->scale -= 8192;
                        complete = 0;
                    }
                }
            }
        }
    }
}
