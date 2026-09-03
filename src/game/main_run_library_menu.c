#include "../types.h"

extern unsigned char D_8009B26C;extern unsigned char D_8009B0C0[];extern void func_8002BFCC(void),func_80015A00(void),func_8002BAB4(void),func_8003FF34(void),Fade_WaitOut(void),func_800134B4(void),func_8004763C(void),func_80047AD0(int),func_80012D84(int),func_800137E4(void);
void Main_RunLibraryMenu(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;func_8002BFCC();func_80015A00();}else{func_8002BAB4();if((D_8009B26C&0x40)==0){D_8009B0C0[0]=0;func_8003FF34();Fade_WaitOut();func_800134B4();func_8004763C();func_80047AD0(2);func_80012D84(4);func_800137E4();}}}
