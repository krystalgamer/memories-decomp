/*
 * Recursively separates the two model records when their projected distance
 * is below the largest paired half-extent, pushing D_800F56F0 apart and
 * retrying up to depth three. Matching C under gcc_2_8_1_g8_split with no
 * register pins, inline assembly or symbol aliases.
 *
 * Two behavioural points read off the retail listing:
 *  - `moved = limit` is set after the main body as well as on the mode == 0
 *    path (retail's `j next` carries `move s1,s5` in its slot), so the loop
 *    is `if (moved) { hits++; continue; } if (mode != 0) { body } moved =
 *    limit;`;
 *  - the push is scaled by the extents, `px = e0.values[i] * scale / 4096`
 *    and `pz = e2.values[i] * scale / 4096`; dx[i] and dz[i] decide only the
 *    sign.
 *
 * Shape that the compiler needs, in the order it was found:
 *  - each clamp pair is written into the struct declared after its
 *    destination and copied down (`e3 = e0`, `e0 = e1`, ...), with a
 *    do { } while (0) around the store before each copy: the loop notes are
 *    sched1 barriers that keep retail's global loads below the pair copies;
 *  - the dead read `nv = D_800F56F0.vpx` above the last pin, and the three
 *    reference names ref/nv/ry, leave the `lui` of D_800F56F0 as the join
 *    block's first insn so reorg can hoist it into the beqz slot;
 *  - the `hits++` arm sits out of line after the loop bottom;
 *  - `sd = ... / len` sits in its own do { } while (0), `cpx`/`apz` are
 *    named before the length call, and `cross` after them;
 *  - the loop-top reads of dy.values[i] and e1.values[i] go through two
 *    pointers that start at the frame base and are stepped twice, and the
 *    limit comparison is stored back into eb; see the comments in the loop.
 *
 * See notes/research/func-80051350-decode.md for the structural map.
 */
#include "../types.h"
#include "func_80051350.h"
#include "model.h"
#include "model_graphics_state.h"
#include "model_update_view_metrics.h"
#include "camera_view.h"
#include "../psyq/libgte.h"
#include "../ygo_types.h"

