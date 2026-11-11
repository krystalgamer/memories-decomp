
#include "../types.h"

#include "../psyq/libgte.h"
#include "sorted_entry_render.h"

void SortedEntry_Append(s32 dx, s32 dy, s32 dz) {
    SortedEntry *p;
    s32 q;
    s32 v;

    gSortedEntry_pCursor->distance = SquareRoot0(dx * dx + dz * dz);
    q = ratan2(dx, dz) / 16;
    if (q >= 256) {
        q = 255;
    }
    v = q << 8;

    p = gSortedEntry_pCursor;
    p->packed = v | (dy >> 4);
    p->append_index = gSortedEntry_nCount;
    p->sorted_position = 0;

    gSortedEntry_pCursor = p + 1;
    gSortedEntry_nCount = gSortedEntry_nCount + 1;
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/inline_c.h"
#include "gpu_packets.h"
#include "direct_polygon_renderers.h"

/* The two HMD primitive drivers for fog-shaded textured polygons that
   Model_GetPrimitiveHandler returns: Model_DrawFogTexturedTriangles for triangles (type
   0x0002000D, POLY_GT3) and Model_DrawFogTexturedQuads for quads (type 0x00020015,
   POLY_GT4). They sit back to back in the image and dispatch on the same
   gModel_dwFogDrawFlags bits: 3 hands the block off, 2 walks it, and 0-1 draw sorted
   entries filled and the rest as outlines. */

#define CVECTOR_WORD(color) (*(u32 *)(color))

/*
 * HMD primitive driver for type 0x0002000D (GsUFGT3, fog-shaded textured
 * triangle), returned by Model_GetPrimitiveHandler. Bit 3 of gModel_dwFogDrawFlags hands
 * the block to Model_DrawGt3Primitives, bit 2 walks the triangles through
 * SortedEntry_Append instead of drawing them, and bits 0-1 draw sorted entries as
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

#define LINE_G4_WORD_VIEW(packet) ((u32 *)(packet))
#define PACKET_OBJECT_VIEW(type, packet) (*(type *)(packet))

static inline s32 SortedEntry_PopPosition(void)
{
    s32 z = gSortedEntry_pCursor->sorted_position;

    gSortedEntry_pCursor++;
    return z;
}

u32 *Model_DrawFogTexturedTriangles(GsARGUNIT_NORMAL *arg)
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

    flags = gModel_dwFogDrawFlags;
    if (flags & 8) {
        return Model_DrawGt3Primitives(arg);
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
            SortedEntry_Append(x >= 0 ? x : -x, y >= 0 ? y : -y, w >= 0 ? w : -w);
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
        CVECTOR_WORD(grey) = gModel_dwFogBaseColor;
        while (--n != -1) {
            if ((gModel_dwFogDrawFlags & 2) &&
                (z = SortedEntry_PopPosition()) < gSortedEntry_nCount) {
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
                if (z > 0 && (scr[0] | scr[1] | scr[2]) >= 0) {
                    setPolyGT3(gt);
                    *(u16 *)&gt->u0 = rec[0];
                    *(u16 *)&gt->u1 = rec[2];
                    *(u16 *)&gt->u2 = rec[4];
                    gt->tpage = rec[3];
                    gt->clut = rec[1];
                    PACKET_OBJECT_VIEW(POLY_GT3, out) = *gt;
                    z = (scr[4] + scr[5] + scr[6]) / 3 >> 4;
                    addPrim(&arg->tagp->org[z], out);
                    out += sizeof(POLY_GT3);
                }
            } else {
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
                if ((scr[0] | scr[1] | scr[2]) >= 0) {
                    setLineG4(lg);
                    PACKET_OBJECT_VIEW(LINE_G4, out) = *lg;
                    z = (scr[4] + scr[5] + scr[6]) / 3 >> 4;
                    addPrim(&arg->tagp->org[z], out);
                    out += sizeof(LINE_G4);
                }
            }
            rec += 11;
            rec++;
        }
    } else {
        POLY_GT3 *gt = (POLY_GT3 *)0x1F800380;

        CVECTOR_WORD(white) = gModel_dwFogBaseColor;
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
                PACKET_OBJECT_VIEW(POLY_GT3, out) = *gt;
                z = (scr[4] + scr[5] + scr[6]) / 3 >> 4;
                addPrim(&arg->tagp->org[z], out);
                out += sizeof(POLY_GT3);
            }
            rec += 11;
            rec++;
        }
    }
    gGraphics_pPacketWorkCursor = (u32 *)out;
    return primp + 2;
}

/*
 * HMD primitive driver for type 0x00020015 (fog-shaded textured quad), the
 * POLY_GT4 twin of Model_DrawFogTexturedTriangles that Model_GetPrimitiveHandler returns.
 * Bit 3 of gModel_dwFogDrawFlags hands the block to Model_DrawGt4Primitives, bit 2 walks the
 * quads through SortedEntry_Append, and bits 0-1 draw sorted entries as POLY_GT4
 * and the rest as a LINE_G4 outline closed by a LINE_G2; otherwise every
 * front-facing quad is drawn as POLY_GT4.
 *
 * Matches under gcc_2_8_1_g8_split_psyq_gte with no hard register
 * assignments; the GTE commands are the official inline_c.h macros. The
 * shapes that decide the bytes, each read off the retail listing:
 *   - the LINE_G4 outline visits the quad's vertices as v0, v1, v3, v2 -- the
 *     third loaded vertex is rec[13] and the fourth rec[11] -- which is the
 *     closed outline of a quad rather than a bow tie. That order also makes
 *     rec[10] the last address giv in the loop, and gcc 2.8's loop pass takes
 *     the last giv as the representative of the combined cursor, which is
 *     why the cursor is biased at rec[10] (`addiu t3,s3,20`);
 *   - the flag words retail addresses through $s8 go through a second
 *     scratchpad pointer, flg = (s32 *)0x1F8003E8, used only at the three
 *     gte_stflg calls where retail uses $s8 and assigned at the top of the
 *     while body so the loop pass hoists it to where retail materialises it;
 *   - the first POLY_GT4 arm's addPrim reads arg->tagp inline, so the field
 *     is reloaded around the store through the packet pointer;
 *   - the flags & 4 arm's three ABS3 statements sit in a do { } while (0),
 *     which keeps each sum in a register of its own so the quotient gets a
 *     fresh one and the sra is duplicated into the bgez delay slot;
 *   - a no-op `rec++; rec--;` after the normal-table load, which removes the
 *     primitive cursor's stack spill, and both returns yielding the live
 *     `primp` rather than re-reading arg->primp.
 */

#define ABS3(v) ((v) >= 0 ? (v) : -(v))
#define LINE_G2_VIEW(packet) ((LINE_G2 *)(packet))

u32 *Model_DrawFogTexturedQuads(GsARGUNIT_NORMAL *arg)
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

    flags = gModel_dwFogDrawFlags;
    if (flags & 8) {
        return Model_DrawGt4Primitives(arg);
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
            do {
                x = ABS3((vertop[rec[7]].vx + vertop[rec[9]].vx + vertop[rec[11]].vx + vertop[rec[13]].vx) / 4);
                y = ABS3((vertop[rec[7]].vy + vertop[rec[9]].vy + vertop[rec[11]].vy + vertop[rec[13]].vy) / 4);
                w = ABS3((vertop[rec[7]].vz + vertop[rec[9]].vz + vertop[rec[11]].vz + vertop[rec[13]].vz) / 4);
            } while (0);
            SortedEntry_Append(x, y, w);
            rec += 14;
        }
        return (u32 *)primp + 2;
        } while (0);
    }

    if (flags & 3) {
        LINE_G4 *lg = (LINE_G4 *)0x1F800380;
        POLY_GT4 *gt = (POLY_GT4 *)0x1F800380;
        CVECTOR *grey = (CVECTOR *)0x1F8003D0;

        CVECTOR_WORD(white) = mask;
        CVECTOR_WORD(grey) = gModel_dwFogBaseColor;
        while (--n != -1) {
            flg = (s32 *)0x1F8003E8;
            if ((gModel_dwFogDrawFlags & 2) &&
                (z = SortedEntry_PopPosition()) < gSortedEntry_nCount) {
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
                if (z > 0 && (scr[0] | scr[1] | scr[2]) >= 0) {
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
                }
            } else {
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
                gte_ldv0(&vertop[rec[13]]);
                gte_rtps();
                gte_stsxy(&lg->x2);
                gte_stflg(flg);
                gte_ldv0(&nortop[rec[12]]);
                gte_ldrgb(white);
                gte_ncds();
                gte_strgb(&lg->r2);
                gte_stszotz(&scr[6]);
                gte_ldv0(&vertop[rec[11]]);
                gte_rtps();
                gte_stsxy(&lg->x3);
                gte_stflg(&scr[3]);
                gte_ldv0(&nortop[rec[10]]);
                gte_ldrgb(white);
                gte_ncds();
                gte_strgb(&lg->r3);
                gte_stszotz(&scr[7]);
                if ((scr[0] | scr[1] | scr[2] | scr[3]) >= 0) {
                    setLineG4(lg);
                    *(LINE_G4 *)out = *lg;
                    z = (scr[4] + scr[5] + scr[6] + scr[7]) / 4 >> 4;
                    addPrim(&arg->tagp->org[z], out);
                    out += 0x28;
                    *(u32 *)&LINE_G2_VIEW(out)->x0 = *(u32 *)&lg->x0;
                    *(u32 *)&LINE_G2_VIEW(out)->x1 = *(u32 *)&lg->x3;
                    *(u32 *)&LINE_G2_VIEW(out)->r0 = *(u32 *)&lg->r0;
                    *(u32 *)&LINE_G2_VIEW(out)->r1 = *(u32 *)&lg->r3;
                    setLineG2(LINE_G2_VIEW(out));
                    addPrim(&arg->tagp->org[z], out);
                    out += 0x14;
                }
            }
            rec += 14;
        }
    } else {
        POLY_GT4 *gt = (POLY_GT4 *)0x1F800380;

        CVECTOR_WORD(white) = gModel_dwFogBaseColor;
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
    gGraphics_pPacketWorkCursor = (u32 *)out;
    return (u32 *)primp + 2;
}

