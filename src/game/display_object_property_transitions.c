#include "../types.h"
#include "func_80043178.h"
#include "display_object_interpolation.h"
#include "display_object_api.h"
#include "display_object_lifecycle.h"
#include "display_object_layout.h"
#include "display_object.h"

void func_8001D344(u8 *object)
{
    s32 step = *(s16 *)(object + 0x60);
    s32 remaining = 3;
    s32 i = 0;
    u8 *current = object + 0xC;
    u8 *target = object;

    for (; i < 3; i++) {
        s32 value = *current;
        s32 limit = *(s16 *)(target + 0x28);

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
        object[0x6C] = 0;
        *(s32 *)(object + 0x24) = 0;
    }
}

void func_8001D3C4(DisplayObject*o)
{
 if(!func_80042B98((DisplayObjectLifecycle*)o)){if(*(s16*)&o->field_2C.h.field_2C!=o->field_20.b.field_21)o->flags|=DISPLAY_OBJECT_FLAG_CLIP_TEST;o->field_2C.h.field_2E=o->field_60;func_80043178((DisplayObjectSnapshot*)o);o->field_60=0;}
 if(o->field_20.b.field_21!=*(s16*)&o->field_2C.h.field_2C)o->field_20.b.field_21+=0x80/o->field_2C.h.field_2E;
 func_8004318C((DisplayObjectPosition *)o,*(s16*)&o->position.h.field_28,*(s16*)&o->position.h.field_2A,o->field_60);o->field_60+=0x800/o->field_2C.h.field_2E;
 if(o->field_60>=0x800){u8 target=*(s16*)&o->field_2C.h.field_2C;int position=o->position.word;o->field_20.b.field_21=target;o->field_30.word=position;if(!(target&0xFF))o->flags&=~DISPLAY_OBJECT_FLAG_CLIP_TEST;o->field_6C=0;o->update=0;}
}
