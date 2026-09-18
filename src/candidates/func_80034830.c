/*
 * HMD primitive driver for type 0x00020015 (fog-shaded textured quad), the
 * POLY_GT4 twin of func_80033DB0 that Model_GetPrimitiveHandler returns.
 * Bit 3 of D_8009B30C hands the block to func_80061A84, bit 2 walks the
 * quads through func_80033CF8, and bits 0-1 draw sorted entries as POLY_GT4
 * and the rest as a LINE_G4 outline closed by a LINE_G2; otherwise every
 * front-facing quad is drawn as POLY_GT4.
 *
 * Built under gcc_2_8_1_g8_split_psyq_gte. 856 instructions against 858,
 * 835 of them aligned on opcode and registers, in 11 divergent blocks of
 * which 3 are structural. Alignment is tools/project/align_functions.py; a
 * block is structural when the two sides differ in length or in opcode
 * sequence.
 *
 * Two edits get that, and they are a COUPLED PAIR: each one alone is worth
 * little or is worse, and only together do they move the function.
 *   - a no-op `rec++; rec--;` after the normal-table load, which removes the
 *     primitive cursor's stack spill. Alone: -4, 746 aligned, 71 blocks, 19
 *     structural.
 *   - both returns yielding the live `primp` instead of re-reading
 *     arg->primp. Alone, without the no-op: +3, 743 aligned, 92 blocks, 33
 *     structural -- i.e. a WORSE length than the 859/858 this replaces.
 * Together: -2, 835 aligned, 11 blocks, 3 structural, from a base of +1, 738
 * aligned, 96 blocks, 39 structural. Only one of the two returns is not
 * enough (the late one alone -2/767/74/20, the early one alone -4/746/73/21).
 *
 * The earlier header treated re-reading arg->primp as a virtue that "shortens
 * the primitive cursor's spill". Retail HAS that spill: instruction 27 is
 * `sw $s5,0x14($sp)`, with arg->primp loaded into the callee-saved $s5 and
 * read at +4 and +2 through it, where this source used the caller-saved $v1
 * and reused it on the next instruction.
 *
 * Measured and dead, recorded so the next attempt does not spend a round on
 * them. Each is the installed source plus one edit, every run measuring the
 * unmodified source first as its control:
 *   - the no-op's POSITION: after vertop, after nortop, before nortop, and in
 *     the else arm alone all give byte-identical results. What matters is that
 *     it exists, not where it sits.
 *   - the `z = (scr[4..7]) / 4 >> 4` spelling at all three sites: split into
 *     two statements against one name, `/ 64`, and `/ 4 / 16` are identical to
 *     the no-op alone; only `>> 2 >> 4` moves, and it is -13/714/77/28. The
 *     old header's claim that z's divide lacks retail's duplicated sra is not
 *     reachable by any spelling of the division. That duplicated sra is real
 *     but it is in the ABS3 arm (target 93-97), not in z.
 *   - the LINE_G4 and POLY_GT4 by-value copies: a typed destination local, a
 *     cast on the source, the POLY_GT4 sites, and both together are all
 *     identical to the base. PACKET is `unsigned char`, so the destination
 *     carries alignment 1, but that is not the lever -- if it were, one of the
 *     four spellings would have moved.
 *   - the 0x1F8003E8 literal, which retail materialises with lui/ori where
 *     this source derives `addiu $s8,$t2,8`: the targeted two-site edit in the
 *     LINE_G4 arm is -6/733/66/23 and the second site alone is -5/762/70/24.
 *
 * A false zero worth keeping: spelling that literal inline at every site
 * reaches EXACT length (+0) with 757 aligned, 51 blocks and 23 structural.
 * Ranking on the length key alone would install the worst of the states
 * measured here.
 *
 * The remaining 3 structural blocks: target[125:127], retail's lui/ori pair
 * against this source's `addiu $s8,$t2,8`; target[346:347], an `addiu
 * $s0,$s0,52` this source has and retail does not; and one more. The LINE_G4
 * arm stores the second and third vertices' flags both to scr[2], as retail
 * does.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/inline_c.h"
#include "../game/sorted_entry.h"
#include "../game/gpu_packets.h"

extern u32 *func_80061A84(GsARGUNIT_NORMAL *arg);

#define ABS3(v) ((v) >= 0 ? (v) : -(v))

u32 *func_80034830(GsARGUNIT_NORMAL *arg)
{
    long z;
    u32 flags;
    u32 mask;
    CVECTOR *white;
    s32 *scr;
    u32 *primp;
    PACKET *out;
    s32 n;
    u16 *rec;
    SVECTOR *vertop;
    SVECTOR *nortop;
    GsOT *tg;

    flags = D_8009B30C;
    if (flags & 8) {
        return func_80061A84(arg);
    }
    mask = 0xFFFFFF;
    white = (CVECTOR *)0x1F8003C0;
    scr = (s32 *)0x1F8003E0;
    primp = (u32 *)arg->primp;
    out = arg->out_packetp;
    n = ((u16 *)primp)[1];
    rec = (u16 *)(arg->primtop + (primp[1] & mask));
    vertop = arg->vertop;
    nortop = arg->nortop;
    rec++;
    rec--;

    if (flags & 4) {
        s32 x;
        s32 y;
        s32 w;
        do {
        while (--n != -1) {
            x = ABS3((vertop[rec[7]].vx + vertop[rec[9]].vx + vertop[rec[11]].vx + vertop[rec[13]].vx) / 4);
            y = ABS3((vertop[rec[7]].vy + vertop[rec[9]].vy + vertop[rec[11]].vy + vertop[rec[13]].vy) / 4);
            w = ABS3((vertop[rec[7]].vz + vertop[rec[9]].vz + vertop[rec[11]].vz + vertop[rec[13]].vz) / 4);
            func_80033CF8(x, y, w);
            rec += 14;
        }
        return (u32 *)primp + 2;
        } while (0);
    }

    if (flags & 3) {
        LINE_G4 *lg = (LINE_G4 *)0x1F800380;
        POLY_GT4 *gt = (POLY_GT4 *)0x1F800380;
        CVECTOR *grey = (CVECTOR *)0x1F8003D0;

        *(u32 *)white = mask;
        *(u32 *)grey = D_8009B300;
        while (--n != -1) {
            if (D_8009B30C & 2) {
                z = D_8009B310->sorted_position;
                D_8009B310++;
                if (z < D_8009B314) {
                    gte_ldv0(&vertop[rec[7]]);
                    gte_rtps();
                    gte_stsxy(&gt->x0);
                    gte_stflg(&scr[0]);
                    gte_ldv0(&nortop[rec[5]]);
                    gte_ldrgb(grey);
                    gte_ncds();
                    gte_strgb(&gt->r0);
                    gte_stszotz(&scr[4]);
                    gte_ldv0(&vertop[rec[9]]);
                    gte_rtps();
                    gte_stsxy(&gt->x1);
                    gte_stflg(&scr[1]);
                    gte_ldv0(&nortop[rec[8]]);
                    gte_ldrgb(grey);
                    gte_ncds();
                    gte_strgb(&gt->r1);
                    gte_stszotz(&scr[5]);
                    gte_ldv0(&vertop[rec[11]]);
                    gte_rtps();
                    gte_stsxy(&gt->x2);
                    gte_stflg(&scr[2]);
                    gte_ldv0(&nortop[rec[10]]);
                    gte_ldrgb(grey);
                    gte_ncds();
                    gte_strgb(&gt->r2);
                    gte_stszotz(&scr[6]);
                    gte_ldsxy3(*(s32 *)&gt->x0, *(s32 *)&gt->x1, *(s32 *)&gt->x2);
                    gte_nclip();
                    gte_stopz(&z);
                    if (z <= 0 || (scr[0] | scr[1] | scr[2]) < 0) {
                        goto next;
                    }
                    gte_ldv0(&vertop[rec[13]]);
                    gte_rtps();
                    gte_stsxy(&gt->x3);
                    gte_stflg(&scr[3]);
                    gte_ldv0(&nortop[rec[12]]);
                    gte_ldrgb(grey);
                    gte_ncds();
                    gte_strgb(&gt->r3);
                    gte_stszotz(&scr[7]);
                    setPolyGT4(gt);
                    *(u16 *)&gt->u0 = rec[0];
                    *(u16 *)&gt->u1 = rec[2];
                    *(u16 *)&gt->u2 = rec[4];
                    *(u16 *)&gt->u3 = rec[6];
                    gt->tpage = rec[3];
                    gt->clut = rec[1];
                    *(POLY_GT4 *)out = *gt;
                    z = (scr[4] + scr[5] + scr[6] + scr[7]) / 4 >> 4;
                    tg = arg->tagp;
                    addPrim(&tg->org[z], out);
                    out += 0x34;
                    goto next;
                }
            }
            gte_ldv0(&vertop[rec[7]]);
            gte_rtps();
            gte_stsxy(&lg->x0);
            gte_stflg(&scr[0]);
            gte_ldv0(&nortop[rec[5]]);
            gte_ldrgb(white);
            gte_ncds();
            gte_strgb(&lg->r0);
            gte_stszotz(&scr[4]);
            gte_ldv0(&vertop[rec[9]]);
            gte_rtps();
            gte_stsxy(&lg->x1);
            gte_stflg(&scr[2]);
            gte_ldv0(&nortop[rec[8]]);
            gte_ldrgb(white);
            gte_ncds();
            gte_strgb(&lg->r1);
            gte_stszotz(&scr[5]);
            gte_ldv0(&vertop[rec[11]]);
            gte_rtps();
            gte_stsxy(&lg->x2);
            gte_stflg(&scr[2]);
            gte_ldv0(&nortop[rec[10]]);
            gte_ldrgb(white);
            gte_ncds();
            gte_strgb(&lg->r2);
            gte_stszotz(&scr[6]);
            gte_ldv0(&vertop[rec[13]]);
            gte_rtps();
            gte_stsxy(&lg->x3);
            gte_stflg(&scr[3]);
            gte_ldv0(&nortop[rec[12]]);
            gte_ldrgb(white);
            gte_ncds();
            gte_strgb(&lg->r3);
            gte_stszotz(&scr[7]);
            if ((scr[0] | scr[1] | scr[2] | scr[3]) < 0) {
                goto next;
            }
            setLineG4(lg);
            *(LINE_G4 *)out = *lg;
            z = (scr[4] + scr[5] + scr[6] + scr[7]) / 4 >> 4;
            addPrim(&arg->tagp->org[z], out);
            out += 0x28;
            *(u32 *)&((LINE_G2 *)out)->x0 = *(u32 *)&lg->x0;
            *(u32 *)&((LINE_G2 *)out)->x1 = *(u32 *)&lg->x3;
            *(u32 *)&((LINE_G2 *)out)->r0 = *(u32 *)&lg->r0;
            *(u32 *)&((LINE_G2 *)out)->r1 = *(u32 *)&lg->r3;
            setLineG2((LINE_G2 *)out);
            addPrim(&arg->tagp->org[z], out);
            out += 0x14;
        next:
            rec += 14;
        }
    } else {
        POLY_GT4 *gt = (POLY_GT4 *)0x1F800380;

        *(u32 *)white = D_8009B300;
        while (--n != -1) {
            gte_ldv0(&vertop[rec[7]]);
            gte_rtps();
            gte_stsxy(&gt->x0);
            gte_stflg(&scr[0]);
            gte_ldv0(&nortop[rec[5]]);
            gte_ldrgb(white);
            gte_ncds();
            gte_strgb(&gt->r0);
            gte_stszotz(&scr[4]);
            gte_ldv0(&vertop[rec[9]]);
            gte_rtps();
            gte_stsxy(&gt->x1);
            gte_stflg(&scr[1]);
            gte_ldv0(&nortop[rec[8]]);
            gte_ldrgb(white);
            gte_ncds();
            gte_strgb(&gt->r1);
            gte_stszotz(&scr[5]);
            gte_ldv0(&vertop[rec[11]]);
            gte_rtps();
            gte_stsxy(&gt->x2);
            gte_stflg(&scr[2]);
            gte_ldv0(&nortop[rec[10]]);
            gte_ldrgb(white);
            gte_ncds();
            gte_strgb(&gt->r2);
            gte_stszotz(&scr[6]);
            gte_ldsxy3(*(s32 *)&gt->x0, *(s32 *)&gt->x1, *(s32 *)&gt->x2);
            gte_nclip();
            gte_stopz(&z);
            if (z > 0 && (scr[0] | scr[1] | scr[2]) >= 0) {
                gte_ldv0(&vertop[rec[13]]);
                gte_rtps();
                gte_stsxy(&gt->x3);
                gte_stflg(&scr[3]);
                gte_ldv0(&nortop[rec[12]]);
                gte_ldrgb(white);
                gte_ncds();
                gte_strgb(&gt->r3);
                gte_stszotz(&scr[7]);
                setPolyGT4(gt);
                *(u16 *)&gt->u0 = rec[0];
                *(u16 *)&gt->u1 = rec[2];
                *(u16 *)&gt->u2 = rec[4];
                *(u16 *)&gt->u3 = rec[6];
                gt->tpage = rec[3];
                gt->clut = rec[1];
                *(POLY_GT4 *)out = *gt;
                z = (scr[4] + scr[5] + scr[6] + scr[7]) / 4 >> 4;
                addPrim(&arg->tagp->org[z], out);
                out += 0x34;
            }
            rec += 14;
        }
    }
    D_800FE240 = (u32 *)out;
    return (u32 *)primp + 2;
}
