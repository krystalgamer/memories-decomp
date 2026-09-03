#include "../types.h"

extern unsigned char*D_8009B290;extern unsigned char D_801A8000[];extern unsigned short D_8009B27C;extern void Library_UpdateCardUsedFlag(int);extern int Campaign_TestStoryFlag(int);
void func_8002E918(void){register unsigned char*p __asm__("$5")=D_8009B290;register unsigned char*next __asm__("$6")=p+2;register int high __asm__("$2");register int command __asm__("$3");int offset;D_8009B290=next;high=p[1];command=p[0];command|=high<<8;if(command&0x4000)Library_UpdateCardUsedFlag(command&0xBFFF);else{int low;D_8009B290=p+4;high=next[1];low=p[2];offset=low|(high<<8);if(Campaign_TestStoryFlag(command))D_8009B290=D_801A8000+offset;}D_8009B27C=0;}
