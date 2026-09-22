#include "../../types.h"

/* SLPM-86398 build of src/game/model_effect_wrapped_value.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define func_8005F1A4 ModelEffect_GetCoefficient

#include "../model_effect_wrapped_value.c"
