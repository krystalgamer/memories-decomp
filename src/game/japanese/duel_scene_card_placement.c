#include "../../types.h"

/* SLPM-86398 build of src/game/duel_scene_card_placement.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define D_8009B1B4 gJapanese_D_8009B1B4
#define D_8009B218 gJapanese_D_8009B218
#define D_800EA030 gJapanese_DuelHandDisplayRecords
#define D_800EB0F8 gJapanese_D_800EB0F8
#define D_801A7AD8 gJapanese_DuelCardRecords

#include "../duel_scene_card_placement.c"
