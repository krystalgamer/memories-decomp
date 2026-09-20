#include "../types.h"
#include "display_object_interpolation.h"
#include "display_object_lifecycle.h"
#include "display_object_layout.h"
#include "display_object.h"
#include "trig_constants.h"

#define DISPLAY_OBJECT_BYTES(object) ((u8 *)(object))
#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->field_0C)
#define DISPLAY_OBJECT_SIGNED_HALFWORD(bytes, offset) \
    (*(s16 *)((bytes) + (offset)))

void func_8001D344(DisplayObject *object)
{
    s32 step = object->field_60;
    s32 remaining = 3;
    s32 i = 0;
    /* Byte cursors even though the object is typed: the three colour bytes
       of field_0C step against the halfwords at 0x28, 0x2A and 0x2C, which
       are position.h and field_2C.h -- two different members, so no member
       array covers the walk. */
    u8 *current = DISPLAY_OBJECT_COLOR_BYTES(object);
    u8 *target = DISPLAY_OBJECT_BYTES(object);

    for (; i < 3; i++) {
        s32 value = *current;
        s32 limit = DISPLAY_OBJECT_SIGNED_HALFWORD(
            target, (u32)&((DisplayObject *)0)->position.h.field_28);

        if (value < limit) {
            value += step;
            if (value >= limit) {
                value = limit;
                remaining--;
            }
        } else {
            value -= step;
            if (value <= limit) {
                value = limit;
                remaining--;
            }
        }
        *current = value;
        current++;
        target += 2;
    }

    if (remaining == 0) {
        object->field_6C = 0;
        object->update = 0;
    }
}

void func_8001D3C4(DisplayObject*o)
{
 if(!DisplayObject_MarkInitialized((DisplayObjectLifecycle*)o)){if((s16)o->field_2C.h.field_2C!=o->field_20.b.field_21)o->flags|=DISPLAY_OBJECT_FLAG_CLIP_TEST;o->field_2C.h.field_2E=o->field_60;DisplayObject_SavePosition((DisplayObjectSnapshot*)o);o->field_60=0;}
 if(o->field_20.b.field_21!=(s16)o->field_2C.h.field_2C)o->field_20.b.field_21+=0x80/o->field_2C.h.field_2E;
 DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)o,(s16)o->position.h.field_28,(s16)o->position.h.field_2A,o->field_60);o->field_60+=TRIG_ANGLE_HALF_TURN/o->field_2C.h.field_2E;
 if(o->field_60>=TRIG_ANGLE_HALF_TURN){u8 target=(s16)o->field_2C.h.field_2C;int position=o->position.word;o->field_20.b.field_21=target;o->field_30.word=position;if(!(target&0xFF))o->flags&=~DISPLAY_OBJECT_FLAG_CLIP_TEST;o->field_6C=0;o->update=0;}
}
