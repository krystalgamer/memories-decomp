#include "../types.h"

extern int rand(void);int func_8004149C(u8*p,u8*t){int i=rand()%t[0];unsigned hi,lo,base;t+=i*2+1;hi=t[1];lo=t[0];base=*(unsigned*)(p+0x54);*(u16*)(p+0x58)=0;*(unsigned*)(p+0x50)=base+((hi<<8)|lo);return 1;}
