#include "../../types.h"
#include "../french_model_variant/variant439_entry.h"

void func_8013C230(u8 *work)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Variant439EntryState *state = (Variant439EntryState *)work;
    Variant439EntryScreenRing *ring = state->screen_rings;
    POLY_GT4 *quad = &state->extra;
    GsOT *ot;
    s32 j;
    s32 active;
    s32 i;
    s32 radius;
    s32 angle;
    s32 tpage;
    s32 depth;
    s32 ring_scale;

    ot = func_80058F10();
    active = GsGetActiveBuff();
    for (j = 0; j < 3; j++, ring++) {
        if (ring->scale > 0) {
            radius = ring->scale * 192 / 4096;
            ring_scale = ring->scale;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
            scale.vx = ring_scale;
            scale.vy = ring_scale;
            scale.vz = ring_scale;
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
            for (i = 0, angle = 0; i < 17; i++, angle = i << 8) {
                ring->a[i].vx = rcos(angle) * (radius + 128) >> 12;
                ring->a[i].vy = rsin(angle) * (radius + 128) >> 12;
                ring->a[i].vz = 0;
            }
            for (i = 0; i < 16; i++) {
                RotTransPers4(&ring->a[i], &ring->a[i + 1],
                              &ring->c[i], &ring->c[i + 1],
                              (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                              (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                              &interpolation, &flag);
                if (quad->x0 < 160) {
                    if (active == 0) {
                        tpage = GetTPage(2, 2, 320, 0);
                    } else {
                        tpage = GetTPage(2, 2, 0, 0);
                    }
                    SetPolyGT4(quad);
                    setUV4(quad, quad->x0, quad->y0, quad->x1, quad->y1,
                           quad->x2, quad->y2, quad->x3, quad->y3);
                    quad->tpage = tpage;
                } else {
                    if (active == 0) {
                        tpage = GetTPage(2, 2, 448, 0);
                    } else {
                        tpage = GetTPage(2, 2, 128, 0);
                    }
                    SetPolyGT4(quad);
                    setUV4(quad, quad->x0 - 128, quad->y0,
                           quad->x1 - 128, quad->y1,
                           quad->x2 - 128, quad->y2,
                           quad->x3 - 128, quad->y3);
                    quad->tpage = tpage;
                }
                depth = RotTransPers4(&ring->a[i], &ring->a[i + 1],
                                     &ring->b[i], &ring->b[i + 1],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag);
                setRGB0(quad, ring->inner.r, ring->inner.g, ring->inner.b);
                setRGB1(quad, ring->inner.r, ring->inner.g, ring->inner.b);
                setRGB2(quad, ring->outer.r, ring->outer.g, ring->outer.b);
                setRGB3(quad, ring->outer.r, ring->outer.g, ring->outer.b);
                if (depth > 0) {
                    GsSortPoly(quad, ot, depth);
                }
            }
        }
        if (ring->scale < 4096) {
            ring->scale += state->step * 96;
            if (ring->scale >= 4096) {
                if (state->phase >= 3) {
                    ring->scale = 4096;
                } else {
                    ring->scale -= 4096;
                    ring->count++;
                }
            }
        }
    }
}
