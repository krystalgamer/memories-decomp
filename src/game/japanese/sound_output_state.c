#include "../../types.h"

/* SLPM-86398 build of src/game/sound_output_state.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define g_SDValue gJapanese_SDValue

/* The sound root is declared in the source with an asm label of its own, and a
 * #define cannot rename a string literal. The identifier is pre-declared here
 * with the Japanese label instead: gcc 2.8.1 makes an extern's RTL at the first
 * declaration and keeps it on redeclaration (varasm.c make_decl_rtl), so this
 * label is the one that binds. sound.h first, because SDValue comes from it. */
#include "../sound.h"
extern SDValue *volatile g_SDValue_output_level asm("gJapanese_SDValue");

#include "../sound_output_state.c"
