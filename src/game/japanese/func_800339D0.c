#include "../../types.h"

/* SLPM-86398 build of src/game/func_800339D0.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009AF74 gJapanese_D_8009AF74
#define D_8009B09C gJapanese_FrameCounter
#define D_8009B140 gJapanese_D_8009B140
#define D_800EB0F8 gJapanese_D_800EB0F8
#define gBuildDeck_pState gBuildDeck_pState

#include "../func_800339D0.c"
