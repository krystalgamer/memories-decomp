#include "../../types.h"

/* SLPM-86398 build of src/game/func_80019CC8.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B360 gJapanese_DuelPlayerState
#define gDuel_bOpponentID gJapanese_DuelOpponentId

#include "../func_80019CC8.c"
