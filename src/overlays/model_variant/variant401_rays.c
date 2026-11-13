#include "../../types.h"
#include "variant401_entry.h"

/* Header 401's copy of the header-416 rays (french_model_variant/
 * variant433_rays.c) over its own entry state; each segment keeps its own
 * projection flag. */
void FrenchModelVariant433_DrawRays(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long flag[16][9];
    u8 r[9];
    u8 g[9];
    u8 b[9];
    long p;
    Variant401EntryState *work;
    GsOT *ot;
    ModelVariantTrail *ray;
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

    work = (Variant401EntryState *)ctx;
    ray = work->records;
    ot = Graphics_GetDefaultOrderingTable();
    line = &work->line;
    ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx);
    ratan2(work->aim.view_delta.vy, work->aim.view_delta.vx);
    ratan2(work->aim.direction.vz, work->aim.direction.vy);
    ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx);
    direction = -1;
    if (work->tail.slot == 0) {
        direction = 1;
    }
    for (i = 0, angle = 0; i < 16; i++, angle = i << 8, ray++) {
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->aim.target.vx;
        m.t[1] = work->aim.target.vy;
        m.t[2] = work->aim.target.vz;
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
            if (ray->offset[j] > 0) {
                size = ray->offset[j];
                level = size;
            } else {
                size = 0;
                level = size;
            }
            if (ray->offset[j] > 0x600) {
                fade = 0x800 - ray->offset[j];
                r[j] = ray->color[j].r * fade / 512;
                g[j] = ray->color[j].g * fade / 512;
                b[j] = ray->color[j].b * fade / 512;
            } else {
                r[j] = ray->color[j].r;
                g[j] = ray->color[j].g;
                b[j] = ray->color[j].b;
            }
            setVector(&ray->point[j],
                      ((rcos((s16)angle) << 9) >> 12) * size / 2048,
                      ((rsin((s16)angle) << 9) >> 12) * size / 2048 - ((rsin(size) << 7) >> 12),
                      spread * direction * size / 2048);
            if (ray->offset[j] < 0x800) {
                ray->offset[j] += work->clock.step << 5;
                if (ray->offset[j] >= 0x800) {
                    ray->offset[j] = 0x800;
                }
            }
        }
        for (j = 0; j < 8; j++) {
            line->attribute = 0x50000000;
            otz = RotTransPers4(&ray->point[j], &ray->point[j + 1],
                               &ray->point[j], &ray->point[j + 1],
                               (long *)&line->x0, (long *)&line->x1,
                               (long *)&line->x0, (long *)&line->x1, &p, &flag[i][j]);
            line->r0 = r[j];
            line->g0 = g[j];
            line->b0 = b[j];
            line->r1 = r[j + 1];
            line->g1 = g[j + 1];
            line->b1 = b[j + 1];
            if (otz >= 0 && flag[i][j] >= 0) {
                GsSortGLine(line, ot, otz);
            }
        }
    }
}
