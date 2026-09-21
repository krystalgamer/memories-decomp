#include "../../types.h"

/* SLPM-86398 build of src/game/ai_fusion.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define gAiScript_State gJapanese_AiScriptState

#include "../ai_fusion.c"
