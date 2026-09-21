#include "../../types.h"

/* SLPM-86398 build of src/game/ai_script_load_duel_globals.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define gDuel_bOpponentID gJapanese_DuelOpponentId
#define gDuel_bTerrain gJapanese_DuelTerrain

#include "../ai_script_load_duel_globals.c"
