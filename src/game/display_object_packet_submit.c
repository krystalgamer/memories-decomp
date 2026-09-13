#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "gpu_packets.h"
#include "display_object_packet_submit.h"

/* Scratchpad work areas: the four quad vertices at 0x1F800300, the
 * RotAverageNclip4 depth/flag results at 0x1F8002E0, and the DivideFT4
 * inputs - its DIVPOLYGON4 at 0x1F800000, the colour at 0x1F800280 and the
 * four texture coordinates from 0x1F800290. */
#define SCRATCH_VERTEX(i) ((SVECTOR *)0x1F800300 + (i))

/* The high half of `mode` selects the handler and the low half is the
 * ordering-table depth. Cases 1-3 hand a GsSPRITE to libgs. Cases 4 and 5 get
 * the display object's attribute word in place of the sprite pointer and
 * submit a prepared POLY_G4 or POLY_GT4, projecting it first when GsPERS
 * (0x04000000) is set. Any other case builds a POLY_FT4 from the sprite and
 * projects it, subdividing through DivideFT4 when `extra` asks for it. */
void func_80042188(SpritePrim *sprite, u8 *packet, s32 ot, s32 mode, u8 *extra)
{
    SVECTOR *v;
    long *otz;
    DisplayObjectPacketOrigin *origin = (DisplayObjectPacketOrigin *)extra;
    s32 pri = (s16)mode;

    switch ((u32)mode >> 16) {
    case 1:
        GsSortFastSprite((GsSPRITE *)sprite, (GsOT *)ot, pri);
        return;
    case 2:
        GsSortFlipSprite((GsSPRITE *)sprite, (GsOT *)ot, pri);
        return;
    case 3:
        GsSortSprite((GsSPRITE *)sprite, (GsOT *)ot, pri);
        return;
    case 4: {
        if ((u32)sprite & 0x04000000) {
            otz = (long *)0x1F8002E0;
            v = SCRATCH_VERTEX(0);
            v[0].vx = ((POLY_G4 *)packet)->x0 - origin->x;
            v[0].vy = ((POLY_G4 *)packet)->y0 - origin->y;
            v[1].vx = ((POLY_G4 *)packet)->x1 - origin->x;
            v[1].vy = ((POLY_G4 *)packet)->y1 - origin->y;
            v[2].vx = ((POLY_G4 *)packet)->x2 - origin->x;
            v[2].vy = ((POLY_G4 *)packet)->y2 - origin->y;
            v[3].vx = ((POLY_G4 *)packet)->x3 - origin->x;
            v[3].vy = ((POLY_G4 *)packet)->y3 - origin->y;
            SCRATCH_VERTEX(3)->vz = 0;
            SCRATCH_VERTEX(2)->vz = 0;
            SCRATCH_VERTEX(1)->vz = 0;
            v[0].vz = 0;
            if (RotAverageNclip4(SCRATCH_VERTEX(0), SCRATCH_VERTEX(1), SCRATCH_VERTEX(2), SCRATCH_VERTEX(3),
                                 (long *)&((POLY_G4 *)packet)->x0, (long *)&((POLY_G4 *)packet)->x1,
                                 (long *)&((POLY_G4 *)packet)->x2, (long *)&((POLY_G4 *)packet)->x3,
                                 otz, otz + 1, otz + 2) <= 0) {
                return;
            }
        }
        if ((u32)sprite & 0x40000000) {
            func_8005B260((u32 *)packet, (GsOT *)ot, (u16)pri, ((u32)sprite >> 28) & 3);
        } else {
            GsSortPoly(packet, (GsOT *)ot, pri);
        }
        return;
    }
    case 5: {
        if ((u32)sprite & 0x04000000) {
            otz = (long *)0x1F8002E0;
            v = SCRATCH_VERTEX(0);
            v[0].vx = ((POLY_GT4 *)packet)->x0 - origin->x;
            v[0].vy = ((POLY_GT4 *)packet)->y0 - origin->y;
            v[1].vx = ((POLY_GT4 *)packet)->x1 - origin->x;
            v[1].vy = ((POLY_GT4 *)packet)->y1 - origin->y;
            v[2].vx = ((POLY_GT4 *)packet)->x2 - origin->x;
            v[2].vy = ((POLY_GT4 *)packet)->y2 - origin->y;
            v[3].vx = ((POLY_GT4 *)packet)->x3 - origin->x;
            v[3].vy = ((POLY_GT4 *)packet)->y3 - origin->y;
            SCRATCH_VERTEX(3)->vz = 0;
            SCRATCH_VERTEX(2)->vz = 0;
            SCRATCH_VERTEX(1)->vz = 0;
            v[0].vz = 0;
            if (RotAverageNclip4(SCRATCH_VERTEX(0), SCRATCH_VERTEX(1), SCRATCH_VERTEX(2), SCRATCH_VERTEX(3),
                                 (long *)&((POLY_GT4 *)packet)->x0, (long *)&((POLY_GT4 *)packet)->x1,
                                 (long *)&((POLY_GT4 *)packet)->x2, (long *)&((POLY_GT4 *)packet)->x3,
                                 otz, otz + 1, otz + 2) <= 0) {
                return;
            }
        }
        if ((u32)sprite & 0x40000000) {
            func_8005B260((u32 *)packet, (GsOT *)ot, (u16)pri, ((u32)sprite >> 28) & 3);
        } else {
            GsSortPoly(packet, (GsOT *)ot, pri);
        }
        return;
    }
    case 0:
        return;
    default: {
        SVECTOR *v2;
        SVECTOR *v3;
        SVECTOR *v1;
        u32 attribute;

        otz = (long *)0x1F8002E0;
        v = SCRATCH_VERTEX(0);
        attribute = sprite->attribute;

        /* GsSPRITE's colour-mode bits (24-25) and semi-transparency rate
         * (28-29) go into the tpage's mode and rate fields. */
        ((POLY_FT4 *)packet)->tpage =
            sprite->tpage | (((attribute >> 17) & 0x180) | ((attribute >> 23) & 0x60));
        if (sprite->attribute & 0x40000000) {
            SetSemiTrans(packet, 1);
        }
        ((POLY_FT4 *)packet)->clut = (sprite->cxcy.h.cy << 6) | ((sprite->cxcy.h.cx >> 4) & 0x3F);
        if (sprite->attribute & 0x800000) {
            /* Horizontally flipped: the left and right u columns swap. */
            ((POLY_FT4 *)packet)->u1 = ((POLY_FT4 *)packet)->u3 = sprite->uv.b.lo;
            ((POLY_FT4 *)packet)->u0 = ((POLY_FT4 *)packet)->u2 =
                sprite->uv.b.lo + sprite->extent.wh.w.word - 1;
            ((POLY_FT4 *)packet)->v0 = ((POLY_FT4 *)packet)->v1 = sprite->uv.b.hi;
            ((POLY_FT4 *)packet)->v2 = ((POLY_FT4 *)packet)->v3 =
                sprite->uv.b.hi + sprite->extent.wh.h - 1;
        } else {
            ((POLY_FT4 *)packet)->u0 = ((POLY_FT4 *)packet)->u2 = sprite->uv.b.lo;
            ((POLY_FT4 *)packet)->v0 = ((POLY_FT4 *)packet)->v1 = sprite->uv.b.hi;
            if (sprite->attribute & 0x80) {
                ((POLY_FT4 *)packet)->u1 = ((POLY_FT4 *)packet)->u3 =
                    sprite->uv.b.lo + sprite->extent.wh.w.word;
                ((POLY_FT4 *)packet)->v2 = ((POLY_FT4 *)packet)->v3 =
                    sprite->uv.b.hi + sprite->extent.wh.h;
            } else {
                ((POLY_FT4 *)packet)->u1 = ((POLY_FT4 *)packet)->u3 =
                    sprite->uv.b.lo + sprite->extent.wh.w.word - 1;
                ((POLY_FT4 *)packet)->v2 = ((POLY_FT4 *)packet)->v3 =
                    sprite->uv.b.hi + sprite->extent.wh.h - 1;
            }
        }
        v1 = v + 1;
        v2 = v + 2;
        v3 = v + 3;
        v->vx = v[2].vx = sprite->xy.h.x - origin->x;
        v[1].vx = v[3].vx = v->vx + sprite->extent.wh.w.word;
        v->vy = v1->vy = sprite->xy.h.y - origin->y;
        v[2].vy = v3->vy = v->vy + sprite->extent.wh.h;
        v3->vz = 0;
        v2->vz = 0;
        v1->vz = 0;
        v->vz = 0;
        if (RotAverageNclip4(v, v1, v2, v3,
                             (long *)&((POLY_FT4 *)packet)->x0, (long *)&((POLY_FT4 *)packet)->x1,
                             (long *)&((POLY_FT4 *)packet)->x2, (long *)&((POLY_FT4 *)packet)->x3,
                             otz, otz + 1, otz + 2) > 0) {
            if (origin->divisions == 0) {
                GsSortPoly(packet, (GsOT *)ot, pri);
                return;
            }
            {
                u32 *rgbc = (u32 *)0x1F800280;
                u32 *uv = (u32 *)0x1F800290;
                DIVPOLYGON4 *divp = (DIVPOLYGON4 *)0x1F800000;

                divp->ndiv = origin->divisions;
                divp->pih = 320;
                divp->piv = 272;
                *rgbc = *(u32 *)&((POLY_FT4 *)packet)->r0;
                uv[0] = *(u32 *)&((POLY_FT4 *)packet)->u0;
                uv[1] = *(u32 *)&((POLY_FT4 *)packet)->u1;
                uv[2] = *(u32 *)&((POLY_FT4 *)packet)->u2;
                uv[3] = *(u32 *)&((POLY_FT4 *)packet)->u3;
                D_800FE240 = (u8 *)DivideFT4(v, v1, v2, v3,
                                             uv, (u32 *)0x1F800294, (u32 *)0x1F800298, (u32 *)0x1F80029C,
                                             (CVECTOR *)rgbc, (POLY_FT4 *)D_800FE240,
                                             (u32 *)((GsOT *)ot)->org + pri, divp);
            }
        }
        return;
    }
    }
}
