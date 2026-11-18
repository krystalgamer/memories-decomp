#include "../../types.h"

#if defined(VARIANT438_WARP_LAYOUT_459)
#include "h442_entry_pal.h"
#define WARP_STATE Variant459EntryState
#define WARP_ORIGIN(state) ((state)->matrices[0].t)
#define WARP_COLOR(ring, c) ((c) * (2048 - (ring)->scale) / 2048)
#define WARP_SORTS(ring) 1
#else
#include "h421_entry_pal.h"
#define WARP_STATE Variant438EntryState
#define WARP_ORIGIN(state) ((state)->matrix.t)
#define WARP_COLOR(ring, c) (c)
#define WARP_SORTS(ring) ((ring)->scale < 2048)
#endif

void SpanishModelVariant438_DrawWarpRings(WARP_STATE *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    WARP_STATE *state = context;
    s32 depth = 0;
    ModelVariantTripleCurtain *ring = state->curtains;
    POLY_GT4 *poly = &state->plain;
    GsOT *ot;
    s32 i, j;
    s32 buffer;
    s32 yaw, pitch;
    u32 tpage;
    u8 r0, g0, b0, r1, g1, b1;

    ot = Graphics_GetDefaultOrderingTable();
    buffer = GsGetActiveBuff();
    yaw = ratan2(state->aim.direction.vz, state->aim.direction.vx) + 2048;
    pitch = ratan2(state->aim.direction.vy, state->aim.direction.vz) + 2048;
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw += 1024;
    }
    for (i = 0; i < 5; i++, ring++) {
        if (ring->scale > 0) {
            rsin(ring->scale);
            rotation.vz = 0;
            rotation.vx = -pitch;
            rotation.vy = yaw;
            matrix.t[0] = WARP_ORIGIN(state)[0] + state->aim.direction.vx * ring->scale / 2048;
            matrix.t[1] = WARP_ORIGIN(state)[1] + state->aim.direction.vy * ring->scale / 2048;
            matrix.t[2] = WARP_ORIGIN(state)[2] + state->aim.direction.vz * ring->scale / 2048;
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = 4096;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            for (j = 0; j < 16; j++) {
                r0 = WARP_COLOR(ring, ring->color_a.r);
                g0 = WARP_COLOR(ring, ring->color_a.g);
                b0 = WARP_COLOR(ring, ring->color_a.b);
                r1 = WARP_COLOR(ring, ring->color_b.r);
                g1 = WARP_COLOR(ring, ring->color_b.g);
                b1 = WARP_COLOR(ring, ring->color_b.b);
                RotTransPers4(&ring->a[j], &ring->a[j + 1], &ring->c[j], &ring->c[j + 1],
                              (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
                              (long *)&poly->x3, &interpolation, &flag);
                if (poly->x0 < 160) {
                    if (buffer == 0) {
                        tpage = GetTPage(2, 1, 320, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0, 0);
                    }
                    SetPolyGT4(poly);
                    poly->u0 = poly->x0;
                    poly->v0 = poly->y0;
                    poly->u1 = poly->x1;
                    poly->v1 = poly->y1;
                    poly->u2 = poly->x2;
                    poly->v2 = poly->y2;
                    poly->u3 = poly->x3;
                    poly->v3 = poly->y3;
                    poly->tpage = tpage;
                } else {
                    if (buffer == 0) {
                        tpage = GetTPage(2, 1, 448, 0);
                    } else {
                        tpage = GetTPage(2, 1, 128, 0);
                    }
                    SetPolyGT4(poly);
                    poly->u0 = poly->x0 - 128;
                    poly->v0 = poly->y0;
                    poly->u1 = poly->x1 - 128;
                    poly->v1 = poly->y1;
                    poly->u2 = poly->x2 - 128;
                    poly->v2 = poly->y2;
                    poly->u3 = poly->x3 - 128;
                    poly->v3 = poly->y3;
                    poly->tpage = tpage;
                }
                depth = RotTransPers4(&ring->a[j], &ring->a[j + 1], &ring->b[j], &ring->b[j + 1],
                                      (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
                                      (long *)&poly->x3, &interpolation, &flag);
                SetSemiTrans(poly, 1);
                SetShadeTex(poly, 0);
                setRGB0(poly, r0, g0, b0);
                setRGB1(poly, r0, g0, b0);
                setRGB2(poly, r1, g1, b1);
                setRGB3(poly, r1, g1, b1);
                if (WARP_SORTS(ring) && depth >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, depth);
                }
            }
        }
        if (ring->scale < 2048) {
            ring->scale += state->clock.step << 6;
            if (ring->scale >= 2048) {
                if (state->phase < 3) {
                    ring->scale -= 2048;
                } else {
                    ring->scale = 2048;
                }
            }
        }
    }
}
