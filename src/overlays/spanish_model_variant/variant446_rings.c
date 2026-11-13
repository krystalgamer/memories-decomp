#include "../../types.h"

#include "../model_variant/model_variant.h"
#include "../french_model_variant/variant446_entry.h"

/* The MODEL385 rings with this family's work block: up to four seventeen-point
 * POLY_GT4 rings around the variant's direction, ring 3 timed by the record and
 * the others grown by step * 128 until phase 5. Quads are sorted whenever the
 * depth is positive. */
void SpanishModelVariant446_DrawRings(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    EntryState446 *work;
    u8 *pulse;
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

    work = (EntryState446 *)ctx;
    ring = work->rings;
    ot = Graphics_GetDefaultOrderingTable();
    first = 1;
    pulse = (u8 *)work + 0x1130;
    poly = &work->ring_packet;
    yaw = ratan2(work->aim.direction.vz, work->aim.direction.vx);
    heading = yaw + 0x800;
    pitch = ratan2(work->aim.direction.vy, work->aim.direction.vz) + 0x800;
    if (heading < 0x800) {
        heading = -heading + 0x400;
    } else {
        heading = yaw + 0xC00;
    }
    amp = ((MODEL_VARIANT_WORD(work, 0x1B90) & 1) << 5) * MODEL_VARIANT_WORD(pulse, 0x80) / 4096;
    if (work->phase < 2) {
        k = 3;
        ring += 3;
    } else {
        k = 0;
        ring = work->rings;
    }
    for (i = k; i < 4; i++, ring++) {
        if (ring->scale > 0) {
            size = ring->scale;
            for (j = 0, a = work->ring_angle; j < 17; j++, a = work->ring_angle + (j << 8)) {
                if (i == 3) {
                    setVector(&ring->a[j], rcos(a) * work->config->inner_radius >> 12,
                              rsin(a) * work->config->inner_radius >> 12, 0);
                    setVector(&ring->b[j], rcos(a) * (work->config->outer_radius + amp) >> 12,
                              rsin(a) * (work->config->outer_radius + amp) >> 12, -(amp + 64));
                } else {
                    setVector(&ring->a[j], rcos(a) * MODEL_VARIANT_WORD(work->config, 0x1C) >> 12,
                              rsin(a) * MODEL_VARIANT_WORD(work->config, 0x1C) >> 12, 0);
                    setVector(&ring->b[j], rcos(a) * (MODEL_VARIANT_WORD(work->config, 0x20) + amp) >> 12,
                              rsin(a) * (MODEL_VARIANT_WORD(work->config, 0x20) + amp) >> 12, amp + 128);
                }
            }
            rot.vx = -pitch;
            rot.vy = heading;
            rot.vz = 0;
            if (i == 3) {
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
            if (i == 3) {
                ir = ((u8 *)work)[0x1BBC];
                ig = ((u8 *)work)[0x1BBD];
                ib = ((u8 *)work)[0x1BBE];
                or = ((u8 *)work)[0x1BC0];
                og = ((u8 *)work)[0x1BC1];
                ob = ((u8 *)work)[0x1BC2];
            } else if (ring->scale < 0x800) {
                ir = ((u8 *)work)[0x1BBC];
                ig = ((u8 *)work)[0x1BBD];
                ib = ((u8 *)work)[0x1BBE];
                or = ((u8 *)work)[0x1BC0];
                og = ((u8 *)work)[0x1BC1];
                ob = ((u8 *)work)[0x1BC2];
            } else {
                fade = 0x1000 - ring->scale;
                ir = ((u8 *)work)[0x1BBC] * fade / 2048;
                ig = ((u8 *)work)[0x1BBD] * fade / 2048;
                ib = ((u8 *)work)[0x1BBE] * fade / 2048;
                or = ((u8 *)work)[0x1BC0] * fade / 2048;
                og = ((u8 *)work)[0x1BC1] * fade / 2048;
                ob = ((u8 *)work)[0x1BC2] * fade / 2048;
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
                if (depth > 0) {
                    GsSortPoly(poly, ot, depth);
                }
            }
        }
        if (i == 3) {
            if (ring->scale < 0x1000 && work->phase < 3) {
                ring->scale = (u32)(work->elapsed << 12) /
                             (u32)work->config->duration;
                if (ring->scale >= 0x1000) {
                    ring->scale = 0x1000;
                    ring->count++;
                }
            } else if ((u32)work->elapsed >= (u32)work->config->fade) {
                ring->scale = 0x1000 - (u32)((work->elapsed - work->config->fade) << 12) /
                             (u32)(work->config->end - work->config->fade);
            }
        } else if (ring->scale < 0x1000) {
            ring->scale += work->step << 7;
            if (ring->scale >= 0x1000) {
                if (work->phase >= 5) {
                    ring->scale = 0x1000;
                    if (i + 1 == 3 && first == 1) {
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
    work->ring_angle += work->step * 80;
}
