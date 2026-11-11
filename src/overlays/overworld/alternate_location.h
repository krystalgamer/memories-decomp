#ifndef MEMORIES_DECOMP_CAMPAIGN_MAP_ALTERNATE_LOCATION_H
#define MEMORIES_DECOMP_CAMPAIGN_MAP_ALTERNATE_LOCATION_H

#include "../../types.h"

#define ALTERNATE_LOCATION_COUNT 16

/* Alternate-state copy with unresolved raw callees, not live map entrypoints.

   The types below are deliberately named for this family and are not shared
   with the live map helpers. The inventory rows for both functions warn
   against aliasing this family to those helpers, and the tables are distinct
   symbols in any case: this family reads gCampaignMapAlt_aLocationTable and gCampaignMapAlt_pLocationMarker, the live
   path reads gCampaignMap_aLocationTable and gCampaignMap_pSpotlight. The layouts happen
   to agree because this is a mechanical copy, which is corroboration for the
   offsets below and not a reason to merge the declarations. */

/* One exit of an alternate location record, 12 bytes. Four of them sit at
   +0x12 of the record. CampaignMap_PickAlternateExit gates an exit on `f0`
   being non-zero, skips it when `f9` is 16, matches the pressed-input word
   against `f6`, and returns `f9` as the destination. */
typedef struct {
    u16 f0;
    u8 pad2[4];
    u16 f6;
    u8 pad8;
    u8 f9;
    u8 f10;
    u8 pad11;
} AlternateExit;

/* An alternate location record, 66 bytes, indexed by the u8 at gCampaignMapAlt_Location.
   The 66-byte stride and the exit array at +0x12 are visible in the index
   arithmetic of CampaignMap_PickAlternateExit.

   This merges the two private views the two sources used to carry. The
   selector saw `f0`, `f16` and the exits and padded over the middle; the tick
   saw only the pair at +0x0C, which it copies into the marker object at +0x30
   and +0x32, and padded over everything else. Neither field falls inside the
   other's padding, so the union of the two is the record. */
typedef struct {
    u16 f0;
    u8 pad2[10];
    u16 a;
    u16 b;
    u8 f16;
    u8 pad17;
    AlternateExit exits[4];
} AlternateLocation;

/* 66 bytes is the stride the selector's index arithmetic uses, and the exit
   array has to start at +0x12 for the same reason. */
typedef char AlternateLocation_size_must_be_66[
    sizeof(AlternateLocation) == 66 ? 1 : -1
];
typedef char AlternateExit_size_must_be_12[
    sizeof(AlternateExit) == 12 ? 1 : -1
];

/* The alternate marker/map object held in gCampaignMapAlt_pLocationMarker
   and gCampaignMapAlt_pSpotlight.
   CampaignMap_UpdateAlternateLocation toggles bit 0x40 of `f8` and writes the
   location record's pair into `f48`/`f50`. */
typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad10[38];
    u16 f48;
    u16 f50;
} AlternateObject;

/* The three symbols both sources share. The comments above already name two
 * of them while describing the layouts, so the declarations belong here too.
 *
 *   gCampaignMapAlt_aLocationTable
 *                The sixteen-record table defined by alternate_location.c,
 *                indexed by gCampaignMapAlt_Location at the 66-byte stride above.
 *   gCampaignMapAlt_Location
 *                The current alternate location, compared against 10 by both
 *                sources before they treat the entry as a real one.
 *   D_800C4E68   A pad or status word; the alternate exit selector tests
 *                0x20/0xC0 in US and 0x40/0xA0 in Japan.
 *
 * The adjacent controller state is defined in alternate_state.c. */
extern AlternateLocation gCampaignMapAlt_aLocationTable[ALTERNATE_LOCATION_COUNT];
extern AlternateObject *gCampaignMapAlt_pLocationMarker;
extern AlternateObject *gCampaignMapAlt_pSpotlight;
extern u8 gCampaignMapAlt_bTransitionFlags;
extern s32 gCampaignMapAlt_MoveState;
extern u8 gCampaignMapAlt_Location;
extern u8 gCampaignMapAlt_bEnterSceneState;
extern u8 gCampaignMapAlt_LocationPrev;
extern u8 D_8016A2C9;
extern u16 D_800C4E68;

/* Resident campaign-map state read or updated by the alternate-location
   controller while this overlay is active. */
extern u8 D_800C4D82;
extern u8 D_800C4DC9;
extern u8 D_800C4DCC;
extern u16 D_800C4E74;
extern u8 D_8011464E;
extern u8 D_8011464F;
extern u16 D_8011C410;

s32 CampaignMap_PickAlternateExit(void);
void CampaignMap_UpdateAlternateLocation(void);
s32 func_80169230(void);
void func_801680E4(s32);
void func_801682D0(s32);
AlternateObject *func_80168A48(s32);
void func_80066574(AlternateObject *);
void func_80168624(void);
void func_80021EA4(void);
void func_80168040(void);
void func_80065BFC(s32);
void func_800158C8(void);
s32 func_8004EB9C(s32);
void func_80065B24(s32);

#endif
