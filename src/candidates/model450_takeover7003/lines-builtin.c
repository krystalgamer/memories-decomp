#define func_8013DD88 func_8013DD20
#include "../../types.h"
#include "../../overlays/spanish_model_variant/variant450_lines.h"

void func_8013DD88(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG interpolation;
    PSXLONG flag;
    Variant450View *work;
    GsOT *ot;
    Variant450Group *group;
    GsGLINE *line;
    s16 i;
    s16 j;
    u8 r, g, b;
    Variant450Primary *primary;
    Variant450Primary *primary_base;
    Variant450Fade *fade;
    Variant450Fade *fade_base;
    s32 extra;
    s32 depth;
    s32 primary_index;
    s32 fade_index;

    work = (Variant450View *)ctx;
    primary_index = 0;
    fade_index = 0;
    group = work->groups;
    line = &work->line;
    ot = func_80058F10();
    ratan2(work->field_4298, work->field_4290);
    ratan2(work->field_4294, work->field_4290);
    ratan2(work->field_4284, work->field_4280);
    ratan2(work->field_428E, work->field_428C);
    fade_base = work->fade;
    primary_base = work->primary;
    for (i = 0; i < 6; i++, group++, primary_index++, fade_index++) {
        fade = fade_base + fade_index;
        primary = primary_base + primary_index;
        extra = 0;
        if (work->field_42A8 & 1) {
            extra = fade->scale / 4;
        }
        if (fade->fading == 0) {
            r = group->color[0];
            g = group->color[1];
            b = group->color[2];
        } else {
            r = group->color[0] * fade->brightness / 1024;
            g = group->color[1] * fade->brightness / 1024;
            b = group->color[2] * fade->brightness / 1024;
            work->field_42F4 += work->field_42B4 * 4;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = work->field_42F4;
        m.t[0] = primary->translation.vx;
        m.t[1] = primary->translation.vy;
        m.t[2] = primary->translation.vz;
        scale.vx = fade->scale * 4 + extra;
        scale.vy = fade->scale * 4 + extra;
        scale.vz = fade->scale * 4 + extra;
        RotMatrix(&rot, &m);
        __builtin_memcpy(&coord.coord, &m, sizeof(m));
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        ReadRotMatrix(&ls);
        RotMatrix(&rot, &ls);
        ScaleMatrix(&ls, &scale);
        SetRotMatrix(&ls);
        for (j = 0; j < 8; j++) {
            line->attribute = 0x50000000;
            depth = RotTransPers3(&group->points[0], &group->points[j + 1],
                                 &group->points[0], (PSXLONG *)&line->x0,
                                 (PSXLONG *)&line->x1, (PSXLONG *)&line->x0,
                                 &interpolation, &flag);
            line->r0 = r;
            line->g0 = g;
            line->b0 = b;
            line->r1 = 0;
            line->g1 = 0;
            line->b1 = 0;
            if (depth >= 0 && flag >= 0 && primary->threshold >= 1024) {
                GsSortGLine(line, ot, depth);
            }
        }
    }
}
