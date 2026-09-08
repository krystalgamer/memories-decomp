#include "../types.h"
#include "display_object_api.h"

#include "display_object_helpers.h"
extern void func_8002A9C0(void);
unsigned char*func_8002ABB4(unsigned char*src,int add){unsigned char*o=func_800400AC(func_8004002C(),6);*(int*)(o+0x48)=*(int*)(src+0x48);*(int*)(o+0x18)=*(int*)(src+0x18);*(short*)(o+0x60)=0x100;*(short*)(o+0x3C)=*(short*)(o+0x18)*2;*(short*)(o+0x3E)=*(short*)(o+0x1A)*2;*(int*)(o+0x30)=*(int*)(src+0x30);*(int*)(o+0x20)=*(int*)(src+0x20);*(int*)(o+0x44)=*(int*)(src+0x44);*(int*)(o+0xC)=0;func_80042918(o);func_800428EC(o,(signed char)(src[0x16]+add));*(int*)(o+0x10)=1;*(void**)(o+0x4C)=func_8002A9C0;*(unsigned int*)(o+4)=(*(unsigned int*)(o+4)|0x50000000)&0xF7FFFFFF;return o;}