/* Zero-initialized storage this unit owns. Each build's generated
 * sorted_entry_render_commons subsegment places it at the retail addresses. */
u32 gModel_dwFogBaseColor;
u32 gModel_dwFogDrawFlags;
SortedEntry *gSortedEntry_pBase;
SortedEntry *gSortedEntry_pCursor;
u32 gSortedEntry_nCount;
u32 gSortedEntry_nSortedCount;

#include "../psyq/qsort.h"

/* Three-way comparator over the first word of two sorted-entry records,
 * returning 0, 1 or -1. SortedEntry_SortAndRelink below is its only consumer
 * and hands it to qsort over eight-byte entries; nothing calls it directly. */
int SortedEntry_Compare(const u32 *left, const u32 *right)
{
    if (*left == *right) {
        return 0;
    }
    if (*left >= *right) {
        return 1;
    }
    return -1;
}

/* Use the historical PSYQ declaration, whose comparator has unspecified args. */
void SortedEntry_SortAndRelink(void)
{
    SortedEntry *base = gSortedEntry_pBase;
    SortedEntry *entry = base;
    u32 count = gSortedEntry_nCount;
    u32 sorted_count;
    u32 index;

    gSortedEntry_nSortedCount = count;
    gModel_dwFogDrawFlags &= ~4;

    if (count >= 2) {
        s32 (*compare)(const u32 *, const u32 *) = SortedEntry_Compare;

        qsort(entry, count, sizeof(*entry), compare);
    }

    sorted_count = gSortedEntry_nSortedCount;
    index = 0;

    if (sorted_count != 0) {
        do {
            base[entry->append_index].sorted_position = index;
            index++;
            entry++;
        } while (index < sorted_count);
    }

    gSortedEntry_nCount = 0;
    gSortedEntry_pCursor = gSortedEntry_pBase;
}

void Model_SetFogDrawFlags(u32 value)
{
    gModel_dwFogDrawFlags = value;
    gModel_dwFogBaseColor = 0x808080;
}

void SortedEntry_BeginCollection(SortedEntry *list)
{
    gSortedEntry_nCount = 0;
    gSortedEntry_pCursor = list;
    gSortedEntry_pBase = list;
    gModel_dwFogDrawFlags |= 4;
}
