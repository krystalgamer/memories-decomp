#include "../../types.h"

/* SLPM-86398 build of src/game/duel_card_effects.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_800907D8 gJapanese_D_800907D8
#define D_8009B112_abs D_8009B112
#define D_8009B1AC gJapanese_D_8009B1AC
#define D_801A7AD8 gJapanese_DuelCardRecords

/* The same object under a second US name, which a #define cannot carry:
 * both names would become one identifier with two declarations. An asm
 * label binds it to the same Japanese symbol instead; an array is left
 * unsized so the US header completes the type. */
extern u8 D_800907D8_2d[][20] asm("gJapanese_D_800907D8");
extern u8 D_800907D8_flat[] asm("gJapanese_D_800907D8");

#include "../duel_card_effects.c"
