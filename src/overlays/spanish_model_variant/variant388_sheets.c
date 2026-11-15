#include "../../types.h"
#include "variant388_entry.h"

/* Draws two four-quad sheets: the first at the origin, the second advanced
 * along the direction by the progress. On the last index of each timing
 * cycle the first sheet grows over the timing window and shrinks over the
 * fade window, while the second follows the phase. */
void SpanishModelVariant388_DrawSheets(Variant388EntryState *context)
{
    SVECTOR rotation;
    /* The target reserves 16 unused bytes between rotation and scale. */
    u8 unknown_stack[16];
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant388EntryState *state = context;
    ModelVariantSheet *sheet = (ModelVariantSheet *)state->sheets;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j;
    s32 extra;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    polygon = &state->packets.textured[1];
    for (i = 0; i < 2; i++, sheet++) {
        extra = 0;
        if (state->clock.frame_count & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
        } else {
            matrix.t[0] = state->matrix.t[0] + state->aim.direction.vx * state->progress / 1024;
            matrix.t[1] = state->matrix.t[1] + state->aim.direction.vy * state->progress / 1024;
            matrix.t[2] = state->matrix.t[2] + state->aim.direction.vz * state->progress / 1024;
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
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (state->index + 1 == state->config->count) {
            if (i == 0) {
                if (state->phase == 0 && sheet->size < 4096) {
                    sheet->size = ((state->clock.frame - state->config->sheet_start) << 12)
                        / (state->config->grow_end - state->config->sheet_start);
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        state->phase = 1;
                    }
                }
                if (state->clock.frame >= state->config->track_end && sheet->size > 0) {
                    sheet->size = 4096 - ((state->clock.frame - state->config->track_end) << 12)
                        / (state->config->shrink_end - state->config->track_end);
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                    }
                }
            } else if (state->phase <= 0) {
                sheet->size = 0;
            } else if (state->phase == 1) {
                sheet->size = 4096;
            } else if (state->phase == 2) {
                if (sheet->size < 8192) {
                    sheet->size += state->clock.step << 9;
                    if (sheet->size >= 8192) {
                        sheet->size = 8192;
                        state->phase = 3;
                    }
                }
            } else if (state->phase == 4) {
                if (sheet->size > 0) {
                    sheet->size -= state->clock.step << 6;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        state->phase = 5;
                    }
                }
            }
        }
    }
}
