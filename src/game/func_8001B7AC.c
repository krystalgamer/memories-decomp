#include "../types.h"
#include "card_constants.h"

typedef struct Object{char p0[4];struct Object*child;char p8[6];s8 index;char pF[6];u8 count;}Object;
typedef struct Spawned{char p0[8];u16 flags;char pA[6];void*callback;char p14[0x1C];s16 x,y;char p34[0x36];u8 mode;}Spawned;
typedef struct{Spawned*base;Spawned*child;char p8;u8 active;char pA[2];}Slot;
extern Slot D_800EA030[HAND_SIZE];extern void*func_8004002C(void);extern Spawned*func_800400AC(void*,int);extern void func_80040510(),func_80042918(Spawned*),func_800428EC(Spawned*,int),SD_SEPlayFull(int);
void func_8001B7AC(Object*arg)
{
 register Object*object=arg;register Slot*slot=&D_800EA030[object->index];register Spawned*child;
 slot->base->y-=4;child=func_800400AC(func_8004002C(),1);func_80040510(child,slot->base->x,slot->base->y,0x10,0x10,object->count<<4,0xB8,0xB,0x250,0xFC);
 func_80042918(child);func_800428EC(child,(s8)(((u8*)slot->base)[0x16]+1));slot->child=child;object->count++;slot->active=object->count;SD_SEPlayFull(0x2F);
}
