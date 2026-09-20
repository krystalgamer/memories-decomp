/*
 * Recursively separates two model records when their projected distance is
 * below the largest paired half-extent. Current best under
 * gcc_2_8_1_g8_split: 446 of 446 instructions at exact length, an EMPTY
 * opcode census by encoded fields, 2 structural blocks, 439 of 446 aligned
 * on opcode and registers, 8 raw words differing with relocations masked,
 * 3 with the register fields masked as well, and a shift-aware structural
 * distance of 4 (difflib over the register-masked words). The previous
 * state was 446, census 0, 15 structural blocks, 392 aligned, 212 raw, 178
 * register-masked, shift-aware 47. Words 0-266 and 277-445 now match the
 * target except for relocations; the residue is one basic block, the pair
 * reads at the loop top (267-276, below).
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
 * WHY THE do { } while (0) PINS AND THE DEAD READ ARE WHERE THEY ARE, read
 * off gcc 2.8.1's sched.c, cse.c, reorg.c and rtlanal.c rather than swept.
 *
 * Retail keeps every global load that follows a pair copy BELOW the copy
 * (words 64, 92, 120 after the copies at 60-63, 88-91, 116-119; the
 * D_800F56F0 loads at 148, 161, 172 after the copies at 144-147, 157-160,
 * 168-171) and never below a plain frame store (the lhu at 47 sits above
 * the store at 49, the lh at 151 above the store at 155). gcc's dependence
 * test cannot produce that from any spelling of a frame copy:
 * memrefs_conflict_p returns 0 for a frame-pointer address against any
 * constant address before it looks at a size or a flag, so a copy whose
 * destination is `fp + c` never holds a load whose base is a known symbol,
 * and a load whose base is unknown is held by the plain stores as well
 * (measured: the base hidden behind an empty asm holds the loads under
 * every store, +5). The reverse list scheduler's own trace shows the retail
 * copy has the load as a successor: with no dependence the copy is the only
 * ready insn in the load's latency gap and lands under it, which is where
 * every unpinned copy of this source lands. Which source property gave
 * retail that dependence is NOT established. The pins emulate it: a
 * NOTE_INSN_LOOP_BEG/END between two insns of a block makes the next insn a
 * full barrier (sched_analyze_insn, `if (loop_notes)`: a true dependence on
 * the last setter of every register plus reg_pending_sets_all), so every
 * later insn depends on it at that insn's own latency. Hence the pin closes
 * on the STORE before the copy, never on the copy or inside a statement
 * with a load: the insn after LOOP_END is then the block move (type
 * "store", cost 1) and the min_extent reload can still fill the next lhu's
 * delay slot; with the pin around the copy the lhu is the barrier and the
 * slot is a nop (the old word 93); with a load inside the pinned statement
 * the lh is the barrier at cost 2 (+2).
 *
 * The barrier holds registers too, and that is why the D_800F56F0 sites
 * needed more than a pin. reorg's fill_slots_from_thread fills the beqz
 * slot from the join block (two predecessors, so not an owned thread) only
 * with that block's FIRST insn; with a pin at `e2 = dx` sched1 leaves the
 * `sw v1,52(sp)` at the head (it reads the v1 the other arm sets, so it
 * cannot move) and the `lui a1,%hi(D_800F56F0)` stays behind the copy: +1.
 * The dead read `nv = D_800F56F0.vpx;` written just above that pin puts the
 * `high` half before the barrier: cse1 ends its extended basic block at
 * every LOOP_END (cse.c, only before loop.c), so it cannot common the two
 * reads; cse2 (after loop.c) shares the `high` between them, flow deletes
 * the dead load, and the surviving `high` is the join block's first insn,
 * which reorg hoists into the slot while the live load stays below the copy
 * (words 141-151 exact). The dead read has to land in a variable cse1
 * cannot copy-propagate away: with one `ref` for the three references and
 * `ref = nv;` after the vpy read, cse1 emptied `nv` and delete_dead_from_cse
 * removed the dead read before cse2 ran (the lui left the slot again, +1).
 * With three names (`ref`, `nv`, `ry`) each reference has its own short
 * pseudo, the dead read survives to cse2, and `ref` gets v1 as in retail
 * (one name for all three was one long local-alloc quantity, allocated
 * after the short temporaries and pushed into a2). At `dy = dz` the pinned
 * store takes a FRESH temporary `dz1`: with `v` or `v5` reused their live
 * ranges reach the reference block and the clamp-7 store moves into the
 * beqz slot (+0 with 89 raw, +1).
 *
 * One more fact from the same reading, for whoever touches the pair
 * stores: rtx_addr_varies_p is 1 for ANY BLKmode MEM, and true_dependence
 * exempts "struct at a varying address vs non-struct at a fixed address",
 * so a pair store spelled as a cast (`*(s32 *)((u8 *)&e0 + 4) = v`) sinks
 * BELOW the copy that reads it (measured, 447 and a miscompile). The
 * members stay members.
 *
 * Levers measured on this body, in the order they were found:
 *  - each clamp pair is written into the struct declared AFTER its
 *    destination and copied down (pair 1 into e0 then `e3 = e0`, ..., dist
 *    into `t` then `dist = t`): retail's frame writes every pair to the next
 *    slot and copies it (the nine pairs sit at 16 + 8k, an array's layout,
 *    but writing them as an array changes no RTL);
 *  - the `hits++` arm sits out of line after the `continue` at the loop
 *    bottom (`goto hit`), which is why retail needs the `j next` whose slot
 *    carries `moved = limit`; inline, gcc emits no jump and is -1;
 *  - `hits` is a PLAIN local, not volatile; `sd = (...) / len;` in its own
 *    do { } while (0);
 *  - the LAST clamp (record 1, field_DC8[2]) is computed in its own name
 *    `v5`, not in `moved`, and the two sums the projection consumes are
 *    declared before the length call (`s32 cpx = cx + px; s32 apz = az +
 *    pz;`): retail computes both into s2/s3 before the jal (346-347);
 *  - the reference vector's length is `SquareRoot0(ux * ux + uz * uz)`, the
 *    x product first;
 *  - the store pins before `e3 = e0`, `e0 = e1`, `e1 = e2` and `e2 = dx`,
 *    the dead read above the last of them, the three reference names and
 *    the `dz1` pin before `dy = dz` (the mechanism above): 15 -> 2
 *    structural blocks, 212 -> 28 raw, 178 -> 3 register-masked, 47 -> 4;
 *  - each clamp as ONE expression, `v = (s16)rec[k].field_DC8[j] / 2;`:
 *    the sign fix-up is then added into the halfword's own register
 *    (`addu v1,v1,v0; sra v1,v1,1`) instead of a temporary, 16 register
 *    words. As two statements against one name it had been measured better
 *    on the old base, with the nop at 93 still open;
 *  - `s32 cross = ax * az - cx * bx;` declared AFTER `cpx` and `apz`: the
 *    `cx * bx` mflo then no longer overlaps the `uz * uz` one before sched2
 *    and local-alloc gives them retail's a1/t6 (4 register words).
 *
 * MEASURED AND DEAD on this base: `v /= 2;` (identical to `v = v / 2`);
 * the halving expanded by hand, `v = v + ((u32)v >> 31); v = v >> 1;`
 * (-16); the sqrt argument named before `cross` (+1); the z square first
 * (425 aligned); the same fresh-temporary store pin before `dx = dy` (432
 * aligned, 21 raw) or before `dz = dist` (433, 15): the loads there are
 * already below their copies and the pin only moves registers; on the
 * one-name base, an empty `do { } while (0);`, an `asm volatile("")` or a
 * pin around the copy at `dy = dz` (the lui leaves the slot, +1/+2); a
 * `ModelSlot *rec` pointer hidden from cse (+5); the
 * pair copies as two word assignments; volatile on the pairs (the copy
 * becomes a memcpy call); the copies through pointers (+4, and the same
 * dependence only where cse's path limit stops folding them);
 * gcc_2_8_1_g8_split_no_sched1 (+15). At the loop top: `v = dy.values[i];
 * eb = e1.values[i];` in either order (445, retail minus the copy);
 * `(u8 *)&dy.values[i] - 56` as a pointer (identical to the installed
 * form); `(u8 *)&e1.values[i] - 32` (the 445 form); a dead `pv = 0` before
 * the loop (no effect); `(u8 *)&e3 - 16 + i * 4` (a pseudo equal to the
 * frame pointer, which cse never substitutes and reload spills, +6).
 *
 * Residual: words 267-276. Retail computes `sp + i*4` into v0, copies it
 * into v1, and reads `dy.values[i]` as 56(v0) and `e1.values[i]` as
 * 32(v1): two pseudos of the same address, the second canonical for cse
 * (make_regs_eqv keeps a later pseudo canonical only when it is referenced
 * outside the extended basic block and dies after the first). The
 * installed `pv = &dy.values[i]; v = *pv; eb = *(s32 *)((u8 *)pv - 24);`
 * keeps the count and gives `addiu v0,sp,56; addu v0,v0,a0; lw v1,0(v0);
 * lw v0,-24(v0)` and the v0/v1 swap of the four words after it. See
 * notes/research/func-80051350-decode.md for the structural map, whose push
 * and loop-exit passage carries both behavioural corrections.
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

        limit = e2.values[i];
        if (limit < e0.values[i]) {
            limit = e0.values[i];
        }
        if (limit < e3.values[i]) {
            limit = e3.values[i];
        }
        pv = &dy.values[i];
        v = *pv;
        eb = *(s32 *)((u8 *)pv - 24);
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
