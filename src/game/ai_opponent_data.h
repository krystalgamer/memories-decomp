#ifndef MEMORIES_DECOMP_AI_OPPONENT_DATA_H
#define MEMORIES_DECOMP_AI_OPPONENT_DATA_H

#include "../ygo_types.h"

#define AI_OPPONENT_DATA_FIELD_COUNT 9
#define AI_OPPONENT_COUNT 40

/* One nine-byte record per duellist. Ai_GetHandSize reads values[0] as the
   opponent's hand size, and AiScript_LoadOpponentData multiplies values[1] by
   100 for opcode field 0 and returns values[field + 1] for the rest, so the
   script reaches every entry by index and the record stays an array.
   src/game/ai_opponent_data.c owns the table at 0x800917F0. */
typedef struct {
    s8 values[AI_OPPONENT_DATA_FIELD_COUNT];
} AiOpponentData;

typedef char AiOpponentData_size_must_be_9[
    sizeof(AiOpponentData) == AI_OPPONENT_DATA_FIELD_COUNT ? 1 : -1
];

extern AiOpponentData gDuel_aOpponentData[AI_OPPONENT_COUNT];

/* The signed opponent selector is loaded absolutely. The array spellings are
 * codegen views used by G8 duel units; every consumer reads only element 0. */
#ifdef GDUEL_BOPPONENTID_AS_SIZED_ARRAY
extern s8 gDuel_bOpponentID[AI_OPPONENT_DATA_FIELD_COUNT];
#elif defined(GDUEL_BOPPONENTID_AS_ARRAY)
extern s8 gDuel_bOpponentID[];
#else
extern s8 gDuel_bOpponentID __attribute__((section(".data")));
#endif

#endif
