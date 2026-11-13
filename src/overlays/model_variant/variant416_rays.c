#include "../../types.h"
#include "variant416_entry.h"

void FrenchModelVariant433_DrawRays(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    u8 red[16];
    u8 green[16];
    u8 blue[16];
    long p;
    long flag;
    Variant416EntryState *work;
    GsOT *ot;
    s16 i, j;
    s32 angle;
    s16 sign;
    s32 radius;
    s32 amplitude;
    GsGLINE *line;
    ModelVariantTrail *ray;
    s32 f;
    s32 otz;
    s32 size;

    work = (Variant416EntryState *)ctx;
    ray = work->records;
    ot = Graphics_GetDefaultOrderingTable();
    ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx);
    ratan2(work->aim.view_delta.vy, work->aim.view_delta.vx);
    ratan2(work->aim.direction.vz, work->aim.direction.vy);
    ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx);
    line = &work->line;
    sign = -1;
    if (work->tail.slot == 0) {
        sign = 1;
    }
    for (i = 0, angle = 0; i < 16; i++, angle = i << 8, ray++) {
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->aim.target.vx;
        m.t[1] = work->aim.target.vy;
        m.t[2] = work->aim.target.vz;
        /* The original stores the prior radius into this unused scale. */
        scale.vx = size;
        scale.vy = size;
        scale.vz = size;
        RotMatrix(&rot, &m);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        amplitude = 768;
        if (!(i & 1)) {
            amplitude = 512;
        }
        for (j = 0; j < 9; j++) {
            if (ray->offset[j] > 0) {
                radius = ray->offset[j];
                size = radius;
            } else {
                radius = 0;
                size = radius;
            }
            if (ray->offset[j] > 1536) {
                f = 2048 - ray->offset[j];
                red[j] = ray->color[j].r * f / 512;
                green[j] = ray->color[j].g * f / 512;
                blue[j] = ray->color[j].b * f / 512;
            } else {
                red[j] = ray->color[j].r;
                green[j] = ray->color[j].g;
                blue[j] = ray->color[j].b;
            }
            (j + ray->point)->vx = ((rcos((s16)angle) * 512 >> 12) * radius) / 2048;
            (j + ray->point)->vy = ((rsin((s16)angle) * 512 >> 12) * radius) / 2048 - (rsin(radius) * 128 >> 12);
            (j + ray->point)->vz = amplitude * sign * radius / 2048;
            if (ray->offset[j] < 2048) {
                ray->offset[j] += work->clock.step << 5;
                if (ray->offset[j] >= 2048) {
                    ray->offset[j] = 2048;
                }
            }
        }
        for (j = 0; j < 8; j++) {
            line->attribute = 0x50000000;
            otz = RotTransPers4(&ray->point[j], &ray->point[j + 1], &ray->point[j], &ray->point[j + 1],
                (long *)&line->x0, (long *)&line->x1, (long *)&line->x0, (long *)&line->x1, &p, &flag);
            line->r0 = red[j];
            line->g0 = green[j];
            line->b0 = blue[j];
            line->r1 = red[j + 1];
            line->g1 = green[j + 1];
            line->b1 = blue[j + 1];
            if (otz >= 0 && flag >= 0) {
                GsSortGLine(line, ot, otz);
            }
        }
    }
}
