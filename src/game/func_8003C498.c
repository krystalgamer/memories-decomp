#include "../types.h"
#include "file_transfer.h"

extern void func_8003C328(void);extern void func_800137E4(void);
void func_8003C498(void){File_RequestAsyncTransfer(0,0,0x2157,0x32,func_8003C328,0,0);func_800137E4();}
