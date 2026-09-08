#include "../types.h"
#include "file_constants.h"
#include "file_transfer.h"

extern void func_8003BA14(void);
void func_8003BBF8(void){File_RequestAsyncTransfer(0,0,FILE_WA_NAME_ENTRY_START_SECTOR,FILE_WA_NAME_ENTRY_SECTOR_COUNT,func_8003BA14,0,0);File_WaitForTransfers();}
