#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "func_800134E0.h"

void func_800134E0(u8 *object, s32 x, s32 y, s32 z)
{
    GsRVIEW2 *view = (GsRVIEW2 *)(object + 0x10);

    x += view->vrx;
    view->vpx = x;
    y += view->vry;
    z += view->vrz;
    view->vpy = y;
    view->vpz = z;
    GsSetRefView2(view);
}
