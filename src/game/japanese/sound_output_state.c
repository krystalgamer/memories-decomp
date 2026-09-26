#include "../../types.h"

/* SLPM-86398 build of src/game/sound_output_state.c: the US source is included unchanged. */

/* The sound root is declared in the source with an asm label of its own, and a
 * #define cannot rename a string literal. The identifier is pre-declared here
 * with the Japanese label instead: gcc 2.8.1 makes an extern's RTL at the first
 * declaration and keeps it on redeclaration (varasm.c make_decl_rtl), so this
 * label is the one that binds. sound.h first, because SDValue comes from it. */
#include "../sound.h"
extern SDValue *volatile g_SDValue_output_level asm("g_SDValue");

#include "../sound_output_state.c"
