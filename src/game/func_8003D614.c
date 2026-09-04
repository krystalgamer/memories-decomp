#include "../types.h"
#include "duel_effect.h"

extern u8 D_8009B3C1;extern void func_80043178(void*),func_8004036C(void*),func_80043230(void*,int,int,int),TextBox_SetPos(void*,int,int);void func_8003D614(u8*s){u8*o;DuelEffectChannel*entry;if(!(D_8009B3C1&0x80)){o=*(u8**)s;D_8009B3C1|=0x80;func_80043178(o);*(u16*)(o+0x60)=0x400;}entry=&D_800EB0F8[s[0x1a]];o=*(u8**)s;if(o){*(u16*)(o+0x60)-=0x40;if(*(s16*)(o+0x60)<=0){func_8004036C(o);*(void**)s=0;}else{func_80043230(o,0x20,-0x40,*(s16*)(o+0x60));TextBox_SetPos(entry,*(s16*)(o+0x30),*(s16*)(o+0x32));}}o=*(u8**)(s+4);if(o){*(u16*)(o+0x4a)+=8;*(u16*)(o+0x48)+=8;if(*(s16*)(o+0x48)>=0xc0){func_8004036C(o);*(void**)(s+4)=0;}}if(!*(void**)s&&!*(void**)(s+4))D_8009B3C1=0;}
