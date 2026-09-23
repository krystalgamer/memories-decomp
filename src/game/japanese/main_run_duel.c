#include "../../types.h"

/* SLPM-86398 build of src/game/main_run_duel.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B362 gJapanese_D_8009B362
#define D_8009B368 gJapanese_D_8009B368
#define D_8009B369 gJapanese_DuelFlag
#define D_8009B370 gJapanese_DuelValue0
#define gDuel_bOpponentID gJapanese_DuelOpponentId

#include "../main_run_duel.c"
