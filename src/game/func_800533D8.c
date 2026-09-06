#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

typedef struct{u16 flags;u8 pad[22];}Record;extern Record D_800F2B50[10];extern u8 D_8009AF9B,D_8009AF94;extern s8 D_8009AF9A;extern s32 D_8009AF9C;extern void func_8005611C(int),func_80059AE0(int);void func_800533D8(void){s16 table[256],packet[4];register s16*p=&table[1];register s32 fill=0xffff;register s32 counter=254;s32 i;table[0]=0;do{*p=fill;counter--;p++;}while(counter>=0);packet[0]=0x200;packet[1]=0xF0;packet[2]=0x100;packet[3]=1;while(IsIdleGPU(3)){}while(LoadImage2((RECT *)packet,(u32 *)table)){}while(IsIdleGPU(3)){}func_8005611C(0);func_8005611C(1);func_8005611C(2);for(i=0;i<10;i++)D_800F2B50[i].flags&=0xfffe;D_8009AF9B=0;D_8009AF9C=0;func_80059AE0(0x8000);D_8009AF94=0;D_8009AF9A=-1;}
