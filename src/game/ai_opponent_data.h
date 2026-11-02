#ifndef MEMORIES_DECOMP_AI_OPPONENT_DATA_H
#define MEMORIES_DECOMP_AI_OPPONENT_DATA_H

#include "../ygo_types.h"

#define AI_OPPONENT_DATA_FIELD_COUNT 9
#define AI_OPPONENT_COUNT 40
#define AI_OPPONENT_HAND_SEARCH_COUNT_FIELD 0

/* One nine-byte record per duellist. Ai_GetHandSearchCount reads values[0] as
   the configured hand/fusion search window, not a live hand count.
   AiScript_LoadOpponentData multiplies values[1] by
   100 for opcode field 0 and returns values[field + 1] for the rest, so the
   script reaches every entry by index and the record stays an array.
   src/game/ai_get_hand_search_count.c owns the table at 0x800917F0. */
typedef struct {
    s8 values[AI_OPPONENT_DATA_FIELD_COUNT];
} AiOpponentData;

typedef char AiOpponentData_size_must_be_9[
    sizeof(AiOpponentData) == AI_OPPONENT_DATA_FIELD_COUNT ? 1 : -1
];

extern AiOpponentData gDuel_aOpponentData[AI_OPPONENT_COUNT];

/* The signed opponent selector, element 1 of gDuel_bSide0ControllerSelector
 * (ai_controller_handoff.h). Every retail access is absolute, so every unit
 * takes this one .data declaration. */
extern s8 gDuel_bOpponentID __attribute__((section(".data")));

#endif
