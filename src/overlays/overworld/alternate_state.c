#include "../../types.h"
#include "alternate_location.h"

/* Alternate campaign-map state at 0x8016A274-0x8016A2C8, defined in address
 * order. Europe places the same names in another order, so these are separate
 * variables, not fields of one record. The gCampaignMapAlt_ names are listed
 * in notes/overlays/alternate-location-data.md; the remaining D_ entries cover
 * bytes no consumer has been seen to name. */
u32 D_8016A274 = 0;
AlternateObject *gCampaignMapAlt_pLocationMarker = 0;
u32 D_8016A27C[3] = {0};
AlternateObject *gCampaignMapAlt_pSpotlight = 0;
u32 D_8016A28C[4] = {0};
u8 gCampaignMapAlt_bTransitionFlags = 0;
u8 D_8016A29D = 0;
u16 D_8016A29E = 0;
u32 D_8016A2A0[6] = {0};
s32 gCampaignMapAlt_MoveState = 0;
u8 gCampaignMapAlt_Location = 0;
u8 gCampaignMapAlt_bEnterSceneState = 0;
u16 D_8016A2BE = 0;
u32 D_8016A2C0[2] = {0};
