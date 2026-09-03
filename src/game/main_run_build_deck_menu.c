#include "../types.h"

extern unsigned char D_8009B26C,D_8009B269;extern unsigned char gDuel_awPlayerDeck[],D_8009B2F8[];extern unsigned int D_80010000[];extern void func_800323F8(unsigned int,void*,int,int),func_80015A00(void),func_8003FF34(void),Fade_WaitOut(void);extern int func_80033BE8(void);
void Main_RunBuildDeckMenu(void){unsigned char flags=D_8009B26C;if((flags&0x40)==0){D_8009B26C=flags|0x40;func_800323F8(D_80010000[0],gDuel_awPlayerDeck,0,D_8009B2F8[0]);func_80015A00();}else if(func_80033BE8()==0){unsigned char value;func_8003FF34();Fade_WaitOut();value=D_8009B269;__asm__ volatile("nop");D_8009B26C=value;}}
