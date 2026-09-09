/* The live map's per-frame logic: the exit picker and the tick that drives
 * it. CampaignMap_PickExit has exactly one caller, the tick below, and is
 * defined ahead of it in executable order, so it needs no declaration -- the
 * prototype in pick_exit.h existed only to cross the file boundary that this
 * unit removes. Every other helper the tick calls keeps its own header
 * because it also has other callers. */
#include "../../types.h"
#include "../../unmatched.h"
#include "camera_transition.h"
#include "location_objects.h"
#include "location_label.h"
#include "location_marker.h"
#include "../../game/campaign_flags.h"
#include "../../game/view_state.h"
#include "../../game/display_object_api.h"
#include "../../game/sound.h"
#include "../../game/fade.h"
#include "campaign_map.h"

extern u16 gInput_wPad1Pressed;
extern volatile u16 gInput_wPad1Held;

extern u8 D_801695EC;
extern u8 D_8016960D;
extern u8 *D_801695C8;
extern u8 D_800E9ECE;
extern u8 D_800E9ECF;
extern u8 D_8009B26C;
extern u8 D_8009B27A;

/* Reads the d-pad against the current location's exit table and returns the
   chosen destination, or 0 when nothing was picked this frame. */
s32 CampaignMap_PickExit(void)
{
    u8 *record;
    u8 *exits;
    s32 ready;
    s32 i;

    record = gCampaignMap_aLocationTable + gCampaignMap_Location * 66;
    exits = record + 18;
    if (gCampaignMap_Location >= 10) {
        if (Campaign_TestStoryFlag(CAMPAIGN_FLAG_TOURNAMENT_COMPLETE) != 0 &&
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

void CampaignMap_UpdateLocation(void)
{
    u8 *table;
    u8 *record;
    s32 exit;

    if (D_801695EC != 0) {
        if (CampaignMap_UpdateLocationTransition() != 0) {
            return;
        }
        D_801695EC = 0;
        CampaignMap_RebuildLocationObjects(gCampaignMap_Location);
        CampaignMap_CreateLocationLabel(gCampaignMap_Location);
        if (gCampaignMap_Location >= 10) {
            if (D_801695C8 == 0) {
                D_801695C8 =
                    CampaignMap_CreateLocationMarker(gCampaignMap_Location);
            }
            table = gCampaignMap_aLocationTable;
            record = table + gCampaignMap_Location * 66;
            *(s16 *)(D_801695C8 + 0x30) = *(u16 *)(record + 12);
            *(s16 *)(D_801695C8 + 0x32) = *(u16 *)(record + 14);
        } else {
            func_8004036C(D_801695C8);
            D_801695C8 = 0;
        }
    }
    if (D_8016960D != 0) {
        if ((D_8016960D & 0x80) == 0) {
            D_8016960D = D_8016960D | 0x80;
            Fade_InitOut();
            D_800E9ECF = 2;
            CampaignMap_ClearLocationObjects();
            func_8004036C(D_801695C8);
            SD_BGMFadeOutWithStep(4);
        }
        D_800F2848.field_00 = D_800F2848.field_00 - 2;
        func_8001352C();
        if ((D_800E9ECE & 0x80) == 0) {
            D_8009B26C = 2;
            D_8009B27A = gCampaignMap_Location + 32;
        }
        return;
    }
    gCampaignMap_LocationPrev = gCampaignMap_Location;
    exit = CampaignMap_PickExit();
    if (exit < 0) {
        return;
    }
    if ((exit & 0x8000) != 0) {
        D_8016960D = 1;
        return;
    }
    gCampaignMap_Location = exit;
    if ((u8)exit < 10) {
        func_8004036C(D_801695C8);
        D_801695C8 = 0;
    }
    CampaignMap_ClearLocationObjects();
    D_801695EC = 1;
}
