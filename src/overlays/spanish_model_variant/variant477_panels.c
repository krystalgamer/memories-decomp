#include "../../types.h"
#include "../french_model_variant/variant477_entry.h"
#include "../../game/gpu_packets.h"

/* Draws up to two textured panels, as many as the time has passed thresholds
 * at descriptor +0x1C and +0x20. Each panel moves along its direction with its
 * size, fades over its second half and grows by step * 48 until it is done;
 * the phase moves to 2 at half size and to 5 once both panels are done. */
void SpanishModelVariant477_DrawPanels(u8 *context)
{
    SVECTOR rotation;
    VECTOR unused;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    GsOT *ot;
    EntryState477 *state = (EntryState477 *)context;
    ModelVariantPanel *panel = state->panels;
    POLY_FT4 *polygon;
    s16 i, j;
    s16 count;
    s32 complete;
    u8 red, green, blue;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    ratan2(state->aim.direction.vy, state->aim.direction.vz);
    ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx);
    complete = 0;
    count = state->clock.frame >=
            state->config->draw_start;
    polygon = &state->panels_packet;
    if (state->clock.frame >= state->config->limit) {
        count = 2;
    }
    for (i = 0; i < count; i++, panel++) {
        for (j = 0; j < 1; j++) {
            if (panel->size[j] >= 0) {
                if (panel->size[j] > 512) {
                    red = panel->color.r * (1024 - panel->size[j]) / 512;
                    green = panel->color.g * (1024 - panel->size[j]) / 512;
                    blue = panel->color.b * (1024 - panel->size[j]) / 512;
                } else {
                    red = panel->color.r;
                    green = panel->color.g;
                    blue = panel->color.b;
                }
                rcos(panel->size[j]);
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = 0;
                matrix.t[0] = panel->matrices[j].t[0] + panel->directions[j].vx * panel->size[j] / 512;
                matrix.t[1] = panel->matrices[j].t[1] + panel->directions[j].vy * panel->size[j] / 512;
                matrix.t[2] = panel->matrices[j].t[2] + panel->directions[j].vz * panel->size[j] / 512;
                scale.vx = 768;
                scale.vy = 768;
                scale.vz = 768;
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
                depth = RotTransPers4(&panel->a[j], &panel->b[j], &panel->c[j], &panel->d[j],
                                      (long *)&polygon->x0, (long *)&polygon->x1,
                                      (long *)&polygon->x2, (long *)&polygon->x3,
                                      &interpolation, &flag);
                polygon->r0 = red;
                polygon->g0 = green;
                polygon->b0 = blue;
                if (depth >= 0 && flag >= 0 && panel->done[j] == 0) {
                    GsSortPoly(polygon, ot, depth);
                }
            }
            if (panel->size[j] < 1024) {
                panel->size[j] += state->clock.step * 48;
                if (panel->size[0] >= 512) {
                    if (state->phase == 0) {
                        state->phase = 2;
                    }
                    if (state->enabled[i] == 0) {
                        state->enabled[i] = 1;
                    }
                }
                if (panel->size[j] >= 1024) {
                    panel->size[j] = 1024;
                    panel->done[j] = 1;
                }
            }
            complete += panel->done[j];
            if (j + 1 == 1 && complete == 2 && state->phase == 2) {
                state->phase = 5;
            }
        }
    }
}
