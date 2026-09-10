#include "../types.h"
#include "file_transfer.h"

extern u8 gFile_szSuMrgPath[];
void File_RequestMainMenuPackage(void)
{
    File_RequestAsyncTransfer(
        1, gFile_szSuMrgPath, 0, 0x73, MainMenu_LoadPackageStage, 0, 0
    );
}
