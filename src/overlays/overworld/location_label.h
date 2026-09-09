#ifndef MEMORIES_DECOMP_OVERWORLD_LOCATION_LABEL_H
#define MEMORIES_DECOMP_OVERWORLD_LOCATION_LABEL_H

#include "../../types.h"

/* Builds the location name box and returns the record it built. The argument
   is accepted and never read: the label text comes from the current location,
   not from a parameter. */
u8 *CampaignMap_CreateLocationLabel(s32 unused);

#endif
