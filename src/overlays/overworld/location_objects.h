#ifndef MEMORIES_DECOMP_OVERWORLD_LOCATION_OBJECTS_H
#define MEMORIES_DECOMP_OVERWORLD_LOCATION_OBJECTS_H

#include "../../types.h"

/* Frees the four display objects the current location owns and clears their
   slots. */
void CampaignMap_ClearLocationObjects(void);

/* Clears the previous location's objects, then spawns up to four for
   location `index`, skipping the ones whose story flag is not set. */
void CampaignMap_RebuildLocationObjects(s32 index);

/* Builds the current location's name box and returns the record it built.
 * The argument is accepted and never read. */
u8 *CampaignMap_CreateLocationLabel(s32 unused);

#endif
