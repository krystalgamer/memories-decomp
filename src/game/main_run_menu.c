#include "../types.h"

extern unsigned char D_8009B26C,D_8009B268,D_8009B26D,D_8009B269;extern void func_8005B85C(void),func_800137E4(void),func_80039E9C(void),func_8018001C(int,int),func_80015A00(void),func_8008E590(void),func_8003FF34(void),Fade_WaitOut(void),func_80180DD0(void),func_8002D458(int);extern int func_80180390(void);
void Main_RunMenu(void){unsigned char f=D_8009B26C;int r;if((f&0x40)==0){D_8009B26C=f|0x40;func_8005B85C();func_800137E4();func_80039E9C();func_8018001C(D_8009B268,D_8009B26D);func_80015A00();}func_8008E590();r=func_80180390();if(r>=0){func_8003FF34();Fade_WaitOut();func_80180DD0();func_8002D458(r);D_8009B269=8;}}
