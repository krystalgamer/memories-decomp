#include "../types.h"

extern unsigned char D_8009B2B2,D_8009B2EB,D_8009B2F0;extern void*D_8009B2E4;extern void func_8003B6AC(int,int),func_80039A14(void*),func_800427DC(void*,int),func_800300C8(void),func_80015A00(void);extern unsigned char*TextBox_Create(int,int,int,int,int,int);extern void*func_8004002C(void),*func_800400AC(void*,int);
void func_80030198(void){unsigned char*a,*b;D_8009B2B2=0x80;D_8009B2EB=0;D_8009B2F0=0;func_8003B6AC(1,1);a=TextBox_Create(1,15,16,16,0x120,0xA0);a[0x5A]=16;a[0x5B]=16;func_80039A14(a);b=func_800400AC(func_8004002C(),4);D_8009B2E4=b;func_800427DC(b,0);b[0x45]=0xC0;b[0x3D]=0xC0;b[0x35]=0xC0;b[0x2D]=0xC0;func_800300C8();func_80015A00();}
