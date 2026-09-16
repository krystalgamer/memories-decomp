#include "../types.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "password_load_package_stage.h"

void File_RequestPasswordPackage(void){File_RequestAsyncTransfer(0,0,FILE_WA_PASSWORD_START_SECTOR,FILE_WA_PASSWORD_SECTOR_COUNT,Password_LoadPackageStage,0,0);File_WaitForTransfers();}
