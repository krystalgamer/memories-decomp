#include "../../types.h"

/* SLPM-86398 build of src/game/func_80018FEC.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B1B4 gJapanese_D_8009B1B4
#define D_8009B21C gJapanese_D_8009B21C
#define D_8009B23A gDuel_wSceneStateFlags
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B369 gJapanese_DuelFlag
#define D_800EA030 gJapanese_DuelHandDisplayRecords

#include "../func_80018FEC.c"
