#include "../../types.h"
#include "alternate_location.h"

void CampaignMap_UpdateAlternateLocation(void)
{
    s32 r;

    if (D_800C4E68 & 4) {
        gCampaignMap_AlternateState.blinkObject->f8 ^= 0x40;
    }
    if (gCampaignMap_AlternateState.rebuildPending != 0) {
        if (func_80169230()) {
            return;
        }
        gCampaignMap_AlternateState.rebuildPending = 0;
        func_801680E4(gCampaignMap_AlternateState.location);
        func_801682D0(gCampaignMap_AlternateState.location);
        if (gCampaignMap_AlternateState.location >= 10) {
            if (gCampaignMap_AlternateState.marker == 0) {
                gCampaignMap_AlternateState.marker = func_80168A48(gCampaignMap_AlternateState.location);
            }
            gCampaignMap_AlternateState.marker->f48 = D_80169E54[gCampaignMap_AlternateState.location].a;
            gCampaignMap_AlternateState.marker->f50 = D_80169E54[gCampaignMap_AlternateState.location].b;
        } else {
            func_80066574(gCampaignMap_AlternateState.marker);
            gCampaignMap_AlternateState.marker = 0;
        }
    }
    if (D_800C4E68 & 0x100) {
        D_8016A2C9 ^= 1;
    }
    if (D_8016A2C9 != 0) {
        func_80168624();
        return;
    }
    if (gCampaignMap_AlternateState.transitionFlags != 0) {
        if ((gCampaignMap_AlternateState.transitionFlags & 0x80) == 0) {
            gCampaignMap_AlternateState.transitionFlags |= 0x80;
            func_80021EA4();
            D_8011464F = 2;
            func_80168040();
            func_80066574(gCampaignMap_AlternateState.marker);
            func_80065BFC(4);
        }
        D_8011C410 -= 2;
        func_800158C8();
        if ((D_8011464E & 0x80) == 0) {
            D_800C4D82 = gCampaignMap_AlternateState.location + 32;
            D_800C4DCC = 2;
        }
        return;
    }
    if (D_800C4E68 & 0x800) {
        D_800C4DCC = D_800C4DC9;
    }
    D_8016A2C8 = gCampaignMap_AlternateState.location;
    r = CampaignMap_PickAlternateExit();
    if (r >= 0) {
        if (r & 0x8000) {
            gCampaignMap_AlternateState.transitionFlags = 1;
        } else {
            if ((gCampaignMap_AlternateState.location = r) < 10) {
                func_80066574(gCampaignMap_AlternateState.marker);
                gCampaignMap_AlternateState.marker = 0;
            }
            func_80168040();
            gCampaignMap_AlternateState.rebuildPending = 1;
            return;
        }
    }
}
