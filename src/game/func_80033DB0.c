/*
 * HMD primitive driver for type 0x0002000D (GsUFGT3, fog-shaded textured
 * triangle), returned by Model_GetPrimitiveHandler. Bit 3 of D_8009B30C hands
 * the block to func_8006151C, bit 2 walks the triangles through
 * func_80033CF8 instead of drawing them, and bits 0-1 draw sorted entries as
 * POLY_GT3 and the rest as closed LINE_G4 outlines; otherwise every
 * front-facing triangle is drawn as POLY_GT3.
 *
 * Built under gcc_2_8_1_g8_split_psyq_gte, whose assembly filter turns the
 * official inline_c.h RTPS, NCDS and NCLIP placeholders into native COP2
 * words. Splitting each 12-halfword primitive stride into `rec += 11;
 * rec++;` preserves the single retail `addiu 24` while giving the cursor
 * enough allocator weight to take $t6 and leave the normal table in $t7.
 * All 672 instructions and relocations then match.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/inline_c.h"
#include "sorted_entry.h"
#include "gpu_packets.h"
#include "func_80033DB0.h"
#include "func_8006151C.h"

#define LINE_G4_WORD_VIEW(packet) ((u32 *)(packet))
#define CVECTOR_WORD(color) (*(u32 *)(color))

u32 *func_80033DB0(GsARGUNIT_NORMAL *arg)
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

    flags = D_8009B30C;
    if (flags & 8) {
        return func_8006151C(arg);
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
            x = (vertop[rec[7]].vx + vertop[rec[9]].vx + vertop[rec[11]].vx) / 3;
            y = (vertop[rec[7]].vy + vertop[rec[9]].vy + vertop[rec[11]].vy) / 3;
            w = (vertop[rec[7]].vz + vertop[rec[9]].vz + vertop[rec[11]].vz) / 3;
            func_80033CF8(x >= 0 ? x : -x, y >= 0 ? y : -y, w >= 0 ? w : -w);
            rec += 11;
            rec++;
        }
        return primp + 2;
        } while (0);
    }

    if (flags & 3) {
        SVECTOR *v1;
        u32 c0;
        u32 c1;
        CVECTOR *grey = (CVECTOR *)0x1F8003D0;
        LINE_G4 *lg = (LINE_G4 *)0x1F800380;
        POLY_GT3 *gt = (POLY_GT3 *)0x1F800380;

        CVECTOR_WORD(white) = mask;
        CVECTOR_WORD(grey) = D_8009B300;
        while (--n != -1) {
            if (D_8009B30C & 2) {
                z = D_8009B310->sorted_position;
                D_8009B310++;
                if (z < D_8009B314) {
                    gte_ldv0(&vertop[rec[7]]);
                    gte_rtps();
                    gte_stsxy(&gt->x0);
                    gte_stflg(&scr[0]);
                    gte_ldv0(&nortop[rec[6]]);
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
                    setPolyGT3(gt);
                    *(u16 *)&gt->u0 = rec[0];
                    *(u16 *)&gt->u1 = rec[2];
                    *(u16 *)&gt->u2 = rec[4];
                    gt->tpage = rec[3];
                    gt->clut = rec[1];
                    *(POLY_GT3 *)out = *gt;
                    z = (scr[4] + scr[5] + scr[6]) / 3 >> 4;
                    addPrim(&arg->tagp->org[z], out);
                    out += sizeof(POLY_GT3);
                    goto next;
                }
            }
            gte_ldv0(&vertop[rec[7]]);
            gte_rtps();
            gte_stsxy(&lg->x0);
            gte_stflg(&scr[0]);
            gte_ldv0(&nortop[rec[6]]);
            gte_ldrgb(white);
            gte_ncds();
            gte_strgb(&lg->r0);
            gte_stszotz(&scr[4]);
            c0 = LINE_G4_WORD_VIEW(lg)[1];
            c1 = LINE_G4_WORD_VIEW(lg)[2];
            v1 = &vertop[rec[9]];
            LINE_G4_WORD_VIEW(lg)[7] = c0;
            LINE_G4_WORD_VIEW(lg)[8] = c1;
            gte_ldv0(v1);
            gte_rtps();
            gte_stsxy(&lg->x1);
            gte_stflg(&scr[1]);
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
            if ((scr[0] | scr[1] | scr[2]) < 0) {
                goto next;
            }
            setLineG4(lg);
            *(LINE_G4 *)out = *lg;
            z = (scr[4] + scr[5] + scr[6]) / 3 >> 4;
            addPrim(&arg->tagp->org[z], out);
            out += sizeof(LINE_G4);
        next:
            rec += 11;
            rec++;
        }
    } else {
        POLY_GT3 *gt = (POLY_GT3 *)0x1F800380;

        CVECTOR_WORD(white) = D_8009B300;
        while (--n != -1) {
            gte_ldv0(&vertop[rec[7]]);
            gte_rtps();
            gte_stsxy(&gt->x0);
            gte_stflg(&scr[0]);
            gte_ldv0(&nortop[rec[6]]);
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
                setPolyGT3(gt);
                *(u16 *)&gt->u0 = rec[0];
                *(u16 *)&gt->u1 = rec[2];
                *(u16 *)&gt->u2 = rec[4];
                gt->tpage = rec[3];
                gt->clut = rec[1];
                *(POLY_GT3 *)out = *gt;
                z = (scr[4] + scr[5] + scr[6]) / 3 >> 4;
                addPrim(&arg->tagp->org[z], out);
                out += sizeof(POLY_GT3);
            }
            rec += 11;
            rec++;
        }
    }
    D_800FE240 = (u32 *)out;
    return primp + 2;
}
