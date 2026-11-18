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
#include "exodia_semantic_views.h"
#include "selected_cards_semantic_views.h"
#include "swords_field_semantic_views.h"
#include "field_burst_semantic_views.h"
#include "card_flash_semantic_views.h"

/* PAL loads the effect palettes at a different VRAM position. */
#ifdef VERSION_EUROPE
#define DUEL_EFFECT_CLUT(ntsc_x, ntsc_y, pal_x, pal_y) pal_x, pal_y
#else
#define DUEL_EFFECT_CLUT(ntsc_x, ntsc_y, pal_x, pal_y) ntsc_x, ntsc_y
#endif

GsIMAGE gDuelEffect_TextureImages[21] = {
    { 9, 512, 256, 32, 64, NULL, DUEL_EFFECT_CLUT(0, 240, 640, 208), 256, 1, NULL },
    { 8, 544, 256, 8, 64, NULL, DUEL_EFFECT_CLUT(0, 241, 640, 209), 16, 1, NULL },
    { 9, 512, 320, 32, 64, NULL, DUEL_EFFECT_CLUT(0, 242, 640, 210), 256, 1, NULL },
    { 8, 552, 256, 16, 64, NULL, DUEL_EFFECT_CLUT(16, 241, 656, 209), 16, 1, NULL },
    { 8, 544, 320, 32, 64, NULL, DUEL_EFFECT_CLUT(32, 241, 672, 209), 16, 1, NULL },
    { 8, 512, 384, 16, 128, NULL, DUEL_EFFECT_CLUT(48, 241, 688, 209), 16, 1, NULL },
    { 8, 528, 384, 16, 128, NULL, DUEL_EFFECT_CLUT(64, 241, 704, 209), 16, 1, NULL },
    { 8, 544, 384, 16, 64, NULL, DUEL_EFFECT_CLUT(80, 241, 720, 209), 16, 1, NULL },
    { 8, 544, 448, 24, 64, NULL, DUEL_EFFECT_CLUT(96, 241, 736, 209), 16, 1, NULL },
    { 8, 568, 256, 8, 64, NULL, DUEL_EFFECT_CLUT(112, 241, 752, 209), 16, 1, NULL },
    { 8, 560, 384, 16, 64, NULL, DUEL_EFFECT_CLUT(128, 241, 768, 209), 16, 1, NULL },
    { 8, 576, 256, 8, 128, NULL, DUEL_EFFECT_CLUT(144, 241, 784, 209), 16, 1, NULL },
    { 8, 568, 448, 8, 32, NULL, DUEL_EFFECT_CLUT(160, 241, 800, 209), 16, 1, NULL },
    { 8, 584, 256, 16, 128, NULL, DUEL_EFFECT_CLUT(176, 241, 816, 209), 16, 1, NULL },
    { 9, 616, 256, 24, 128, NULL, DUEL_EFFECT_CLUT(192, 241, 832, 209), 256, 1, NULL },
    { 8, 600, 256, 16, 64, NULL, DUEL_EFFECT_CLUT(240, 241, 880, 209), 16, 1, NULL },
    { 8, 576, 384, 32, 64, NULL, DUEL_EFFECT_CLUT(0, 243, 640, 211), 16, 1, NULL },
    { 8, 600, 320, 16, 64, NULL, DUEL_EFFECT_CLUT(16, 243, 656, 211), 16, 1, NULL },
    { 8, 624, 384, 16, 128, NULL, DUEL_EFFECT_CLUT(32, 243, 672, 211), 16, 1, NULL },
    { 8, 612, 384, 12, 128, NULL, DUEL_EFFECT_CLUT(48, 243, 688, 211), 16, 1, NULL },
    { 8, 576, 448, 32, 64, NULL, DUEL_EFFECT_CLUT(64, 243, 704, 211), 16, 1, NULL },
};

DuelEffectImageInputs gDuelEffect_TextureModesAndVariants = {
    .variant = {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1 },
        {
            { 9, 56, 128, 24, 28, NULL, DUEL_EFFECT_CLUT(256, 241, 640, 225), 256, 1, NULL },
            { 9, 0, 192, 24, 28, NULL, DUEL_EFFECT_CLUT(256, 241, 640, 225), 256, 1, NULL },
            { 9, 56, 192, 24, 28, NULL, DUEL_EFFECT_CLUT(256, 242, 640, 226), 256, 1, NULL },
            { 9, 56, 192, 24, 28, NULL, DUEL_EFFECT_CLUT(256, 243, 640, 227), 256, 1, NULL },
            { 9, 56, 192, 24, 28, NULL, DUEL_EFFECT_CLUT(256, 244, 640, 228), 256, 1, NULL },
        },
    },
};

void DuelEffect_DispatchRequest(s32 effect, s32 phase, void *buffer, DuelEffectRequest *context)
{
    s32 i;
    u16 flags = context->base_priority;

    setVector(&gDuelEffect_DrawOffset, context->x, context->y, context->z);
    gDuelEffect_wBasePriority = flags;
    if (phase >= 0) {
        for (i = 0; i < 21; i++) {
            DuelEffect_PackTexturePageClut(gDuelEffect_TexturePageClutPairs.pairs[i], &gDuelEffect_TextureImages[i], gDuelEffect_TextureModesAndVariants.modes[i]);
        }
    }
    if (effect == 0) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunContourBurst(buffer, phase);
    }
    if (effect == 1) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunFusion(buffer, phase);
    }
    if (effect == 2) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunAttackSlash(buffer, phase, context->number);
    }
    if (effect == 3) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunDestroyFlames(buffer, phase);
    }
    if (effect == 4) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunCardFlashBurst(buffer, phase);
    }
    if (effect == 5) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunLifePointRecovery(buffer, phase);
    }
    if (effect == 6) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunDirectDamage(buffer, phase);
    }
    if (effect == 7) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunReflectedDirectDamage(buffer, phase);
    }
    if (effect == 8) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunFieldColumnBurst(buffer, phase);
    }
    if (effect == 9) {
        /* Both stores are present; the callee takes only two arguments. */
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunInvertedLifePointRecovery(buffer, phase);
    }
    if (effect == 10) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunTerrainChange(buffer, phase);
    }
    if (effect == 11) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunCardBreakup(buffer, phase);
    }
    if (effect == 12) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunStopDefenseBurst(buffer, phase);
    }
    if (effect == 13) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunStatPenalty(buffer, phase, context->number);
    }
    if (effect == 14) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunGuardianStarBonus(buffer, phase);
    }
    if (effect == 15) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunSelectedCardOverlay(buffer, phase);
    }
    if (effect == 16) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunRowStrike(buffer, phase);
    }
    if (effect == 17) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunBoardDestruction(buffer, phase);
    }
    if (effect == 18) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunParticleContourBurst(buffer, phase);
    }
    if (effect == 19) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunExpandingRingBurst(buffer, phase);
    }
    if (effect == 20) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        if (phase >= 0) {
            DuelEffect_RunRowStrike(buffer, 1);
        } else {
            DuelEffect_RunRowStrike(buffer, phase);
        }
    }
    if (effect == 21) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunSwordsField(buffer, phase);
    }
    if (effect == 22) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunRitual(buffer, phase);
    }
    if (effect == 23) {
        gDuelEffect_pOrderingTable = context->ordering_table_2;
        SetGeomOffset(160, 108);
        DuelEffect_RunBackRowSweep(buffer, phase);
    }
    if (effect == 24) {
        gDuelEffect_pOrderingTable = context->ordering_table_1;
        DuelEffect_RunExodiaBurst(buffer, phase);
    }
}
