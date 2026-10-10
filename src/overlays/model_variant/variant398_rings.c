#include "../../types.h"

#include "variant398_entry.h"

/* Draws three screen rings of sixteen POLY_GT4 strips textured from the
 * frame-buffer half each strip lands on, facing the work direction. Past
 * half scale the inner row is pushed back and widened; each scale grows by
 * step * 96 and wraps with a count until phase 3, then stays at full size. */
void func_8013C2B8(u8 *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    Variant398EntryState *work = (Variant398EntryState *)context;
    Variant398EntryScreenRing *ring;
    POLY_GT4 *poly;
    GsOT *ot;
    s32 i, j;
    s32 angle;
    s32 r;
    SVECTOR *point;
    s32 size;
    s32 depth;
    s32 yaw, heading, pitch;
    s32 buff;
    u32 tpage;
    u8 red, green, blue;

    ot = func_80058F10();
    ring = work->screen_rings;
    yaw = ratan2(work->direction.vz, work->direction.vx);
    heading = yaw + 0x800;
    pitch = ratan2(work->direction.vy, work->direction.vz) + 0x800;
    buff = GsGetActiveBuff();
    poly = &work->extra;
    if (heading < 0x800) {
        heading = -heading + 0x400;
    } else {
        heading = yaw + 0xC00;
    }
    for (i = 0; i < 3; i++, ring++) {
        if (ring->scale > 0x800) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                r = (ring->scale - 0x800) * 192 / 2048;
                setVector(&ring->a[j], rcos(angle) * (r + 256) >> 12,
                          rsin(angle) * (r + 256) >> 12, -r);
            }
        } else {
            /* Codegen steering: retail keeps this flat row out of the loop
             * optimizer. A structured loop weights angle more heavily, so
             * global allocation swaps it with the first row's pointer. */
            j = 0;
            angle = 0;
            point = ring->a;
        flat_row:
            setVector(point, rcos(angle) * 256 >> 12, rsin(angle) * 256 >> 12, 0);
            point++;
            j++;
            angle = j << 8;
            if (j < 17) {
                goto flat_row;
            }
        }
        if (ring->scale > 0) {
            size = ring->scale;
            rot.vx = -pitch;
            rot.vy = heading;
            rot.vz = 0;
            m.t[0] = work->matrix.t[0];
            m.t[1] = work->matrix.t[1];
            m.t[2] = work->matrix.t[2];
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            for (j = 0; j < 16; j++) {
                RotTransPers4(&ring->a[j], &ring->a[j + 1], &ring->c[j], &ring->c[j + 1],
                              (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                              (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                if (poly->x0 < 160) {
                    if (buff == 0) {
                        tpage = GetTPage(2, 1, 320, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0, 0);
                    }
                    SetPolyGT4(poly);
                    poly->tpage = tpage;
                    setUV4(poly, poly->x0, poly->y0, poly->x1, poly->y1,
                           poly->x2, poly->y2, poly->x3, poly->y3);
                } else {
                    if (buff == 0) {
                        tpage = GetTPage(2, 1, 448, 0);
                    } else {
                        tpage = GetTPage(2, 1, 128, 0);
                    }
                    SetPolyGT4(poly);
                    poly->tpage = tpage;
                    setUV4(poly, poly->x0 - 128, poly->y0, poly->x1 - 128, poly->y1,
                           poly->x2 - 128, poly->y2, poly->x3 - 128, poly->y3);
                }
                depth = RotTransPers4(&ring->a[j], &ring->a[j + 1], &ring->b[j], &ring->b[j + 1],
                                      (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                      (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                SetSemiTrans(poly, 0);
                SetShadeTex(poly, 0);
                if (work->frame_count % 8 == 0) {
                    red = 255;
                    green = 0;
                    blue = 0;
                } else if (work->frame_count % 8 == 1) {
                    red = 255;
                    green = 128;
                    blue = 0;
                } else if (work->frame_count % 8 == 2) {
                    red = 255;
                    green = 255;
                    blue = 0;
                } else if (work->frame_count % 8 == 3) {
                    red = 0;
                    green = 255;
                    blue = 0;
                } else if (work->frame_count % 8 == 4) {
                    red = 0;
                    green = 255;
                    blue = 255;
                } else if (work->frame_count % 8 == 5) {
                    red = 0;
                    green = 0;
                    blue = 255;
                } else if (work->frame_count % 8 == 6) {
                    red = 255;
                    green = 0;
                    blue = 255;
                } else {
                    red = 255;
                    green = 0;
                    blue = 128;
                }
                setRGB0(poly, 255, 255, 255);
                setRGB1(poly, 255, 255, 255);
                setRGB2(poly, red, green, blue);
                setRGB3(poly, red, green, blue);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, (u16)depth);
                }
            }
        }
        if (ring->scale < 0x1000) {
            ring->scale += work->step * 96;
            if (ring->scale >= 0x1000) {
                if (work->phase < 3) {
                    ring->scale -= 0x1000;
                    ring->count++;
                } else {
                    ring->scale = 0x1000;
                }
            }
        }
    }
}
