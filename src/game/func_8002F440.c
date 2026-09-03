#include "../types.h"

extern unsigned char*D_8009B290;extern unsigned char D_800E9ECE[],D_800E9ECF[];extern unsigned short D_8009B27C;extern int func_8002E3B4(void);extern void Fade_InitOut(void);
void func_8002F440(void){unsigned char*base=D_800E9ECE;int command;if(func_8002E3B4()==0){command=*D_8009B290++;Fade_InitOut();if(command&0x3F)D_800E9ECF[0]=command&0x3F;if((command&0x80)==0)goto clear;goto end;}if(base[0]&0x80)goto end;clear:D_8009B27C=0;end:;}
