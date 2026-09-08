#ifndef MEMORIES_DECOMP_CAMPAIGN_MAP_ALTERNATE_LOCATION_H
#define MEMORIES_DECOMP_CAMPAIGN_MAP_ALTERNATE_LOCATION_H

#include "../../types.h"

/* Alternate-state copy with unresolved raw callees, not live map entrypoints. */
s32 CampaignMap_PickAlternateExit(void);
void CampaignMap_UpdateAlternateLocation(void);

#endif
