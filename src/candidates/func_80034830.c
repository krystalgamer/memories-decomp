/*
 * HMD primitive driver for type 0x00020015 (fog-shaded textured quad), the
 * POLY_GT4 twin of func_80033DB0 that Model_GetPrimitiveHandler returns.
 * Bit 3 of D_8009B30C hands the block to func_80061A84, bit 2 walks the
 * quads through func_80033CF8, and bits 0-1 draw sorted entries as POLY_GT4
 * and the rest as a LINE_G4 outline closed by a LINE_G2; otherwise every
 * front-facing quad is drawn as POLY_GT4.
 *
 * Built under gcc_2_8_1_g8_split_psyq_gte. 859 instructions against 858.
 * Both returns re-read arg->primp rather than keeping primp live, and the
 * POLY_GT4 arm reads arg->tagp into tg before addPrim; each shortens the
 * primitive cursor's spill.
 * The residual is allocation: this source spills the primitive cursor to
 * the stack where retail spills the hoisted 0xFFFFFF mask, which adds the
 * reloads, and z's divide-by-four lacks retail's duplicated sra. A no-op
 * `rec++; rec--;` after the normal-table load removes the spill, the same
 * tie func_80033DB0's candidate records. The LINE_G4 arm stores the second
 * and third vertices' flags both to scr[2], as retail does.
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
extern void func_80033CF8(s32 dx, s32 dy, s32 dz);

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
        return (u32 *)arg->primp + 2;
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
    return (u32 *)arg->primp + 2;
}
