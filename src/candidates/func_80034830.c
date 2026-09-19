/*
 * HMD primitive driver for type 0x00020015 (fog-shaded textured quad), the
 * POLY_GT4 twin of func_80033DB0 that Model_GetPrimitiveHandler returns.
 * Bit 3 of D_8009B30C hands the block to func_80061A84, bit 2 walks the
 * quads through func_80033CF8, and bits 0-1 draw sorted entries as POLY_GT4
 * and the rest as a LINE_G4 outline closed by a LINE_G2; otherwise every
 * front-facing quad is drawn as POLY_GT4.
 *
 * Built under gcc_2_8_1_g8_split_psyq_gte. 858 of 858 instructions at exact
 * length, opcode census distance 2 by encoded fields (sra -1, nop +1), 0
 * structural blocks of 7, 842 of 858 aligned on opcode and registers, and
 * 43 raw words differing from retail with relocations masked, all of them
 * register names. The previous state was 856 (-2), distance 6, 3 structural
 * blocks, 835 aligned. No hard register assignments; the GTE commands are the
 * official inline_c.h macros.
 *
 * The two edits that took it from -2 to exact length and closed every
 * structural block, each read off the target before it was measured:
 *   - the first POLY_GT4 arm's addPrim reads `arg->tagp` INLINE, not through
 *     a named `tg`. Retail loads `4(s6)` twice around addPrim's store through
 *     the packet pointer (words 346-359): the store may alias, so the source
 *     re-read the field. A named local kept it in a1 across the store and
 *     dropped a load: -2 -> -1, 3 structural blocks -> 1. The second arm was
 *     already written inline. The earlier header called naming `tg` a lever;
 *     it was measured on a base that still spilled the cursor, and on this
 *     base it is the wrong direction.
 *   - the flag stores retail addresses through $s8. The cfc2 map of the
 *     target shows twelve flag stores whose destinations this source already
 *     matched one for one -- 0(t2), 0(v0) and 0(s8) in the same order -- and
 *     the one remaining block was how s8 is made: retail materialises the
 *     literal 0x1F8003E8 with lui/ori, this source derived `addiu s8,t2,8`
 *     from scr. So `flg = (s32 *)0x1F8003E8` is a second pointer used at
 *     exactly the three gte_stflg calls where retail uses s8 (the second
 *     vertex of the first POLY_GT4 arm, the second and third vertices of the
 *     LINE_G4 arm), and nowhere else: -1 -> +0. WHERE it is assigned decides
 *     WHERE it is materialised: beside scr it lands at word 24 (2 structural
 *     blocks), before the while at word 41 (3), and assigned at the top of
 *     the `while (--n != -1)` body it is hoisted to word 125, exactly where
 *     retail has it, with 0 structural blocks.
 *
 * Two earlier edits still hold and are a COUPLED PAIR: a no-op `rec++;
 * rec--;` after the normal-table load, which removes the primitive cursor's
 * stack spill, and both returns yielding the live `primp` instead of
 * re-reading arg->primp. Alone each is worse (the no-op -4 with 19 structural
 * blocks, the returns +3 with 33); together they took the candidate from +1
 * with 39 structural blocks to the -2 this replaces. Retail HAS the cursor
 * spill: instruction 27 is `sw $s5,0x14($sp)`.
 *
 * MEASURED AND DEAD on this base, so the next attempt does not repeat them:
 * the literal inline at the three stflg sites (-2, 14 structural); a second
 * pointer for every scr[2]/scr[3] reference (+6, 36 structural); the ABS3
 * arm's sums named, one name or three (worse or identical); its four vertex
 * pointers named (3 structural); the quotient named (-2); ABS3 as an if
 * (4 structural); the fourth vertex written first in each sum (4); and all
 * orders of the x/y/w declarations (identical). From the earlier header, on
 * the old base: the no-op's position (identical wherever it sits), every
 * spelling of the `/ 4 >> 4` z divide (identical; only `>> 2 >> 4` moves, and
 * it is -13), and the LINE_G4/POLY_GT4 by-value copies through a typed local
 * or a cast (identical).
 *
 * Residual, all in the `flags & 4` arm (words 58-96): retail allocates the
 * fourth vertex pointer to a3 and the y sum to a2, this source the reverse,
 * and the w sum then lands in a2 and is divided in place with a nop in the
 * bgez delay slot where retail sums in v1 and divides into a fresh a2 with
 * the sra duplicated into the slot. That is one local-alloc priority
 * decision between two quantities and no hand spelling above moves it.
 * Measure by raw words with relocations masked: align_functions.py erases
 * the a0-a3 register names (#5358).
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
    s32 *flg;
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
            flg = (s32 *)0x1F8003E8;
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
                    gte_stflg(flg);
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
                    addPrim(&arg->tagp->org[z], out);
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
            gte_stflg(flg);
            gte_ldv0(&nortop[rec[8]]);
            gte_ldrgb(white);
            gte_ncds();
            gte_strgb(&lg->r1);
            gte_stszotz(&scr[5]);
            gte_ldv0(&vertop[rec[11]]);
            gte_rtps();
            gte_stsxy(&lg->x2);
            gte_stflg(flg);
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
