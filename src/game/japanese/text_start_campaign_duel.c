#include "../../types.h"

/* SLPM-86398 build of src/game/text_start_campaign_duel.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B360 gJapanese_DuelPlayerState
#define D_8009B368 gJapanese_D_8009B368
#define D_8009B369 gJapanese_DuelFlag
#define D_8009B370 gJapanese_DuelValue0
#define D_8009B372 gJapanese_DuelValue1
#define D_8009B374 gJapanese_DuelBgmId2
#define gDuel_bOpponentID gJapanese_DuelOpponentId
#define gDuel_bTerrain gJapanese_DuelTerrain
#define gDuel_wBgmId gJapanese_DuelBgmId

#include "../text_start_campaign_duel.c"
