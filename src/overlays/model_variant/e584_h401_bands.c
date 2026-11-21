#include "../../types.h"
#include "model_variant.h"
#include "e584_h401_entry.h"

/* The French build reads the second strip's next column from the record
 * instead of the column view. */
#if defined(VERSION_FRENCH)
#define VARIANT401_BANDS_NEXT_X(row, ofs) rec->screen[row][j + 1]
#define VARIANT401_BANDS_NEXT_Y(row, ofs) (rec->screen[row][j + 1] >> 16)
#else
#define VARIANT401_BANDS_NEXT_X(row, ofs) *(u16 *)((column + (ofs)))
#define VARIANT401_BANDS_NEXT_Y(row, ofs) *(s16 *)((column + (ofs)) + 2)
#endif

void ModelVariant401_DrawBand(Variant401EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long flags[1][9];
    GsOT *ot;
    Variant401EntryBand *rec;
    s16 j3, j4;
    Variant401EntryState *work;
    POLY_GT4 *poly;
    s16 j, q5, q6;
    u16 i;
    s32 yaw;
    long p;
    u16 i2;
    s32 a;
    s32 radius;
    s32 t, tc, tn, tz, tr;
    s32 yv;
    s32 lo, lo2, yv2, yv3, yv4;
    SVECTOR *row0, *row1, *row2;
    u8 *column;

    work = context;
    poly = work->packets.textured;
    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx) + 0x800;
    if (yaw != 0) {
        lo = 0;
    }
    tr = work->scale * 21;
    if (tr < 0) {
        tr += 0xFFF;
    }
    tr = (u32)tr >> 12;
    radius = tr;
    if (tr != 0) {
        lo = 0;
    }
    rec = work->bands;
    for (j3 = 0; j3 < 1; j3++, rec++) {
        for (j = 0; j < 9; j++) {
            tc = rec->offset[j];
            a = 0;
            if (tc > 0) {
                if (rec) {
                    a = tc;
                } else {
                    a = tc;
                }
            }
            /* point[0][j] with retail's index-first add (&rec->point[0][j]
             * swaps the addu operands); point[1][j] and point[2][j] are reached
             * from it (+9, +18), as retail does. */
            row0 = (SVECTOR *)(j * 8 + (s32)rec);
            row0->vx = rcos(0x400) * (s16)radius >> 12;
            lo = rsin(0x400) * (s16)radius;
            row0->vz = 0;
            row0[9].vx = 0;
            row0->vy = lo >> 12;
            row1 = &row0[9];
            row1->vy = 0;
            row1->vz = 0;
            row0[18].vx = rcos(0xC00) * (s16)radius >> 12;
            lo2 = rsin(0xC00) * (s16)radius;
            row2 = &row0[18];
            row2->vz = 0;
            row2->vy = lo2 >> 12;
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = yaw;
            m.t[0] = work->matrix.t[0] + work->aim.direction.vx * a / 1024;
            m.t[1] = work->matrix.t[1] + work->aim.direction.vy * a / 1024;
            m.t[2] = work->matrix.t[2] + work->aim.direction.vz * a / 1024;
            scale.vx = 0x1000;
            scale.vy = 0x1000;
            scale.vz = 0x1000;
            RotMatrix(&rot, &m);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            ReadRotMatrix(&ls);
            RotMatrix(&rot, &ls);
            ScaleMatrix(&ls, &scale);
            SetRotMatrix(&ls);
            rec->otz[j] = RotTransPers3(row0, &rec->point[1][j],
                                                   &rec->point[2][j],
                                                   &rec->screen[0][j],
                                                   &rec->screen[1][j],
                                                   &rec->screen[2][j],
                                                   &p, &flags[j3][j]);
            if (work->tail.phase < 3 && rec->offset[j] < 0x400) {
                if (j == 0) {
                    t = work->config->burst_start;
                    rec->offset[0] = ((work->clock.frame - t) << 10) /
                                     (work->config->burst_end - t);
                } else {
                    rec->offset[j] += work->clock.step << 6;
                }
                if (rec->offset[j] >= 0x400) {
                    rec->offset[j] = 0x400;
                    if (j == 0) {
                        work->tail.phase = 2;
                    }
                }
            }
            if (j != 0) {
                lo = 0;
            }
        }
    }
    rec = work->bands;
    for (j3 = 0; j3 < 1; j3++, rec++) {
        for (j = 0; j < 8; j++) {
            poly->x0 = rec->screen[0][j];
            poly->y0 = rec->screen[0][j] >> 16;
            poly->x1 = rec->screen[0][j + 1];
            poly->y1 = rec->screen[0][j + 1] >> 16;
            poly->x2 = rec->screen[1][j];
            poly->y2 = rec->screen[1][j] >> 16;
            poly->x3 = rec->screen[1][j + 1];
            poly->y3 = rec->screen[1][j + 1] >> 16;
            poly->r0 = rec->ca[j].r; poly->g0 = rec->ca[j].g; poly->b0 = rec->ca[j].b;
            poly->r1 = rec->ca[j + 1].r; poly->g1 = rec->ca[j + 1].g; poly->b1 = rec->ca[j + 1].b;
            poly->r2 = rec->cb[j].r; poly->g2 = rec->cb[j].g; poly->b2 = rec->cb[j].b;
            poly->r3 = rec->cb[j + 1].r; poly->g3 = rec->cb[j + 1].g; poly->b3 = rec->cb[j + 1].b;
            if (rec->otz[j] < 0) {
                rec->otz[j] = 0;
            }
            flags[j3][j] = 0;
            if (rec->otz[j] >= 0) {
                GsSortPoly(poly, ot, (u16)rec->otz[j]);
            }
            /* Column j of the band's word arrays (screen rows, ca, cb, otz) as
             * a separate copy of rec + j * 4: retail moves the address into a
             * new register here, which the member forms do not reproduce. */
            column = (u8 *)rec + (((u8 *)rec + j * 4) - (u8 *)rec);
            poly->x0 = *(u16 *)((column + 0x120));
            yv = *(s16 *)((column + 0x120) + 2);
            poly->y0 = yv;
            poly->x1 = VARIANT401_BANDS_NEXT_X(2, 0x124);
            yv = VARIANT401_BANDS_NEXT_Y(2, 0x124);
            poly->y1 = yv;
            poly->x2 = *(u16 *)((column + 0xFC));
            yv = *(s16 *)((column + 0xFC) + 2);
            poly->y2 = yv;
            poly->x3 = VARIANT401_BANDS_NEXT_X(1, 0x100);
            yv = VARIANT401_BANDS_NEXT_Y(1, 0x100);
            poly->y3 = yv;
            poly->r0 = column[0x1F8]; poly->g0 = column[0x1F9]; poly->b0 = column[0x1FA];
            poly->r1 = rec->ca[j + 1].r; poly->g1 = rec->ca[j + 1].g; poly->b1 = rec->ca[j + 1].b;
            poly->r2 = column[0x21C]; poly->g2 = column[0x21D]; poly->b2 = column[0x21E];
            poly->r3 = rec->cb[j + 1].r; poly->g3 = rec->cb[j + 1].g; poly->b3 = rec->cb[j + 1].b;
            if (*(s32 *)(column + 0x258) < 0) {
                *(s32 *)(column + 0x258) = 0;
            }
            flags[j3][j] = 0;
            if (rec->otz[j] >= 0) {
                GsSortPoly(poly, ot, (u16)rec->otz[j]);
            }
        }
    }
    if (work->tail.phase == 2 && work->scale > 0) {
        t = work->config->shrink_start;
        if (work->clock.frame >= (u32)t) {
            work->scale = 0x1000 - ((work->clock.frame - t) << 12) /
                                               (work->config->shrink_end - t);
            if (work->scale <= 0) {
                work->scale = 0;
                work->tail.phase = 3;
            }
        }
    }
}
