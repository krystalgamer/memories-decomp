#include "../types.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "name_entry_load_package_stage.h"

void File_RequestNameEntryPackage(void){File_RequestAsyncTransfer(0,0,FILE_WA_NAME_ENTRY_START_SECTOR,FILE_WA_NAME_ENTRY_SECTOR_COUNT,NameEntry_LoadPackageStage,0,0);File_WaitForTransfers();}
