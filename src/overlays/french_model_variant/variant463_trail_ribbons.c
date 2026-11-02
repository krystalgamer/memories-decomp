#include "../../types.h"
#include "variant463_ribbons.h"

/* The North American header-446 build reads the projected points through
 * pointer arithmetic; the default spelling is the French/Spanish one. */
#ifdef MODEL_VARIANT446_TRAIL_RIBBONS_POINTER_SCREEN
#define TRAIL463_SA(k) (*(ribbon->sa + (k)))
#define TRAIL463_SB(k) (*(ribbon->sb + (k)))
#else
#define TRAIL463_SA(k) (ribbon->sa[k])
#define TRAIL463_SB(k) (ribbon->sb[k])
#endif

void FrenchModelVariant463_DrawTrailRibbons(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[5][13];
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    GsOT *ot;
    s16 i;
    s16 j;
    s32 wave;
    s32 base;
    s32 total;
    s32 turn;
    s16 length;
    s32 phase;
    s32 size;
    s32 amp;
    s32 sway;
    s32 dx;
    s32 dy;
    s32 n;
    Ribbon463 *ribbon;
    POLY_FT4 *poly;

    work = ctx;
    ribbon = ((TrailRibbon463Work *)work)->ribbons;
    ot = func_80058F10();
    total = 0;
    i = 0;
    base = 0;
    length = 96;
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x4A40), MODEL_VARIANT_WORD(work, 0x4A38)) + 0xC00;
    ratan2(MODEL_VARIANT_WORD(work, 0x4A3C), MODEL_VARIANT_WORD(work, 0x4A38));
    for (; i < 5; ribbon++) {
        if (i < 0) {
            dy = 0;
        }
        for (j = 0; j < 13; j++) {
            if (j < 0) {
                if (j == -1) {
                    dy = 0;
                }
            }
            setVector(&ribbon->a[j], ((TrailRibbon463Work *)work)->dirs[i].vx * j / 12,
                      ((TrailRibbon463Work *)work)->dirs[i].vy * j / 12, ((TrailRibbon463Work *)work)->dirs[i].vz * j / 12);
            setVector(&ribbon->b[j], ribbon->a[j].vx + (rcos(turn) * length >> 12), ribbon->a[j].vy,
                      ribbon->a[j].vz + (rsin(turn) * length >> 12));
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = ((TrailRibbon463Work *)work)->positions[i].vx;
        m.t[1] = ((TrailRibbon463Work *)work)->positions[i].vy;
        m.t[2] = ((TrailRibbon463Work *)work)->positions[i].vz;
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        wave = MODEL_VARIANT_WORD(work, 0x4A94);
        for (j = 0; j < 13; j++, wave += 1300) {
            if (j == 12) {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[11], &ribbon->a[12], &ribbon->a[11], &ribbon->a[12],
                                               &ribbon->sa[11].packed, &ribbon->sa[12].packed,
                                               &ribbon->sa[11].packed, &ribbon->sa[12].packed, &p,
                                               &flags[i][j]);
                RotTransPers(&ribbon->b[12], &ribbon->sb[12].packed, &p, &flag);
                dx = TRAIL463_SA(12).point.vx - TRAIL463_SA(11).point.vx;
                dy = TRAIL463_SA(12).point.vy - TRAIL463_SA(11).point.vy;
            } else {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[j], &ribbon->a[j + 1], &ribbon->a[j], &ribbon->a[j + 1],
                                               &ribbon->sa[j].packed, &ribbon->sa[j + 1].packed,
                                               &ribbon->sa[j].packed, &ribbon->sa[j + 1].packed, &p,
                                               &flags[i][j]);
                RotTransPers(&ribbon->b[j], &ribbon->sb[j].packed, &p, &flag);
                dx = TRAIL463_SA(j + 1).point.vx - TRAIL463_SA(j).point.vx;
                dy = TRAIL463_SA(j + 1).point.vy - TRAIL463_SA(j).point.vy;
            }
            ribbon->angle[j] = ratan2(dy, dx) - 0x400;
            ribbon->width[j] = TRAIL463_SB(j).point.vx - TRAIL463_SA(j).point.vx;
            size = ribbon->scale * ribbon->width[j] / 1024;
            ribbon->ox[j] = rcos(ribbon->angle[j]) * size >> 12;
            ribbon->oy[j] = rsin(ribbon->angle[j]) * size >> 12;
            if (j == 0 || j == 12) {
                phase = 0;
            } else {
                phase = base - j * 512;
            }
            amp = ribbon->width[j] * 64 / 48;
            sway = ribbon->width[j] * 32 / 48;
            TRAIL463_SA(j).packed = (u32)TRAIL463_SA(j).packed + (u32)(rcos(ribbon->angle[j]) * ((rsin(phase) * amp >> 12) + (rsin(wave) * sway >> 12)) >> 12);
            TRAIL463_SA(j).packed = (u32)TRAIL463_SA(j).packed + ((u32)(rsin(ribbon->angle[j]) * ((rsin(phase) * amp >> 12) + (rsin(wave) * sway >> 12)) >> 12) << 16);
        }
        if (ribbon->count < 0) {
            n = 0;
        } else {
            n = ribbon->count;
        }
        poly = ((TrailRibbon463Work *)work)->polygons;
        for (j = 0; j < n; j++) {
            if ((u32)MODEL_VARIANT_WORD(work, 0x4A50) % 2 == 1) {
                if (!(j & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
            if (j + 1 == n) {
                poly->x0 = TRAIL463_SA(j).point.vx + ribbon->ox[j];
                poly->y0 = (TRAIL463_SA(j).packed >> 16) + ribbon->oy[j];
                poly->x1 = TRAIL463_SA(j + 1).point.vx;
                poly->y1 = TRAIL463_SA(j + 1).packed >> 16;
                poly->x2 = TRAIL463_SA(j).point.vx - ribbon->ox[j];
                poly->y2 = (TRAIL463_SA(j).packed >> 16) - ribbon->oy[j];
                poly->x3 = TRAIL463_SA(j + 1).point.vx;
                poly->y3 = TRAIL463_SA(j + 1).packed >> 16;
            } else {
                poly->x0 = TRAIL463_SA(j).point.vx + ribbon->ox[j];
                poly->y0 = (TRAIL463_SA(j).packed >> 16) + ribbon->oy[j];
                poly->x1 = TRAIL463_SA(j + 1).point.vx + ribbon->ox[j + 1];
                poly->y1 = (TRAIL463_SA(j + 1).packed >> 16) + ribbon->oy[j + 1];
                poly->x2 = TRAIL463_SA(j).point.vx - ribbon->ox[j];
                poly->y2 = (TRAIL463_SA(j).packed >> 16) - ribbon->oy[j];
                poly->x3 = TRAIL463_SA(j + 1).point.vx - ribbon->ox[j + 1];
                poly->y3 = (TRAIL463_SA(j + 1).packed >> 16) - ribbon->oy[j + 1];
            }
            poly->r0 = ribbon->color[0];
            poly->g0 = ribbon->color[1];
            poly->b0 = ribbon->color[2];
            if (ribbon->otz[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[j]);
            }
            if (!(MODEL_VARIANT_WORD(work, 0x4A50) & 1)) {
                if (!(j & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
        }
        if (ribbon->count < 12 && ribbon->state == 0) {
            ribbon->count += MODEL_VARIANT_WORD(work, 0x4A5C) * 2;
            if (ribbon->count >= 12) {
                ribbon->count = 12;
                ribbon->state = 1;
                if (MODEL_VARIANT_WORD(work, 0x4AB0) == 1) {
                    MODEL_VARIANT_WORD(work, 0x4AB0) = 2;
                }
            }
        } else if (ribbon->state == 1 && ribbon->scale > 0) {
            ribbon->scale -= MODEL_VARIANT_WORD(work, 0x4A5C) * 64;
            if (ribbon->scale <= 0) {
                if (MODEL_VARIANT_WORD(work, 0x4A8C) == 0) {
                    ribbon->scale = 0;
                    ribbon->count = 0;
                    ribbon->state = 2;
                } else {
                    ribbon->scale = 1024;
                    ribbon->count = -50;
                    ribbon->state = 0;
                }
            }
        }
        if (MODEL_VARIANT_WORD(work, 0x4AB0) >= 4 && MODEL_VARIANT_WORD(work, 0x4A8C) > 0) {
            MODEL_VARIANT_WORD(work, 0x4A8C) -= (u32)(MODEL_VARIANT_WORD(work, 0x4A5C) * 3) >> 1;
            if (MODEL_VARIANT_WORD(work, 0x4A8C) <= 0) {
                MODEL_VARIANT_WORD(work, 0x4A8C) = 0;
            }
        }
        total += ribbon->state;
        if (total == 10 && MODEL_VARIANT_WORD(work, 0x4AB0) == 6) {
            MODEL_VARIANT_WORD(work, 0x4AB0) = 7;
        }
        i++;
        base = i * 0x1800 / 5;
    }
    MODEL_VARIANT_WORD(work, 0x4A94) += MODEL_VARIANT_WORD(work, 0x4A5C) * 128;
    /* A zero step: the PAL build drops it, the USA build keeps its load. */
    MODEL_VARIANT_WORD(work, 0x4A98) += 0;
}
