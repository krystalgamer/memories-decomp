#include "../../types.h"
#include "dispatch.h"
#include "texture_semantic_views.h"
#include "render_state_semantic_views.h"
#include "handler_semantic_views.h"
#include "texture_state_semantic_views.h"
#include "contour_burst_semantic_views.h"
#include "damage_semantic_views.h"
#include "recovery_semantic_views.h"
#include "row_strike_semantic_views.h"
#include "ritual_semantic_views.h"
#include "stat_penalty_semantic_views.h"

void func_80146258(s32 effect, s32 phase, void *buffer, DuelEffectRequest *context)
{
    s32 i;
    u16 flags = context->field_10;

    setVector(&gDuelEffect_DrawOffset, context->field_00, context->field_02, context->field_04);
    gDuelEffect_wBasePriority = flags;
    if (phase >= 0) {
        for (i = 0; i < 21; i++) {
            DuelEffect_PackTexturePageClut(gDuelEffect_TexturePageClutPairs.pairs[i], &gDuelEffect_TextureImages[i], gDuelEffect_TextureModesAndVariants.modes[i]);
        }
    }
    if (effect == 0) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        DuelEffect_RunContourBurst(buffer, phase);
    }
    if (effect == 1) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        DuelEffect_RunFusion(buffer, phase);
    }
    if (effect == 2) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        DuelEffect_RunAttackSlash(buffer, phase, context->field_12);
    }
    if (effect == 3) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        DuelEffect_RunDestroyFlames(buffer, phase);
    }
    if (effect == 4) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        func_80159AAC(buffer, phase);
    }
    if (effect == 5) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        DuelEffect_RunLifePointRecovery(buffer, phase);
    }
    if (effect == 6) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        DuelEffect_RunDirectDamage(buffer, phase);
    }
    if (effect == 7) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        DuelEffect_RunReflectedDirectDamage(buffer, phase);
    }
    if (effect == 8) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80147B18(buffer, phase);
    }
    if (effect == 9) {
        /* Both stores are present; the callee takes only two arguments. */
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        DuelEffect_RunInvertedLifePointRecovery(buffer, phase);
    }
    if (effect == 10) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        DuelEffect_RunTerrainChange(buffer, phase);
    }
    if (effect == 11) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        DuelEffect_RunCardBreakup(buffer, phase);
    }
    if (effect == 12) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80150E00(buffer, phase);
    }
    if (effect == 13) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        DuelEffect_RunStatPenalty(buffer, phase, context->field_12);
    }
    if (effect == 14) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        DuelEffect_RunGuardianStarBonus(buffer, phase);
    }
    if (effect == 15) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_8014FF40(buffer, phase);
    }
    if (effect == 16) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        DuelEffect_RunRowStrike(buffer, phase);
    }
    if (effect == 17) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        DuelEffect_RunBoardDestruction(buffer, phase);
    }
    if (effect == 18) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        DuelEffect_RunParticleContourBurst(buffer, phase);
    }
    if (effect == 19) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        func_80153ADC(buffer, phase);
    }
    if (effect == 20) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        if (phase >= 0) {
            DuelEffect_RunRowStrike(buffer, 1);
        } else {
            DuelEffect_RunRowStrike(buffer, phase);
        }
    }
    if (effect == 21) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_8014E3EC(buffer, phase);
    }
    if (effect == 22) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        DuelEffect_RunRitual(buffer, phase);
    }
    if (effect == 23) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80152048(buffer, phase);
    }
    if (effect == 24) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        func_8014D3E8(buffer, phase);
    }
}
