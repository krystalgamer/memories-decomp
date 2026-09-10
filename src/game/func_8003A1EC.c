#include "../types.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "func_8003A1EC.h"
#include "../unmatched.h"

extern s32 func_8003A198(u8 *, s32, s32, s32);
s32 func_8003A1EC(MenuRecord *a, u8 **out, s32 c) {
    DisplayObject *p;
    u8 *tb;
    s32 f;
    s32 g;
    s32 h;
    s32 m;

    if (a->field_3C != 0) {
        f = -0xA;
        tb = D_801AF800;
        g = 0x1A;
        h = 0x202;
    } else {
        f = -0xE;
        tb = D_801AF000;
        g = 0x1D;
        h = 0x200;
    }

    if (func_8003A198(tb, c, 0, 0) == 0) {
        return 0;
    }

    {
        m = 0x41000000;
        p = func_800400AC(func_8004002C(), 2);
        func_800428A8(p, *(s16 *)&a->field_34, *(s16 *)&a->field_36, c, 0, 0, g, h,
                      tb);
        func_80042918(p);
        func_800428EC((u8 *)p, f);
        p->attribute = p->attribute | m;
        p->flags = p->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        out[0] = (u8 *)p;

        if (func_8003A198(tb, c, 1, 0) != 0) {
            p = func_800400AC(func_8004002C(), 2);
            func_800428A8(p, *(s16 *)&a->field_34, *(s16 *)&a->field_36, c, 1, 0, g,
                          h, tb);
            func_80042918(p);
            func_800428EC((u8 *)p, f | 1);
            p->attribute = p->attribute | m;
            p->flags = p->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        } else {
            p = (DisplayObject *)0;
        }
        out[1] = (u8 *)p;

        if (func_8003A198(tb, c, 2, 0) != 0) {
            p = func_800400AC(func_8004002C(), 2);
            func_800428A8(p, *(s16 *)&a->field_34, *(s16 *)&a->field_36, c, 2, 0, g,
                          h, tb);
            func_80042918(p);
            func_800428EC((u8 *)p, f | 1);
            p->attribute = p->attribute | 0x41000000;
            p->flags = p->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        } else {
            p = (DisplayObject *)0;
        }
        out[2] = (u8 *)p;
    }

    return 1;
}
