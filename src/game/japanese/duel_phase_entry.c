#include "../../types.h"

/* SLPM-86398 build of src/game/duel_phase_entry.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_800907D8 gJapanese_D_800907D8
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define D_8009B1B4 gJapanese_D_8009B1B4
#define D_8009B1EC gJapanese_D_8009B1EC
#define D_8009B21C gJapanese_D_8009B21C
#define D_800EA030 gJapanese_DuelHandDisplayRecords
#define D_801A7AD8 gJapanese_DuelCardRecords

#include "../duel_phase_entry.c"
