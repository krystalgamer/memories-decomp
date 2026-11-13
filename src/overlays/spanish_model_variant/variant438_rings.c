#include "../../types.h"

#include "../model_variant/model_variant.h"
#include "variant438_entry.h"

#define RING_COUNT 1
#define LAST_RING (RING_COUNT - 1)

/* The MODEL446 rings with a single ring: one seventeen-point POLY_GT4 ring
 * around the variant's direction, timed by the record, with radii scaled from
 * the timing record. Quads are sorted when depth and flag are non-negative. */
void SpanishModelVariant438_DrawRing(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant438EntryState *work;
    ModelVariantSheet *pulse;
    POLY_GT4 *poly;
    ModelVariantCurtain *ring;
    GsOT *ot;
    s32 i;
    s32 j;
    s32 k;
    s32 a;
    s32 size;
    s32 depth;
    s32 amp;
    s32 first;
    s32 yaw;
    s32 heading;
    s32 pitch;
    s16 fade;
    u8 ir, ig, ib;
    u8 or, og, ob;

    work = (Variant438EntryState *)ctx;
    ring = work->rings;
    ot = Graphics_GetDefaultOrderingTable();
    first = 1;
    pulse = work->sheets;
    poly = &work->ray;
    yaw = ratan2(work->aim.direction.vz, work->aim.direction.vx);
    heading = yaw + 0x800;
    pitch = ratan2(work->aim.direction.vy, work->aim.direction.vz) + 0x800;
    if (heading < 0x800) {
        heading = -heading + 0x400;
    } else {
        heading = yaw + 0xC00;
    }
    amp = (((s32)work->clock.frame_count & 1) << 5) * pulse->size / 4096;
    if (work->phase < 2) {
        k = LAST_RING;
        ring += LAST_RING;
    } else {
        k = 0;
        ring = work->rings;
    }
    for (i = k; i < RING_COUNT; i++, ring++) {
        if (ring->scale > 0) {
            size = ring->scale;
            for (j = 0, a = work->ring_angle; j < 17; j++, a = work->ring_angle + (j << 8)) {
                if (i == LAST_RING) {
                    setVector(&ring->a[j], rcos(a) * (work->config->size / 4) >> 12,
                              rsin(a) * (work->config->size / 4) >> 12, 0);
                    setVector(&ring->b[j], rcos(a) * (work->config->size * (amp + 128) / 256) >> 12,
                              rsin(a) * (work->config->size * (amp + 128) / 256) >> 12,
                              -(work->config->size * (amp + 64) / 256));
                } else {
                    setVector(&ring->a[j], rcos(a) * work->config->size >> 12,
                              rsin(a) * work->config->size >> 12, 0);
                    setVector(&ring->b[j], rcos(a) * (work->config->size * (amp + 384) / 256) >> 12,
                              rsin(a) * (work->config->size * (amp + 384) / 256) >> 12,
                              work->config->size * (amp + 128) / 256);
                }
            }
            rot.vx = -pitch;
            rot.vy = heading;
            rot.vz = 0;
            if (i == LAST_RING) {
                m.t[0] = work->matrix.t[0];
                m.t[1] = work->matrix.t[1];
                m.t[2] = work->matrix.t[2];
            } else {
                m.t[0] = work->aim.target.vx;
                m.t[1] = work->aim.target.vy;
                m.t[2] = work->aim.target.vz;
            }
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
            if (i == LAST_RING) {
                ir = work->color_22CC.r;
                ig = work->color_22CC.g;
                ib = work->color_22CC.b;
                or = work->color_22D0.r;
                og = work->color_22D0.g;
                ob = work->color_22D0.b;
            } else if (ring->scale < 0x800) {
                ir = work->color_22CC.r;
                ig = work->color_22CC.g;
                ib = work->color_22CC.b;
                or = work->color_22D0.r;
                og = work->color_22D0.g;
                ob = work->color_22D0.b;
            } else {
                fade = 0x1000 - ring->scale;
                ir = work->color_22CC.r * fade / 2048;
                ig = work->color_22CC.g * fade / 2048;
                ib = work->color_22CC.b * fade / 2048;
                or = work->color_22D0.r * fade / 2048;
                og = work->color_22D0.g * fade / 2048;
                ob = work->color_22D0.b * fade / 2048;
            }
            for (j = 0; j < 16; j++) {
                depth = RotTransPers4(&ring->a[j], &ring->a[j + 1],
                                      &ring->b[j], &ring->b[j + 1],
                                      (long *)&poly->x0, (long *)&poly->x1,
                                      (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
                poly->r0 = ir;
                poly->g0 = ig;
                poly->b0 = ib;
                poly->r1 = ir;
                poly->g1 = ig;
                poly->b1 = ib;
                poly->r2 = or;
                poly->g2 = og;
                poly->b2 = ob;
                poly->r3 = or;
                poly->g3 = og;
                poly->b3 = ob;
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, depth);
                }
            }
        }
        if (i == LAST_RING) {
            if (ring->scale < 0x1000 && work->phase < 3) {
                ring->scale = (work->clock.frame << 12) /
                             work->config->field_20;
                if (ring->scale >= 0x1000) {
                    ring->scale = 0x1000;
                    ring->count++;
                }
            } else if (work->clock.frame >= work->config->fade_start) {
                ring->scale = 0x1000 - ((work->clock.frame - work->config->fade_start) << 12) /
                             (work->config->fade_end - work->config->fade_start);
            }
        } else if (ring->scale < 0x1000) {
            ring->scale += work->clock.step << 7;
            if (ring->scale >= 0x1000) {
                if (work->phase >= 5) {
                    ring->scale = 0x1000;
                    if (i + 1 == LAST_RING && first == 1) {
                        work->phase = 6;
                    }
                } else {
                    first = 0;
                    ring->scale -= 0x1000;
                    ring->count++;
                }
            }
        }
    }
    work->ring_angle += work->clock.step * 80;
}
