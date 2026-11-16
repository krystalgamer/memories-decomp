#include "../../types.h"
#include "variant479_entry.h"

void SpanishModelVariant479_DrawSheets(Variant479EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    GsOT *ot;
    Variant479EntryState *state = context;
    Variant479Rib *primary = state->ribs;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *polygon;
    SVECTOR *vertex;
    s32 i, j;
    s32 extra;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    polygon = &state->packets.textured[1];
    for (i = 0; i < 8; i++, sheet++) {
        extra = 0;
        if (state->clock.frame_count & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i < 5) {
            matrix.t[0] = state->matrices[i % 3].t[0] + primary->a[8].vx;
            matrix.t[1] = state->matrices[i % 3].t[1] + primary->a[8].vy;
            matrix.t[2] = state->matrices[i % 3].t[2] + primary->a[8].vz;
        } else {
            matrix.t[0] = state->matrices[i - 5].t[0];
            matrix.t[1] = state->matrices[i - 5].t[1];
            matrix.t[2] = state->matrices[i - 5].t[2];
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
        j = 0;
        vertex = sheet->v0;
        for (; j < 4; j++, vertex++) {
            depth = RotTransPers4(vertex, &sheet->v1[j], &sheet->v2[j], &sheet->v3[j],
                                 (long *)&polygon->x0, (long *)&polygon->x1,
                                 (long *)&polygon->x2, (long *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = sheet->inner[0];
            polygon->g0 = sheet->inner[1];
            polygon->b0 = sheet->inner[2];
            polygon->r1 = sheet->inner[0];
            polygon->g1 = sheet->inner[1];
            polygon->b1 = sheet->inner[2];
            polygon->r2 = sheet->inner[0];
            polygon->g2 = sheet->inner[1];
            polygon->b2 = sheet->inner[2];
            polygon->r3 = sheet->outer[0];
            polygon->g3 = sheet->outer[1];
            polygon->b3 = sheet->outer[2];
            depth = depth * 8 / 10;
            if ((i >= 5 || primary->state != 0) && depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (i < 5) {
            if (state->phase == 2) {
                sheet->size = 4096;
            } else if (state->phase == 3 && sheet->size < 8192) {
                sheet->size += state->clock.step * 512;
                if (sheet->size >= 8192) {
                    sheet->size = 8192;
                    state->phase = 4;
                }
            }
            if (state->clock.frame >= state->config->fade_start && sheet->size > 0) {
                sheet->size = 8192 - ((state->clock.frame - state->config->fade_start) << 13) /
                                    (state->config->fade_end - state->config->fade_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
            primary++;
        } else {
            if (state->phase == 0 && sheet->size < 4096) {
                sheet->size = ((state->clock.frame - state->config->start) << 12) /
                              (state->config->grow_end - state->config->start);
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->phase = 1;
                }
            }
            if (state->clock.frame >= state->config->fade_start && sheet->size > 0) {
                sheet->size = 4096 - ((state->clock.frame - state->config->fade_start) << 12) /
                                    (state->config->fade_end - state->config->fade_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        }
    }
}
