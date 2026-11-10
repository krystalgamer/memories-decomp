#include "../../types.h"

/* SLPM-86398 build of src/game/duel_effect_text.c. The Japanese release
 * swaps Cross and Circle, so both choice confirm masks are named here. */
#define VERSION_JAPAN
#define DIALOG_UPDATE_CHOICE_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DUEL_EFFECT_CHOICE_CONFIRM_MASK \
    (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#include "../duel_effect_text.c"
