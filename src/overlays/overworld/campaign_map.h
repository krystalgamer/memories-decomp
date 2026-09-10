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
 * set_location.c keeps two same-symbol views of D_801695F8: `s32 []` for the
 * setter's zero-only stores and `u8 *[]` for object creation and release.
 * They do not carry equal weight, so the element question is answerable from
 * what they do:
 *
 *   The object view is constrained. It passes an element straight to
 *   func_8004036C, whose display_object_api.h prototype takes `void *object`,
 *   and stores the object func_800400AC returned back into the same slot. The
 *   elements are display-object pointers there.
 *
 *   The word view abstains. Its only use is `D_801695F8_words[i] = 0`, and a
 *   zero store is valid for either element type, so nothing about the spelling
 *   survives into the generated code.
 *
 * That is the same shape as the abstention this tree has recorded before --
 * dialog_choice.h notes that dialog_read_choice_input.c's u8 spelling of
 * gDialog_bChoice was an abstention because every use assigned straight into a
 * u8 local. So the pointer reading is the constrained one and the s32 spelling
 * is not evidence against it.
 *
 * Both declarations stay local because the two spellings are matching
 * levers, not a shared interface. */
extern u8 D_801695EC;
extern u8 D_8016960D;

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
