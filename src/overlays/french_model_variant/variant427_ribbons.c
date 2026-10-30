#include "../../types.h"
#include "variant427_ribbons.h"

/* The North American header-410 build reads the projected points through
 * pointer arithmetic so CDK GCC folds sa[k + 1] into a fixed offset from
 * sa[k]; the default spelling is the French/Spanish one. */
#ifdef MODEL_VARIANT410_RIBBONS_POINTER_SCREEN
#define RIBBONS427_SA(k, n) (*(ribbon->sa + (k) + (n)))
#define RIBBONS427_SB(k) (*(ribbon->sb + (k)))
#else
#define RIBBONS427_SA(k, n) (ribbon->sa[(k) + (n)])
#define RIBBONS427_SB(k) (ribbon->sb[k])
#endif

void func_8013CFC4(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    GsOT *ot;
    s16 i;
    s16 k;
    s32 yaw;
    s32 pitch;
    s16 len;
    Ribbon379 *ribbon;
    POLY_GT4 *poly;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 sy;
    Ribbon379Timing *timing;
    u8 *v;
    s32 n;
    s32 kk;
    s32 py, pz;

    work = ctx;
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x372C), MODEL_VARIANT_WORD(work, 0x3724)) + 0xC00;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x3728), MODEL_VARIANT_WORD(work, 0x3724)) + 0xC00;
    poly = (POLY_GT4 *)(work + 0x3560);
    if (!(MODEL_VARIANT_WORD(work, 0x373C) & 1)) {
        len = MODEL_VARIANT_WORD(work, 0x3768) / 16;
    } else {
        len = MODEL_VARIANT_WORD(work, 0x3768) * 72 / 1024;
    }
    ribbon = (Ribbon379 *)(work + 0xED0);
    for (i = 0; i < 3; i++, ribbon++) {
        if (ribbon->amp > 0) {
            if (i < 0) {
                dy = 0;
            }
            for (k = 0, v = work + i * 16; k < 17; k++) {
                dy = MODEL_VARIANT_WORD(v, 0x36F4) * ribbon->amp / 1024;
                dz = MODEL_VARIANT_WORD(v, 0x36F8) * ribbon->amp / 1024;
                n = kk = k;
                setVector(&ribbon->a[n], MODEL_VARIANT_WORD(v, 0x36F0) * ribbon->amp / 1024 * k / 16, dy * (kk = n) / 16, dz * k / 16);
                setVector(&ribbon->b[k],
                          ribbon->a[k].vx + (rcos(yaw) * len >> 12),
                          ribbon->a[k].vy,
                          ribbon->a[k].vz + (rsin(yaw) * len >> 12));
            }
        }
        setVector(&rot, 0, 0, 0);
        m.t[0] = ((Ribbon379Work *)work)->anchor[i % 3].position.vx;
        m.t[1] = ((Ribbon379Work *)work)->anchor[i % 3].position.vy;
        m.t[2] = ((Ribbon379Work *)work)->anchor[i % 3].position.vz;
        setVector(&scale, 4096, 4096, 4096);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        if (ribbon->amp > 0) {
            for (k = 0; k < 17; k++) {
                if (k == 16) {
                    ribbon->otz[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k], &ribbon->a[k - 1], &ribbon->a[k],
                                                  &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed,
                                                  &p, &ribbon->flags[k]);
                    RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                    dx = RIBBONS427_SA(k, 0).point.vx - RIBBONS427_SA(k, -1).point.vx;
                    sy = (RIBBONS427_SA(k, 0).packed >> 16) - (RIBBONS427_SA(k, -1).packed >> 16);
                    ribbon->angle[k] = ratan2(sy, dx) - 0x400;
                    ribbon->width[k] = RIBBONS427_SB(k).point.vx - RIBBONS427_SA(k, 0).point.vx;
                    ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                } else {
                    ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                                  &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                                  &p, &ribbon->flags[k]);
                    RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                    dx = RIBBONS427_SA(k, 1).point.vx - RIBBONS427_SA(k, 0).point.vx;
                    sy = RIBBONS427_SA(k, 1).point.vy - RIBBONS427_SA(k, 0).point.vy;
                    ribbon->angle[k] = ratan2(sy, dx) - 0x400;
                    ribbon->width[k] = RIBBONS427_SB(k).point.vx - RIBBONS427_SA(k, 0).point.vx;
                    ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                }
            }
        }
    }
    ribbon = (Ribbon379 *)(work + 0xED0);
    for (i = 0; i < 3; i++, ribbon++) {
        if (ribbon->amp > 0) {
            for (k = 0; k < 16; k++) {
                setRGB0(poly, 0, 192, 0);
                setRGB1(poly, 0, 192, 0);
                setRGB2(poly, 192, 192, 192);
                setRGB3(poly, 192, 192, 192);
                poly->x0 = RIBBONS427_SA(k, 0).point.vx + ribbon->ox[k];
                poly->y0 = (RIBBONS427_SA(k, 0).packed >> 16) + ribbon->oy[k];
                poly->x1 = RIBBONS427_SA(k, 1).point.vx + ribbon->ox[k + 1];
                poly->y1 = (RIBBONS427_SA(k, 1).packed >> 16) + ribbon->oy[k + 1];
                poly->x2 = RIBBONS427_SA(k, 0).point.vx;
                poly->y2 = (RIBBONS427_SA(k, 0).packed >> 16);
                poly->x3 = RIBBONS427_SA(k, 1).point.vx;
                poly->y3 = (RIBBONS427_SA(k, 1).packed >> 16);
                if (ribbon->otz[k] >= 0 && ribbon->flags[k] >= 0) {
                    GsSortPoly(poly, ot, (u16)ribbon->otz[k]);
                }
                poly->x0 = RIBBONS427_SA(k, 0).point.vx - ribbon->ox[k];
                poly->y0 = (RIBBONS427_SA(k, 0).packed >> 16) - ribbon->oy[k];
                poly->x1 = RIBBONS427_SA(k, 1).point.vx - ribbon->ox[k + 1];
                poly->y1 = (RIBBONS427_SA(k, 1).packed >> 16) - ribbon->oy[k + 1];
                poly->x2 = RIBBONS427_SA(k, 0).point.vx;
                poly->y2 = (RIBBONS427_SA(k, 0).packed >> 16);
                poly->x3 = RIBBONS427_SA(k, 1).point.vx;
                poly->y3 = (RIBBONS427_SA(k, 1).packed >> 16);
                if (ribbon->otz[k] >= 0 && ribbon->flags[k] >= 0) {
                    GsSortPoly(poly, ot, (u16)ribbon->otz[k]);
                }
            }
        }
        if (ribbon->amp < 0x400) {
            ribbon->amp += MODEL_VARIANT_WORD(work, 0x3748) * 64;
            if (ribbon->amp >= 0x400) {
                ribbon->amp = 0x400;
                if (MODEL_VARIANT_WORD(work, 0x37C4) == 2) {
                    MODEL_VARIANT_WORD(work, 0x37C4) = 3;
                }
            }
        }
    }
    ((Ribbon379Work *)work)->phase += ((Ribbon379Work *)work)->step * 850;
    ((Ribbon379Work *)work)->wave += ((Ribbon379Work *)work)->step * 128;
    timing = ((Ribbon379Work *)work)->timing;
    if (timing->fade_start <= ((Ribbon379Work *)work)->clock && ((Ribbon379Work *)work)->fade > 0) {
        ((Ribbon379Work *)work)->fade = 0x400 - ((((Ribbon379Work *)work)->clock - timing->fade_start) << 10) / (timing->fade_end - timing->fade_start);
        if (((Ribbon379Work *)work)->fade <= 0) {
            ((Ribbon379Work *)work)->fade = 0;
            if (((Ribbon379Work *)work)->state == 4) {
                ((Ribbon379Work *)work)->state = 5;
            }
        }
    }
}
