#include "../types.h"
#include "duel_card.h"

typedef struct{char p0[8];u16 flags;char pA[0x17];u8 current,mode;char p23;void*callback;s16 dx,dy;char p2C[0x3E];u8 index;char p6B;u8 active;}Object;
extern int func_80042B98(Object*);
void func_8001D240(Object*o)
{
 if(!func_80042B98(o)){o->flags|=4;if(!o->mode){o->dx=-8;o->dy=0xC0;}else{o->dx=8;o->dy=0;}}
 o->mode+=o->dx;if(o->mode<0xC0){DuelCardRecord*e=&D_801A7AD8[o->index];e->flags|=DUEL_CARD_FLAG_DEFENSE_POSITION;o->mode=o->dy;
  if(!o->mode){e->flags&=~DUEL_CARD_FLAG_DEFENSE_POSITION;if(!o->current)o->flags&=~4;}o->active=0;o->callback=0;}
}
