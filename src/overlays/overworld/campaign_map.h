#ifndef MEMORIES_DECOMP_OVERLAYS_OVERWORLD_CAMPAIGN_MAP_H
#define MEMORIES_DECOMP_OVERLAYS_OVERWORLD_CAMPAIGN_MAP_H

#include "../../types.h"

/* The campaign map's shared state.
 *
 * Eight sources in this directory work on the same map: a table of location
 * records, the location the player is standing on, the one they came from,
 * and the state of the move between them. None of the four is defined in C,
 * so this header is a declaration point rather than an owner.
 *
 * gCampaignMap_aLocationTable used to live in src/unmatched.h, which said it
 * belonged there only because "there is no subsystem header to put it in" and
 * that it should move if an overworld map header were ever written. This is
 * that header, so it has moved.
 */

/* Location records, 66 bytes each; consumers index it with an explicit
 * stride rather than a typed element, which is load-bearing under -G8. */
extern u8 gCampaignMap_aLocationTable[];

extern u8 gCampaignMap_Location;
extern u8 gCampaignMap_LocationPrev;
extern s32 gCampaignMap_MoveState;

#endif
