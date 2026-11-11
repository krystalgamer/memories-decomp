#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "variant335_entry.h"

void FrenchModelVariant335_DrawSheets(u8 *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long projection;
    long flag;
    ModelVariantSheet *sheet;
    GsOT *ot;
    Model335EntryState *work;
    POLY_GT4 *poly;
    s32 i;
    s32 k;
    s32 bias;
    s32 depth;
    s32 turn;
    s32 angle;
    s32 radius;
    s32 rotation_bias;

    work = (Model335EntryState *)context;
    sheet = (ModelVariantSheet *)work->sheets;
    ot = Graphics_GetDefaultOrderingTable();
    poly = &work->textured[1];
    turn = ratan2(work->view_delta.vz, work->view_delta.vx);
    rotation_bias = work->spin + 3072;
    turn += rotation_bias;
    for (i = 0; i < 4; i++, sheet++) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        setVector(&rot, 0, 0, 0);
        if (i == 3) {
            matrix.t[0] = work->target.vx;
            matrix.t[1] = work->target.vy;
            matrix.t[2] = work->target.vz;
        } else {
            angle = turn + (i << 12) / 3;
            radius = work->orbit_diameter / 2;
            matrix.t[0] = work->origin.vx + (rcos(angle) * radius >> 12);
            matrix.t[1] = work->origin.vy;
            matrix.t[2] = work->origin.vz + (rsin(angle) * radius >> 12);
        }
        setVector(&scale, sheet->size + bias, sheet->size + bias, sheet->size + bias);
        RotMatrix(&rot, &matrix);
        ScaleMatrix(&matrix, &scale);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ReadRotMatrix(&light);
        RotMatrix(&rot, &light);
        ScaleMatrix(&light, &scale);
        SetRotMatrix(&light);
        for (k = 0; k < 4; k++) {
            depth = RotTransPers4(
                &sheet->v0[k], &sheet->v1[k], &sheet->v2[k], &sheet->v3[k],
                (long *)&poly->x0, (long *)&poly->x1,
                (long *)&poly->x2, (long *)&poly->x3,
                &projection, &flag);
            setRGB0(poly, sheet->inner[0], sheet->inner[1], sheet->inner[2]);
            setRGB1(poly, sheet->inner[0], sheet->inner[1], sheet->inner[2]);
            setRGB2(poly, sheet->inner[0], sheet->inner[1], sheet->inner[2]);
            setRGB3(poly, sheet->outer[0], sheet->outer[1], sheet->outer[2]);
            if ((work->phase != 0 || i == 0)
                && depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if (i == 3) {
            if (work->phase == 5) {
                if (sheet->size < 16384) {
                    sheet->size += work->clock.step * 2048;
                    if (sheet->size >= 16384) {
                        sheet->size = 16384;
                        work->phase = 6;
                    }
                }
            } else if (work->phase >= 6) {
                if ((u32)work->clock.frame
                    > (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x24)
                    && sheet->size > 0) {
                    sheet->size = 16384 - ((((u32)work->clock.frame
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x24)) << 14)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x28)
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x24)));
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                    }
                }
            }
        } else {
            if (work->phase == 0) {
                if (sheet->size < 4096) {
                    sheet->size = (((u32)work->clock.frame
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x0C)) << 12)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x10)
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x0C));
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        work->phase = 1;
                    }
                }
            } else if (work->phase == 1) {
                if (work->orbit_diameter < 1024) {
                    work->orbit_diameter = (((u32)work->clock.frame
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x10)) << 10)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x14)
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x10));
                    if (work->orbit_diameter >= 1024) {
                        work->orbit_diameter = 1024;
                        work->phase = 2;
                    }
                }
            } else if (work->phase == 2) {
                if (work->lift < 256) {
                    work->lift = (((u32)work->clock.frame
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x14)) << 8)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x18)
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x14));
                    if (work->lift >= 256) {
                        work->lift = 256;
                        work->phase = 3;
                    }
                }
            } else if (work->phase == 3) {
                if (work->travel_angle < 1024) {
                    work->travel_angle = (((u32)work->clock.frame
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x18)) << 10)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x1C)
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x18));
                    if (work->travel_angle >= 1024) {
                        work->travel_angle = 1024;
                        work->phase = 4;
                    }
                }
            } else if (work->phase >= 6) {
                if ((u32)work->clock.frame
                    > (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x24)
                    && sheet->size > 0) {
                    sheet->size = 4096 - ((((u32)work->clock.frame
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x24)) << 12)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x28)
                        - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x24)));
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        work->phase = 7;
                    }
                }
            }
        }
    }
}
