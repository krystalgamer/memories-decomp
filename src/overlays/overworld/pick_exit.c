#include "../../types.h"

extern u8 gCampaignMap_Location;
extern u8 gCampaignMap_aLocationTable[];
extern u16 D_8009B398;
extern u16 D_8009B3A4;
extern s32 gCampaignMap_MoveState;
extern s32 func_8002CCA8(s32);
extern void func_8003FEE0(s32);

s32 CampaignMap_PickExit(void)
{
    u8 *record;
    u8 *exits;
    s32 ready;
    s32 i;

    record = gCampaignMap_aLocationTable + gCampaignMap_Location * 66;
    exits = record + 18;
    if (gCampaignMap_Location >= 10) {
        if (func_8002CCA8(71) != 0 && (D_8009B398 & 0x20) != 0) {
            func_8003FEE0(48);
            gCampaignMap_MoveState = 24;
            return 0;
        }
    }
    if ((D_8009B398 & 0xC0) != 0) {
        ready = *(u16 *)record;
        if (ready != 0) {
            if (func_8002CCA8(*(u16 *)exits) != 0) {
                ready = 0;
            }
        }
        if (ready == 0) {
            if (record[0x10] != 0) {
                gCampaignMap_MoveState = 24;
                func_8003FEE0(48);
                return record[0x10];
            }
            func_8003FEE0(48);
            return gCampaignMap_Location | 0x8000;
        }
    }
    for (i = 0; i < 4; i++) {
        if ((exits + 6)[3] != 16) {
            if (*(u16 *)exits == 0 || func_8002CCA8(*(u16 *)exits) != 0) {
                if ((D_8009B3A4 & *(u16 *)(exits + 6)) != 0) {
                    gCampaignMap_MoveState = exits[0xA];
                    func_8003FEE0(6);
                    return exits[9];
                }
            }
        }
        exits += 12;
    }
    return -1;
}
