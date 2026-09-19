/*
 * Recursively separates two model records when their projected distance is
 * below the largest paired half-extent. Current best under
 * gcc_2_8_1_g8_split: 446 instructions against 446, opcode census distance 2
 * over the divergent blocks (lw +1, j -1) and 202 of 446 aligned on opcode
 * and registers in 24 structural blocks, with no hard register assignments
 * and no inline assembly.
 *
 * The full vector block, inverted negation guard, reference-vector push, and
 * depth-three retry are present. A scalar clamp temporary feeding one shared
 * two-word ModelSeparationPair temporary reproduces the target assignments.
 *
 * Measured levers on top of that shape:
 *  - the sign test of the push is written `sd < 0` first, which gives the
 *    target's bgez instead of bltz;
 *  - the last clamp (record 1, field_DC8[2]) computes its value in `moved`
 *    before `moved` is zeroed, which gives `moved` the long-lived register
 *    and brings back four of the target's five register copies;
 *  - `hits` is volatile: the target keeps it in memory and reloads, adds and
 *    stores it on each increment;
 *  - `e0 = t;` and `e1 = t;` each sit in their own do { } while (0) block,
 *    which keeps the next clamp's halfword load after the pair copy;
 *  - the y reference is read into `nv` as its own statement before `ref`
 *    takes it (197 -> 202 aligned, census and structural blocks unchanged).
 *    A FRESH name is the lever here: borrowing a dead `ox` is 199 over 26
 *    structural and borrowing `v` is 198 over 25, both worse than either.
 *    `s32` and `u32` tie exactly.
 *
 * MEASURED AND CLOSED -- four axes, fourteen spellings, every one of them
 * byte-identical to the state it was measured against, so the next attempt
 * should not spend a round on any:
 *  - which side of the clamp comparison the loaded value sits on:
 *    `if (v < min_extent)` against `if (min_extent > v)` at all seven sites
 *    and at the `moved` site. gcc canonicalises both to the same RTL.
 *  - `min_extent` named into a block-scoped local per clamp, assigned before
 *    and after the division.
 *  - the negation guards `if (dx.values[i] <= 0) px = -px;` written as a
 *    ternary, for px, for pz, for both, and with an empty else arm.
 *  - the two `continue` statements written as an if/else if/else chain, in
 *    which all three arms fall through to one loop end. The target has one
 *    jump where this source has three, but gcc builds the same CFG from
 *    either spelling.
 *
 * decomp-permuter, rerun from this source, produced 126 outputs; all were
 * re-scored by splicing each body into the real source rather than by the
 * tool's own metric. Three ranked better on the project's key and were
 * rejected on reading their diffs: one duplicates a call under `if
 * (min_extent)` with identical arms, one moves `hits = 0;` inside the loop
 * (which resets the counter the recursion test reads), and one recomputes
 * `dy.values[1]` from the vpz reference and deletes the D_8009AF98 update.
 * The two latter change behaviour.
 *
 * Residual: in the `moved != 0` dispatch `moved` sits in $fp where the target
 * uses $s1 with the opposite branch polarity, and `mode` is reloaded from its
 * stack slot. All eleven of the target's min_extent reloads from its stack
 * slot are present. Addressing matches. See
 * notes/research/func-80051350-decode.md for the structural map.
 */
#include "../types.h"
#include "../game/model.h"
#include "../game/model_graphics_state.h"
#include "../game/model_update_view_metrics.h"
#include "../game/camera_view.h"
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
    volatile s32 hits;
    s32 i;
    s32 ox;
    s32 oz;
    s32 ref;
    s32 nv;
    s32 v;

    ox = rcos(*(s16 *)&D_8009B47A + 0x800) * min_extent / 4096;
    oz = rsin(*(s16 *)&D_8009B47A + 0x800) * min_extent / 4096;

    v = (s16)D_800F2C40[0].field_DC8[3] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.values[0] = v;
    v = (s16)D_800F2C40[1].field_DC8[3] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.values[1] = v;
    e3 = t;
    v = (s16)D_800F2C40[0].field_DC8[0] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.values[0] = v;
    v = (s16)D_800F2C40[1].field_DC8[0] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.values[1] = v;
    do {
        e0 = t;
    } while (0);
    v = (s16)D_800F2C40[0].field_DC8[1] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.values[0] = v;
    v = (s16)D_800F2C40[1].field_DC8[1] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.values[1] = v;
    do {
        e1 = t;
    } while (0);
    v = (s16)D_800F2C40[0].field_DC8[2] / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.values[0] = v;
    moved = (s16)D_800F2C40[1].field_DC8[2] / 2;
    if (moved < min_extent) {
        moved = min_extent;
    }
    t.values[1] = moved;
    e2 = t;

    ref = D_800F56F0.vpx + ox;
    t.values[0] = ref - *(s16 *)&D_800F2C40[0].field_DD0[0];
    t.values[1] = ref - *(s16 *)&D_800F2C40[1].field_DD0[0];
    dx = t;
    nv = D_800F56F0.vpy;
    ref = nv;
    t.values[0] = ref - *(s16 *)&D_800F2C40[0].field_DD0[1];
    t.values[1] = ref - *(s16 *)&D_800F2C40[1].field_DD0[1];
    dy = t;
    ref = D_800F56F0.vpz + oz;
    t.values[0] = ref - *(s16 *)&D_800F2C40[0].field_DD0[2];
    t.values[1] = ref - *(s16 *)&D_800F2C40[1].field_DD0[2];
    dz = t;

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
        s32 d;
        s32 v;

        limit = e2.values[i];
        if (limit < e0.values[i]) {
            limit = e0.values[i];
        }
        if (limit < e3.values[i]) {
            limit = e3.values[i];
        }
        v = dy.values[i];
        if (v < 0) {
            v = -v;
        }
        if (e1.values[i] < v) {
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
            hits = hits + 1;
            continue;
        }
        if (mode == 0) {
            moved = limit;
            continue;
        }
        {
            s32 scale;
            s32 px;
            s32 pz;

            scale = ((limit - d) << 12) / limit;
            px = dx.values[i] * scale / 4096;
            if (dx.values[i] <= 0) {
                px = -px;
            }
            pz = dz.values[i] * scale / 4096;
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
                s32 cross = ax * az - cx * bx;
                s32 len = SquareRoot0(ux * ux + uz * uz);
                s32 sd = 0;

                if (len != 0) {
                    sd = ((cx + px) * ux + (az + pz) * uz + cross) / len;
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
    }

    if (mode != 0 && moved != 0) {
        Model_UpdateViewMetrics(0);
    }
    if (hits != 0 && mode != 0 && depth < 3) {
        func_80051350(mode, min_extent, depth);
    }
    return moved;
}
