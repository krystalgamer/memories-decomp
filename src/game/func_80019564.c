#include "../types.h"
#include "display_object_api.h"

typedef struct{char p[4];int flags;char p8[0x10];u16 a,b;char p1c[0x14];s16 x,y;char p34[0x14];u16 c,d;}Obj;
extern void func_80040510(),func_80042918(Obj*);
Obj*func_80019564(Obj*src){Obj*o=func_800400AC(func_8004002C(),1);func_80040510(o,src->x,src->y,0x8C,0xC4,0,0,0x15,0,0);
 o->a=0x46;o->c=0x46;o->b=0x62;o->d=0x62;func_80042918(o);o->flags|=0x2000000;return o;}
