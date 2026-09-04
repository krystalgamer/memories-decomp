#include "../types.h"
#include "duel_card.h"

typedef struct{u16 x,y;}Pair;typedef struct{char p[4];u16 y;}NewObject;
extern u8 D_8009B1D5;extern u16 D_8009B220;extern Pair D_80090800[2][20];extern u8 D_800907D8[];extern char D_8009AF40[];
extern int func_80024E24(void);extern NewObject*func_8002C68C(int);extern void func_8008E870(char*,int),func_80024954(DuelCardRecord*);
void func_80026A3C(void)
{
 int i;if(!func_80024E24()){NewObject*o=func_8002C68C(0x17);int side=D_8009B1D5^1;o->y=D_80090800[side][0].y;func_8008E870(D_8009AF40,(short)o->y);}
 else{for(i=0;i<DUEL_FIELD_ROW_SIZE;i++){int position=i+D_8009B1D5*20;DuelCardRecord*e=&D_801A7AD8[D_800907D8[position]];if(e->flags&0x8000)func_80024954(e);}D_8009B220=0;}
}
