#include "../types.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"
#include "../psyq/libgte.h"
#include "trig_constants.h"
#include "display_object_position.h"
#include "func_8003A990.h"

/* Eases one display-effect record from its 0x34/0x36 position to the
   0x40/0x42 destination over a quarter turn of cosine, then clears the step
   byte. The record is a MenuRecord, the element type of D_800EB010;
   display_effect_step_table.c hands this callback a u8 *, so the parameter
   stays that and the record is taken through a local.

   Every use goes through that local, including the two calls that want a
   u8 * again. That is not tidiness: leaving `p` live alongside `r` makes
   GCC 2.8.1 hold both in callee-saved registers, which grows the frame by
   eight bytes and the function with it. One name, one register. */
void func_8003A990(u8 *p)
{
    MenuRecord *r = (MenuRecord *)p;
    s32 d;
    s32 t;
    s32 c;
    s32 dx;
    s32 dy;

    if (func_80039F1C((DisplayEffectState *)r) == 0) {
        r->field_48 = TRIG_ANGLE_QUARTER_TURN;
        d = TRIG_ANGLE_QUARTER_TURN / r->field_44;
        r->field_4A = d;
        if (d >= 0) {
            r->field_48 = 0;
        }
        r->field_44 = *(u16 *)&r->field_40 - r->field_34;
        r->field_46 = *(u16 *)&r->field_42 - r->field_36;
    }

    t = *(u16 *)&r->field_48 + *(u16 *)&r->field_4A;
    r->field_48 = t;

    if ((u16)(t - 1) >= TRIG_ANGLE_QUARTER_TURN - 1) {
        func_8003A95C((DisplayPositionGroup *)r, r->field_40, r->field_42);
        r->display_effect_step = 0;
    } else {
        c = rcos((s16)t);
        dx = c * r->field_44 / ONE;
        dy = c * r->field_46 / ONE;
        if (r->field_4A < 0) {
            dx = r->field_44 - dx;
            dy = r->field_46 - dy;
        }
        func_8003A95C(
            (DisplayPositionGroup *)r,
            (s16)(*(u16 *)&r->field_40 - dx),
            (s16)(*(u16 *)&r->field_42 - dy)
        );
    }
}
