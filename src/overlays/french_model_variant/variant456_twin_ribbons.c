#include "../../types.h"
#include "variant456_twin_ribbons.h"

void func_8013C01C(u8 *ctx)
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
    s16 j;
    s32 turn;
    s32 wave;
    s32 base;
    s32 size;
    s16 length;
    s32 phase;
    s32 width;
    s32 amp;
    s32 dx;
    s32 dy;
    s32 c;
    s32 a;
    TwinRibbon456 *ribbon;
    POLY_FT4 *poly;

    work = ctx;
    ribbon = (TwinRibbon456 *)(work + 0xA0);
    ot = func_80058F10();
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x230C), MODEL_VARIANT_WORD(work, 0x2304)) + 0xC00;
    ratan2(MODEL_VARIANT_WORD(work, 0x2308), MODEL_VARIANT_WORD(work, 0x2304));
    length = 48;
    for (i = 0; i < 2; i++, ribbon++, base = i * 0xC00) {
        if (i == 1) {
            dy = 0;
        }
        if (i == length) {
            dy = 0;
        }
        wave = -MODEL_VARIANT_WORD(work, 0x2358);
        for (j = 0; j < 3; j++, wave += 1300) {
            a = ribbon->amp[j];
            if (a <= 0) {
                c = 0;
            } else if (a < 1024) {
                dx = 0;
                c = a;
            } else {
                c = 1024;
            }
            setVector(&ribbon->a[j], ribbon->dir[0] * c / 1024, ribbon->dir[1] * c / 1024, ribbon->dir[2] * c / 1024);
            setVector(&ribbon->b[j], ribbon->a[j].vx + (rcos(turn) * length >> 12), ribbon->a[j].vy,
                      ribbon->a[j].vz + (rsin(turn) * length >> 12));
            if (ribbon->amp[j] <= 1024) {
                ribbon->amp[j] += MODEL_VARIANT_WORD(work, 0x2328) * 64;
                if (ribbon->amp[j] >= 1024) {
                    ribbon->amp[j] = 1024;
                    if (i == 0 && MODEL_VARIANT_WORD(work, 0x236C) == 1) {
                        MODEL_VARIANT_WORD(work, 0x236C) = 2;
                    }
                    if (j == 2) {
                        ribbon->state = 1;
                    }
                }
            }
        }
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x22DC);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x22E0);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x22E4);
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
    ribbon = (TwinRibbon456 *)(work + 0xA0);
    for (i = 0; i < 2; i++, ribbon++) {
        for (j = 0; j < 3; j++) {
            if (j == 2) {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[1], &ribbon->a[2], &ribbon->a[1], &ribbon->a[2],
                                               &ribbon->sa[1].packed, &ribbon->sa[2].packed, &ribbon->sa[1].packed,
                                               &ribbon->sa[2].packed, &p, &flag);
                RotTransPers(&ribbon->b[2], &ribbon->sb[2].packed, &p, &flag);
                dx = ribbon->sa[j].point.vx - ribbon->sa[j - 1].point.vx;
                dy = ribbon->sa[j].point.vy - ribbon->sa[j - 1].point.vy;
            } else {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[j], &ribbon->a[j + 1], &ribbon->a[j], &ribbon->a[j + 1],
                                               &ribbon->sa[j].packed, &ribbon->sa[j + 1].packed, &ribbon->sa[j].packed,
                                               &ribbon->sa[j + 1].packed, &p, &flag);
                RotTransPers(&ribbon->b[j], &ribbon->sb[j].packed, &p, &flag);
                dx = ribbon->sa[j + 1].point.vx - ribbon->sa[j].point.vx;
                dy = ribbon->sa[j + 1].point.vy - ribbon->sa[j].point.vy;
            }
            ribbon->angle[j] = ratan2(dy, dx) - 0x400;
            ribbon->width[j] = ribbon->sb[j].point.vx - ribbon->sa[j].point.vx;
            size = ribbon->scale * ribbon->width[j] / 1024;
            ribbon->ox[j] = rcos(ribbon->angle[j]) * size >> 12;
            ribbon->oy[j] = rsin(ribbon->angle[j]) * size >> 12;
            if (j == 0 || j == 2) {
                phase = 0;
            } else {
                phase = base - j * 0xC00 + MODEL_VARIANT_WORD(work, 0x235C);
            }
            width = ribbon->width[j];
            amp = (width << 7) / 48;
            ribbon->sa[j].packed += rcos(ribbon->angle[j]) * ((rsin(phase) * amp >> 12) + (rsin(wave) * width >> 12)) >> 12;
            ribbon->sa[j].packed += (rsin(ribbon->angle[j]) * ((rsin(phase) * amp >> 12) + (rsin(wave) * width >> 12)) >> 12) << 16;
        }
    }
    ribbon = (TwinRibbon456 *)(work + 0xA0);
    for (i = 0; i < 2; i++, ribbon++) {
        poly = (POLY_FT4 *)(work + 0x2204);
        for (j = 0; j < 2; j++) {
            poly->x0 = ribbon->sa[j].point.vx + ribbon->ox[j];
            poly->y0 = (ribbon->sa[j].packed >> 16) + ribbon->oy[j];
            poly->x1 = ribbon->sa[j + 1].point.vx + ribbon->ox[j + 1];
            poly->y1 = (ribbon->sa[j + 1].packed >> 16) + ribbon->oy[j + 1];
            poly->x2 = ribbon->sa[j].point.vx - ribbon->ox[j];
            poly->y2 = (ribbon->sa[j].packed >> 16) - ribbon->oy[j];
            poly->x3 = ribbon->sa[j + 1].point.vx - ribbon->ox[j + 1];
            poly->y3 = (ribbon->sa[j + 1].packed >> 16) - ribbon->oy[j + 1];
            poly->r0 = ribbon->color[0];
            poly->g0 = ribbon->color[1];
            poly->b0 = ribbon->color[2];
            if (ribbon->otz[j] > 0 && ribbon->state == 0) {
                GsSortPoly(poly, ot, ribbon->otz[j]);
            }
            if (!(j & 1)) {
                poly++;
            } else {
                poly--;
            }
        }
    }
}
