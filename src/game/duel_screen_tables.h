#ifndef MEMORIES_DECOMP_DUEL_SCREEN_TABLES_H
#define MEMORIES_DECOMP_DUEL_SCREEN_TABLES_H

#include "../types.h"
#include "../ygo_types.h"
#include "duel_grid.h"
#include "duel_result_outro.h"

#define DUEL_SCREEN_CARD_POSITION_COUNT 30
#define DUEL_EXODIA_CARD_POSE_COUNT 5

typedef struct {
    u8 work_slot;
    u8 biased_x;
    u8 biased_y;
} DuelExodiaCardPose;

typedef struct {
    DuelExodiaCardPose poses[DUEL_EXODIA_CARD_POSE_COUNT];
    u8 padding;
} DuelExodiaCardPoseTable;

/* The card-slot projection coordinates and the two duel-result sprite
   tables, declared here so the source that defines them and the units that
   read them cannot drift apart. gDuel_aCardSlotGroundPositions is declared as its DuelFieldPosition pairs
   everywhere: the defining unit duel_ground_projection.c and
   duel_card_record_lifecycle.c, whose Duel_SetupFieldCardFromDeck loads the
   pair with lh, read it that way. debug_effect_screen.c reads one pair's
   members as unsigned halfwords and duel_project_card_slot_screen_x.c casts
   to a halfword pointer. The
   pair supplies ground X/Z; consumers choose height independently and the
   shared type retains its generic x/y names. */
extern DuelFieldPosition gDuel_aCardSlotGroundPositions[DUEL_SCREEN_CARD_POSITION_COUNT];
extern ScreenPair gDuel_aCardSlotScreenPositions[DUEL_SCREEN_CARD_POSITION_COUNT];
extern DuelExodiaCardPoseTable gDuel_ExodiaCardPoses;
extern DuelResultSpriteSpec gDuelResult_aSpriteSpecsOpponent[][DUEL_RESULT_SPRITE_COUNT];
extern DuelResultSpriteSpec gDuelResult_aSpriteSpecsNoOpponent[][DUEL_RESULT_SPRITE_COUNT];

/* Projects one card-slot ground pair at height -24; returns signed screen X. */
s16 Duel_ProjectCardSlotScreenX(s32 index);

#endif
