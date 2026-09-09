#include "../types.h"
#include "card_list_sort.h"
#include "card_constants.h"
#include "func_80031EE4.h"
void func_80031EE4(unsigned char*base,int index){register unsigned char*p __asm__("$6")=base+index;register unsigned int raw __asm__("$7")=p[0x5D97];int c=raw&255;if(c==0){unsigned char*e=base+4;(*(int*)(base+0x5A9C))++;p[0x5D97]++;do{int v=*(short*)(e+4);e+=16;if(v==index)break;}while(1);e-=16;__asm__ volatile("" : "+r"(e));e[0xD]=1;func_80032C48(base+4);}else if(c!=CARD_CHEST_QUANTITY_MAX){register int next __asm__("$2")=raw+1;p[0x5D97]=next;(*(int*)(base+0x5A9C))++;}}
