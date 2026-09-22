#include "../types.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "free_duel_load_package_stage.h"
#include "main_init_free_duel_menu.h"
#include "../overlays/free_duel/free_duel.h"

#include "high_memory_addresses.h"

#ifndef MAIN_FREE_DUEL_START_SECTOR
#define MAIN_FREE_DUEL_START_SECTOR FILE_WA_FREE_DUEL_START_SECTOR
#endif

void Main_InitFreeDuelMenu(void){File_RequestAsyncTransfer(0,0,MAIN_FREE_DUEL_START_SECTOR,FILE_WA_FREE_DUEL_SECTOR_COUNT,FreeDuel_LoadPackageStage,0,0);File_WaitForTransfers();FreeDuel_Init(D_80010000);}
