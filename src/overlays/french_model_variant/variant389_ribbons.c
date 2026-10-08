#include "../../types.h"
#include "variant389_ribbons.h"

void func_8013BE68(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Ribbons389State *state = (Ribbons389State *)context;
    Ribbon389 *ribbon;
    ModelVariantSheetSet *sheet;
    POLY_GT4 *quad;
    GsOT *ot;
    s16 i, k;
    s32 angle;
    s32 done;
    s32 sign;
    s32 yaw;
    s32 progress;
    s16 length = 64;
    s32 amplitude = 900;
    s32 dx;
    s32 dy;

    ot = func_80058F10();
    done = 0;
    sign = -1;
    yaw = ratan2(state->view_direction[2], state->view_direction[0]) + 3072;
    ratan2(state->view_direction[1], state->view_direction[0]);
    quad = &state->quad;
    if (state->mode == 0) {
        sign = 1;
    }
    ribbon = state->ribbons;
    sheet = &state->sheets[8];
    for (i = 0; i < 8; i++, ribbon++, sheet++) {
        if (i == 0) {
            angle = 1024;
        } else if ((s16)(i % 2) == 1) {
            angle = 1024 + (i + 1) * 225;
        } else {
            angle = 1024 - i * 225;
        }
        for (k = 0; k < 17; k++) {
            progress = ribbon->progress[k];
            if (progress <= 0) {
                progress = 0;
            }
            setVector(&ribbon->a[k],
                      state->directions[i].vx * progress / 1024
                          + (rcos(angle) * (-(rsin(progress * 2) * 512) >> 12) >> 12),
                      state->directions[i].vy * progress / 1024
                          + (rsin(angle) * (-(rsin(progress * 2) * 512) >> 12) >> 12),
                      state->directions[i].vz * progress / 1024
                          + (rsin(progress * 2) * amplitude >> 12) * sign);
            setVector(&ribbon->b[k],
                      ribbon->a[k].vx + (rcos(yaw) * length >> 12),
                      ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(yaw) * length >> 12));
            if (ribbon->progress[0] < 0 && state->phase >= 3) {
                ribbon->done = 1;
                sheet->shown = 1;
            } else if (ribbon->progress[k] < 1024) {
                ribbon->progress[k] += state->step * 32;
                if (ribbon->progress[k] >= 1024) {
                    ribbon->progress[k] = 1024;
                    if (i == 0 && state->phase == 1) {
                        state->phase = 2;
                    }
                    if (k + 1 == 16) {
                        if (state->phase < 3) {
                            for (k = 0; k < 17; k++) {
                                ribbon->progress[k] = -256 - k * 48;
                            }
                            sheet->shown = 0;
                        } else if (state->phase == 3) {
                            ribbon->done = 1;
                            sheet->shown = 0;
                        }
                    }
                }
            }
        }
        done += ribbon->done;
        if (done == 8 && state->phase == 3) {
            state->phase = 4;
        }
        setVector(&rotation, 0, 0, 0);
        matrix.t[0] = state->transforms[i].t[0];
        matrix.t[1] = state->transforms[i].t[1];
        matrix.t[2] = state->transforms[i].t[2];
        setVector(&scale, 4096, 4096, 4096);
        RotMatrix(&rotation, &matrix);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                ribbon->depth[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k],
                                               &ribbon->a[k - 1], &ribbon->a[k],
                                               (PSXLONG *)&ribbon->sa[k - 1], (PSXLONG *)&ribbon->sa[k],
                                               (PSXLONG *)&ribbon->sa[k - 1], (PSXLONG *)&ribbon->sa[k],
                                               &interpolation, &ribbon->flag[k]);
                RotTransPers(&ribbon->b[k], (PSXLONG *)&ribbon->sb[k], &interpolation, &flag);
                dx = (s16)ribbon->sa[k].packed - (s16)ribbon->sa[k - 1].packed;
                dy = (ribbon->sa[k].packed >> 16) - (ribbon->sa[k - 1].packed >> 16);
                ribbon->angle[k] = ratan2(dy, dx) - 1024;
                ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            } else {
                ribbon->depth[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1],
                                               &ribbon->a[k], &ribbon->a[k + 1],
                                               (PSXLONG *)&ribbon->sa[k], (PSXLONG *)&ribbon->sa[k + 1],
                                               (PSXLONG *)&ribbon->sa[k], (PSXLONG *)&ribbon->sa[k + 1],
                                               &interpolation, &ribbon->flag[k]);
                RotTransPers(&ribbon->b[k], (PSXLONG *)&ribbon->sb[k], &interpolation, &flag);
                dx = (s16)ribbon->sa[k + 1].packed - (s16)ribbon->sa[k].packed;
                dy = (ribbon->sa[k + 1].packed >> 16) - (ribbon->sa[k].packed >> 16);
                ribbon->angle[k] = ratan2(dy, dx) - 1024;
                ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            }
        }
    }
    ribbon = state->ribbons;
    for (i = 0; i < 8; i++, ribbon++) {
        for (k = 0; k < 16; k++) {
            if (k == 0) {
                quad->u0 = 96;
                quad->v0 = 255;
                quad->u1 = 96;
                quad->v1 = 192;
                quad->u2 = 127;
                quad->v2 = 255;
                quad->u3 = 127;
                quad->v3 = 192;
            } else if (k + 1 == 16) {
                quad->u0 = 96;
                quad->v0 = 191;
                quad->u1 = 96;
                quad->v1 = 128;
                quad->u2 = 127;
                quad->v2 = 191;
                quad->u3 = 127;
                quad->v3 = 128;
            } else {
                quad->u0 = 96;
                quad->v0 = 192;
                quad->u1 = 96;
                quad->v1 = 192;
                quad->u2 = 127;
                quad->v2 = 192;
                quad->u3 = 127;
                quad->v3 = 192;
            }
            quad->x0 = ribbon->sa[k].point.vx + ribbon->ox[k];
            quad->y0 = (ribbon->sa[k].packed >> 16) + ribbon->oy[k];
            quad->x1 = ribbon->sa[k + 1].point.vx + ribbon->ox[k + 1];
            quad->y1 = (ribbon->sa[k + 1].packed >> 16) + ribbon->oy[k + 1];
            quad->x2 = ribbon->sa[k].point.vx;
            quad->y2 = ribbon->sa[k].packed >> 16;
            quad->x3 = ribbon->sa[k + 1].point.vx;
            quad->y3 = ribbon->sa[k + 1].packed >> 16;
            quad->r0 = ribbon->color[k].r;
            quad->g0 = ribbon->color[k].g;
            quad->b0 = ribbon->color[k].b;
            quad->r1 = ribbon->color[k + 1].r;
            quad->g1 = ribbon->color[k + 1].g;
            quad->b1 = ribbon->color[k + 1].b;
            quad->r2 = ribbon->color[k].r;
            quad->g2 = ribbon->color[k].g;
            quad->b2 = ribbon->color[k].b;
            quad->r3 = ribbon->color[k + 1].r;
            quad->g3 = ribbon->color[k + 1].g;
            quad->b3 = ribbon->color[k + 1].b;
            if (ribbon->depth[k] >= 0 && ribbon->flag[k] >= 0) {
                GsSortPoly(quad, ot, (u16)ribbon->depth[k]);
            }
        }
    }
    state->angle_a += state->step * 384;
    state->angle_b += state->step << 6;
}
