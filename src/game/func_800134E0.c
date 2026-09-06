#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

typedef struct {
    u8 padding[0x10];
    GsRVIEW2 view;
} Object;

void func_800134E0(Object *object, int x, int y, int z)
{
    GsRVIEW2 *view = &object->view;

    x += view->vrx;
    view->vpx = x;
    y += view->vry;
    z += view->vrz;
    view->vpy = y;
    view->vpz = z;
    GsSetRefView2(view);
}
