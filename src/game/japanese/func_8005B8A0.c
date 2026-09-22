#include "../../types.h"

/* SLPM-86398 build of src/game/func_8005B8A0.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B060 gJapanese_D_8009B060
#define D_8009B064 gJapanese_D_8009B064
#define D_8009B068 gJapanese_D_8009B068
#define D_8009B06C gJapanese_D_8009B06C
#define D_8009B070 gJapanese_D_8009B070
#define D_8009B144 gJapanese_D_8009B144

#include "../func_8005B8A0.c"
