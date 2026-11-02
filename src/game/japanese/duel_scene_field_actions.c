#include "../../types.h"
#define VERSION_JAPAN

/* SLPM-86398 build of src/game/duel_scene_field_actions.c. */

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_FIELD_ACTIONS_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DUEL_FIELD_ACTIONS_CANCEL_BUTTON PAD_BUTTON_CROSS

#include "../duel_scene_field_actions.c"
