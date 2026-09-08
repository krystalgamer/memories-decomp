#include "../types.h"
#include "file_transfer.h"
#include "sound.h"

typedef struct{u16 x,y;char p4[0x16];u16 one;}Obj;
extern s16 D_8009B1D2,D_8009B1A0;extern u16 D_8009B220;extern int D_8009B134[3];
extern int func_80024E24(void),Duel_CheckRitual(int,int);extern Obj*func_8002C68C(int);extern void func_80029164(int,int);
void func_8002622C(void){if(!func_80024E24()){D_8009B1A0=Duel_CheckRitual(0,D_8009B1D2);if(D_8009B1A0){Obj*o=func_8002C68C(0x12);o->x=0xA0;o->y=0x78;o->one=1;SD_SEPlayFull(2);func_80029164(1,D_8009B1A0);}}
 else if(!((D_8009B0F4_abs&FILE_TRANSFER_REQUEST_BLOCKED_MASK)|D_8009B134[0]))D_8009B220=0;}
