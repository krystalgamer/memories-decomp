#include "../types.h"
#include "../psyq/libds.h"

extern int CdControlB(int,int,int);
void func_8005C62C(int value){while(!CdControlB(2,value,0)){}while(!CdControlB(0x16,value,0)){}while(!DsRead2((DslLOC *)value,0x1E0)){} }
