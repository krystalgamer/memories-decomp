#ifndef MEMORIES_DECOMP_DUEL_EFFECT_TABLES_H
#define MEMORIES_DECOMP_DUEL_EFFECT_TABLES_H

#include "../ygo_types.h"
#include "card_constants.h"

#define DUEL_EFFECT_GROUP_COUNT 15
#define DUEL_EFFECT_ID_COUNT 104
#define DUEL_EFFECT_STATE_HANDLER_COUNT 5

typedef void (*DuelEffectHandler)(void);

void func_80024E58(void);

/* The dispatch tables src/game/duel_effect_tables.c owns at 0x80090A5C.

   func_80026B34 turns the active card effect id into a group through
   gDuelEffect_abGroupByEffectId, and runs one of that group's two handlers
   depending on DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER; the first of each pair
   is the no-op reset at func_80024E4C for every group but three.
   DuelEffect_UpdateState runs one of the five state handlers by
   the state index it latched. */
extern DuelEffectHandler gDuelEffect_apfnGroupHandler
    [DUEL_EFFECT_GROUP_COUNT * DUEL_CARD_EFFECT_HANDLERS_PER_GROUP];
extern u8 gDuelEffect_abGroupByEffectId[DUEL_EFFECT_ID_COUNT];
extern DuelEffectHandler
    gDuelEffect_apfnStateHandler[DUEL_EFFECT_STATE_HANDLER_COUNT];

#endif
