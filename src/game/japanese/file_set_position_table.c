#include "../../types.h"

/* SLPM-86398 build of src/game/file_set_position_table.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0E0 gJapanese_D_8009B0E0
#define D_8009B10C gJapanese_D_8009B10C
#define D_800E9DF0 gJapanese_D_800E9DF0

#include "../file_set_position_table.c"
