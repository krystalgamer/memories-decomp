#include "../../types.h"
#include "h410_entry_pal.h"

void SpanishModelVariant427_DrawPanels(EntryState427 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    EntryState427 *state = context;
    EntryRibbon427 *primary = state->ribbons;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j;
    s32 extra;
    s32 x, y, z;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    polygon = &state->packets.textured[1];
    /* Only the first three panels have a primary record; the cursor steps past them. */
    for (i = 0; i < 6; i++, sheet++, primary++) {
        extra = 0;
        if (state->clock.frame_count & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i < 3) {
            x = state->ribbon_directions[i].vx * primary->progress / 1024;
            y = state->ribbon_directions[i].vy * primary->progress / 1024;
            z = state->ribbon_directions[i].vz * primary->progress / 1024;
            matrix.t[0] = state->matrices[i].t[0] + x;
            matrix.t[1] = state->matrices[i].t[1] + y;
            matrix.t[2] = state->matrices[i].t[2] + z;
        } else {
            matrix.t[0] = state->matrices[i - 3].t[0];
            matrix.t[1] = state->matrices[i - 3].t[1];
            matrix.t[2] = state->matrices[i - 3].t[2];
        }
        scale.vx = sheet->size + extra;
        scale.vy = sheet->size + extra;
        scale.vz = sheet->size + extra;
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
        for (j = 0; j < 4; j++) {
            depth = RotTransPers4(&sheet->v0[j], &sheet->v1[j], &sheet->v2[j], &sheet->v3[j],
                                 (long *)&polygon->x0, (long *)&polygon->x1,
                                 (long *)&polygon->x2, (long *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = sheet->inner.r;
            polygon->g0 = sheet->inner.g;
            polygon->b0 = sheet->inner.b;
            polygon->r1 = sheet->inner.r;
            polygon->g1 = sheet->inner.g;
            polygon->b1 = sheet->inner.b;
            polygon->r2 = sheet->inner.r;
            polygon->g2 = sheet->inner.g;
            polygon->b2 = sheet->inner.b;
            polygon->r3 = sheet->outer.r;
            polygon->g3 = sheet->outer.g;
            polygon->b3 = sheet->outer.b;
            depth = depth * 8 / 10;
            if (i >= 3 || primary->progress >= 0) {
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(polygon, ot, depth);
                }
            }
        }
        if (i < 3) {
            if (state->phase == 2) {
                sheet->size = 4096;
            } else if (state->phase == 3) {
                if (sheet->size < 8192) {
                    sheet->size += state->clock.step * 512;
                    if (sheet->size >= 8192) {
                        sheet->size = 8192;
                        state->phase = 4;
                    }
                }
            }
            if (state->clock.frame >= state->config->fade_start && sheet->size > 0) {
                sheet->size = 8192 - (state->clock.frame - state->config->fade_start) * 8192
                    / (state->config->fade_end - state->config->fade_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else {
            if (state->phase == 0 && sheet->size < 4096) {
                sheet->size = (state->clock.frame - state->config->draw_start) * 4096
                    / (state->config->grow_end - state->config->draw_start);
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->phase = 1;
                }
            }
            if (state->clock.frame >= state->config->fade_start && sheet->size > 0) {
                sheet->size = 4096 - (state->clock.frame - state->config->fade_start) * 4096
                    / (state->config->fade_end - state->config->fade_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        }
    }
}
