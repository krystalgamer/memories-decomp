#include "../types.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "func_8003BD14.h"

void func_8003BEB8(void){File_RequestAsyncTransfer(0,0,FILE_WA_PASSWORD_START_SECTOR,FILE_WA_PASSWORD_SECTOR_COUNT,func_8003BD14,0,0);File_WaitForTransfers();}
