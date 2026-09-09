#include "../types.h"
#include "func_80043178.h"
#include "display_object_interpolation.h"
#include "display_object_api.h"
#include "display_object_lifecycle.h"
#include "display_object_layout.h"

typedef struct{char p0[8];u16 flags;char pA[0x17];u8 current,mode;char p23;void*callback;s16 x,y,target,step;int oldPosition;char p34[0x2C];s16 speed;char p62[0xA];u8 active;}Object;

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

void func_8001D3C4(Object*o)
{
 if(!func_80042B98((DisplayObjectLifecycle*)o)){if(o->target!=o->current)o->flags|=DISPLAY_OBJECT_FLAG_CLIP_TEST;o->step=o->speed;func_80043178((DisplayObjectSnapshot*)o);o->speed=0;}
 if(o->current!=o->target)o->current+=0x80/o->step;
 func_8004318C((u8*)o,o->x,o->y,o->speed);o->speed+=0x800/o->step;
 if(o->speed>=0x800){u8 target=o->target;int position=*(int*)&o->x;o->current=target;o->oldPosition=position;if(!(target&0xFF))o->flags&=~DISPLAY_OBJECT_FLAG_CLIP_TEST;o->active=0;o->callback=0;}
}
