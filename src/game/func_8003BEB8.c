#include "../types.h"
#include "file_transfer.h"

extern void func_8003BD14(void);extern void func_800137E4(void);
void func_8003BEB8(void){File_RequestAsyncTransfer(0,0,0x1F2F,0x56,func_8003BD14,0,0);func_800137E4();}
