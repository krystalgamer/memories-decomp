#include "../../types.h"

/* SLPM-86398 build of src/game/func_800218F0.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */
#define D_8009B360 gJapanese_DuelPlayerState

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_RESULT_REWARDS_CONFIRM_BUTTON PAD_BUTTON_CIRCLE

/* gFade_State under its second US name, as in fade_runtime.c: a #define
 * would give one identifier fade.h's two declarations, so an asm label
 * binds the array to the same symbol and the header completes its type. */
extern u8 D_800E9EC8_arr[] asm("gFade_State");

#include "../func_800218F0.c"
