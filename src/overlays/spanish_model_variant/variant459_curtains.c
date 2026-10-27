#include "../../types.h"
#include "variant459_sheets.h"

#define CURTAIN459_TIMING(work) ((Timing459 *)MODEL_VARIANT_WORD(work, 0x309C))

/* Loads both rows' colours unfaded. */
#define CURTAIN459_LOAD_COLOURS(work) \
    do {                              \
        r0 = (work)[0x30C0];          \
        g0 = (work)[0x30C1];          \
        b0 = (work)[0x30C2];          \
        r1 = (work)[0x30C4];          \
        g1 = (work)[0x30C5];          \
        b1 = (work)[0x30C6];          \
    } while (0)

/* Header 459's form of the variant curtains: each frame rebuilds the two rows of
 * every timed curtain around the rotating angle at work + 0x30C8, turned to
 * face the variant's direction and placed at its position in work + 0x2F8C.
 * Curtain 3 and curtains below half size keep their colours, and the rest
 * fade with size. Before phase 3 each scale grows with the clock at
 * work + 0x3084 toward the timing record's limit, which counts a cycle;
 * otherwise it shrinks back between the record's fade start and fade end. */
void func_8013C9AC(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    ModelVariantCurtain *curtain;
    GsOT *ot;
    s32 i;
    s32 lift;
    s32 yaw;
    s32 pitch;
    s32 radius;
    POLY_GT4 *poly;
    s32 j;
    s32 otz;
    s32 size;
    s32 angle;
    ModelVariantSheet *sheet;
    s32 fade;
    Timing459 *rec;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 r1;
    u8 g1;
    u8 b1;

    work = ctx;
    curtain = (ModelVariantCurtain *)(work + 0x848);
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x305C), MODEL_VARIANT_WORD(work, 0x3054)) + 0x800;
    sheet = (ModelVariantSheet *)(work + 0x2568);
    poly = (POLY_GT4 *)(work + 0x2DFC);
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x3058), MODEL_VARIANT_WORD(work, 0x305C)) + 0x800;
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw += 0x400;
    }
    lift = ((MODEL_VARIANT_WORD(work, 0x3080) & 1) << 5) * sheet->size / 4096;
    for (i = 0; i < CURTAIN459_TIMING(work)->count; i++, curtain++) {
        radius = lift + 0x80;
        if (curtain->scale > 0) {
            size = curtain->scale;
            for (j = 0, angle = MODEL_VARIANT_WORD(work, 0x30C8); j < 17;
                 j++, angle = MODEL_VARIANT_WORD(work, 0x30C8) + j * 256) {
                setVector(&curtain->a[j], (u32)rcos(angle) >> 6, (u32)rsin(angle) >> 6, 0);
                setVector(&curtain->b[j], rcos(angle) * radius >> 12, rsin(angle) * radius >> 12, -(lift + 0x40));
            }
            rot.vx = -pitch;
            rot.vy = yaw;
            rot.vz = 0;
            m.t[0] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F8C);
            m.t[1] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F90);
            m.t[2] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F94);
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            if (i == 3 || curtain->scale < 0x800) {
                CURTAIN459_LOAD_COLOURS(work);
            } else {
                fade = (s16)(0x1000 - curtain->scale);
                r0 = work[0x30C0] * fade / 2048;
                g0 = work[0x30C1] * fade / 2048;
                b0 = work[0x30C2] * fade / 2048;
                r1 = work[0x30C4] * fade / 2048;
                g1 = work[0x30C5] * fade / 2048;
                b1 = work[0x30C6] * fade / 2048;
            }
            for (j = 0; j < 16; j++) {
                otz = RotTransPers4(&curtain->a[j], &curtain->a[j + 1], &curtain->b[j], &curtain->b[j + 1],
                                    (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                                    (PSXLONG *)&poly->x3, &p, &flag);
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = r0;
                poly->g1 = g0;
                poly->b1 = b0;
                poly->r2 = r1;
                poly->g2 = g1;
                poly->b2 = b1;
                poly->r3 = r1;
                poly->g3 = g1;
                poly->b3 = b1;
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, (u16)otz);
                }
            }
        }
        rec = CURTAIN459_TIMING(work);
        if (curtain->scale < rec->limit && MODEL_VARIANT_WORD(work, 0x30DC) < 3) {
            curtain->scale = rec->limit * (u32)MODEL_VARIANT_WORD(work, 0x3084) / rec->grow_end;
            if (curtain->scale >= CURTAIN459_TIMING(work)->limit) {
                curtain->scale = CURTAIN459_TIMING(work)->limit;
                curtain->count++;
            }
        } else if ((u32)(otz = MODEL_VARIANT_WORD(work, 0x3084)) >= (u32)(fade = rec->fade_start)) {
            curtain->scale = rec->limit - rec->limit * ((u32)otz - (u32)fade) / (rec->fade_end - (u32)fade);
        }
    }
    MODEL_VARIANT_WORD(work, 0x30C8) += MODEL_VARIANT_WORD(work, 0x308C) * 80;
}
