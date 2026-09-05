#include "../types.h"
#include "input.h"

extern signed char D_8009B32C;extern unsigned short gInput_wPad1Held[],gInput_wPad1Pressed[];extern unsigned char*Dialog_OpenChoice(unsigned char*);extern void SD_SEPlayFull(int),func_8004036C(void*);
void func_800375A4(unsigned char*o){unsigned char f=o[0x51];if((f&0x80)==0){o[0x51]=f|0x80;D_8009B32C=10;*(void**)(o+0x30)=Dialog_OpenChoice(o);}else{if(gInput_wPad1Held[0]&0x80){D_8009B32C--;if(D_8009B32C<0)D_8009B32C=0;}else D_8009B32C=10;if(D_8009B32C!=0&&!(gInput_wPad1Pressed[0]&PAD_BUTTON_CONFIRM_MASK))return;SD_SEPlayFull(11);o[0x51]=2;func_8004036C(*(void**)(o+0x30));*(void**)(o+0x30)=0;}}
