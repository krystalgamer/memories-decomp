#include "../../types.h"
#include "variant582_sheets.h"

void func_8013CB98(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheets129State *state = (Sheets129State *)context;
    Sheet129 *sheet = state->sheets;
    POLY_GT4 *quad = &state->quad;
    POLY_F4 *mask = &state->mask;
    GsOT *ot;
    s32 i, j;
    s32 total;
    s32 active;
    s32 lift, radius, angle;
    s32 yaw, pitch;
    s32 depth;
    s32 size;
    u32 page;
    u8 tone;

    ot = func_80058F10();
    total = 0;
    yaw = ratan2(state->direction[2], state->direction[0]) + 2048;
    pitch = ratan2(state->direction[1], state->direction[2]) + 2048;
    active = GsGetActiveBuff();
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else if (yaw < 4096) {
        yaw += 1024;
    } else {
        yaw -= 3072;
    }
    for (i = 0; i < 5; i++, sheet++) {
        if (sheet->progress > 2048) {
            for (j = 0, angle = 0; j < 13;) {
                lift = (sheet->progress - 2048) * 96 / 2048;
                sheet->points[0][j].vx = rcos(angle) * (radius = lift + 384) >> 12;
                sheet->points[0][j].vy = rsin(angle) * radius >> 12;
                sheet->points[0][j].vz = 0;
                j++;
                angle = (j << 12) / 12;
            }
            size = sheet->progress;
        } else {
            for (j = 0, angle = 0; j < 13;) {
                radius = 384;
                sheet->points[0][j].vx = (s16)(rcos(angle) * 384 >> 12);
                sheet->points[0][j].vy = (s16)(rsin(angle) * 384 >> 12);
                sheet->points[0][j].vz = 0;
                j++;
                angle = (j << 12) / 12;
            }
            size = sheet->progress;
        }
        if (size > 0) {
            rotation.vx = 1024;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->positions[i].vx;
            matrix.t[1] = state->positions[i].vy;
            matrix.t[2] = state->positions[i].vz;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            for (j = 0, tone = 192; j < 12; j++) {
                RotTransPers4(&sheet->points[0][j], &sheet->points[0][j + 1],
                              &sheet->points[2][j], &sheet->points[2][j + 1],
                              (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                              (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                              &interpolation, &flag);
                if (quad->x0 < 160) {
                    if (active == 0) {
                        page = GetTPage(2, 1, 0, 0);
                    } else {
                        page = GetTPage(2, 1, 320, 0);
                    }
                    SetPolyGT4(quad);
                    quad->tpage = page;
                    setUV4(quad, quad->x0, quad->y0, quad->x1, quad->y1,
                           quad->x2, quad->y2, quad->x3, quad->y3);
                } else {
                    if (active == 0) {
                        page = GetTPage(2, 1, 128, 0);
                    } else {
                        page = GetTPage(2, 1, 448, 0);
                    }
                    SetPolyGT4(quad);
                    quad->tpage = page;
                    setUV4(quad, quad->x0 - 128, quad->y0, quad->x1 - 128, quad->y1,
                           quad->x2 - 128, quad->y2, quad->x3 - 128, quad->y3);
                }
                depth = RotTransPers4(&sheet->points[0][j], &sheet->points[0][j + 1],
                                      &sheet->points[1][j], &sheet->points[1][j + 1],
                                      (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                      (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                      &interpolation, &flag);
                SetSemiTrans(quad, 1);
                SetShadeTex(quad, 0);
                setRGB0(quad, tone, 64, tone);
                setRGB1(quad, tone, 64, tone);
                setRGB2(quad, tone, 64, tone);
                setRGB3(quad, tone, 64, tone);
                setRGB0(mask, 64, 0, 64);
                setXY4(mask, quad->x0, quad->y0, quad->x1, quad->y1,
                       quad->x2, quad->y2, quad->x3, quad->y3);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                    Graphics_SubmitMaskWritePacket((u32 *)mask, ot, (u16)depth, 1);
                }
            }
        }
        if (sheet->progress < 4096) {
            sheet->progress += state->step * 96;
            if (sheet->progress >= 4096) {
                if (state->phase < 3) {
                    sheet->progress -= 4096;
                    sheet->state = 1;
                } else {
                    sheet->progress = 4096;
                    sheet->state = 2;
                }
            }
        }
        total += sheet->state;
        if (total == 10 && state->phase == 3) {
            state->phase = 4;
        }
    }
}
