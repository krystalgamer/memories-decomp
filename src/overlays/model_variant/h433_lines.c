#include "../../types.h"
#include "h433_entry.h"

void SpanishModelVariant450_DrawLines(Variant433EntryState *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long interpolation;
    long flag;
    Variant433EntryState *work;
    GsOT *ot;
    Variant433EntryFan *group;
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

    work = ctx;
    primary_index = 0;
    fade_index = 0;
    group = work->fans;
    line = &work->line;
    ot = Graphics_GetDefaultOrderingTable();
    ratan2(work->view_delta.vz, work->view_delta.vx);
    ratan2(work->view_delta.vy, work->view_delta.vx);
    ratan2(work->direction.vz, work->direction.vy);
    ratan2(work->screen_y, work->screen_x);
    /* Likewise each fade record starts at the next sheet's size (+0x88). */
    fade_base = (Variant450Fade *)&work->sheets[1].size;
    /* Retail addresses each ribbon from its phase words (+0x1D0), so the
     * primary records start there and keep their own view. */
    primary_base = (Variant450Primary *)work->ribbons[0].phase;
    for (i = 0; i < 6; i++, group++, primary_index++, fade_index++) {
        fade = fade_base + fade_index;
        primary = primary_base + primary_index;
        extra = 0;
        if (work->clock.frame_count & 1) {
            extra = fade->scale / 4;
        }
        if (fade->fading == 0) {
            r = group->color.r;
            g = group->color.g;
            b = group->color.b;
        } else {
            r = group->color.r * fade->brightness / 1024;
            g = group->color.g * fade->brightness / 1024;
            b = group->color.b * fade->brightness / 1024;
            work->spin += work->clock.step * 4;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = work->spin;
        m.t[0] = primary->translation.vx;
        m.t[1] = primary->translation.vy;
        m.t[2] = primary->translation.vz;
        scale.vx = fade->scale * 4 + extra;
        scale.vy = fade->scale * 4 + extra;
        scale.vz = fade->scale * 4 + extra;
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
        for (j = 0; j < 8; j++) {
            line->attribute = 0x50000000;
            depth = RotTransPers3(&group->point[0], &group->point[j + 1],
                                 &group->point[0], (long *)&line->x0,
                                 (long *)&line->x1, (long *)&line->x0,
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
