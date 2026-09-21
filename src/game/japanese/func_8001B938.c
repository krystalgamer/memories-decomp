#include "../../types.h"

/* SLPM-86398 build of src/game/func_8001B938.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_800907D8 gJapanese_D_800907D8
#define D_8009B1B4 gJapanese_D_8009B1B4
#define D_800EA030 gJapanese_DuelHandDisplayRecords
#define D_801A7AD8 gJapanese_DuelCardRecords

#include "../func_8001B938.c"
