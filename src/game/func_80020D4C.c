#define D_8009B0CC_IN_DATA
#include "../types.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_lifecycle.h"
#include "graphics_frame.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_helpers.h"
#include "../unmatched.h"

/* Per-frame update for an object that orbits a fixed base position
   (field_2C/field_2E) at a constant angular step of 0x30/frame, with radius
   the object's field_28 (which also serves as its countdown timer -- it decays by
   2/frame). When the timer reaches 0: clears field_6C/update, snaps the current
   position (field_30/field_32) back to the base position in one 32-bit copy, and
   returns. Otherwise, while D_8009B0CC's bit 0 is set, spawns a companion
   slot object at the object's current position (tagged via
   sub_table_lookup_set_flag using field_16-1), then advances the angle and
   recomputes the orbit position from base + (rcos,rsin)*radius/ONE. */

void func_80020D4C(DisplayObject *arg0) {
    s16 timer;
    u16 angle;
    DisplayObject *slot;
    s32 vy;

    timer = (s16)arg0->position.h.field_28 - 2;
    arg0->position.h.field_28 = timer;
    if (timer <= 0) {
        arg0->field_6C = 0;
        arg0->update = 0;
        arg0->field_30.word = arg0->field_2C.word;
        return;
    }

    if (D_8009B0CC & 1) {
        slot = func_800400AC(func_8004002C(), 2);
        if (slot != 0) {
            func_800428A8(slot, (s16)arg0->field_30.h.field_30,
                                   (s16)arg0->field_30.h.field_32, 0,
                                   arg0->field_68, arg0->field_69, 0x11, 9,
                                   (s32) D_801AF000);
            slot->field_40.h.field_40 = (u16)arg0->field_40.h.field_40 + 0x80;
            slot->flags |= 0x28;
            slot->attribute |= (GsALON | GsAONE);
            func_800428EC(slot, (u8)arg0->field_16 - 1);
            slot->field_60 = 8;
            slot->update = func_80042BC0;
        }
    }

    angle = arg0->position.h.field_2A + 0x30;
    arg0->position.h.field_2A = angle;
    arg0->field_30.h.field_30 =
        (s16)arg0->field_2C.h.field_2C +
        rcos((s16) angle) * (s16)arg0->position.h.field_28 / ONE;
    vy = rsin((s16) arg0->position.h.field_2A) * (s16)arg0->position.h.field_28;
    arg0->field_30.h.field_32 = arg0->field_2C.h.field_2E + vy / ONE;
}
