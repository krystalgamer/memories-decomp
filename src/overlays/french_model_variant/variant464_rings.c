#include "../../types.h"
#include "variant464_entry.h"
#include "../../game/gpu_packets.h"

void FrenchModelVariant464_DrawRings(Variant464EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant464EntryState *state = context;
    ModelVariantScreenRing *ring = state->screen_rings;
    POLY_GT4 *quad = &state->ring_poly;
    GsOT *ot;
    s32 i, j;
    s32 total;
    s32 angle_y, angle_x;
    s32 buffer;
    s32 lift, radius;
    s32 angle;
    s32 size;
    s32 depth;
    u32 tpage;
    u8 full;

    ot = Graphics_GetDefaultOrderingTable();
    total = 0;
    angle_y = ratan2(state->aim.direction.vz, state->aim.direction.vx) + 2048;
    angle_x = ratan2(state->aim.direction.vy, state->aim.direction.vz) + 2048;
    buffer = GsGetActiveBuff();
    if (angle_y < 2048) {
        angle_y = -angle_y + 1024;
    } else {
        angle_y += 1024;
    }
    for (i = 0; i < 3; i++, ring++) {
        if (ring->scale > 2048) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                lift = (ring->scale - 2048) / 16;
                radius = lift + 256;
                ring->a[j].vx = rcos(angle) * radius >> 12;
                ring->a[j].vy = rsin(angle) * radius >> 12;
                ring->a[j].vz = -lift;
            }
            size = ring->scale;
        } else {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                ring->a[j].vx = rcos(angle) * 256 >> 12;
                ring->a[j].vy = rsin(angle) * 256 >> 12;
                ring->a[j].vz = 0;
            }
            size = ring->scale;
        }
        if (size > 0) {
            rotation.vx = -angle_x;
            rotation.vy = angle_y;
            rotation.vz = 0;
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            for (j = 0, full = 255; j < 16; j++) {
                RotTransPers4(&ring->a[j], &ring->a[j+1],
                              &ring->c[j], &ring->c[j+1],
                              (long *)&quad->x0, (long *)&quad->x1,
                              (long *)&quad->x2, (long *)&quad->x3,
                              &interpolation, &flag);
                if (quad->x0 < 160) {
                    if (buffer == 0) {
                        tpage = GetTPage(2, 1, 320, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0, 0);
                    }
                    SetPolyGT4(quad);
                    quad->tpage = tpage;
                    setUV4(quad, quad->x0, quad->y0, quad->x1, quad->y1,
                           quad->x2, quad->y2, quad->x3, quad->y3);
                } else {
                    if (buffer == 0) {
                        tpage = GetTPage(2, 1, 448, 0);
                    } else {
                        tpage = GetTPage(2, 1, 128, 0);
                    }
                    SetPolyGT4(quad);
                    quad->tpage = tpage;
                    setUV4(quad, quad->x0-128, quad->y0, quad->x1-128, quad->y1,
                           quad->x2-128, quad->y2, quad->x3-128, quad->y3);
                }
                depth = RotTransPers4(&ring->a[j], &ring->a[j+1],
                                     &ring->b[j], &ring->b[j+1],
                                     (long *)&quad->x0, (long *)&quad->x1,
                                     (long *)&quad->x2, (long *)&quad->x3,
                                     &interpolation, &flag);
                SetSemiTrans(quad, 0);
                SetShadeTex(quad, 0);
                setRGB0(quad, full, full, full);
                setRGB1(quad, full, full, full);
                setRGB2(quad, 0, full, full);
                setRGB3(quad, 0, full, full);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (ring->scale < 4096) {
            ring->scale += state->clock.step * 96;
            if (ring->scale >= 4096) {
                if (state->tail.phase < 3) {
                    ring->scale -= 4096;
                    ring->count = 1;
                } else {
                    ring->scale = 4096;
                    ring->count = 2;
                }
            }
        }
        total += ring->count;
        if (total == 6 && state->tail.phase == 3) {
            state->tail.phase = 5;
        }
    }
}
