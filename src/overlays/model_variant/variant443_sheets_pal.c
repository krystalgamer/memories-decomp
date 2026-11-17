#include "../../types.h"

#include "../model_variant/variant443_entry_pal.h"

/* Five sheets of four POLY_GT4 quads: three at their positions in their own inner
 * colour, and two at the origin and along the direction (or at the rest position
 * once the travel reaches 0x400) in a colour cycling through eight hues. The
 * sheets grow and fade over the timing record's windows as the phase advances. */
void SpanishModelVariant443_DrawSheets(EntryState443 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    EntryState443 *state = context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, k;
    s32 depth;
    u8 r, g, b;

    ot = Graphics_GetDefaultOrderingTable();
    quad = &state->packets.textured[1];
    for (i = 0; i < 5; i++, sheet++) {
        depth = 0;
        if (state->clock.frame_count & 1) {
            depth = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i < 3) {
            matrix.t[0] = state->positions[i].vx;
            matrix.t[1] = state->positions[i].vy;
            matrix.t[2] = state->positions[i].vz;
        } else if (i == 3) {
            matrix.t[0] = state->origin.vx;
            matrix.t[1] = state->origin.vy;
            matrix.t[2] = state->origin.vz;
        } else if (state->travel < 1024) {
            matrix.t[0] = state->origin.vx + state->direction.vx * state->progress / 1024;
            matrix.t[1] = state->origin.vy + state->direction.vy * state->progress / 1024;
            matrix.t[2] = state->origin.vz + state->direction.vz * state->progress / 1024;
        } else {
            matrix.t[0] = state->target.vx;
            matrix.t[1] = state->target.vy;
            matrix.t[2] = state->target.vz;
        }
        scale.vx = sheet->size + depth;
        scale.vy = sheet->size + depth;
        scale.vz = sheet->size + depth;
        RotMatrix(&rotation, &matrix);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ReadRotMatrix(&light);
        RotMatrix(&rotation, &light);
        ScaleMatrix(&light, &scale);
        SetRotMatrix(&light);
        for (k = 0; k < 4; k++) {
            depth = RotTransPers4(&sheet->v0[k], &sheet->v1[k], &sheet->v2[k], &sheet->v3[k],
                                 (long *)&quad->x0, (long *)&quad->x1,
                                 (long *)&quad->x2, (long *)&quad->x3,
                                 &interpolation, &flag);
            if (i < 3) {
                quad->r0 = sheet->inner.r;
                quad->g0 = sheet->inner.g;
                quad->b0 = sheet->inner.b;
                quad->r1 = sheet->inner.r;
                quad->g1 = sheet->inner.g;
                quad->b1 = sheet->inner.b;
                quad->r2 = sheet->inner.r;
                quad->g2 = sheet->inner.g;
                quad->b2 = sheet->inner.b;
                quad->r3 = sheet->outer.r;
                quad->g3 = sheet->outer.g;
                quad->b3 = sheet->outer.b;
            } else {
                if (state->clock.frame_count % 8 == 0) {
                    r = 255;
                    g = 0;
                    b = 0;
                } else if (state->clock.frame_count % 8 == 1) {
                    r = 255;
                    g = 128;
                    b = 0;
                } else if (state->clock.frame_count % 8 == 2) {
                    r = 255;
                    g = 255;
                    b = 0;
                } else if (state->clock.frame_count % 8 == 3) {
                    r = 0;
                    g = 255;
                    b = 0;
                } else if (state->clock.frame_count % 8 == 4) {
                    r = 0;
                    g = 255;
                    b = 255;
                } else if (state->clock.frame_count % 8 == 5) {
                    r = 0;
                    g = 0;
                    b = 255;
                } else if (state->clock.frame_count % 8 == 6) {
                    r = 255;
                    g = 0;
                    b = 255;
                } else {
                    r = 255;
                    g = 0;
                    b = 128;
                }
                quad->r0 = r;
                quad->g0 = g;
                quad->b0 = b;
                quad->r1 = r;
                quad->g1 = g;
                quad->b1 = b;
                quad->r2 = r;
                quad->g2 = g;
                quad->b2 = b;
                quad->r3 = sheet->outer.r;
                quad->g3 = sheet->outer.g;
                quad->b3 = sheet->outer.b;
            }
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, (u16)depth);
            }
        }
        if (i < 3) {
            if (state->phase == 0) {
                if (sheet->size < 4096) {
                    sheet->size = (state->clock.frame - state->config->start) * 4096 /
                                  (state->config->web_end - state->config->start);
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        state->phase = 1;
                    }
                }
            } else if (state->config->fade_start < state->clock.frame && sheet->size > 0) {
                sheet->size = 4096 - (state->clock.frame - state->config->fade_start) * 4096 /
                                     (state->config->fade_end - state->config->fade_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                    if (state->phase == 5) {
                        state->phase = 6;
                    }
                }
            }
        } else if (i == 3) {
            if (state->phase == i) {
                if (sheet->size < 6144) {
                    sheet->size = (state->clock.frame - state->config->spread_end) * 6144 /
                                  (state->config->mesh_start - state->config->spread_end);
                    if (sheet->size >= 6144) {
                        sheet->size = 6144;
                        state->phase = 4;
                    }
                }
            } else if (state->config->fade_start < state->clock.frame && sheet->size > 0) {
                sheet->size = 6144 - (state->clock.frame - state->config->fade_start) * 6144 /
                                     (state->config->fade_end - state->config->fade_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else if (state->phase < 4) {
            sheet->size = 0;
        } else if (state->phase == 4) {
            sheet->size = 6144;
        } else if (state->config->fade_start < state->clock.frame && sheet->size > 0) {
            sheet->size = 8192 - (state->clock.frame - state->config->fade_start) * 8192 /
                                 (state->config->fade_end - state->config->fade_start);
            if (sheet->size <= 0) {
                sheet->size = 0;
            }
        }
    }
}
