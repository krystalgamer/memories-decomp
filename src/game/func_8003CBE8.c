#include "../types.h"

extern u8 D_800EF668[];extern void func_80073E1C(void*,int,void*,int);extern void func_80073EAC(void);extern void Input_ResetPads(void);extern u8 D_8009B39C,D_8009B3A2;
void func_8003CBE8(void){func_80073E1C(D_800EF668,0x22,D_800EF668+0x22,0x22);func_80073EAC();D_8009B39C=0x18;D_8009B3A2=0x14;Input_ResetPads();}
