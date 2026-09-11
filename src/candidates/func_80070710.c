/* Reclassified from matching_c (#3859). This was
 * src/game/ai_get_hand_size.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g0_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G0). Under gcc_2_8_1_g0_split, a single
 * threshold, it is 10 of 10 instructions with 8 differing, opcode distance
 * 0. The source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../game/ai.h"
#include "../game/ai_opponent_data.h"

extern s8 gDuel_bOpponentID;

s8 Ai_GetHandSize(void)
{
    return gDuel_aOpponentData[gDuel_bOpponentID].values[0];
}
