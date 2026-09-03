#include "../types.h"

extern unsigned char D_8009B26C,D_8009B269,D_8009B268,D_8009B26D;extern unsigned char D_800E9DC0[];extern void func_8003C498(void),func_8003C950(void),func_8003FF34(void),Fade_WaitOut(void),func_8008FB8C(void*,int);extern int func_8003CA5C(void);
void Main_RunGameOver(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;func_8003C498();func_8003C950();}if(func_8003CA5C()==0){unsigned char v;func_8003FF34();Fade_WaitOut();v=D_8009B269;__asm__ volatile("nop");D_8009B26C=v;if(v){D_8009B268=1;D_8009B26D=0;D_8009B26C=8;func_8008FB8C(D_800E9DC0,1);}}}
