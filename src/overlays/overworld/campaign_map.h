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

/* Two flag bytes the location machinery shares.
 *
 *   D_801695EC  Read into a local, OR'd with 0x80, 0x60 and 0x40 at different
 *               points, and written back. Three sources agree it is a u8.
 *   D_8016960D  Tested whole, then for 0x80, set to 1 on entry and cleared.
 *
 * NOT HERE, ON PURPOSE
 *
 * D_801695F8 is spelled `s32 []` and `u8 *[]`, which is an element question
 * and wants its own change rather than a spelling picked here. */
extern u8 D_801695EC;
extern u8 D_8016960D;

/* The two display objects the map keeps between frames. Member names are the
 * decimal byte offset.
 *
 * D_801695C8 is the location marker. CampaignMap_SetLocation clears it
 * (set_location.c:41) and stores CampaignMap_CreateLocationMarker's return in
 * it (set_location.c:81); CampaignMap_UpdateLocation stores the same return
 * (location_tick.c:94-95), stores f48/f50 through a byte view
 * (location_tick.c:99-100) and clears it (location_tick.c:103, :135);
 * CampaignMap_UpdateLocationTransition copies it into `marker`
 * (camera_transition.c:90, :127), stores and reads f96 through that copy
 * (:94, :129-140) and stores f48/f50 through the global (:162-169).
 *
 * D_801695D8 holds the object func_800400AC returned (set_location.c:61,
 * stored at :72); CampaignMap_SetLocation reads and stores f8 through a byte
 * view (set_location.c:80, :82). CampaignMap_UpdateLocationTransition copies
 * it into `obj` (camera_transition.c:100) for f72/f74/f8 (:101-106), into
 * `marker` (:115) for f72/f74 (:117-125), and stores f72/f74 through the
 * global (:158-159).
 *
 * Both were `MapObject *` in camera_transition.c and `u8 *` in the other two
 * sources. Every byte offset those two reach through the globals is a named
 * member, so the struct is the wider view and lives here; they keep their
 * byte locals and cast at the global. alternate_location.h:63-66 spells the
 * same f8/f48/f50 for the mechanical copy it describes. */
typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad10[38];
    s16 f48;
    s16 f50;
    u8 pad52[20];
    u16 f72;
    u16 f74;
    u8 pad76[20];
    s16 f96;
} MapObject;

extern MapObject *D_801695C8;
extern MapObject *D_801695D8;

void CampaignMap_SetLocation(s32 index);
void CampaignMap_UpdateLocation(void);
/* Resident entry alias used before this overlay is linked into its slot. */
void func_8016866C(s32 index);

#endif
