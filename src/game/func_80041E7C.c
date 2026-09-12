#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "../game/display_object.h"
#include "../game/display_object_projection.h"

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
        SVECTOR *v318 = (SVECTOR *)0x1F800318;
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
        s32 *p = &otz;
        gte_stopz(p);
    }

    return otz;
}
