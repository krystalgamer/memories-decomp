#include "../types.h"
#include "file_transfer.h"

void File_RequestGameOverPackage(void){File_RequestAsyncTransfer(0,0,0x2157,0x32,GameOver_LoadPackageStage,0,0);File_WaitForTransfers();}
