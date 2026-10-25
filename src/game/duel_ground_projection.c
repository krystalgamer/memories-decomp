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
    gViewState_Current.field_00 = 0x14E;
    gViewState_Current.field_04 = 0x3FE;
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
    D_8009B200 = p.x - 0xA0;
    D_8009B202 = y - 0x6C;
    SetGeomOffset(0, 0);
    ViewState_ResetDefaults();
}
