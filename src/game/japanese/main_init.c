#include "../../types.h"

/* SLPM-86398 build of src/game/main_init.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B098 gJapanese_D_8009B098
#define D_8009B09C gJapanese_FrameCounter
#define D_8009B0C0 gJapanese_D_8009B0C0
#define D_8009B0C4 gJapanese_D_8009B0C4
#define D_8009B0C8 gJapanese_D_8009B0C8
#define D_8009B0CC gJapanese_D_8009B0CC
#define D_8009B0D8 gJapanese_FrameStep
#define func_80035A64 Japanese_func_80035A64

#include "../main_init.c"
