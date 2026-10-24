#include "../../types.h"
#include "dispatch.h"
#include "texture_semantic_views.h"
#include "render_state_semantic_views.h"
#include "handler_semantic_views.h"
#include "contour_burst_semantic_views.h"

void func_80146258(s32 effect, s32 phase, void *buffer, DuelEffectRequest *context)
{
    s32 i;
    u16 flags = context->field_10;

    setVector(&gDuelEffect_DrawOffset, context->field_00, context->field_02, context->field_04);
    gDuelEffect_wBasePriority = flags;
    if (phase >= 0) {
        for (i = 0; i < 21; i++) {
            DuelEffect_PackTexturePageClut(D_8015B748.pairs[i], &D_8015A1E4[i], D_8015A430.modes[i]);
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
        func_80157E10(buffer, phase);
    }
    if (effect == 6) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        func_80154B30(buffer, phase);
    }
    if (effect == 7) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_0C;
        func_801587D8(buffer, phase);
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
        func_801593A8(buffer, phase);
    }
    if (effect == 10) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        DuelEffect_RunTerrainChange(buffer, phase);
    }
    if (effect == 11) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80146760(buffer, phase);
    }
    if (effect == 12) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80150E00(buffer, phase);
    }
    if (effect == 13) {
        gDuelEffect_pOrderingTable = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_801503F8(buffer, phase, context->field_12);
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
        func_80151558(buffer, phase);
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
            func_80151558(buffer, 1);
        } else {
            func_80151558(buffer, phase);
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
        func_8014A8E4(buffer, phase);
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
