#include "../../types.h"

/* SLPM-86398 build of src/game/model_keyframe_update.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define gModel_pCurrentEffectKey gJapanese_D_8009B074
#define gModel_bEffectKeyCount gJapanese_D_8009B078

#include "../model_keyframe_update.c"
