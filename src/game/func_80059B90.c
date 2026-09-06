#include "../types.h"
#include "../psyq/memory.h"

typedef struct{unsigned char a[8],b[80],c[32];}Local;extern void func_800580D4(int,int,void*,void*),func_8008A4A0(void*,void*);void func_80059B90(short value,short*out){Local l;unsigned short v;memset(l.a,0,8);*(short*)l.a=value;func_800580D4(0,15,l.a,l.b);func_8008A4A0(l.b,l.c);v=*(volatile unsigned short*)(l.c+20);out[0]=v;v=*(volatile unsigned short*)(l.c+24);out[1]=v;v=*(volatile unsigned short*)(l.c+28);out[3]=0;out[2]=v;}
