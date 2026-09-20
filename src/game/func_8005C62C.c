#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "func_8005C62C.h"

#define CD_LOCATION_BYTES(location) ((u8 *)(location))
#define DS_LOCATION_VIEW(location) ((DslLOC *)(location))

void Movie_SeekAndStartRead(CdlLOC *loc){while(!CdControlB(CdlSetloc,CD_LOCATION_BYTES(loc),0)){}while(!CdControlB(CdlSeekP,CD_LOCATION_BYTES(loc),0)){}while(!DsRead2(DS_LOCATION_VIEW(loc),0x1E0)){} }
