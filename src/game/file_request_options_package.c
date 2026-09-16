#include "../types.h"
#include "file_transfer.h"

void File_RequestOptionsPackage(void){File_RequestAsyncTransfer(0,0,0x2115,0x32,Options_LoadPackageStage,0,0);File_WaitForTransfers();File_RequestAsyncTransfer(0,0,0x2147,0x10,0,0,(int)0x80140000);File_WaitForTransfers();}
