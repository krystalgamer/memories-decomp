/*
 * Recursively separates two model records when their projected distance is
 * below the largest paired half-extent. Current best under
 * gcc_2_8_1_g8_split: 446 of 446 instructions at exact length, an EMPTY
 * opcode census by encoded fields, 15 structural blocks, 392 of 446 aligned
 * on opcode and registers, 212 raw words differing with relocations masked,
 * 178 with the register fields masked as well, and a shift-aware structural
 * distance of 47 (difflib over the register-masked words). The previous
 * state was 446, census 0, 11 structural blocks, 324 aligned, 290 raw, 261
 * register-masked, shift-aware 51. Almost all of the register-masked count
 * is ONE displaced word: the nop in the load-delay slot of clamp 3's lhu at
 * word 93, where retail schedules the min_extent reload, shifts everything
 * from 98 to 267 by one; read the shift-aware distance, not the positional
 * counts, on this function.
 *
 * TWO BEHAVIOURAL CORRECTIONS, both read off the retail listing before they
 * were measured, and both worth more than any alignment figure:
 *  - retail sets `moved = limit` AFTER the main body as well as on the
 *    `mode == 0` path: the body's own fall-through lands on `j next` with
 *    `move s1,s5` in the delay slot (words 399-400), and the `mode == 0`
 *    branch jumps to that same instruction. The loop is therefore
 *    `if (moved) { hits++; continue; } if (mode != 0) { body } moved = limit;`
 *    The earlier source wrote `if (mode == 0) { moved = limit; continue; }`
 *    and left `moved` untouched after the body, so with `mode != 0` it never
 *    reached the `hits++` path and never recursed.
 *  - the push is scaled by the EXTENTS, not by the distance: `mult a2,a1` at
 *    word 304 multiplies the e0[i] loaded for the max test by `scale`, and
 *    `lw v0,0(a3)` at 314 re-reads e2[i] through the `&e2.values[i]` pointer
 *    materialised at 253-254 (the two instructions the earlier source
 *    lacked) before multiplying it by `scale`; dx[i] and dz[i] decide only
 *    the sign. So `px = e0.values[i] * scale / 4096` and
 *    `pz = e2.values[i] * scale / 4096`, with the sign tests unchanged.
 *
 * Levers measured on this body, in the order they were found:
 *  - each clamp pair is written into the struct declared AFTER its
 *    destination and copied down (pair 1 into e0 then `e3 = e0`, ..., dist
 *    into `t` then `dist = t`): retail's frame writes every pair to the next
 *    slot and copies it, and the declaration order is otherwise unchanged;
 *  - the `hits++` arm sits out of line after the `continue` at the loop
 *    bottom (`goto hit`), which is why retail needs the `j next` whose slot
 *    carries `moved = limit`; inline, gcc emits no jump and is -1;
 *  - `eb = e1.values[i]` is read before the |dy| test, where retail loads it
 *    ahead of the bgez (word 271); this is the last instruction of the count;
 *  - each clamp is `v = (s16)x; v = v / 2;` against one name, which is
 *    retail's `sra v1,v1,0x1` into the halfword's own register;
 *  - `hits` is a PLAIN local, not volatile; the reference vector's length is
 *    `SquareRoot0(uz * uz + ux * ux)`, the z product first; the z reference
 *    `D_800F56F0.vpz + oz` is computed into `ry` right after clamp 0 and
 *    consumed as `ref = ry;` (inline at the dist computation it is +1);
 *    `sd = (...) / len;` in its own do { } while (0);
 *  - the LAST clamp (record 1, field_DC8[2]) is computed in its own name
 *    `v5`, not in `moved`: with the clamp in `moved` that name carried four
 *    more references, won the allocation and took s0, pushing the record
 *    base D_800F2C40 into s1 and ox into s2; retail has the base in s0 and
 *    `moved` in s1 sharing with ox (word 25). Alone it is -1 (the address
 *    chain of D_800F56F0 re-forms); it is exact with the next lever, which
 *    is the coupled pair the permuter found;
 *  - the two sums the projection consumes are DECLARED before the length
 *    call: `s32 cpx = cx + px; s32 apz = az + pz;` above
 *    `s32 len = SquareRoot0(uz * uz + ux * ux);` and `sd = (cpx * ux + apz *
 *    uz + cross) / len;`. Retail computes both sums into s2/s3 before the
 *    jal (words 346-347) and keeps cross in the delay slot; written in the
 *    quotient the candidate keeps cx and az alive across the call instead
 *    and computes the sums after it. The permuter reached the second sum
 *    by borrowing `limit`, which is live (`moved = limit` at the loop
 *    bottom) and therefore wrong; the fresh name measures identically.
 *    With `v5`: 290 -> 212 raw, 261 -> 178 register-masked, 51 -> 47,
 *    324 -> 392 aligned, and the s1/s2 pair of the record base is gone.
 *
 * COUPLED RESIDUE, recorded so it is not chased one half at a time: the two
 * do { } while (0) pins hold two instructions (without them the count is -2,
 * without the first -1) and they are also what leaves a nop in the delay
 * slot of clamp 3's lhu at word 93, where retail schedules the min_extent
 * reload (`lw t3,148(sp)`; this source issues it at 98 after the halfword
 * arithmetic). The pin's extent was swept on both sides on the earlier
 * base and the axis is closed there; it has not been re-swept on this base.
 *
 * MEASURED AND DEAD on this base: a second name for `moved` at the loop-top
 * test (nothing); `oz` sharing the name `moved` (+1); the last clamp in the
 * shared `v` (-1 and worse); the sum named inside the do { } while (0)
 * (identical to none); `cx + px` left in the quotient with only `az + pz`
 * named (221 raw, 185 masked); the two declarations in the other order
 * (identical). From the earlier headers: `dx.values[1] * dx.values[1]`
 * named before the first SquareRoot0 call (a false positional gain, the
 * square is computed after the call), a `u8 *` local for field_DC0, `oz`
 * borrowed for the pz product, `pz = scale;` chained, a named read of
 * `hits`, the declaration position of `t`, do { } while (0) around whole
 * clamp groups or every copy, and the earlier if/else chain claim.
 *
 * Residual: the nop at 93 and the one-word shift it causes to 267; retail's
 * `move v1,v0` at 269 (a second address register for the pair reads at the
 * loop top, which this source reads through one); the order of the four
 * D_800F56F0 reads in the push block (retail loads vrx, vpz, vrz, vpx; this
 * source vrx, vpz, vpx, vrz) and the register-only words that follow. See
 * notes/research/func-80051350-decode.md for the structural map, whose push
 * and loop-exit passage carries both corrections.
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
    s32 hits;
    s32 i;
    s32 ox;
    s32 oz;
    s32 ref;
    s32 nv;
    s32 ry;
    s32 v;
    s32 v5;

    ox = rcos(*(s16 *)&D_8009B47A + 0x800) * min_extent / 4096;
    oz = rsin(*(s16 *)&D_8009B47A + 0x800) * min_extent / 4096;

    v = (s16)D_800F2C40[0].field_DC8[3];

    v = v / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e0.values[0] = v;
    v = (s16)D_800F2C40[1].field_DC8[3];
    v = v / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e0.values[1] = v;
    e3 = e0;
    v = (s16)D_800F2C40[0].field_DC8[0];
    v = v / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e1.values[0] = v;
    v = (s16)D_800F2C40[1].field_DC8[0];
    v = v / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e1.values[1] = v;
    do {
        e0 = e1;
    } while (0);
    v = (s16)D_800F2C40[0].field_DC8[1];
    v = v / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e2.values[0] = v;
    v = (s16)D_800F2C40[1].field_DC8[1];
    v = v / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    e2.values[1] = v;
    do {
        e1 = e2;
    } while (0);
    v = (s16)D_800F2C40[0].field_DC8[2];
    v = v / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    ry = D_800F56F0.vpz + oz;
    dx.values[0] = v;
    v5 = (s16)D_800F2C40[1].field_DC8[2];
    v5 = v5 / 2;
    if (v5 < min_extent) {
        v5 = min_extent;
    }
    dx.values[1] = v5;
    e2 = dx;

    ref = D_800F56F0.vpx + ox;
    dy.values[0] = ref - *(s16 *)&D_800F2C40[0].field_DD0[0];
    dy.values[1] = ref - *(s16 *)&D_800F2C40[1].field_DD0[0];
    dx = dy;
    nv = D_800F56F0.vpy;
    ref = nv;
    dz.values[0] = ref - *(s16 *)&D_800F2C40[0].field_DD0[1];
    dz.values[1] = ref - *(s16 *)&D_800F2C40[1].field_DD0[1];
    dy = dz;
    ref = ry;
    dist.values[0] = ref - *(s16 *)&D_800F2C40[0].field_DD0[2];
    dist.values[1] = ref - *(s16 *)&D_800F2C40[1].field_DD0[2];
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

        limit = e2.values[i];
        if (limit < e0.values[i]) {
            limit = e0.values[i];
        }
        if (limit < e3.values[i]) {
            limit = e3.values[i];
        }
        eb = e1.values[i];
        v = dy.values[i];
        if (v < 0) {
            v = -v;
        }
        if (eb < v) {
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
                s32 cross = ax * az - cx * bx;
                s32 cpx = cx + px;
                s32 apz = az + pz;
                s32 len = SquareRoot0(uz * uz + ux * ux);
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
