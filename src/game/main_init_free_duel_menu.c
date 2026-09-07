#include "../types.h"
#include "file_transfer.h"

extern void func_8003B808(void);extern void func_800137E4(void);extern void func_8016824C(void*);extern void *D_80010000;
void Main_InitFreeDuelMenu(void){File_RequestAsyncTransfer(0,0,0x1E88,0x57,func_8003B808,0,0);func_800137E4();func_8016824C(D_80010000);}
