#include "../types.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"
#include "display_object.h"
#include "graphics_frame.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "func_8003A440.h"
#include "func_8003A1EC.h"
#include "func_8003AC48.h"

void func_8003AC48(MenuRecord *p)
{
    DisplayObject *h;
    u8 **d;
    DisplayObject *e;
    s32 x;
    s32 y;
    s32 t;
    s32 u;
    s32 m;
    s32 i;

    if (func_80039F1C((DisplayEffectState *)p) == 0) {
        p->field_32 |= 0x10;
        h = (DisplayObject *)p->grid[0][0];
        x = h->field_16;
        y = h->field_67;
        func_8003A440((u8 **)p->grid[0], (GsALON | GsAONE), x);
        d = (u8 **)p->grid[1];
        func_8003A1EC(p, d, y);
        func_8003A440(d, (GsALON | GsATWO), x - 1);
        p->field_40 = 0x80;
    }

    t = *(u16 *)&p->field_40 - (D_8009B0D8 << 3);
    p->field_40 = t;
    u = (s16)t;
    if (u <= 0) {
        p->display_effect_step = 0;
        func_80039F90((void **)p->grid[1]);
        func_80039FD4((u8 *)p);
    } else {
        m = u;
        m |= (m << 8) | (m << 16);
        for (i = 2; i >= 0; i--) {
            e = (DisplayObject *)p->grid[0][i];
            if (e != 0) {
                e->field_0C = m;
            }
            e = (DisplayObject *)p->grid[1][i];
            if (e != 0) {
                e->field_0C = m;
            }
        }
    }
}
