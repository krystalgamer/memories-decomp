#include "../types.h"
#include "display_object_api.h"

extern unsigned char D_8009B34E;extern void*TextBox_Create(int,int,int,int,int,int);extern void func_80039A14(void*),func_80042918(void*),func_800428EC(void*,int);void func_800610E0(int mode){unsigned char*parent,*obj;D_8009B34E=mode;parent=TextBox_Create(3,10,0x30,0x78,0x120,0x10);parent[0x59]=0x12;func_80039A14(parent);obj=func_800400AC(func_8004002C(),2);func_800404CC(obj,*(short*)(parent+0x3C)+mode*160,*(short*)(parent+0x40),0,4,0xE,0xC,0x208);*(unsigned short*)(obj+8)|=0x28;func_80042918(obj);func_800428EC(obj,0x11);*(void**)(parent+0x2C)=obj;}
