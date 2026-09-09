#include "../types.h"
#include "display_object_api.h"
#include "display_object_lifecycle.h"

typedef struct{char p[0x24];void*cb;s16 x;u16 y,dy,z,oldx;s16 oldy;char p34[0x2C];s16 speed;char p62[0xA];u8 active;}Obj;
extern void func_80043178(Obj*),func_80043230(Obj*,int,int,int);
void func_80018C34(Obj*o){if(!func_80042B98((DisplayObjectLifecycle*)o)){o->dy=0x400/o->speed;o->x=o->oldx-0x140;func_80043178(o);o->speed=-0x400;}
 func_80043230(o,o->x,o->oldy,o->speed);o->speed+=o->dy;if(o->speed>=0){o->cb=0;o->active=0;o->oldx=o->x;}}
