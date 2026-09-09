#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_init_entry.h"

extern unsigned short D_80090E58[];
DuelEffectChannel*DuelEffect_InitEntry(int index,int value,int flags){register int offset=index<<1;DuelEffectChannel*e;unsigned short*range;e=&D_800EB0F8[index];flags|=0x8000;e->field_5A=8;e->field_5B=12;e->field_53=2;range=(unsigned short*)((unsigned char*)D_80090E58+offset);e->index_57=index;e->field_36=value;e->field_54=0;e->flags_34=flags;e->field_38=0;e->field_3A=0;e->field_59=0;e->field_61=0;e->range_start_5C=range[0];e->range_count_5E=range[1]-range[0];return e;}
