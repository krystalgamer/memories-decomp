#include "../types.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "func_8003B808.h"

extern void func_8016824C(void*);extern void *D_80010000;
void Main_InitFreeDuelMenu(void){File_RequestAsyncTransfer(0,0,FILE_WA_FREE_DUEL_START_SECTOR,FILE_WA_FREE_DUEL_SECTOR_COUNT,func_8003B808,0,0);File_WaitForTransfers();func_8016824C(D_80010000);}
