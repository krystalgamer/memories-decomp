#include "../../types.h"
#include "variant477_panels.h"
#include "../../game/gpu_packets.h"

/* Draws up to two textured panels, as many as the time has passed thresholds
 * at descriptor +0x1C and +0x20. Each panel moves along its direction with its
 * size, fades over its second half and grows by step * 48 until it is done;
 * the phase moves to 2 at half size and to 5 once both panels are done. */
void func_8013C484(u8 *context)
{
    SVECTOR rotation;
    VECTOR unused;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    GsOT *ot;
    Quads477State *state = (Quads477State *)context;
    Panel477 *panel = (Panel477 *)context;
    POLY_FT4 *polygon;
    s16 i, j;
    s16 count;
    s32 complete;
    u8 red, green, blue;
    s32 depth;

    ot = func_80058F10();
    ratan2(state->direction.vy, state->direction.vz);
    ratan2(state->projected.vy, state->projected.vx);
    complete = 0;
    count = (u32)MODEL_VARIANT_WORD(context, 0x2068) >=
            *(u32 *)(MODEL_VARIANT_WORD(context, 0x2078) + 0x1C);
    polygon = (POLY_FT4 *)(context + 0x1FC4);
    if ((u32)MODEL_VARIANT_WORD(context, 0x2068) >= *(u32 *)(MODEL_VARIANT_WORD(context, 0x2078) + 0x20)) {
        count = 2;
    }
    for (i = 0; i < count; i++, panel++) {
        for (j = 0; j < 1; j++) {
            if (panel->size[j] >= 0) {
                if (panel->size[j] > 512) {
                    red = panel->color[0] * (1024 - panel->size[j]) / 512;
                    green = panel->color[1] * (1024 - panel->size[j]) / 512;
                    blue = panel->color[2] * (1024 - panel->size[j]) / 512;
                } else {
                    red = panel->color[0];
                    green = panel->color[1];
                    blue = panel->color[2];
                }
                rcos(panel->size[j]);
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = 0;
                matrix.t[0] = panel->matrix[j].t[0] + panel->direction[j].vx * panel->size[j] / 512;
                matrix.t[1] = panel->matrix[j].t[1] + panel->direction[j].vy * panel->size[j] / 512;
                matrix.t[2] = panel->matrix[j].t[2] + panel->direction[j].vz * panel->size[j] / 512;
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
                                      (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                      (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
                                      &interpolation, &flag);
                polygon->r0 = red;
                polygon->g0 = green;
                polygon->b0 = blue;
                if (depth >= 0 && flag >= 0 && panel->done[j] == 0) {
                    GsSortPoly(polygon, ot, depth);
                }
            }
            if (panel->size[j] < 1024) {
                panel->size[j] += state->step * 48;
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
