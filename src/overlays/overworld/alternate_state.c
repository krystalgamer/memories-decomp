#include "../../types.h"
#include "alternate_location.h"

/* Alternate campaign-map state at 0x8016A274-0x8016A2C8, defined in address
 * order. Europe places the same names in another order, so these are separate
 * variables, not fields of one record. The gCampaignMapAlt_ names are listed
 * in notes/overlays/alternate-location-data.md; the remaining D_ entries cover
 * bytes no consumer has been seen to name. */
#define ALTERNATE_STATE __attribute__((section(".data")))

u32 D_8016A274 ALTERNATE_STATE = 0;
AlternateObject *gCampaignMapAlt_pLocationMarker ALTERNATE_STATE = 0;
u32 D_8016A27C[3] ALTERNATE_STATE = {0};
AlternateObject *D_8016A288 ALTERNATE_STATE = 0;
u32 D_8016A28C[4] ALTERNATE_STATE = {0};
u8 gCampaignMapAlt_bTransitionFlags ALTERNATE_STATE = 0;
u8 D_8016A29D ALTERNATE_STATE = 0;
u16 D_8016A29E ALTERNATE_STATE = 0;
u32 D_8016A2A0[6] ALTERNATE_STATE = {0};
s32 gCampaignMapAlt_MoveState ALTERNATE_STATE = 0;
u8 gCampaignMapAlt_Location ALTERNATE_STATE = 0;
u8 gCampaignMapAlt_bEnterSceneState ALTERNATE_STATE = 0;
u16 D_8016A2BE ALTERNATE_STATE = 0;
u32 D_8016A2C0[2] ALTERNATE_STATE = {0};

#undef ALTERNATE_STATE
