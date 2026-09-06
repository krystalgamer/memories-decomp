#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

typedef struct{char p[4];u16 x;char p6[8];u8 value;char pF[8];u8 index;char p18[0x52];}Obj;
extern int D_800E9D90[];extern void Text_EncodeDecimalDigits(int,int,u8*);
void func_80016D2C(Obj*a,Obj*b,int source,int count){
 u8 temp[8];int i;Text_EncodeDecimalDigits(source,count,temp);
 for(i=count-1;i>=0;i--){b->value=temp[i]<<3;GsSortFastSprite((GsSPRITE*)b,(GsOT*)D_800E9D90[a->index],*(u16*)((char*)a+0x14));b->x+=8;}
}
