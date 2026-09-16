#include "../types.h"
#include "campaign_flags.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "campaign_map_load_package_stage.h"

void File_RequestEgyptOverworldPackage(void){int x=0;if(Campaign_TestStoryFlag(CAMPAIGN_FLAG_TOURNAMENT_COMPLETE))x=FILE_WA_EGYPT_OVERWORLD_SECTOR_COUNT;File_RequestAsyncTransfer(0,0,x+FILE_WA_EGYPT_OVERWORLD_START_SECTOR,FILE_WA_EGYPT_OVERWORLD_SECTOR_COUNT,CampaignMap_LoadPackageStage,0,0);File_WaitForTransfers();}
