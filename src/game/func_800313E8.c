#include "../types.h"
#include "display_object_lifecycle.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "sound.h"
#include "display_object_api.h"
#include "display_object.h"
#include "display_object_helpers.h"

void func_800313E8(DisplayObject *p) {
    s32 v;
    s32 t;

    if (func_80042B98((DisplayObjectLifecycle *)p) == 0) {
        p->field_44.word = 0;
        p->field_60 = 0;
        p->attribute = p->attribute & ~GsROTOFF;
        p->field_6C = p->field_6C | 0x40;
        SD_SEPlayFull(0xA);
    }

    v = p->field_6C;

    if ((v & 0x40) != 0) {
        t = *(u16 *)&p->field_60 + 1;
        p->field_60 = t;
        p->field_44.h.field_46 = (s16)t * 512;
        p->field_44.h.field_44 = (s16)t * 512;
        if (p->field_60 >= 8) {
            DisplayObject_ResetVelocity((DisplayObjectVelocity *)p);
            p->field_34.h.field_36 =
            ((*(s16 *)&p->field_18 -
              *(s16 *)&p->field_30.h.field_30) << 8) / 12;
            p->field_6C = p->field_6C & 0xBF;
            p->field_60 = 0xC;
            p->field_38.h.field_38 =
            ((*(s16 *)&p->field_1A -
              *(s16 *)&p->field_30.h.field_32) << 8) / 12;
        }
    } else if ((v & 0x20) != 0) {
        t = *(u16 *)&p->field_60 - 1;
        p->field_60 = t;
        p->field_44.h.field_46 = (s16)t * 512;
        p->field_44.h.field_44 = (s16)t * 512;
        if (p->field_60 == 0) {
            func_8004036C(p);
        }
    } else {
        DisplayObject_StepPositionXY((DisplayObjectVelocity *)p);
        t = *(u16 *)&p->field_60 - 1;
        p->field_60 = t;
        if (p->field_60 <= 0) {
            p->field_30.word = *(s32 *)&p->field_18;
            p->field_60 = 8;
            p->field_6C = p->field_6C | 0x20;
        }
    }
}
