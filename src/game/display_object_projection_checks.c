#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "display_object.h"
#include "display_object_projection.h"
#include "model.h"

/* Contiguous projection helpers sharing the scratchpad matrix/vector layout.
 * The first clips packed rotation bytes; the second extends the same path with
 * display-object scale, callback, and mirrored-rotation handling. */

s32 DisplayObject_ProjectPackedRotationFacing(
    u32 packed_rotation, s32 x, s32 y, struct ProjectionOut *out)
{
    SVECTOR *v308;
    MATRIX *mtx;
    s32 clip_value;

    out->x = x;
    out->y = y;
    out->divisions = 0;

    SetGeomOffset(x, y);
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);

    mtx = (MATRIX *)0x1F8002D0;
    v308 = (SVECTOR *)0x1F800308;

    v308->vx = (s16)((packed_rotation & 0xFF) * 0x10);
    v308->vy = (s16)((packed_rotation >> 4) & 0xFF0);
    v308->vz = (s16)((packed_rotation >> 0xC) & 0xFF0);

    RotMatrixZYX_gte(v308, mtx);

    mtx->t[0] = 0;
    mtx->t[1] = 0;
    mtx->t[2] = MODEL_DEFAULT_PROJECTION;

    GsSetLsMatrix(mtx);

    {
        register SVECTOR *v318 = (SVECTOR *)0x1F800318;
        SVECTOR *v310 = (SVECTOR *)0x1F800310;

        v318->vx = 0;
        v308->vx = 0;
        v308[1].vx = 0x200;
        v310->vy = 0;
        v308->vy = 0;
        v308[2].vy = 0x200;
        v318->vz = 0;
        v310->vz = 0;
        v308->vz = 0;

        RotAverageNclip3_nom(v308, v310, v318);
    }

    {
        s32 *p = &clip_value;
        /* NCLIP leaves the signed projected area in MAC0. */
        gte_stopz(p);
    }

    return clip_value;
}

s32 DisplayObject_ProjectRotationFacing(
    DisplayObject *obj,
    s32 x,
    s32 y,
    struct ProjectionOut *out
)
{
    MATRIX *mtx = (MATRIX *)0x1F8002D0;
    SVECTOR *v308 = (SVECTOR *)0x1F800308;
    s32 facing_area;

    out->x = x;
    out->y = y;
    out->divisions = obj->field_65;

    SetGeomOffset(x, y);
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);

    v308->vx = (s16)(obj->field_20.b.field_20 * 0x10);
    v308->vy = (s16)(obj->field_20.b.field_21 * 0x10);
    v308->vz = (s16)(obj->field_20.b.field_22 * 0x10);
    RotMatrixZYX_gte(v308, mtx);

    mtx->t[0] = 0;
    mtx->t[1] = 0;
    mtx->t[2] = MODEL_DEFAULT_PROJECTION;

    if (!(obj->attribute & GsROTOFF)) {
        VECTOR *scale = (VECTOR *)v308;

        scale->vx = obj->field_44.h.field_44;
        scale->vy = obj->field_44.h.field_46;
        scale->vz = ONE;
        ScaleMatrix(mtx, scale);
    }

    GsSetLsMatrix(mtx);

    {
        SVECTOR *v318 = (SVECTOR *)0x1F800318;
        SVECTOR *v310 = (SVECTOR *)0x1F800310;
        s32 *area_result;

        v318->vx = 0;
        v308->vx = 0;
        v308[1].vx = 0x200;
        v310->vy = 0;
        v308->vy = 0;
        v308[2].vy = 0x200;
        v318->vz = 0;
        v310->vz = 0;
        v308->vz = 0;
        RotAverageNclip3_nom(v308, v310, v318);

        area_result = &facing_area;
        gte_stopz(area_result);

        {
            s32 cb = obj->field_10;
            if (cb != 0) {
                if (cb < 0) {
                    ((void (*)(DisplayObject *, s32))cb)(obj, facing_area);
                }
                if (facing_area >= 0) {
                    return facing_area;
                }

                mtx->m[0][0] = -mtx->m[0][0];
                mtx->m[1][0] = -mtx->m[1][0];
                mtx->m[2][0] = -mtx->m[2][0];
                mtx->m[0][2] = -mtx->m[0][2];
                mtx->m[1][2] = -mtx->m[1][2];
                mtx->m[2][2] = -mtx->m[2][2];
                SetRotMatrix(mtx);

                RotAverageNclip3_nom(
                    v308, v310, (SVECTOR *)0x1F800318);

                area_result = &facing_area;
                gte_stopz(area_result);
            }
        }
    }

    return facing_area;
}
