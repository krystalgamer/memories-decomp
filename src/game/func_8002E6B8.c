#include "../types.h"
#include "script_state.h"
#include "func_8002E6B8.h"

extern unsigned short D_8009B270;
void func_8002E6B8(void){register unsigned char*p=D_8009B290;register unsigned char*p2=p+2;register unsigned char*p4;unsigned short value;D_8009B2AA=0;D_8009B2A8=0;D_8009B290=p2;value=p[0]|(p[1]<<8);D_8009B270=value;if(value&0x8000){p4=p+4;D_8009B290=p4;D_8009B2A8=p[2]|(p2[1]<<8);D_8009B290=p+6;D_8009B2AA=p[4]|(p4[1]<<8);}D_8009B27C=5;}
