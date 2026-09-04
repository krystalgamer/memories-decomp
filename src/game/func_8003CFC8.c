#include "../types.h"

extern u16 func_8003CEB8(void*,int);extern unsigned D_8009AF64,D_8009AF68;extern unsigned SaveData_NextMaskWord(void);void func_8003CFC8(unsigned char*p){register int i;register unsigned*dst;u16 v=func_8003CEB8(p+0x400,0x204);dst=(unsigned*)(p+0x624);i=8;*(u16*)(p+0x606)=v;*(u16*)(p+0x604)=v;{unsigned seed=v|(v<<16);D_8009AF68=seed;D_8009AF64=seed;}do{--i;*dst=SaveData_NextMaskWord();dst--;}while(i);}
