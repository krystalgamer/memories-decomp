#include "../../types.h"

/* SLES-03948 to SLES-03951 build of src/game/duel_effect_text.c: the
 * European values, plus the choice input read that also clears a held R1. */
#define DIALOG_READ_CHOICE_CLEARS_HELD_R1

#include "../european/duel_effect_text.c"
