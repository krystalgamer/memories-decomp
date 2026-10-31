#include "../../types.h"
#include "variant366_entry.h"
#include "../../game/graphics_packet_submission.h"

/* Draws the three ribbon cones above the target. Each cone is sixteen
 * POLY_GT4 strips textured from the frame-buffer half the strip lands on,
 * with a POLY_F4 mask-write packet behind each one. Before phase 7 each cone
 * breathes with its progress; afterwards it opens by field_1558[k] towards
 * the opponent. Progress then advances and wraps with a count. */
void func_8013CEC8(u8 *context)
{
    SVECTOR rotation;
    /* Retail leaves 16-byte gaps after rotation and coordinate. */
    u8 unknown_30[16];
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    u8 unknown_D0[16];
    CVECTOR outer;
    CVECTOR inner;
    PSXLONG interpolation;
    PSXLONG flag;
    Variant366State *work = (Variant366State *)context;
    Variant366Ribbon *ribbon;
    GsOT *ot;
    s32 k;
    s32 sign;
    s32 buff;
    s32 inner_r, outer_r, middle_r, height;
    POLY_GT4 *plain;
    POLY_F4 *mask;
    s32 i, angle;
    s32 spread;
    s32 depth;
    /* Word-sized so the u16 GetTPage result is zero-extended. */
    u32 tpage;
    s32 blue;
    s32 white;
    s32 top;

    ribbon = work->ribbons;
    ot = func_80058F10();
    buff = GsGetActiveBuff();
    ratan2(work->direction.vz, work->direction.vx);
    sign = -1;
    ratan2(work->direction.vy, work->direction.vz);
    plain = &work->plain;
    mask = &work->mask;
    if (work->slot == 1) {
        sign = 1;
    }
    /* Constants assigned to fresh function-level locals: retail keeps
     * them out of the loop body, so reload rematerialises them in t2. */
    white = 255;
    top = 48;
    for (k = 0; k < 3; k++, ribbon++) {
        if (ribbon->progress > 0) {
            if (work->phase < 7) {
                for (i = 0, angle = 0; i < 17; i++, angle = i << 8) {
                    setVector(&ribbon->inner[i], rcos(angle) * (rsin(ribbon->progress) * 320 >> 12) >> 12,
                              0,
                              rsin(angle) * (rsin(ribbon->progress) * 320 >> 12) >> 12);
                    setVector(&ribbon->middle[i], rcos(angle) * (rsin(ribbon->progress) * 416 >> 12) >> 12,
                              (u32)(rsin(ribbon->progress) * 3) >> 8,
                              rsin(angle) * (rsin(ribbon->progress) * 416 >> 12) >> 12);
                    setVector(&ribbon->outer[i], rcos(angle) * (rsin(ribbon->progress) * 512 >> 12) >> 12,
                              (u32)(rsin(ribbon->progress) * 3) >> 8,
                              rsin(angle) * (rsin(ribbon->progress) * 512 >> 12) >> 12);
                }
            } else {
                /* The division is repeated in each radius and shared by
                 * CSE. A named quotient changes its registers and lets
                 * reload reuse inner_r from t2 instead of reloading it. */
                spread = (work->direction.vz * sign - 320) * work->field_1558[k];
                inner_r = spread / 1024 + 320;
                {
                    /* Block-scoped temporaries; a direct middle_r formula
                     * folds (mid - inner_r) to 96. */
                    s32 mid = spread / 1024 + 416;
                    s32 d, q;
                    outer_r = spread / 1024 + 512;
                    d = (mid - inner_r) * work->field_1558[k] / 1024;
                    q = work->field_1558[k] * 48 / 1024;
                    /* The loop starts before the last two radii, which
                     * retail schedules after the counter. */
                    i = 0;
                    angle = 0;
                    middle_r = mid - d;
                    height = top - q;
                }
                for (; i < 17; i++, angle = i << 8) {
                    setVector(&ribbon->inner[i], rcos(angle) * (rsin(ribbon->progress) * inner_r >> 12) >> 12,
                              0,
                              rsin(angle) * (rsin(ribbon->progress) * inner_r >> 12) >> 12);
                    setVector(&ribbon->middle[i], rcos(angle) * (rsin(ribbon->progress) * middle_r >> 12) >> 12,
                              rsin(ribbon->progress) * height >> 12,
                              rsin(angle) * (rsin(ribbon->progress) * middle_r >> 12) >> 12);
                    setVector(&ribbon->outer[i], rcos(angle) * (rsin(ribbon->progress) * outer_r >> 12) >> 12,
                              (u32)(rsin(ribbon->progress) * 3) >> 8,
                              rsin(angle) * (rsin(ribbon->progress) * outer_r >> 12) >> 12);
                }
            }
            inner.r = white;
            inner.g = white;
            inner.b = white;
            outer.r = 64;
            outer.g = 192;
            outer.b = white;
            setVector(&rotation, 0, 0, 0);
            matrix.t[0] = work->target.vx;
            matrix.t[1] = work->target.vy + (rcos(ribbon->progress) * 320 >> 12);
            matrix.t[2] = work->target.vz;
            RotMatrix(&rotation, &matrix);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            setRGB0(plain, inner.r, inner.g, inner.b);
            setRGB1(plain, inner.r, inner.g, inner.b);
            setRGB2(plain, outer.r, outer.g, outer.b);
            setRGB3(plain, outer.r, outer.g, outer.b);
            /* A word-sized local keeps this 64 from sharing a register
             * with the byte constant stored to outer.r. */
            blue = 64;
            setRGB0(mask, 32, 32, blue);
            for (i = 0; i < 16; i++) {
                RotTransPers4(&ribbon->inner[i], &ribbon->inner[i + 1],
                              &ribbon->outer[i], &ribbon->outer[i + 1],
                              (PSXLONG *)&plain->x0, (PSXLONG *)&plain->x1,
                              (PSXLONG *)&plain->x2, (PSXLONG *)&plain->x3,
                              &interpolation, &flag);
                if (plain->x0 < 160) {
                    if (buff == 0) {
                        tpage = GetTPage(2, 1, 320, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0, 0);
                    }
                    SetPolyGT4(plain);
                    plain->tpage = tpage;
                    setUV4(plain, plain->x0, plain->y0, plain->x1, plain->y1,
                           plain->x2, plain->y2, plain->x3, plain->y3);
                } else {
                    if (buff == 0) {
                        tpage = GetTPage(2, 1, 448, 0);
                    } else {
                        tpage = GetTPage(2, 1, 128, 0);
                    }
                    SetPolyGT4(plain);
                    plain->tpage = tpage;
                    setUV4(plain, plain->x0 - 128, plain->y0, plain->x1 - 128, plain->y1,
                           plain->x2 - 128, plain->y2, plain->x3 - 128, plain->y3);
                }
                depth = RotTransPers4(&ribbon->inner[i], &ribbon->inner[i + 1],
                                      &ribbon->middle[i], &ribbon->middle[i + 1],
                                      (PSXLONG *)&plain->x0, (PSXLONG *)&plain->x1,
                                      (PSXLONG *)&plain->x2, (PSXLONG *)&plain->x3,
                                      &interpolation, &flag);
                setXY4(mask, plain->x0, plain->y0, plain->x1, plain->y1,
                       plain->x2, plain->y2, plain->x3, plain->y3);
                SetSemiTrans(plain, 0);
                SetShadeTex(plain, 0);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(plain, ot, (u16)depth);
                    Graphics_SubmitMaskWritePacket((u32 *)mask, ot, (u16)depth, 1);
                }
            }
        }
        if (work->phase < 7) {
            if (ribbon->progress < 2048) {
                ribbon->progress += work->step * 64;
                if (ribbon->progress >= 2048) {
                    ribbon->progress -= 2048;
                    ribbon->field_19C++;
                }
            }
        } else if (ribbon->progress <= 1024) {
            ribbon->progress += work->step * 48;
            if (ribbon->progress >= 1024) {
                ribbon->progress = 1024;
                if (work->field_1558[k] < 1024) {
                    work->field_1558[k] += work->step * 32;
                    if (work->field_1558[k] >= 1024) {
                        work->field_1558[k] = 1024;
                        if (work->phase == 7) {
                            work->phase = 8;
                        }
                    }
                }
            }
        } else {
            ribbon->progress += work->step * 48;
            if (ribbon->progress >= 2048) {
                ribbon->progress -= 2048;
                ribbon->field_19C++;
            }
        }
    }
}
