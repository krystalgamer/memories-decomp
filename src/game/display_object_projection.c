#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "display_object.h"
#include "display_object_projection.h"

/* RotAverageNclip3_nom at 0x80089CF0. libgte.h declares it
   `void RotAverageNclip3_nom(SVECTOR *, SVECTOR *, SVECTOR *)`; retail calls
   it with a fourth pointer and reads the result, so it keeps its address name
   and this local prototype until that arity is settled (notes/psyq.md). The
   parameter types are the header's. */
extern s32 func_80089CF0(SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, SVECTOR *v3);

s32 func_80041E7C(u32 arg0, s32 arg1, s32 arg2, struct ProjectionOut *arg3)
{
    SVECTOR *v308;
    MATRIX *mtx;
    s32 otz;

    arg3->f0 = arg1;
    arg3->f2 = arg2;
    arg3->f4 = 0;

    SetGeomOffset(arg1, arg2);
    SetGeomScreen(0x12C);

    mtx = (MATRIX *)0x1F8002D0;
    v308 = (SVECTOR *)0x1F800308;

    v308->vx = (s16)((arg0 & 0xFF) * 0x10);
    v308->vy = (s16)((arg0 >> 4) & 0xFF0);
    v308->vz = (s16)((arg0 >> 0xC) & 0xFF0);

    RotMatrixZYX_gte(v308, mtx);

    mtx->t[0] = 0;
    mtx->t[1] = 0;
    mtx->t[2] = 0x12C;

    GsSetLsMatrix(mtx);

    {
        register SVECTOR *v318 __asm__("$7") = (SVECTOR *)0x1F800318;
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

        func_80089CF0(v308, v310, v318, v318);
    }

    {
        s32 *p = &otz;
        gte_stopz(p);
    }

    return otz;
}

s32 func_80041F90(DisplayObject *obj, s32 arg1, s32 arg2, struct ProjectionOut *out) {
    MATRIX *mtx = (MATRIX *)0x1F8002D0;
    SVECTOR *v308 = (SVECTOR *)0x1F800308;
    s32 otz;

    out->f0 = arg1;
    out->f2 = arg2;
    out->f4 = obj->field_65;

    SetGeomOffset(arg1, arg2);
    SetGeomScreen(0x12C);

    v308->vx = (s16)(obj->field_20.b.field_20 * 0x10);
    v308->vy = (s16)(obj->field_20.b.field_21 * 0x10);
    v308->vz = (s16)(obj->field_20.b.field_22 * 0x10);
    RotMatrixZYX_gte(v308, mtx);

    mtx->t[0] = 0;
    mtx->t[1] = 0;
    mtx->t[2] = 0x12C;

    if (!(obj->attribute & GsROTOFF)) {
        ((VECTOR *)v308)->vx = obj->field_44.h.field_44;
        ((VECTOR *)v308)->vy = obj->field_44.h.field_46;
        ((VECTOR *)v308)->vz = ONE;
        ScaleMatrix(mtx, (VECTOR *)0x1F800308);
    }

    GsSetLsMatrix(mtx);

    {
        register SVECTOR *v318 asm("a3") = (SVECTOR *)0x1F800318;
        SVECTOR *v310 = (SVECTOR *)0x1F800310;
        s32 *otzp;

        v318->vx = 0;
        v308->vx = 0;
        v308[1].vx = 0x200;
        v310->vy = 0;
        v308->vy = 0;
        v308[2].vy = 0x200;
        v318->vz = 0;
        v310->vz = 0;
        v308->vz = 0;
        func_80089CF0(v308, v310, v318, v318);

        otzp = &otz;
        gte_stopz(otzp);

        {
            s32 cb = obj->field_10;
            if (cb != 0) {
                if (cb < 0) {
                    ((void (*)(DisplayObject *, s32))cb)(obj, otz);
                }
                if (otz >= 0) {
                    return otz;
                }

                mtx->m[0][0] = -mtx->m[0][0];
                mtx->m[1][0] = -mtx->m[1][0];
                mtx->m[2][0] = -mtx->m[2][0];
                mtx->m[0][2] = -mtx->m[0][2];
                mtx->m[1][2] = -mtx->m[1][2];
                mtx->m[2][2] = -mtx->m[2][2];
                SetRotMatrix(mtx);

                func_80089CF0(v308, v310, (SVECTOR *)0x1F800318, v318);

                otzp = &otz;
                gte_stopz(otzp);
            }
        }
    }

    return otz;
}
