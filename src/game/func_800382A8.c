#include "../types.h"
#include "func_800382A8.h"

void func_800382A8(u8*o){register u8*obj asm("$5")=o;register u8**p asm("$3");register u8*q asm("$2");register unsigned v asm("$4");*(u16*)(obj+0x34)&=0xfeff;p=&((u8**)obj)[*(s8*)(obj+0x58)];q=*p;v=*q++;*p=q;switch(v){case 1:obj[0x5a]=8;obj[0x5b]=8;break;case 2:obj[0x5a]=8;obj[0x5b]=12;break;}if(v==1)*(u16*)(obj+0x34)|=0x100;}
