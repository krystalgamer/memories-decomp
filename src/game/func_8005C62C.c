#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"

void func_8005C62C(int value){while(!CdControlB(CdlSetloc,(u8 *)value,0)){}while(!CdControlB(CdlSeekP,(u8 *)value,0)){}while(!DsRead2((DslLOC *)value,0x1E0)){} }
