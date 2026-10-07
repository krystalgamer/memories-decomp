#include "../../types.h"
#include "../model_variant/model_variant.h"

/* MODEL417 places the same work block 0x5AC bytes lower. */
#ifndef SHEETS_SHIFT
#define SHEETS_SHIFT 0
#endif
#define SHEETS_OFFSET(offset) ((offset) - SHEETS_SHIFT)
#define SHEETS_WORD(offset) MODEL_VARIANT_WORD(work, SHEETS_OFFSET(offset))

/* Draws two four-quad sheets in a colour cycling through eight hues with
 * the frame count: the first at the origin, the second advanced along the
 * direction by the progress. On the last index of each timing cycle the
 * first sheet grows over the timing window and shrinks over the fade
 * window, while the second follows the phase. */
void func_8013DA48(u8 *context)
{
    SVECTOR rot;
    /* The target reserves 16 unused bytes between rot and scale. */
    u8 unknown_stack[16];
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG projection;
    PSXLONG flag;
    ModelVariantSheet *quad;
    GsOT *ot;
    u8 *work;
    POLY_GT4 *poly;
    s32 i;
    u8 red;
    u8 green;
    s32 k;
    s32 bias;
    s32 depth;
    u8 blue;

    work = context;
    quad = (ModelVariantSheet *)(work + SHEETS_OFFSET(0x1044));
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + SHEETS_OFFSET(0x25A0));
    if (SHEETS_WORD(0x26D0) % 8 == 0) {
        red = 255;
        green = 0;
        blue = 0;
    } else if (SHEETS_WORD(0x26D0) % 8 == 1) {
        red = 255;
        green = 128;
        blue = 0;
    } else if (SHEETS_WORD(0x26D0) % 8 == 2) {
        red = 255;
        green = 255;
        blue = 0;
    } else if (SHEETS_WORD(0x26D0) % 8 == 3) {
        red = 0;
        green = 255;
        blue = 0;
    } else if (SHEETS_WORD(0x26D0) % 8 == 4) {
        red = 0;
        green = 255;
        blue = 255;
    } else if (SHEETS_WORD(0x26D0) % 8 == 5) {
        red = 0;
        green = 0;
        blue = 255;
    } else if (SHEETS_WORD(0x26D0) % 8 == 6) {
        red = 255;
        green = 0;
        blue = 255;
    } else {
        red = 255;
        green = 0;
        blue = 128;
    }
    for (i = 0; i < 2; i++, quad++) {
        bias = 0;
        if (SHEETS_WORD(0x26D0) & 1) {
            bias = quad->size / 8;
        }
        setVector(&rot, 0, 0, 0);
        if (i == 0) {
            matrix.t[0] = SHEETS_WORD(0x2690);
            matrix.t[1] = SHEETS_WORD(0x2694);
            matrix.t[2] = SHEETS_WORD(0x2698);
        } else {
            matrix.t[0] = SHEETS_WORD(0x2690) +
                          SHEETS_WORD(0x26A4) * SHEETS_WORD(0x2708) / 1024;
            matrix.t[1] = SHEETS_WORD(0x2694) +
                          SHEETS_WORD(0x26A8) * SHEETS_WORD(0x2708) / 1024;
            matrix.t[2] = SHEETS_WORD(0x2698) +
                          SHEETS_WORD(0x26AC) * SHEETS_WORD(0x2708) / 1024;
        }
        setVector(&scale, quad->size + bias, quad->size + bias, quad->size + bias);
        RotMatrix(&rot, &matrix);
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
                &quad->v0[k], &quad->v1[k], &quad->v2[k], &quad->v3[k],
                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3,
                &projection, &flag);
            poly->r0 = red;
            poly->g0 = green;
            poly->b0 = blue;
            poly->r1 = red;
            poly->g1 = green;
            poly->b1 = blue;
            poly->r2 = red;
            poly->g2 = green;
            poly->b2 = blue;
            poly->r3 = quad->outer[0];
            poly->g3 = quad->outer[1];
            poly->b3 = quad->outer[2];
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if (SHEETS_WORD(0x26F8) + 1 == *(u16 *)(*(u8 *G32 *)(work + SHEETS_OFFSET(0x26E4)) + 0x18)) {
            if (i == 0) {
                if (SHEETS_WORD(0x271C) == 0 && quad->size < 4096) {
                    quad->size = ((SHEETS_WORD(0x26D4)
                        - MODEL_VARIANT_WORD(*(u8 *G32 *)(work + SHEETS_OFFSET(0x26E4)), 0x1C)) << 12)
                        / (MODEL_VARIANT_WORD(*(u8 *G32 *)(work + SHEETS_OFFSET(0x26E4)), 0x20)
                        - MODEL_VARIANT_WORD(*(u8 *G32 *)(work + SHEETS_OFFSET(0x26E4)), 0x1C));
                    if (quad->size >= 4096) {
                        quad->size = 4096;
                        SHEETS_WORD(0x271C) = 1;
                    }
                }
                if (SHEETS_WORD(0x26D4) >= MODEL_VARIANT_WORD(*(u8 *G32 *)(work + SHEETS_OFFSET(0x26E4)), 0x28)
                    && quad->size > 0) {
                    quad->size = 4096 - ((SHEETS_WORD(0x26D4)
                        - MODEL_VARIANT_WORD(*(u8 *G32 *)(work + SHEETS_OFFSET(0x26E4)), 0x28)) << 12)
                        / (MODEL_VARIANT_WORD(*(u8 *G32 *)(work + SHEETS_OFFSET(0x26E4)), 0x2C)
                        - MODEL_VARIANT_WORD(*(u8 *G32 *)(work + SHEETS_OFFSET(0x26E4)), 0x28));
                    if (quad->size <= 0) {
                        quad->size = 0;
                    }
                }
            } else if (SHEETS_WORD(0x271C) <= 0) {
                quad->size = 0;
            } else if (SHEETS_WORD(0x271C) == 1) {
                quad->size = 4096;
            } else if (SHEETS_WORD(0x271C) == 2) {
                if (quad->size < 8192) {
                    quad->size += SHEETS_WORD(0x26DC) << 9;
                    if (quad->size >= 8192) {
                        quad->size = 8192;
                        SHEETS_WORD(0x271C) = 3;
                    }
                }
            } else if (SHEETS_WORD(0x271C) == 4) {
                if (quad->size > 0) {
                    quad->size -= SHEETS_WORD(0x26DC) << 6;
                    if (quad->size <= 0) {
                        quad->size = 0;
                        SHEETS_WORD(0x271C) = 5;
                    }
                }
            }
        }
    }
}
