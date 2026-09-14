#ifndef MEMORIES_DECOMP_DUEL_EFFECT_TABLES_H
#define MEMORIES_DECOMP_DUEL_EFFECT_TABLES_H

#include "../ygo_types.h"
#include "card_constants.h"

#define DUEL_EFFECT_GROUP_COUNT 15
#define DUEL_EFFECT_ID_COUNT 104
#define DUEL_EFFECT_STATE_HANDLER_COUNT 5

#define DUEL_EFFECT_GROUP_NONE 0
#define DUEL_EFFECT_GROUP_TERRAIN 1
#define DUEL_EFFECT_GROUP_LIFE_POINT_RECOVERY 2
#define DUEL_EFFECT_GROUP_DIRECT_DAMAGE 3
#define DUEL_EFFECT_GROUP_BOARD_DESTRUCTION 4
#define DUEL_EFFECT_GROUP_MONSTER_REMOVAL 5
#define DUEL_EFFECT_GROUP_STOP_DEFENSE 6
#define DUEL_EFFECT_GROUP_RAIGEKI 7
#define DUEL_EFFECT_GROUP_DARK_PIERCING_LIGHT 8
#define DUEL_EFFECT_GROUP_STAT_PENALTY 9
#define DUEL_EFFECT_GROUP_SWORDS 10
#define DUEL_EFFECT_GROUP_CURSEBREAKER 11
#define DUEL_EFFECT_GROUP_RITUAL 12
#define DUEL_EFFECT_GROUP_HARPIES_FEATHER_DUSTER 13

typedef void (*DuelEffectHandler)(void);

/* The dispatch tables src/game/duel_effect_tables.c owns at 0x80090A5C.

   DuelEffect_UpdateCardEffect turns the active card effect id into a group
   through gDuelEffect_abGroupByEffectId and runs one of that group's two
   handlers depending on DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER. The first of
   each pair is DuelEffect_ClearCardEffect for every group but three.
   DuelEffect_UpdateState runs one of the five state handlers by the state
   index it latched. */
extern DuelEffectHandler gDuelEffect_apfnGroupHandler
    [DUEL_EFFECT_GROUP_COUNT * DUEL_CARD_EFFECT_HANDLERS_PER_GROUP];
extern u8 gDuelEffect_abGroupByEffectId[DUEL_EFFECT_ID_COUNT];
extern DuelEffectHandler
    gDuelEffect_apfnStateHandler[DUEL_EFFECT_STATE_HANDLER_COUNT];

#endif
