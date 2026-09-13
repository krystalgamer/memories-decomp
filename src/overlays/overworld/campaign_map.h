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
 * came from, and the state of the move between them. location_table.h and
 * live_state.h own the C table and aligned state-prefix declarations.
 */

/* The live table's camera targets, marker coordinates and four exits share
 * one 66-byte record. ygo_types.h owns the layout; the separate alternate
 * table in alternate_location.h is not an alias of this one. Field evidence
 * and signed-copy details are in notes/overlays/campaign-map-records.md. */
/* These two bytes remain in the raw word at 0x80169618; its high halfword is
 * 0x0043 and must not be absorbed as zero-fill padding. */
extern u8 gCampaignMap_LocationPrev;
extern u8 D_80169619;

/* Two flag bytes the location machinery shares.
 *
 *   D_801695EC  Read into a local, OR'd with 0x80, 0x60 and 0x40 at different
 *               points, and written back through a u8.
 *   D_8016960D  Tested whole, then for 0x80, set to 1 on entry and cleared.
 *
 * live_state.h declares them once. It also owns D_801695F8's four pointer
 * slots. The former integer view only cleared slots, while creation/release
 * constrained them to object pointers; one pointer array preserves both. */

/* The two display objects the map keeps between frames. Member names are the
 * decimal byte offset.
 *
 * D_801695C8 is the location marker. CampaignMap_SetLocation clears it and
 * stores CampaignMap_CreateLocationMarker's return; CampaignMap_UpdateLocation
 * stores the same return, updates f48/f50 and clears it; and
 * CampaignMap_UpdateLocationTransition reads and writes f96/f48/f50.
 *
 * D_801695D8 holds the object func_800400AC returned.
 * CampaignMap_SetLocation reaches f8 through a byte view, while
 * CampaignMap_UpdateLocationTransition reaches f72/f74/f8 through MapObject.
 *
 * All active-path users now share set_location.c. Every byte offset reached
 * through the globals is a named member, so the struct remains the wider view;
 * byte locals stay local and cast at the global. alternate_location.h:63-66
 * spells the same f8/f48/f50 for the mechanical copy it describes. */
void CampaignMap_SetLocation(s32 index);
void CampaignMap_UpdateLocation(void);
/* Resident entry alias used before this overlay is linked into its slot. */
void func_8016866C(s32 index);

#endif
