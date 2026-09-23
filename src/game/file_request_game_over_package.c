#include "../types.h"
#include "file_transfer.h"

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef GAME_OVER_PACKAGE_START_SECTOR
#define GAME_OVER_PACKAGE_START_SECTOR 0x2157
#endif

void File_RequestGameOverPackage(void){File_RequestAsyncTransfer(0,0,GAME_OVER_PACKAGE_START_SECTOR,0x32,GameOver_LoadPackageStage,0,0);File_WaitForTransfers();}
