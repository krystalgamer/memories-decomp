#define DUEL_SCREEN_TABLES_TYPED_POSITIONS
/* Duel ground projection: both functions project ground-plane points through
 * the GTE at screen offset (0xA0, 0x6C) from the scratchpad vector at
 * 0x1F8003E0. Typed scratchpad stores and official Psy-Q GTE macros reproduce
 * the projection blocks through the RTPS-normalizing profile. */
#include "../types.h"
#include "duel_side_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "duel_card.h"
#include "screen_projection.h"
#include "view_state.h"
#include "duel_screen_tables.h"
#include "duel_init_ground_projection_reference.h"
#include "duel_side_view_angles.h"
#include "../unmatched.h"
#include "view_state_reset_defaults.h"
#include "view_state_current.h"
#include "duel_result_outro.h"

/* Small data at 0x8009AF20: opposing headings for the two duel sides.
   Field transitions and row movement feed the view-tween heading channel;
   Duel_InitGroundProjectionReference samples element 0 directly. */
u16 gDuel_awSideViewHeadings[2] = {
    0x0400,
    0x0C00,
};

/* Initialized data this unit owns at 0x800908A0 through 0x80090998, previously a generated
   blob (#2602).

   gDuel_aCardSlotGroundPositions is the thirty X/Z pairs Duel_ProjectCardSlotScreenPositions projects through
   the GTE, one per card slot, writing the biased result to gDuel_aCardSlotScreenPositions. The
   pairs form a five-column grid at x = -140, -70, 0, 70 and 140.
   debug_effect_screen.c also reads slot 6's pair as halfwords 0xC/0xD.
   Projection and object creation supply their own height, 0 or -24;
   these pairs are not projected screen pixels.

   The owning source records those signed pairs as DuelFieldPosition entries.
   The shared header preserves a flat u16 view for the projection candidate
   and debug screen because their exact code generation depends on halfword
   pointer arithmetic.

   gDuel_ExodiaCardPoses supplies the five Exodia card poses consumed by the retained
   func_80018FEC candidate. Each card ID from 0x11 through 0x15 selects a
   record, whose first byte chooses one of the five shared display-object work
   slots and whose remaining bytes become object x/y after removing the
   candidate's 0x1A/0x1E coordinate biases. The trailing byte preserves the
   table's 16-byte extent before gDuelResult_aSpriteSpecsOpponent.

   gDuelResult_aSpriteSpecsOpponent and gDuelResult_aSpriteSpecsNoOpponent are the duel-result sprite tables, for a real
   opponent and for none. The outro already declared both as
   DuelResultSpriteSpec[][DUEL_RESULT_SPRITE_COUNT], and the extracted sizes
   agree: two winning sides by seven sprites by a four-byte spec. */
DuelFieldPosition gDuel_aCardSlotGroundPositions[DUEL_SCREEN_CARD_POSITION_COUNT] = {
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { -140, -95 },
    { -70, -95 },
    { 0, -95 },
    { 70, -95 },
    { 140, -95 },
    { -140, -161 },
    { -70, -161 },
    { 0, -161 },
    { 70, -161 },
    { 140, -161 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 140, 95 },
    { 70, 95 },
    { 0, 95 },
    { -70, 95 },
    { -140, 95 },
    { 140, 161 },
    { 70, 161 },
    { 0, 161 },
    { -70, 161 },
    { -140, 161 },
};

DuelExodiaCardPoseTable gDuel_ExodiaCardPoses = {
    {
        { 3, 104, 206 },
        { 2, 216, 206 },
        { 4, 69, 98 },
        { 1, 251, 98 },
        { 0, 160, 32 },
    },
    0,
};

