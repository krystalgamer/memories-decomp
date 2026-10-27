#include "../../types.h"
#include "model_variant.h"
#include "variant401_entry.h"

/* The French build adds the record base first in the first growth read and
 * reads the second strip's next column from the record instead of the
 * column-shifted q2 view. */
#if defined(VERSION_FRENCH)
#define VARIANT401_BANDS_COLUMN(rec, off4) ((s32)(rec) + (off4))
#define VARIANT401_BANDS_NEXT_X(row, ofs) rec->screen[row][j + 1]
#define VARIANT401_BANDS_NEXT_Y(row, ofs) (rec->screen[row][j + 1] >> 16)
#else
#define VARIANT401_BANDS_COLUMN(rec, off4) ((off4) + (s32)(rec))
#define VARIANT401_BANDS_NEXT_X(row, ofs) *(u16 *)((q2 + (ofs)))
#define VARIANT401_BANDS_NEXT_Y(row, ofs) *(s16 *)((q2 + (ofs)) + 2)
#endif

void func_8013BFE4(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[1][9];
    GsOT *ot;
    Variant401EntryBand *rec;
    s16 j3, j4;
    u8 *work;
    POLY_GT4 *poly;
    s16 j, q5, q6;
    u16 i;
    s32 yaw;
    PSXLONG p;
    u16 i2;
    s32 a;
    s32 radius;
    s32 t, tc, tn, tz, tr;
    s32 yv;
    s32 off4, off8, lo, lo2, yv2, yv3, yv4;
    u8 *p0, *q1, *q2;

    work = ctx;
    poly = (POLY_GT4 *)(work + 0x197C);
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_HALF(work, 0x1AC6), MODEL_VARIANT_HALF(work, 0x1AC4)) + 0x800;
    if (yaw != 0) {
        lo = 0;
    }
    tr = MODEL_VARIANT_HALF(work, 0x1B0C) * 21;
    if (tr < 0) {
        tr += 0xFFF;
    }
    tr = (u32)tr >> 12;
    radius = tr;
    if (tr != 0) {
        lo = 0;
    }
    rec = (Variant401EntryBand *)(work + 0xF60);
    for (j3 = 0; (s16)j3 < 1; j3++, rec++) {
        for (j = 0; j < 9; j++) {
            off4 = j * 4;
            tc = *(s32 *)(((u8 *)VARIANT401_BANDS_COLUMN(rec, off4)) + 0x1D4);
            a = 0;
            if (tc > 0) {
                if (rec) {
                    a = tc;
                } else {
                    a = tc;
                }
            }
            off8 = j * 8;
            p0 = (u8 *)(off8 + (s32)rec);
            *(s16 *)p0 = rcos(0x400) * (s16)radius >> 12;
            lo = rsin(0x400) * (s16)radius;
            *(s16 *)(p0 + 4) = 0;
            *(s16 *)(p0 + 0x48) = 0;
            *(s16 *)(p0 + 2) = lo >> 12;
            q1 = p0 + 0x48;
            *(s16 *)(q1 + 2) = 0;
            *(s16 *)(q1 + 4) = 0;
            *(s16 *)(p0 + 0x90) = rcos(0xC00) * (s16)radius >> 12;
            lo2 = rsin(0xC00) * (s16)radius;
            q2 = p0 + 0x90;
            *(s16 *)(q2 + 4) = 0;
            *(s16 *)(q2 + 2) = lo2 >> 12;
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = yaw;
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1AA0) + MODEL_VARIANT_WORD(work, 0x1AB4) * a / 1024;
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1AA4) + MODEL_VARIANT_WORD(work, 0x1AB8) * a / 1024;
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1AA8) + MODEL_VARIANT_WORD(work, 0x1ABC) * a / 1024;
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
            *(s32 *)(((u8 *)(off4 + (s32)rec)) + 0x258) = RotTransPers3((SVECTOR *)p0, (SVECTOR *)((u8 *)rec + (off8 + 0x48)),
                                                   (SVECTOR *)((u8 *)rec + (off8 + 0x90)),
                                                   (PSXLONG *)((u8 *)rec + (off4 + 0xD8)),
                                                   (PSXLONG *)((u8 *)rec + (off4 + 0xFC)),
                                                   (PSXLONG *)((u8 *)rec + (off4 + 0x120)),
                                                   &p, &flags[j3][j]);
            if (MODEL_VARIANT_WORD(work, 0x1B18) < 3 && *(s32 *)(((u8 *)(off4 + (s32)rec)) + 0x1D4) < 0x400) {
                if (j == 0) {
                    t = MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1AF4), 0x20);
                    rec->offset[0] = (u32)((MODEL_VARIANT_WORD(work, 0x1AE4) - t) << 10) /
                                     (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1AF4), 0x24) - t);
                } else {
                    *(s32 *)((u8 *)rec + off4 + 0x1D4) += MODEL_VARIANT_WORD(work, 0x1AEC) << 6;
                }
                if (rec->offset[j] >= 0x400) {
                    rec->offset[j] = 0x400;
                    if (j == 0) {
                        MODEL_VARIANT_WORD(work, 0x1B18) = 2;
                    }
                }
            }
            if (j != 0) {
                lo = 0;
            }
        }
    }
    rec = (Variant401EntryBand *)(work + 0xF60);
    for (j3 = 0; (s16)j3 < 1; j3++, rec++) {
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
            q2 = (u8 *)rec + (((u8 *)rec + j * 4) - (u8 *)rec);
            poly->x0 = *(u16 *)((q2 + 0x120));
            yv = *(s16 *)((q2 + 0x120) + 2);
            poly->y0 = yv;
            poly->x1 = VARIANT401_BANDS_NEXT_X(2, 0x124);
            yv = VARIANT401_BANDS_NEXT_Y(2, 0x124);
            poly->y1 = yv;
            poly->x2 = *(u16 *)((q2 + 0xFC));
            yv = *(s16 *)((q2 + 0xFC) + 2);
            poly->y2 = yv;
            poly->x3 = VARIANT401_BANDS_NEXT_X(1, 0x100);
            yv = VARIANT401_BANDS_NEXT_Y(1, 0x100);
            poly->y3 = yv;
            poly->r0 = q2[0x1F8]; poly->g0 = q2[0x1F9]; poly->b0 = q2[0x1FA];
            poly->r1 = rec->ca[j + 1].r; poly->g1 = rec->ca[j + 1].g; poly->b1 = rec->ca[j + 1].b;
            poly->r2 = q2[0x21C]; poly->g2 = q2[0x21D]; poly->b2 = q2[0x21E];
            poly->r3 = rec->cb[j + 1].r; poly->g3 = rec->cb[j + 1].g; poly->b3 = rec->cb[j + 1].b;
            if (*(s32 *)(q2 + 0x258) < 0) {
                *(s32 *)(q2 + 0x258) = 0;
            }
            flags[j3][j] = 0;
            if (rec->otz[j] >= 0) {
                GsSortPoly(poly, ot, (u16)rec->otz[j]);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x1B18) == 2 && MODEL_VARIANT_HALF(work, 0x1B0C) > 0) {
        t = MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1AF4), 0x28);
        if ((u32)MODEL_VARIANT_WORD(work, 0x1AE4) >= (u32)t) {
            MODEL_VARIANT_HALF(work, 0x1B0C) = 0x1000 - (u32)((MODEL_VARIANT_WORD(work, 0x1AE4) - t) << 12) /
                                               (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1AF4), 0x2C) - t);
            if (MODEL_VARIANT_HALF(work, 0x1B0C) <= 0) {
                MODEL_VARIANT_HALF(work, 0x1B0C) = 0;
                MODEL_VARIANT_WORD(work, 0x1B18) = 3;
            }
        }
    }
}
