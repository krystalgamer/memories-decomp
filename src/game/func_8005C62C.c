#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "func_8005C62C.h"

void func_8005C62C(CdlLOC *loc){while(!CdControlB(CdlSetloc,(u8 *)loc,0)){}while(!CdControlB(CdlSeekP,(u8 *)loc,0)){}while(!DsRead2((DslLOC *)loc,0x1E0)){} }
