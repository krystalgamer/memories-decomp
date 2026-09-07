#include "../types.h"
#include "file_transfer.h"

extern void func_8003C120(void),func_800137E4(void);void func_8003C2B4(void){File_RequestAsyncTransfer(0,0,0x2115,0x32,func_8003C120,0,0);func_800137E4();File_RequestAsyncTransfer(0,0,0x2147,0x10,0,0,(int)0x80140000);func_800137E4();}