s32 func_80051350(s32 mode, s32 min_extent, s32 depth)
{
    ModelSeparationPair e3;
    ModelSeparationPair e0;
    ModelSeparationPair e1;
    ModelSeparationPair e2;
    ModelSeparationPair dx;
    ModelSeparationPair dy;
    ModelSeparationPair dz;
    ModelSeparationPair dist;
    ModelSeparationPair t;
    s32 moved;
    s32 hits;
    s32 i;
    s32 ox;
    s32 oz;
    s32 ref;
    s32 nv;
    s32 ry;
    s32 dz1;
    s32 v;
    s32 v5;
    ModelSlot *rec;

    ox = rcos(*(s16 *)&D_8009B47A + 0x800) * min_extent / 4096;
    oz = rsin(*(s16 *)&D_8009B47A + 0x800) * min_extent / 4096;

    rec = D_800F2C40;
    v = (s16)rec[0].field_DC8[3] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e0.values[0] = v;
    v = (s16)rec[1].field_DC8[3] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    do {
        e0.values[1] = v;
    } while (0);
    e3 = e0;
    v = (s16)rec[0].field_DC8[0] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e1.values[0] = v;
    v = (s16)rec[1].field_DC8[0] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    do {
        e1.values[1] = v;
    } while (0);
    e0 = e1;
    v = (s16)rec[0].field_DC8[1] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e2.values[0] = v;
    v = (s16)rec[1].field_DC8[1] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    do {
        e2.values[1] = v;
    } while (0);
    e1 = e2;
    v = (s16)rec[0].field_DC8[2] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    dx.values[0] = v;
    v5 = (s16)rec[1].field_DC8[2] / 2;
    if (v5 < min_extent) {
        v5 = min_extent;
    }
    nv = D_800F56F0.vpx;
    do {
        dx.values[1] = v5;
    } while (0);
    e2 = dx;

    ref = D_800F56F0.vpx;
    ref = ref + ox;
    dy.values[0] = ref - *(s16 *)&rec[0].field_DD0[0];
    dy.values[1] = ref - *(s16 *)&rec[1].field_DD0[0];
    dx = dy;
    nv = D_800F56F0.vpy;
    dz.values[0] = nv - *(s16 *)&rec[0].field_DD0[1];
    dz1 = nv - *(s16 *)&rec[1].field_DD0[1];
    do {
        dz.values[1] = dz1;
    } while (0);
    dy = dz;
    ry = D_800F56F0.vpz + oz;
    dist.values[0] = ry - *(s16 *)&rec[0].field_DD0[2];
    dist.values[1] = ry - *(s16 *)&rec[1].field_DD0[2];
    dz = dist;

    t.values[0] = SquareRoot0(
        dx.values[0] * dx.values[0] + dz.values[0] * dz.values[0]);
    t.values[1] = SquareRoot0(
        dx.values[1] * dx.values[1] + dz.values[1] * dz.values[1]);
    dist = t;

    moved = 0;
    hits = 0;
    if (D_800F2C40[0].field_E1F != 0) {
        if (D_800F2C40[0].field_DC0[3] >= 2) {
            if ((*(u32 *)D_800F2C40[0].field_DC0 & 0xFFFFFF) == 0) {
                dist.values[0] = -1;
            }
        }
    } else {
        dist.values[0] = -1;
    }
    if (D_800F2C40[1].field_E1F != 0) {
        if (D_800F2C40[1].field_DC0[3] >= 2) {
            if ((*(u32 *)D_800F2C40[1].field_DC0 & 0xFFFFFF) == 0) {
                dist.values[1] = -1;
            }
        }
    } else {
        dist.values[1] = -1;
    }

    depth = depth + 1;
    for (i = 0; i < 2; i++) {
        s32 limit;
        s32 eb;
        s32 d;
        s32 v;
        s32 *pv;
        s32 *pb;

        limit = e2.values[i];
        if (limit < e0.values[i]) {
            limit = e0.values[i];
        }
        if (limit < e3.values[i]) {
            limit = e3.values[i];
        }
        /* dy.values[i] and e1.values[i] through two pointers that start at
         * the frame base (e3 is the first pair, at sp+16) and step by i and
         * then by one. The final step folds into the load offsets, but it
         * leaves each pointer with two sets, so neither address insn gets
         * sched1's launch priority and both are computed before the first
         * load; reload_cse then turns the second into retail's copy. */
        pv = e3.values - 4;
        pv += i;
        pv++;
        pb = e3.values - 4;
        pb += i;
        pb++;
        v = pv[13];
        eb = pb[7];
        if (v < 0) {
            v = -v;
        }
        /* The comparison lands in eb itself: a block-local flag would be
         * placed first by local-alloc and take v0, pushing v to v1. */
        eb = eb < v;
        if (eb) {
            continue;
        }
        d = dist.values[i];
        if (d < 0) {
            continue;
        }
        if (d >= limit) {
            continue;
        }
        if (moved != 0) {
            goto hit;
        }
        if (mode != 0) {
            s32 scale;
            s32 px;
            s32 pz;

            scale = ((limit - d) << 12) / limit;
            px = e0.values[i] * scale / 4096;
            if (dx.values[i] <= 0) {
                px = -px;
            }
            pz = e2.values[i] * scale / 4096;
            if (dz.values[i] <= 0) {
                pz = -pz;
            }
            {
                s32 ax = D_800F56F0.vrx;
                s32 az = D_800F56F0.vpz;
                s32 bx = D_800F56F0.vrz;
                s32 cx = D_800F56F0.vpx;
                s32 ux = bx - az;
                s32 uz = cx - ax;
                s32 cpx = cx + px;
                s32 apz = az + pz;
                s32 cross = ax * az - cx * bx;
                s32 len = SquareRoot0(ux * ux + uz * uz);
                s32 sd = 0;

                if (len != 0) {
                    do {
                        sd = (cpx * ux + apz * uz + cross) / len;
                    } while (0);
                }
                if (D_8009AF98 == 0) {
                    if (sd < 0) {
                        D_8009AF99 = -1;
                    } else {
                        D_8009AF99 = 1;
                    }
                    D_8009AF98 = 0x1E;
                } else if (D_8009AF98 < 0xFF) {
                    D_8009AF98 = D_8009AF98 - 1;
                }
            }
            D_800F56F0.vpx = D_800F56F0.vpx + px;
            D_800F56F0.vpz = D_800F56F0.vpz + pz;
        }
        moved = limit;
        continue;
    hit:
        hits = hits + 1;
    }

    if (mode != 0 && moved != 0) {
        Model_UpdateViewMetrics(0);
    }
    if (hits != 0 && mode != 0 && depth < 3) {
        func_80051350(mode, min_extent, depth);
    }
    return moved;
}
