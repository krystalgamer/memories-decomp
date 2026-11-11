#include "../../types.h"
#include "alternate_location.h"

void CampaignMap_UpdateAlternateLocation(void)
{
    s32 r;

    if (D_800C4E68 & 4) {
        gCampaignMapAlt_pSpotlight->f8 ^= 0x40;
    }
    if (gCampaignMapAlt_bTransitionFlags != 0) {
        if (func_80169230()) {
            return;
        }
        gCampaignMapAlt_bTransitionFlags = 0;
        func_801680E4(gCampaignMapAlt_Location);
        func_801682D0(gCampaignMapAlt_Location);
        if (gCampaignMapAlt_Location >= 10) {
            if (gCampaignMapAlt_pLocationMarker == 0) {
                gCampaignMapAlt_pLocationMarker = func_80168A48(gCampaignMapAlt_Location);
            }
            gCampaignMapAlt_pLocationMarker->f48 = gCampaignMapAlt_aLocationTable[gCampaignMapAlt_Location].a;
            gCampaignMapAlt_pLocationMarker->f50 = gCampaignMapAlt_aLocationTable[gCampaignMapAlt_Location].b;
        } else {
            func_80066574(gCampaignMapAlt_pLocationMarker);
            gCampaignMapAlt_pLocationMarker = 0;
        }
    }
    if (D_800C4E68 & 0x100) {
        D_8016A2C9 ^= 1;
    }
    if (D_8016A2C9 != 0) {
        func_80168624();
        return;
    }
    if (gCampaignMapAlt_bEnterSceneState != 0) {
        if ((gCampaignMapAlt_bEnterSceneState & 0x80) == 0) {
            gCampaignMapAlt_bEnterSceneState |= 0x80;
            func_80021EA4();
            D_8011464F = 2;
            func_80168040();
            func_80066574(gCampaignMapAlt_pLocationMarker);
            func_80065BFC(4);
        }
        D_8011C410 -= 2;
        func_800158C8();
        if ((D_8011464E & 0x80) == 0) {
            D_800C4D82 = gCampaignMapAlt_Location + 32;
            D_800C4DCC = 2;
        }
        return;
    }
    if (D_800C4E68 & 0x800) {
        D_800C4DCC = D_800C4DC9;
    }
    gCampaignMapAlt_LocationPrev = gCampaignMapAlt_Location;
    r = CampaignMap_PickAlternateExit();
    if (r >= 0) {
        if (r & 0x8000) {
            gCampaignMapAlt_bEnterSceneState = 1;
        } else {
            if ((gCampaignMapAlt_Location = r) < 10) {
                func_80066574(gCampaignMapAlt_pLocationMarker);
                gCampaignMapAlt_pLocationMarker = 0;
            }
            func_80168040();
            gCampaignMapAlt_bTransitionFlags = 1;
            return;
        }
    }
}
