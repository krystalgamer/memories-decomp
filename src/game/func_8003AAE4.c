#include "../types.h"
#include "display_object_position.h"
#include "display_effect_lifecycle.h"
#include "graphics_frame.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "func_8003A440.h"
#include "func_8003A1EC.h"
#include "menu_record.h"

void func_8003AAE4(MenuRecord *p) {
    u8 *q;
    s32 *e;
    s32 *c;
    s32 a;
    s32 b;
    s32 v;
    s32 m;
    s32 i;

    if (func_80039F1C((DisplayEffectState *)p) == 0) {
        *(s16 *)&p->field_34 = 0x68;
        p->field_32 |= 0x10;
        if (p->field_3C != 0) {
            *(s16 *)&p->field_34 = 0xD8;
        }
        func_8003A920((DisplayPositionGroup *)p, *(s16 *)&p->field_34,
                      *(s16 *)&p->field_36);
        q = *(u8 **)&p->grid[0][0];
        a = *(s8 *)(q + 0x16);
        b = q[0x67];
        func_8003A440((u8 **)p->grid[0], (GsALON | GsAONE), a);
        e = p->grid[1];
        func_8003A1EC((u8 *)p, (u8 **)e, b);
        func_8003A440((u8 **)e, (GsALON | GsATWO), a - 1);
        p->field_40 = 0;
    }

    v = (u16)p->field_40 + D_8009B0D8 * 8;
    p->field_40 = v;

    if (p->field_40 >= 0x80) {
        p->display_effect_step = 0;
        func_8003A440((u8 **)p->grid[0], 0,
                      *(s8 *)(*(u8 **)&p->grid[0][0] + 0x16));
        func_80039F90((void **)p->grid[1]);
        p->field_32 &= 0xEF;
    } else {
        m = p->field_40;
        m |= (m << 8) | (m << 16);
        for (i = 2, c = &p->grid[0][2]; i >= 0; i--, c--) {
            if (*(u8 **)c != 0) {
                *(s32 *)(*(u8 **)c + 0xC) = m;
            }
            if (*(u8 **)(c + 3) != 0) {
                *(s32 *)(*(u8 **)(c + 3) + 0xC) = m;
            }
        }
    }
}
