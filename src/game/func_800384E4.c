#include "../types.h"
#include "func_800384E4.h"

void func_800384E4(u8*object){register u8*obj;register u8**stream;register u8*current;register unsigned int value;obj=object;*(u16*)(obj+0x34)&=0xEFFF;stream=&((u8**)obj)[*(s8*)(obj+0x58)];current=*stream;value=*current;current++;*stream=current;if(value)*(u16*)(obj+0x34)|=0x1000;}
