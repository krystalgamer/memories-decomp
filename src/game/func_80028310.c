#include "../types.h"
#include "duel_effect.h"

extern unsigned short D_8009B244;extern unsigned char D_8009B248,D_8009B254;extern int func_800282E8(void),func_8003B734(void);extern unsigned char*TextBox_Create(int,int,int,int,int,int),*Dialog_OpenChoice(void*);extern void func_80039794(void),SD_SEPlayFull(int),TextBox_Destroy(void*);
void func_80028310(void){DuelEffectChannel*o;if(func_800282E8()==0){TextBox_Create(0,D_8009B244,0x10,0xB0,0x120,0x30);return;}func_80039794();o=D_800EB0F8;if((D_8009B248&0x40)==0){unsigned short f=o->flags_34;if((f&0x2000)==0)return;if((f&0x10)==0){o->field_30=Dialog_OpenChoice(o);D_8009B248|=0x40;return;}}else{if(func_8003B734()==0)return;SD_SEPlayFull(11);}TextBox_Destroy(o);D_8009B254|=0x40;}
