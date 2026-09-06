#include "../types.h"
#include "display_object_layout.h"

extern s16 D_800F2878[],D_800EFE38[],D_800EFE48[];extern s16 D_8009B410,D_8009B412;
void DisplayObject_ResetPool(void){int i=0;int neg=-1;s16*a; s16*b;a=D_800F2878;b=D_800EFE38;D_8009B410=0;D_8009B412=0;for(;i<DISPLAY_OBJECT_LIST_COUNT;i++){*b=neg;*a=neg;a++;b++;}{u8*p=(u8*)D_800EFE48;for(i=DISPLAY_OBJECT_POOL_CAPACITY-1;i>=0;i--){*(s16*)(p+8)=0;p+=DISPLAY_OBJECT_RECORD_SIZE;}}}
