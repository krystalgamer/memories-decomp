#include "../../types.h"
#define VERSION_JAPAN

/* SLPM-86398 build of src/game/duel_card_effects.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define gFile_wTransferStepFlags_abs gFile_wTransferStepFlags

/* The same object under a second US name, which a #define cannot carry:
 * both names would become one identifier with two declarations. An asm
 * label binds it to the same Japanese symbol instead; an array is left
 * unsized so the US header completes the type. */

#include "../duel_card_effects.c"