DuelResultSpriteSpec gDuelResult_aSpriteSpecsOpponent[][DUEL_RESULT_SPRITE_COUNT] = {
    {
        { 112, 68, 1, 20 },
        { 160, 68, 2, 20 },
        { 208, 68, 3, 20 },
        { 88, 124, 4, 20 },
        { 136, 124, 5, 8 },
        { 184, 124, 6, 20 },
        { 232, 124, 7, 8 },
    },
    {
        { 112, 68, 8, 20 },
        { 160, 68, 9, 20 },
        { 208, 68, 10, 20 },
        { 88, 124, 11, 20 },
        { 136, 124, 12, 8 },
        { 184, 124, 13, 20 },
        { 232, 124, 14, 8 },
    },
};

DuelResultSpriteSpec gDuelResult_aSpriteSpecsNoOpponent[][DUEL_RESULT_SPRITE_COUNT] = {
    {
        { 136, 68, 16, 12 },
        { 184, 68, 17, 20 },
        { 0, 0, 0, 20 },
        { 88, 124, 4, 20 },
        { 136, 124, 5, 8 },
        { 184, 124, 6, 20 },
        { 232, 124, 7, 8 },
    },
    {
        { 136, 68, 24, 12 },
        { 184, 68, 25, 20 },
        { 0, 0, 0, 20 },
        { 88, 124, 26, 20 },
        { 136, 124, 27, 8 },
        { 184, 124, 28, 20 },
        { 232, 124, 29, 8 },
    },
};

/* Projects all duel-slot coordinates into the biased screen-position table. */
void Duel_ProjectCardSlotScreenPositions(void)
{
    ProjectedPair p;
    SVECTOR *scratch;
    ProjectedPair *projected;
    ScreenPair *out;
    u16 *source;
    s32 i;

    SetGeomScreen(gViewState_Current.projection);
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&gGraphics_LightSourceMatrix);
    scratch = (SVECTOR *)0x1F8003E0;
    i = 0;
    projected = &p;
    out = gDuel_aCardSlotScreenPositions;
    source = gDuel_aCardSlotGroundPositions;
    do {
        s32 y;
        u16 x = source[0];

        scratch->vy = 0;
        scratch->vx = x;
        scratch->vz = source[1];
        gte_ldv0(scratch);
        gte_rtps();
        gte_stsxy(projected);
        out->x = p.x - 0x1A;
        y = p.y;
        out->y = y - 0x1E;
        if (gDuel_bActiveSide != 0) {
            if (i < DUEL_CARD_SIDE_RECORD_COUNT) {
                out->y = y - 0x1D;
            }
        } else if (i >= DUEL_CARD_SIDE_RECORD_COUNT) {
            out->y = y - 0x1D;
        }
        out++;
        source += 2;
        i++;
    } while (i < DUEL_CARD_RECORD_COUNT);
    SetGeomOffset(0, 0);
}

void Duel_InitGroundProjectionReference(void)
{
    ProjectedPair p;
    s32 y;

    ViewState_ResetDefaults();
    SetGeomScreen(gViewState_Current.projection);
    SetGeomOffset(0xA0, 0x6C);
    gViewState_Current.distance = 0x14E;
    gViewState_Current.orbit_angle = 0x3FE;
    gViewState_Current.angle = gDuel_awSideViewHeadings[0];
    ViewState_ApplyOrbit();
    GsSetLsMatrix(&gGraphics_LightSourceMatrix);
    {
        SVECTOR *scratch = (SVECTOR *)0x1F8003E0;

        scratch->vx = 0x3E8;
        scratch->vy = 0;
        scratch->vz = 0x3E8;
        gte_ldv0(scratch);
        gte_rtps();
        gte_stsxy(&p);
    }
    /* Taking y through an s32 keeps the sign extension in the load, which is
       what makes it lh rather than the lhu the x read gets. */
    y = p.y;
    gDuel_sGroundReferenceScreenX = p.x - 0xA0;
    gDuel_sGroundReferenceScreenY = y - 0x6C;
    SetGeomOffset(0, 0);
    ViewState_ResetDefaults();
}
