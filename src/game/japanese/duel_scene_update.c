#include "../../types.h"

/* SLPM-86398 build of src/game/duel_scene_update.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B238 gJapanese_D_8009B238
#define gDuel_bOpponentID gJapanese_DuelOpponentId

#include "../duel_scene_update.c"
