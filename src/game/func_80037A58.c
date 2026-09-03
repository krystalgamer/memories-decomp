#include "../types.h"

extern unsigned short D_8009B146[],D_8009B148[],D_8009B348[2];extern signed short D_8009B322;extern unsigned int D_8009B0CC[];extern int func_80036D3C(void*),rand(void);
void func_80037A58(unsigned char*o){unsigned char f=o[0x51];if((f&0x80)==0){o[0x51]=f|0x80;D_8009B322=func_80036D3C(o);D_8009B348[0]=D_8009B146[0];D_8009B348[1]=D_8009B148[0];}if(D_8009B0CC[0]&1){D_8009B146[0]=D_8009B348[0]+((rand()&7)-4);D_8009B148[0]=D_8009B348[1]+((rand()&3)-2);}D_8009B322--;if(D_8009B322==0){D_8009B146[0]=D_8009B348[0];D_8009B148[0]=D_8009B348[1];o[0x51]=0;}}
