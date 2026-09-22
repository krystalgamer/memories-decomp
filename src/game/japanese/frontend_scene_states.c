#include "../../types.h"

/* SLPM-86398 build of src/game/frontend_scene_states.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B268 gJapanese_bMenuRequest
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B26D gJapanese_bRequestedMenu
#define D_8009B368 gJapanese_D_8009B368
#define gDuel_wBgmId gJapanese_DuelBgmId

#include "../frontend_scene_states.c"
