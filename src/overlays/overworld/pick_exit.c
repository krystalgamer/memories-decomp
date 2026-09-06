#include "../../types.h"

extern u8 gCampaignMap_Location;
extern u8 gCampaignMap_aLocationTable[];
extern u16 gInput_wPad1Pressed;
extern volatile u16 gInput_wPad1Held;
extern s32 gCampaignMap_MoveState;
extern s32 Campaign_TestStoryFlag(s32);
extern void SD_SEPlayFull(s32);

s32 CampaignMap_PickExit(void)
{
    u8 *record;
    u8 *exits;
    s32 ready;
    s32 i;

    record = gCampaignMap_aLocationTable + gCampaignMap_Location * 66;
    exits = record + 18;
    if (gCampaignMap_Location >= 10) {
        if (Campaign_TestStoryFlag(71) != 0 &&
            (gInput_wPad1Pressed & 0x20) != 0) {
            SD_SEPlayFull(48);
            gCampaignMap_MoveState = 24;
            return 0;
        }
    }
    if ((gInput_wPad1Pressed & 0xC0) != 0) {
        ready = *(u16 *)record;
        if (ready != 0) {
            if (Campaign_TestStoryFlag(*(u16 *)exits) != 0) {
                ready = 0;
            }
        }
        if (ready == 0) {
            if (record[0x10] != 0) {
                gCampaignMap_MoveState = 24;
                SD_SEPlayFull(48);
                return record[0x10];
            }
            SD_SEPlayFull(48);
            return gCampaignMap_Location | 0x8000;
        }
    }
    for (i = 0; i < 4; i++) {
        if ((exits + 6)[3] != 16) {
            if (*(u16 *)exits == 0 ||
                Campaign_TestStoryFlag(*(u16 *)exits) != 0) {
                if ((gInput_wPad1Held & *(u16 *)(exits + 6)) != 0) {
                    gCampaignMap_MoveState = exits[0xA];
                    SD_SEPlayFull(6);
                    return exits[9];
                }
            }
        }
        exits += 12;
    }
    return -1;
}
