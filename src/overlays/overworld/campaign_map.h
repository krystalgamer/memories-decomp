#ifndef MEMORIES_DECOMP_OVERLAYS_OVERWORLD_CAMPAIGN_MAP_H
#define MEMORIES_DECOMP_OVERLAYS_OVERWORLD_CAMPAIGN_MAP_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "location_table.h"
#include "live_state.h"

/* The campaign map's shared state.
 *
 * The live helpers in set_location.c work on the same map: a table of
 * location records, the location the player is standing on, the one they
 * came from, and the state of the move between them. location_table.h
 * declares the C table and live_state.h the state set_location.c defines.
 */

/* The live table's camera targets, marker coordinates and four exits share
 * one 66-byte record. ygo_types.h owns the layout; the separate alternate
 * table in alternate_location.h is not an alias of this one. Field evidence
 * and signed-copy details are in notes/overlays/campaign-map-records.md. */
/* The last two bytes of the live state in North America and Japan, where
 * the module's stale bytes follow from 0x8016961A. */
extern u8 gCampaignMap_LocationPrev;
extern u8 D_80169619;

/* Two flag bytes the location machinery shares.
 *
 *   gCampaignMap_bTransitionFlags  Read into a local, OR'd with 0x80, 0x60 and 0x40 at different
 *               points, and written back through a u8.
 *   gCampaignMap_bEnterSceneState  Set to 1 when the confirm path returns the
 *               current location with bit 0x8000 (enter its scene); bit 0x80
 *               latches the one-time fade-out setup; cleared on map entry.
 *
 * live_state.h declares them once. It also owns gCampaignMap_apExitObjects's four pointer
 * slots. The former integer view only cleared slots, while creation/release
 * constrained them to object pointers; one pointer array preserves both. */

/* The two display objects the map keeps between frames.
 *
 * gCampaignMap_pLocationMarker is the location marker. CampaignMap_SetLocation
 * clears it and stores CampaignMap_CreateLocationMarker's return;
 * CampaignMap_UpdateLocation stores the same return, moves its xy to the
 * location's marker position and clears it; and
 * CampaignMap_UpdateLocationTransition eases it there, using field_60.phase as the
 * cosine phase.
 *
 * gCampaignMap_pSpotlight is the spotlight mask. The location transition
 * toggles its flag 0x40 and widens or narrows field_48.spotlight. */
void CampaignMap_SetLocation(s32 index);
void CampaignMap_UpdateLocation(void);

#endif
