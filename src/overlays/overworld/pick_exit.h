#ifndef MEMORIES_DECOMP_OVERWORLD_PICK_EXIT_H
#define MEMORIES_DECOMP_OVERWORLD_PICK_EXIT_H

#include "../../types.h"

/* Reads the d-pad against the current location's exit table and returns the
   chosen destination, or 0 when nothing was picked this frame. */
s32 CampaignMap_PickExit(void);

#endif
