#include "../types.h"
#include "display_object_lifecycle.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "sound.h"
#include "display_object_api.h"
#include "display_object_helpers.h"
#include "display_object.h"

/* The update callback func_80031574 (src/candidates/func_80031574.c)
   installs: a card's stat panel
   scales up over eight frames, then flies to its target at 0x18/0x1A, waits,
   and scales back down before releasing itself. field_6C carries the phase --
   0x40 while it grows, 0x20 while it shrinks, neither while it travels.

   Two views of one object, on purpose. DisplayObject names the scale word at
   0x44, the countdown at 0x60, the attribute and the phase byte;
   DisplayObjectVelocity names 0x30/0x32 and 0x36/0x38 as position and
   velocity, which is what this function computes there -- it seeds both
   halves with (target - position) << 8 / 12 and then hands the record to
   DisplayObject_StepPositionXY, whose parameter is that view. Taking the
   velocity names from the view that owns them is what
   display_object_helpers.h asks a caller to do rather than pushing one
   reading of 0x36/0x38 onto the shared record, where it has five. */
void func_800313E8(DisplayObject *o) {
    DisplayObjectVelocity *motion = (DisplayObjectVelocity *)o;
    s32 v;
    s32 t;

    if (func_80042B98((DisplayObjectLifecycle *)o) == 0) {
        o->field_44.word = 0;
        o->field_60 = 0;
        o->attribute = o->attribute & ~GsROTOFF;
        o->field_6C = o->field_6C | 0x40;
        SD_SEPlayFull(0xA);
    }

    v = o->field_6C;

    if ((v & 0x40) != 0) {
        t = *(u16 *)&o->field_60 + 1;
        o->field_60 = t;
        o->field_44.h.field_46 = (s16)t * 512;
        o->field_44.h.field_44 = (s16)t * 512;
        if (o->field_60 >= 8) {
            DisplayObject_ResetVelocity(motion);
            motion->velocity_x = ((*(s16 *)&o->field_18 - motion->x) << 8) / 12;
            o->field_6C = o->field_6C & 0xBF;
            o->field_60 = 0xC;
            motion->velocity_y = ((*(s16 *)&o->field_1A - motion->y) << 8) / 12;
        }
    } else if ((v & 0x20) != 0) {
        t = *(u16 *)&o->field_60 - 1;
        o->field_60 = t;
        o->field_44.h.field_46 = (s16)t * 512;
        o->field_44.h.field_44 = (s16)t * 512;
        if (o->field_60 == 0) {
            func_8004036C(o);
        }
    } else {
        DisplayObject_StepPositionXY(motion);
        t = *(u16 *)&o->field_60 - 1;
        o->field_60 = t;
        if (o->field_60 <= 0) {
            o->field_30.word = *(s32 *)&o->field_18;
            o->field_60 = 8;
            o->field_6C = o->field_6C | 0x20;
        }
    }
}
