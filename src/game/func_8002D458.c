#include "../types.h"
#include "main_mode_state.h"
#include "duel_effect_mode_7.h"
#include "file_transfer.h"
#include "func_8002D458.h"
#include "main_services.h"

extern s16 gCampaignSavedSceneIndex[];
extern u8 gCampaignSceneIndex __attribute__((section(".data")));
extern u8 D_8009B3D4 __attribute__((section(".data")));
extern u8 D_8009B0D1 __attribute__((section(".data")));
extern u8 gFreeDuel_bReturnFlags __attribute__((section(".data")));

void func_8016AA6C(void);

void func_8002D458(s32 mode)
{
    D_8009B268 = 1;
    D_8009B26D = mode;

    switch (mode) {
    case 0:
        File_RequestNameEntryPackage();
        func_8016AA6C();
        gCampaignSavedSceneIndex[0] = 0x30;
        gCampaignSceneIndex = 0x30;
        D_8009B3D4 = 1;
        D_8009B0D1 = 0;
        D_8009B26C = 2;
        break;
    case 2:
        D_8009B26C = 0x10;
        break;
    case 3:
        D_8009B26C = 0xE;
        break;
    case 8:
        D_8009B26C = 4;
        break;
    case 5:
        gCampaignSceneIndex = gCampaignSavedSceneIndex[0];
        D_8009B26C = 2;
        break;
    case 6:
        gFreeDuel_bReturnFlags = 0;
        D_8009B26C = 6;
        break;
    case 7:
        func_80033C90();
        D_8009B268 = 0;
        break;
    case 4:
        D_8009B26C = 0xB;
        D_8009B268 = 0;
        break;
    case 9:
        D_8009B26C = 0xA;
        D_8009B268 = 0;
        break;
    case 10:
    default:
        D_8009B26C = 0;
        break;
    }
}
