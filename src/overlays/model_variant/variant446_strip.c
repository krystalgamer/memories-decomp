#include "../../types.h"
#include "../model_variant/variant446_entry_pal.h"

void SpanishModelVariant446_DrawStrip(EntryState446 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    GsOT *ot;
    EntryState446 *state = context;
    EntryStrip446 *strip;
    POLY_GT4 *polygon;
    s16 i, j;
    s16 radius;
    s32 angle;

    ot = Graphics_GetDefaultOrderingTable();
    angle = ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx);
    polygon = &state->textured[0];
    angle += 2048;
    if (state->phase > 0) {
        if ((state->frame_count & 1) == 0) {
            radius = state->config->width * state->radius / 1024;
        } else {
            radius = state->config->width * state->radius / 1024 + state->radius / 128;
        }
        strip = state->strips;
        for (i = 0; i < 1; i++, strip++) {
            for (j = 0; j < 5; j++) {
                setVector(&strip->points[0][j], (rcos(1024) * radius) >> 12,
                          (rsin(1024) * radius) >> 12, 0);
                setVector(&strip->points[1][j], 0, 0, 0);
                setVector(&strip->points[2][j], (rcos(3072) * radius) >> 12,
                          (rsin(3072) * radius) >> 12, 0);
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = angle;
                matrix.t[0] = state->matrix.t[0] + state->aim.direction.vx * (state->progress * j / 4) / 1024;
                matrix.t[1] = state->matrix.t[1] + state->aim.direction.vy * (state->progress * j / 4) / 1024;
                matrix.t[2] = state->matrix.t[2] + state->aim.direction.vz * (state->progress * j / 4) / 1024;
                scale.vx = 4096;
                scale.vy = 4096;
                scale.vz = 4096;
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
                strip->depth[j] = RotTransPers3(&strip->points[0][j], &strip->points[1][j],
                                               &strip->points[2][j],
                                               &strip->projected[0][j],
                                               &strip->projected[1][j],
                                               &strip->projected[2][j],
                                               &interpolation, &flag);
            }
        }
        strip = state->strips;
        for (i = 0; i < 1; i++, strip++) {
            for (j = 0; j < 4; j++) {
                polygon->x0 = strip->projected[0][j];
                polygon->y0 = strip->projected[0][j] >> 16;
                polygon->x1 = strip->projected[0][j+1];
                polygon->y1 = strip->projected[0][j+1] >> 16;
                polygon->x2 = strip->projected[1][j];
                polygon->y2 = strip->projected[1][j] >> 16;
                polygon->x3 = strip->projected[1][j+1];
                polygon->y3 = strip->projected[1][j+1] >> 16;
                polygon->r0 = 0;
                polygon->g0 = 32;
                polygon->b0 = 192;
                polygon->r1 = 0;
                polygon->g1 = 32;
                polygon->b1 = 192;
                polygon->r2 = 192;
                polygon->g2 = 192;
                polygon->b2 = 192;
                polygon->r3 = 192;
                polygon->g3 = 192;
                polygon->b3 = 192;
                if (strip->depth[j] > 0) {
                    GsSortPoly(polygon, ot, (u16)strip->depth[j+1]);
                }
                polygon->x0 = strip->projected[2][j];
                polygon->y0 = strip->projected[2][j] >> 16;
                polygon->x1 = strip->projected[2][j+1];
                polygon->y1 = strip->projected[2][j+1] >> 16;
                polygon->x2 = strip->projected[1][j];
                polygon->y2 = strip->projected[1][j] >> 16;
                polygon->x3 = strip->projected[1][j+1];
                polygon->y3 = strip->projected[1][j+1] >> 16;
                polygon->r0 = 0;
                polygon->g0 = 32;
                polygon->b0 = 192;
                polygon->r1 = 0;
                polygon->g1 = 32;
                polygon->b1 = 192;
                polygon->r2 = 192;
                polygon->g2 = 192;
                polygon->b2 = 192;
                polygon->r3 = 192;
                polygon->g3 = 192;
                polygon->b3 = 192;
                if (strip->depth[j] > 0) {
                    GsSortPoly(polygon, ot, (u16)strip->depth[j+1]);
                }
            }
        }
        if (state->part_index + 1 == state->config->count) {
            if (state->phase == 1) {
                if (state->progress <= 1024) {
                    state->progress = ((state->elapsed-state->config->duration)*1024) /
                                      (state->config->expanded-state->config->duration);
                    if (state->progress >= 1024) {
                        state->progress = 1024;
                        state->phase = 2;
                    }
                }
            } else if (state->config->fade <= state->elapsed) {
                if (state->radius > 0) {
                    state->radius = 1024 - ((state->elapsed-state->config->fade)*1024) /
                                          (state->config->end-state->config->fade);
                    if (state->radius <= 0) {
                        state->radius = 0;
                    }
                }
            }
        }
    }
}
