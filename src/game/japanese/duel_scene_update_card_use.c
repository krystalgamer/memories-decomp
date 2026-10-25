#include "../../types.h"
#define VERSION_JAPAN

/* SLPM-86398 build of src/game/duel_scene_update_card_use.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest

#include "../duel_scene_update_card_use.c"
