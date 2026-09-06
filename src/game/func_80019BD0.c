#include "../types.h"
#include "duel_card.h"

typedef struct Object{char p0[8];u16 flags;char pA[0x16];u8 alt,current,mode;char p23;void*callback;char p28[6];s16 limit;char p30[0x37];u8 field67;char p68[2];u8 index;char p6B;u8 active;}Object;
extern int func_80042B98(Object*);
void func_80019BD0(Object*o)
{
 int current;if(!func_80042B98(o)){o->limit=0x40;o->flags|=4;if(o->current)o->limit=0xC0;}
 current=o->current+8;o->current=current;if(o->active&0x40){if((signed char)current>=0){o->current=0;D_801A7AD8[o->index].flags&=~DUEL_CARD_FLAG_DISPLAY_MARKER;if((o->mode|o->alt)==0)o->flags&=~4;o->active=0;o->callback=0;}}
 else if(o->current>=o->limit){o->active|=0x40;o->current=0xC0;o->field67=0;}
}
