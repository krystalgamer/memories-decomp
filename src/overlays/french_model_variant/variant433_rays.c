#include "../../types.h"
#include "variant433_rays.h"

#ifdef MODEL_VARIANT418_RAYS
#define MODEL_VARIANT_RAY_TAIL_OFFSET 0x94
#define MODEL_VARIANT_RAY_FLAG flag[i][j]
#else
#define MODEL_VARIANT_RAY_TAIL_OFFSET 0
#define MODEL_VARIANT_RAY_FLAG flag
#endif

void func_8013D1CC(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
#ifdef MODEL_VARIANT418_RAYS
    PSXLONG flag[16][9];
#endif
    u8 r[9];
    u8 g[9];
    u8 b[9];
    PSXLONG p;
#ifndef MODEL_VARIANT418_RAYS
    PSXLONG flag;
#endif
    u8 *work;
    GsOT *ot;
    struct ModelVariant433Ray *ray;
    GsGLINE *line;
    s16 i;
    s16 j;
    s32 angle;
    s16 direction;
    s32 otz;
    s32 size;
    s32 level;
    s32 spread;
    s32 fade;

    work = ctx;
    ray = (struct ModelVariant433Ray *)work;
    ot = func_80058F10();
    line = (GsGLINE *)(work + (0x19E4 + MODEL_VARIANT_RAY_TAIL_OFFSET));
    ratan2(MODEL_VARIANT_WORD(work, (0x1A3C + MODEL_VARIANT_RAY_TAIL_OFFSET)), MODEL_VARIANT_WORD(work, (0x1A34 + MODEL_VARIANT_RAY_TAIL_OFFSET)));
    ratan2(MODEL_VARIANT_WORD(work, (0x1A38 + MODEL_VARIANT_RAY_TAIL_OFFSET)), MODEL_VARIANT_WORD(work, (0x1A34 + MODEL_VARIANT_RAY_TAIL_OFFSET)));
    ratan2(MODEL_VARIANT_WORD(work, (0x1A28 + MODEL_VARIANT_RAY_TAIL_OFFSET)), MODEL_VARIANT_WORD(work, (0x1A24 + MODEL_VARIANT_RAY_TAIL_OFFSET)));
    ratan2(MODEL_VARIANT_HALF(work, (0x1A32 + MODEL_VARIANT_RAY_TAIL_OFFSET)), MODEL_VARIANT_HALF(work, (0x1A30 + MODEL_VARIANT_RAY_TAIL_OFFSET)));
    direction = -1;
    if (MODEL_VARIANT_HALF(work, (0x1A94 + MODEL_VARIANT_RAY_TAIL_OFFSET)) == 0) {
        direction = 1;
    }
    for (i = 0, angle = 0; i < 16; i++, angle = i << 8, ray++) {
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_HALF(work, (0x1A18 + MODEL_VARIANT_RAY_TAIL_OFFSET));
        m.t[1] = MODEL_VARIANT_HALF(work, (0x1A1A + MODEL_VARIANT_RAY_TAIL_OFFSET));
        m.t[2] = MODEL_VARIANT_HALF(work, (0x1A1C + MODEL_VARIANT_RAY_TAIL_OFFSET));
        /* Retail reads an unwritten local on the first iteration into unused scale. */
        scale.vx = level;
        scale.vy = level;
        scale.vz = level;
        RotMatrix(&rot, &m);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        spread = 0x300;
        if (!(i & 1)) {
            spread = 0x200;
        }
        for (j = 0; j < 9; j++) {
            if (ray->progress[j] > 0) {
                size = ray->progress[j];
                level = size;
            } else {
                size = 0;
                level = size;
            }
            if (ray->progress[j] > 0x600) {
                fade = 0x800 - ray->progress[j];
                r[j] = ray->color[j][0] * fade / 512;
                g[j] = ray->color[j][1] * fade / 512;
                b[j] = ray->color[j][2] * fade / 512;
            } else {
                r[j] = ray->color[j][0];
                g[j] = ray->color[j][1];
                b[j] = ray->color[j][2];
            }
            setVector(&ray->point[j],
                      ((rcos((s16)angle) << 9) >> 12) * size / 2048,
                      ((rsin((s16)angle) << 9) >> 12) * size / 2048 - ((rsin(size) << 7) >> 12),
                      spread * direction * size / 2048);
            if (ray->progress[j] < 0x800) {
                ray->progress[j] += MODEL_VARIANT_WORD(work, (0x1A58 + MODEL_VARIANT_RAY_TAIL_OFFSET)) << 5;
                if (ray->progress[j] >= 0x800) {
                    ray->progress[j] = 0x800;
                }
            }
        }
        for (j = 0; j < 8; j++) {
            line->attribute = 0x50000000;
            otz = RotTransPers4(&ray->point[j], &ray->point[j + 1],
                               &ray->point[j], &ray->point[j + 1],
                               (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                               (PSXLONG *)&line->x0, (PSXLONG *)&line->x1, &p, &MODEL_VARIANT_RAY_FLAG);
            line->r0 = r[j];
            line->g0 = g[j];
            line->b0 = b[j];
            line->r1 = r[j + 1];
            line->g1 = g[j + 1];
            line->b1 = b[j + 1];
            if (otz >= 0 && MODEL_VARIANT_RAY_FLAG >= 0) {
                GsSortGLine(line, ot, otz);
            }
        }
    }
}
