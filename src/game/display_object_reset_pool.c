#include "../types.h"

extern s16 D_800F2878[],D_800EFE38[],D_800EFE48[];extern s16 D_8009B410,D_8009B412;
void DisplayObject_ResetPool(void){int i=0;int neg=-1;s16*a; s16*b;a=D_800F2878;b=D_800EFE38;D_8009B410=0;D_8009B412=0;for(;i<7;i++){*b=neg;*a=neg;a++;b++;}{u8*p=(u8*)D_800EFE48;for(i=0x5F;i>=0;i--){*(s16*)(p+8)=0;p+=0x70;}}}
