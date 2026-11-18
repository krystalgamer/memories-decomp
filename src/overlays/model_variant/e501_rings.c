#include "../../types.h"
#include "e501_entry_pal.h"

void FrenchModelVariant335_DrawRings(Model335EntryState *work)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    ModelVariantTwoToneScreenRing *ring = work->rings;
    POLY_GT4 *quad = &work->textured[2];
    GsOT *ot;
    s32 j;
    s32 done;
    s32 active;
    s32 i;
    s32 radius;
    s32 angle;
    s32 tpage;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    done = 1;
    active = GsGetActiveBuff();
    for (j = 0; j < 3; j++, ring++) {
        if (ring->count > 0) {
            radius = ring->scale / 8;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = work->aim.target.vx;
            matrix.t[1] = work->aim.target.vy;
            matrix.t[2] = work->aim.target.vz;
            scale.vx = ring->scale;
            scale.vy = ring->scale;
            scale.vz = ring->scale;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            for (i = 0, angle = 0; i < 17; i++, angle = i << 8) {
                ring->a[i].vx = rcos(angle) * (radius + 512) >> 12;
                ring->a[i].vy = 0;
                ring->a[i].vz = rsin(angle) * (radius + 512) >> 12;
            }
            for (i = 0; i < 16; i++) {
                RotTransPers4(&ring->a[i], &ring->a[i + 1],
                              &ring->c[i], &ring->c[i + 1],
                              (long *)&quad->x0, (long *)&quad->x1,
                              (long *)&quad->x2, (long *)&quad->x3,
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
                                     (long *)&quad->x0, (long *)&quad->x1,
                                     (long *)&quad->x2, (long *)&quad->x3,
                                     &interpolation, &flag);
                setRGB0(quad, ring->inner.r, ring->inner.g, ring->inner.b);
                setRGB1(quad, ring->inner.r, ring->inner.g, ring->inner.b);
                setRGB2(quad, ring->outer.r, ring->outer.g, ring->outer.b);
                setRGB3(quad, ring->outer.r, ring->outer.g, ring->outer.b);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, depth);
                }
            }
        }
        if (ring->scale <= 4096) {
            ring->scale += work->clock.step * 64;
            if (ring->scale >= 4096) {
                if (work->phase < 7) {
                    ring->scale -= 4096;
                    ring->count++;
                } else {
                    ring->scale = 4096;
                    done = 1;
                }
            }
        }
        if (j + 1 == 3 && done == 1
            && work->phase == 7) {
            work->phase = 8;
        }
    }
}
