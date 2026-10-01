#include "../../types.h"
#include "variant458_quads.h"

/* Draws the visible quads, fading past scale 0x600, then advances the
 * timed quad count. The packet is reused for each submitted quad. */
void func_8013CE20(u8 *ctx)
{
    SVECTOR rot;
    /* Retail reserves 16 bytes between rotation and scale. */
    u8 unknown_stack[16];
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    ModelVariantSheet *sheet;
    GsOT *ot;
    s16 i;
    s16 j;
    u8 r, g, b;
    u8 *work;
    Variant458Quads *quads;
    POLY_FT4 *poly;
    s32 fade;
    s32 otz;

    work = ctx;
    sheet = (ModelVariantSheet *)(work + 0x1904);
    quads = (Variant458Quads *)work;
    poly = (POLY_FT4 *)(work + 0x2D98);
    ot = func_80058F10();
    i = 0;
    ratan2(MODEL_VARIANT_WORD(work, 0x2F90), MODEL_VARIANT_WORD(work, 0x2F94));
    ratan2(MODEL_VARIANT_HALF(work, 0x2F9E), MODEL_VARIANT_HALF(work, 0x2F9C));
    do {
        j = 0;
        while (j < MODEL_VARIANT_WORD(work, 0x2FFC)) {
            if (quads->level[j] > 0x600) {
                fade = 0x800 - quads->level[j];
                r = quads->color.r * fade / 512;
                g = quads->color.g * fade / 512;
                b = quads->color.b * fade / 512;
            } else {
                r = quads->color.r;
                g = quads->color.g;
                b = quads->color.b;
            }
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            m.t[0] = MODEL_VARIANT_WORD(work - -(j * 16), 0x2E0C);
            m.t[1] = MODEL_VARIANT_WORD(work - -(j * 16), 0x2E10);
            m.t[2] = MODEL_VARIANT_WORD(work - -(j * 16), 0x2E14);
            if (MODEL_VARIANT_WORD(work, 0x3020) >= 4) {
                scale.vx = sheet->size;
                scale.vy = sheet->size;
                scale.vz = sheet->size;
            } else {
                scale.vx = 0x1000;
                scale.vy = 0x1000;
                scale.vz = 0x1000;
            }
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
            otz = RotTransPers4(&quads->a[j], &quads->b[j], &quads->c[j], &quads->d[j],
                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                (PSXLONG *)&poly->x3, &p, &flag);
            poly->r0 = r;
            poly->g0 = g;
            poly->b0 = b;
            if (otz >= 0 && flag >= 0 && quads->hidden[j] == 0) {
                GsSortPoly(poly, ot, otz);
            }
            /* Keeping the increment in the body preserves retail scheduling. */
            j++;
        }
        i++;
        quads++;
    } while (i < 1);
    if (MODEL_VARIANT_WORD(work, 0x2FFC) < 8) {
        MODEL_VARIANT_WORD(work, 0x2FFC) =
            (u32)((MODEL_VARIANT_WORD(work, 0x2FBC) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2FCC), 0x20)) << 3) /
            (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2FCC), 0x24) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2FCC), 0x20));
        if (MODEL_VARIANT_WORD(work, 0x2FFC) >= 8 && MODEL_VARIANT_WORD(work, 0x3020) == 0) {
            MODEL_VARIANT_WORD(work, 0x3020) = 1;
        }
    }
}
