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
 * D_801695C8 and D_801695D8 are shared by these same sources and are spelled
 * `MapObject *` in camera_transition.c, against a struct defined in that file,
 * and `u8 *` elsewhere. That is the canonical-versus-local question #2501
 * exists for and it wants its own change rather than a spelling picked here.
 * D_801695F8 is spelled `s32 []` and `u8 *[]`, which is an element question
 * on top of the same problem. */
extern u8 D_801695EC;
extern u8 D_8016960D;

#endif
