#include "../../types.h"

/* SLPM-86398 build of src/game/fade_runtime.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0D8 gJapanese_FrameStep
#define D_8009B140 gJapanese_D_8009B140
#define D_8009B144 gJapanese_D_8009B144
#define D_800E9D94 gJapanese_D_800E9D94
#define gFade_State gJapanese_FadeState

/* The same object under a second US name, which a #define cannot carry:
 * both names would become one identifier with two declarations. An asm
 * label binds it to the same Japanese symbol instead; an array is left
 * unsized so the US header completes the type. */
extern u8 D_800E9EC8_arr[] asm("gJapanese_FadeState");

#include "../fade_runtime.c"
