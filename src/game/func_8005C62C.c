#include "../types.h"
#include "../psyq/libds.h"

extern int func_8007E7F0(int,int,int);
void func_8005C62C(int value){while(!func_8007E7F0(2,value,0)){}while(!func_8007E7F0(0x16,value,0)){}while(!DsRead2((DslLOC *)value,0x1E0)){} }
